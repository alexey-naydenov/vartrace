/* vartrace.h
 *
 * Copyright (C) 2010 Alexey Naydenov
 *
 * Author: Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*! \file vartrace.h 
 * Template for a variable trace object. 
 */

#ifndef VARTRACE_H
#define VARTRACE_H

#include <stddef.h>
#include <cstring>
#include <cassert>
#include <iostream>

#include "copytraits.h"
#include "tracetypes.h"
#include "datatypeid.h"
#include "datatraits.h"

namespace vartrace {

/*! Class for variable trace objects. */
class VarTrace
{
public:
    enum {MaxNestingDepth = 20 /*!< Maximum message nesting depth. */ };
    enum
    {
	/*! Size of a header without a timestamp. */
	NestedHeaderSize = (sizeof(LengthType) + sizeof(MessageIdType)
			    + sizeof(DataIdType)),
	/*! Size of a header with a timestamp. */
	HeaderSize = (sizeof(TimestampType) + sizeof(LengthType)
		      + sizeof(MessageIdType) + sizeof(DataIdType)),
	/*! Length of a header without a timestamp. */
	NestedHeaderLength = (NestedHeaderSize/sizeof(AlignmentType)
			      + (NestedHeaderSize%sizeof(AlignmentType) == 0
				 ? 0 : 1)),
	/*! Length of a header with a timestamp. */
	HeaderLength = (HeaderSize/sizeof(AlignmentType)
			+ (HeaderSize%sizeof(AlignmentType) == 0 ? 0 : 1))
    };

    /*! Create a trace of a given size. */
    VarTrace(size_t size);
    VarTrace(VarTrace * parent);
    /*! Destructor. */
    virtual ~VarTrace();

    /*! Returns the pointer to the begining of data buffer. */
    AlignmentType * rawData() const;
    /*! Returns the pointer to the first message. */
    AlignmentType * head();
    /*! Dump trace content into a buffer.
     *
     * The trace is emtied after this operation. */
    unsigned dump(void * buffer, unsigned size);
    
    template <typename T> void log(MessageIdType message_id, const T& value);
    
    //! Add an array of values to the trace.
    /*! An array of size known at compiletime can be logged by log
      function.
  
      \param message_id message type id;
      \param data pointer to the data;
      \param length length of the array.
    */
    template <typename T> void logArray(MessageIdType message_id,
					const T * data, int length);

    template <typename T> void doLog(
	MessageIdType message_id, const T * value,
	const SizeofCopyTag& copy_tag, unsigned data_id,
	unsigned object_size);

    VarTrace * createSubtrace(MessageIdType message_id);
    void subtraceClosed();
    
    /*! Checks if trace is empty. */
    bool isEmpty();

    /*! Checks if internal structure is consistent. */
    bool isConsistent();

    /*! Return error state of the trace. */
    unsigned errorFlags() const;

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
    unsigned nextHead();
    /*! Clean up trace. */
    void reset();

    /*! Disabled default copy constructor. */
    VarTrace(const VarTrace&);
    /*! Disabled default assingment operator. */
    VarTrace& operator=(const VarTrace&);
};

/*! Calculates the number of AlingmentType elements required to store
 *  an object. */
template <typename T>
unsigned aligned_size()
{
    return sizeof(T)/sizeof(AlignmentType)
	+ (sizeof(T)%sizeof(AlignmentType) == 0 ? 0 : 1);
}

/*! Calculates the number of AlingmentType elements required to store
 *  an object. */
unsigned message_length(unsigned size, bool isNested);

template <typename T>
void VarTrace::log(MessageIdType message_id, const T& value) 
{
    doLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
	  DataTypeTraits<T>::DataTypeId, DataTypeTraits<T>::TypeSize);
}

template <typename T>
void VarTrace::logArray(MessageIdType message_id, const T * data, int length)
{
    doLog(message_id, data, typename CopyTraits<T>::CopyCategory(),
	  DataTypeTraits<T[1]>::DataTypeId, sizeof(T)*length);
}

template <typename T>
void VarTrace::doLog(MessageIdType message_id, const T * value,
		     const SizeofCopyTag& copy_tag, unsigned data_id,
		     unsigned object_size)
{
    assert(isWritingEnabled_);
    unsigned required_length = message_length(object_size, isNested_);
    unsigned copy_index = storage_->tail_;
    unsigned new_tail_index = storage_->tail_ + required_length;
    unsigned next_head;
	
    // check if there is enough space at the end of the trace memory
    // block
    if (new_tail_index >= storage_->length_) {
	copy_index = 0; // write from trace start
	new_tail_index = required_length;
	storage_->wrap_ = storage_->tail_; // set new wrap point
    }
    // head of the trace must be outside of the indices range for the
    // new message
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
    if (storage_->wrap_ < storage_->tail_) storage_->wrap_ = storage_->tail_;
    // log is not empty anymore
    isEmpty_ = false;
}

//! Spacialization of trait structure for subtraces.
template<> struct DataTypeTraits<VarTrace>
{
    enum {
	DataTypeId = 0,
	TypeSize = 0
    };
};
    
}

#endif /* VARTRACE_H */
