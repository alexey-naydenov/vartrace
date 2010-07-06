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

#include <boost/scoped_array.hpp>

#include "copytraits.h"
#include "tracetypes.h"
#include "simplestack.h"
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
    /*! Destructor. */
    virtual ~VarTrace() {}

    /*! Returns the pointer to the begining of data buffer. */
    AlignmentType* rawData() const;
    /*! Returns the pointer to the first message. */
    AlignmentType* head();
    /*! Dump trace content into a buffer.
     *
     * The trace is emtied after this operation. */
    unsigned dump(void *buffer, unsigned size);
    
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

    /*! Checks if trace is empty. */
    bool isEmpty();

    /*! Checks if internal structure is consistent. */
    bool isConsistent();

    /*! Return error state of the trace. */
    unsigned errorFlags() const;
    
private:
    /*! Length of the data array. */
    unsigned length_;
    /*! Pointer to the memory block that contains the trace. */
    boost::scoped_array<AlignmentType> data_;
    /*! Positions of heads for recursive messages. */
    SimpleStack<unsigned> heads_;
    /*! Index of the element after the end of the last message. */
    unsigned tail_;
    /*! Position of the trace wrap. */
    unsigned wrap_;
    /*! Error states of the trace. */
    unsigned errorFlags_;

    bool isEmpty_;
    
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
unsigned message_length(unsigned size);

template <typename T>
void VarTrace::log(MessageIdType message_id, const T& value) 
{
    doLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
	  DataTypeTraits<T>::DataTypeId, DataTypeTraits<T>::TypeSize);
}

template <typename T>
void VarTrace::logArray(MessageIdType message_id, const T* data, int length)
{
    
}

template <typename T>
void VarTrace::doLog(MessageIdType message_id, const T * value,
		     const SizeofCopyTag& copy_tag, unsigned data_id,
		     unsigned object_size)
{
    unsigned required_length = message_length(object_size);
    unsigned copy_index = tail_;
    unsigned new_tail_index = tail_ + required_length;
    unsigned next_head;
	
    // check for need to wrap around
    if (new_tail_index >= length_) {
	copy_index = 0;
	new_tail_index = required_length;
	wrap_ = tail_;
    }
    // check if head should be moved and shift it
    if (!isEmpty()) {
	while (heads_.top() >= copy_index && heads_.top() < new_tail_index) {
	    next_head = nextHead();
	    if (next_head == heads_.top()) break;
	    heads_.top() = next_head;
	}
    }

    // create header
    ShortestType *tail = reinterpret_cast<ShortestType*>(&data_[copy_index]);
    *(reinterpret_cast<TimestampType*>(tail)) = getTimestamp();
    tail += sizeof(TimestampType);
    *(reinterpret_cast<LengthType*>(tail)) = object_size;
    tail += sizeof(LengthType);
    *(reinterpret_cast<MessageIdType*>(tail)) = message_id;
    tail += sizeof(MessageIdType);
    *(reinterpret_cast<DataIdType*>(tail)) = data_id;
    tail += sizeof(DataIdType);

    // copy data
    std::memcpy(tail, value, object_size);

    // move tail
    tail_ = new_tail_index;
    // advance the wrap point if the tail moved past it
    if (wrap_ < tail_) wrap_ = tail_;
    // log is not empty anymore
    isEmpty_ = false;
}

}

#endif /* VARTRACE_H */
