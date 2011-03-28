/* vartrace.h
   Copyright (C) 2011 Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file vartrace.h 
  Template for a variable trace object. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_H_

#include <cstring>
#include <cassert>

#include "vartrace/copytraits.h"
#include "vartrace/tracetypes.h"
#include "vartrace/datatypeid.h"
#include "vartrace/datatraits.h"
#include "vartrace/policies.h"

namespace vartrace {

/*! Calculates the number of AlingmentType elements required to store
 *  an object. */
template <typename T> unsigned aligned_size() {
    return sizeof(T)/sizeof(AlignmentType)
        + (sizeof(T)%sizeof(AlignmentType) == 0 ? 0 : 1);
}

TimestampType incremental_timestamp() {
    static TimestampType timestamp = 0;
    return timestamp++;
}

/*! Class for variable trace objects. */
template <
  template <class> class CP = CreateNewObject,
  template <class> class LP = LockSingleThread,
  class AP = AllocateNewStorage
  >
class VarTrace
    : public CP< VarTrace<CP, LP, AP> >,
      public LP< VarTrace<CP, LP, AP> >,
      public AP {
 public:
    enum {
      /*! Size of a header without a timestamp. */
      NestedHeaderSize = (sizeof(LengthType) + sizeof(MessageIdType)
                          + sizeof(DataIdType)),
      /*! Size of a header with a timestamp. */
      HeaderSize = (sizeof(TimestampType) + sizeof(LengthType)
                    + sizeof(MessageIdType) + sizeof(DataIdType)),
      /*! Length of a header without a timestamp. */
      NestedHeaderLength =
      (NestedHeaderSize/sizeof(AlignmentType)
       + (NestedHeaderSize%sizeof(AlignmentType) == 0 ? 0 : 1)),
      /*! Length of a header with a timestamp. */
      HeaderLength = (HeaderSize/sizeof(AlignmentType)
                      + (HeaderSize%sizeof(AlignmentType) == 0 ? 0 : 1))
    };

  /*! Create a trace of a given size. */
  explicit VarTrace(size_t size):
      storage_(new TraceStorage),
      errorFlags_(0),
      isEmpty_(true),
      isNested_(false),
      isWritingEnabled_(true),
      parent_(this),
      getTimestamp(incremental_timestamp) {
    storage_->length_ = size/sizeof(AlignmentType) + 1;
    storage_->data_ = new AlignmentType[storage_->length_];
    storage_->head_ = 0;
    storage_->tail_ = 0;
    storage_->wrap_ = 0;
  }

  explicit VarTrace(VarTrace * parent) :
      storage_(parent->storage_),
      errorFlags_(0),
      isEmpty_(parent->isEmpty_),
      isNested_(true),
      isWritingEnabled_(true),
      parent_(parent),
      getTimestamp(parent->getTimestamp)
  {}

  /*! Destructor. */
  virtual ~VarTrace() {
    // notify parent that subtrace is getting destroyed
    if (isNested_) {
      parent_->subtraceClosed();
    }
    // delete allocated memory if top level trace is destroyed
    if (!isNested_) {
      delete []  storage_->data_;
      delete storage_;
    }
  }

  /*! Returns the pointer to the begining of data buffer. */
  AlignmentType * rawData() const {
    return storage_->data_;
  }
  /*! Returns the pointer to the first message. */
  AlignmentType * head() {
    return storage_->data_ + storage_->head_;
  }
  /*! Dump trace content into a buffer.
   
    The trace is emtied after this operation.
  */
  unsigned dump(void * buffer, unsigned buffer_size) {
    if (isEmpty()) return 0;
    // dump is possible if only from top level trace after all
    // subtraces were closed
    if ((!isWritingEnabled_) || isNested_) return 0;

    unsigned copied_size = 0;
    // check if the trace is wrapped around
    if (storage_->head_ < storage_->tail_) { 
      // copy trace of buffer_size bytes
      unsigned size2copy =
          (storage_->tail_ - storage_->head_)*sizeof(AlignmentType);
      if (size2copy > buffer_size) size2copy = buffer_size;
      std::memcpy(buffer, storage_->data_ + storage_->head_,
                  size2copy);
      copied_size += size2copy;
    } else {
      // copy from head to wrap
      unsigned size2copy =
          (storage_->wrap_ - storage_->head_)*sizeof(AlignmentType);
      if (size2copy > buffer_size) size2copy = buffer_size;
      std::memcpy(buffer, storage_->data_ + storage_->head_,
                  size2copy);
      copied_size += size2copy;
      buffer_size -= copied_size;
      // copy from the trace storage start to the tail
      if (buffer_size > 0) {
        size2copy = storage_->tail_*sizeof(AlignmentType);
        if (size2copy > buffer_size) size2copy = buffer_size;
        std::memcpy(reinterpret_cast<char*>(buffer) + copied_size,
                    storage_->data_, size2copy);
        copied_size += size2copy;
      }
    }
    
    // clean up the trace
    reset();

    return copied_size;
  }
    
  template <typename T>
  void log(MessageIdType message_id, const T & value) 
  {
    doLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
          DataTypeTraits<T>::DataTypeId, DataTypeTraits<T>::TypeSize);
  }
    
  //! Add an array of values to the trace.
  /*! An array of size known at compiletime can be logged by log
    function.
  
    \param message_id message type id;
    \param data pointer to the data;
    \param length length of the array.
  */
  template <typename T>
  void logArray(MessageIdType message_id, const T * data, int length)
  {
    doLog(message_id, data, typename CopyTraits<T>::CopyCategory(),
          DataTypeTraits<T[1]>::DataTypeId, sizeof(T)*length);
  }
    
  template <typename T>
  void doLog(MessageIdType message_id, const T * value,
             const SizeofCopyTag& copy_tag, unsigned data_id,
             unsigned object_size)
  {
    assert(isWritingEnabled_);
    unsigned required_length = message_length(object_size, isNested_);
    unsigned copy_index = storage_->tail_;
    unsigned new_tail_index = storage_->tail_ + required_length;
    unsigned next_head;
	
    // check if there is enough space at the end of the trace
    // memory block
    if (new_tail_index >= storage_->length_) {
      copy_index = 0; // write from trace start
      new_tail_index = required_length;
      storage_->wrap_ = storage_->tail_; // set new wrap point
    }
    // head of the trace must be outside of the indices range for
    // the new message
    while (storage_->head_ >= copy_index
           && storage_->head_ < new_tail_index) {
      next_head = nextHead();
      if (next_head == storage_->head_) break; // trace is empty
      storage_->head_ = next_head;
    }

    // create header
    ShortestType *tail =
        reinterpret_cast<ShortestType*>(&storage_->data_[copy_index]);
    // write timestamp only for top level trace
    if (!isNested_) {
      *(reinterpret_cast<TimestampType*>(tail)) = getTimestamp();
      tail += sizeof(TimestampType);
    }
    *(reinterpret_cast<LengthType*>(tail)) = object_size;
    tail += sizeof(LengthType);
    *(reinterpret_cast<MessageIdType*>(tail)) = message_id;
    tail += sizeof(MessageIdType);
    *(reinterpret_cast<DataIdType*>(tail)) = data_id;
    tail += sizeof(DataIdType);

    // copy data
    std::memcpy(tail, value, object_size);

    // move tail
    storage_->tail_ = new_tail_index;
    // advance the wrap point if the tail moved past it
    if (storage_->wrap_ < storage_->tail_) {
      storage_->wrap_ = storage_->tail_;
    }
    // log is not empty anymore
    isEmpty_ = false;
    }

    unsigned message_length(unsigned size, bool isNested)
    {
	if (isNested) {
	    return NestedHeaderLength + size/sizeof(AlignmentType)
		+ (size%sizeof(AlignmentType) == 0 ? 0 : 1);
	} else {
	    return HeaderLength + size/sizeof(AlignmentType)
		+ (size%sizeof(AlignmentType) == 0 ? 0 : 1);
	}
    }

    VarTrace * createSubtrace(MessageIdType message_id)
    {
	// create header for subtrace with timestamp and 0 data size
	subtraceStart_ = storage_->tail_;
	log(message_id, *this);
	// if new tail is smaller then the saved one then wrap
	// happened, save real position
	if (storage_->tail_ < subtraceStart_) {
	    subtraceStart_ = storage_->tail_;
	}
	// create new trace object
	VarTrace * sub_trace = new VarTrace(this);
	isWritingEnabled_ = false;

	return sub_trace;
    }
    
    void subtraceClosed()
    {
	// enable trace writing
	isWritingEnabled_ = true;
	// clculate size of the closed subtrace child
	int subtrace_length;
	int header_length = isNested_ ?
	    VarTrace::NestedHeaderLength : VarTrace::HeaderLength;
	if (storage_->tail_ > subtraceStart_) {
	    subtrace_length = storage_->tail_ - subtraceStart_
		- header_length;
	} else {
	    subtrace_length = storage_->wrap_ - subtraceStart_
		- header_length + storage_->tail_;
	}
	// update size of the closed subtrace
	ShortestType * subtrace_header =
	    reinterpret_cast<ShortestType*>(
		&storage_->data_[subtraceStart_]);
	// if *this is a top level trace skip over timestamp
	if (!isNested_) subtrace_header += sizeof(TimestampType);
	*(reinterpret_cast<LengthType*>(subtrace_header)) =
	    subtrace_length*sizeof(AlignmentType);
    }

    void setTimestampFunction(TimestampFunctionType fnct)
    {
	assert(fnct != 0);
	getTimestamp = fnct;
    }
    
    /*! Checks if trace is empty. */
    bool isEmpty()
    {
	return isEmpty_;
    }

    /*! Checks if internal structure is consistent. */
    bool isConsistent()
    {
	bool has_error = false;

	if ((storage_->head_ + NestedHeaderLength >= storage_->wrap_)
	    && !isEmpty()) {
	    errorFlags_ |= 1;
	    has_error = true;
	}
	if ((storage_->head_ + NestedHeaderLength >= storage_->length_)
	    && !isEmpty()) {
	    errorFlags_ |= 2;
	    has_error = true;
	}

	if (storage_->tail_ >= storage_->length_) {
	    errorFlags_ |= 4;
	    has_error = true;
	}
	if (storage_->tail_ > storage_->wrap_) {
	    errorFlags_ |= 8;
	    has_error = true;
	}
    
	if (storage_->wrap_ > storage_->length_) {
	    errorFlags_ |= 16;
	    has_error = true;
	}

	return !has_error;
    }

    /*! Return error state of the trace. */
    unsigned errorFlags() const
    {
	return errorFlags_;
    }

    struct TraceStorage 
    {
	/*! Length of the data array. */
	unsigned length_;
	/*! Pointer to the memory block that contains the trace. */
	AlignmentType * data_;
	/*! Positions of heads for recursive messages. */
	unsigned head_;
	/*! Index of the element after the end of the last message. */
	unsigned tail_;
	/*! Position of the trace wrap. */
	unsigned wrap_;
    };
    
private:
    TraceStorage * storage_;
    unsigned subtraceStart_;
    
    /*! Error states of the trace. */
    unsigned errorFlags_;

    bool isEmpty_;
    bool isNested_;
    bool isWritingEnabled_;

    VarTrace * parent_;
    
    /*! Pointer to function that returns timestamp. */
    TimestampFunctionType getTimestamp;

    /*! Get position of the next head. */
    unsigned nextHead()
    {
	if (isEmpty()) return 0;
	// get the pointer to the length field
	ShortestType *ch =
	    reinterpret_cast<ShortestType*>(head()) + sizeof(TimestampType);
	// find the end of head message
	unsigned next_head =
	    storage_->head_
	    + message_length(*(reinterpret_cast<LengthType*>(ch)), false);
	// if the wrap point is reached return 0
	if (next_head == storage_->wrap_) return 0;

	return next_head;
    }
    /*! Clean up trace. */
    void reset()
    {
	// reset all pointers and errors
	storage_->tail_ = 0;
	storage_->wrap_ = 0;
	storage_->head_ = 0;
	errorFlags_ = 0;
	isEmpty_ = true;
    }

    /*! Disabled default copy constructor. */
    VarTrace(const VarTrace&);
    /*! Disabled default assingment operator. */
    VarTrace& operator=(const VarTrace&);
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
