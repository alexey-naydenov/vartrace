/* vartrace.cpp
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

/*! \file vartrace.cpp 
 * Definitions for trace object. 
 */

#include <stddef.h>
#include <cstring>

#include "vartrace/tracetypes.h"
#include "vartrace/vartrace.h"
#include "vartrace/simplestack.h"

namespace vartrace {

unsigned message_length(unsigned size)
{
    return VarTrace::HeaderLength + size/sizeof(AlignmentType)
	   + (size%sizeof(AlignmentType) == 0 ? 0 : 1);
}

TimestampType incremental_timestamp() 
{
    static TimestampType timestamp = 0;
    return timestamp++;
}

VarTrace::VarTrace(size_t size) :
    length_(size/sizeof(AlignmentType) + 1),
    data_(new AlignmentType[length_]),
    heads_(MaxNestingDepth),
    tail_(0), wrap_(0),
    errorFlags_(0),
    isEmpty_(true),
    getTimestamp(incremental_timestamp)
{
    heads_.push(0);
}

AlignmentType* VarTrace::rawData() const
{
    return data_.get();
}

AlignmentType* VarTrace::head()
{
    return data_.get() + heads_.top();
}

unsigned VarTrace::dump(void *buffer, unsigned buffer_size)
{
    if (isEmpty()) return 0;

    unsigned copied_size = 0;
    // check if the trace is wrapped around
    if (heads_.top() < tail_) { 
	// copy trace of buffer_size bytes
	unsigned size2copy = (tail_ - heads_.top())*sizeof(AlignmentType);
	if (size2copy > buffer_size) size2copy = buffer_size;
	std::memcpy(buffer, data_.get() + heads_.top(), size2copy);
	copied_size += size2copy;
    } else {
	// copy from head to wrap
	unsigned size2copy = (wrap_ - heads_.top())*sizeof(AlignmentType);
	if (size2copy > buffer_size) size2copy = buffer_size;
	std::memcpy(buffer, data_.get() + heads_.top(), size2copy);
	copied_size += size2copy;
	buffer_size -= copied_size;
	// copy from the trace storage start to the tail
	if (buffer_size > 0) {
	    size2copy = tail_*sizeof(AlignmentType);
	    if (size2copy > buffer_size) size2copy = buffer_size;
	    std::memcpy(reinterpret_cast<char*>(buffer) + copied_size,
			data_.get(), size2copy);
	    copied_size += size2copy;
	}
    }
    
    // clean up the trace
    reset();

    return copied_size;
}

bool VarTrace::isEmpty()
{
    return isEmpty_;
}

bool VarTrace::isConsistent() 
{
    bool has_error = false;

    if (heads_.top() + NestedHeaderLength >= wrap_) {
	errorFlags_ |= 1;
	has_error = true;
    }
    if (heads_.top() + NestedHeaderLength >= length_) {
	errorFlags_ |= 2;
	has_error = true;
    }

    if (tail_ >= length_) {
	errorFlags_ |= 4;
	has_error = true;
    }
    if (tail_ > wrap_) {
	errorFlags_ |= 8;
	has_error = true;
    }
    
    if (wrap_ > length_) {
	errorFlags_ |= 16;
	has_error = true;
    }

    return !has_error;
}

unsigned VarTrace::errorFlags() const
{
    return errorFlags_;
}

unsigned VarTrace::nextHead()
{
    if (isEmpty()) return 0;
    // get the pointer to the length field
    ShortestType *ch =
	reinterpret_cast<ShortestType*>(head()) + sizeof(TimestampType);
    // find the end of head message
    unsigned next_head =
	heads_.top() + message_length(*(reinterpret_cast<LengthType*>(ch)));
    // if the wrap point is reached return 0
    if (next_head == wrap_) return 0;

    return next_head;
}

void VarTrace::reset() 
{
    // reset all pointers and errors
    tail_ = 0;
    wrap_ = 0;
    errorFlags_ = 0;
    isEmpty_ = true;
    // clean up stack of heads
    while (!heads_.empty()) {
	heads_.pop();
    }
    heads_.push(0);
}

}
