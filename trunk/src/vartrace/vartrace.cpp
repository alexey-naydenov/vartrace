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

namespace vartrace {

unsigned message_length(unsigned size, bool isNested)
{
    if (isNested) {
	return VarTrace::NestedHeaderLength + size/sizeof(AlignmentType)
	    + (size%sizeof(AlignmentType) == 0 ? 0 : 1);
    } else {
	return VarTrace::HeaderLength + size/sizeof(AlignmentType)
	    + (size%sizeof(AlignmentType) == 0 ? 0 : 1);
    }
}

TimestampType incremental_timestamp() 
{
    static TimestampType timestamp = 0;
    return timestamp++;
}

VarTrace::VarTrace(size_t size) :
    storage_(new TraceStorage),
    errorFlags_(0),
    isEmpty_(true),
    isNested_(false),
    isWritingEnabled_(true),
    parent_(this),
    getTimestamp(incremental_timestamp)
{
    storage_->length_ = size/sizeof(AlignmentType) + 1;
    storage_->data_ = new AlignmentType[storage_->length_];
    storage_->head_ = 0;
    storage_->tail_ = 0;
    storage_->wrap_ = 0;
}

VarTrace::~VarTrace()
{
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


AlignmentType * VarTrace::rawData() const
{
    return storage_->data_;
}

AlignmentType * VarTrace::head()
{
    return storage_->data_ + storage_->head_;
}

unsigned VarTrace::dump(void *buffer, unsigned buffer_size)
{
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
	std::memcpy(buffer, storage_->data_ + storage_->head_, size2copy);
	copied_size += size2copy;
    } else {
	// copy from head to wrap
	unsigned size2copy =
	    (storage_->wrap_ - storage_->head_)*sizeof(AlignmentType);
	if (size2copy > buffer_size) size2copy = buffer_size;
	std::memcpy(buffer, storage_->data_ + storage_->head_, size2copy);
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

void VarTrace::setTimestampFunction(TimestampFunctionType fnct)
{
    assert(fnct != 0);
    getTimestamp = fnct;
}


bool VarTrace::isEmpty()
{
    return isEmpty_;
}

bool VarTrace::isConsistent() 
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
	 storage_->head_ + message_length(*(reinterpret_cast<LengthType*>(ch)),
					  false);
    // if the wrap point is reached return 0
    if (next_head == storage_->wrap_) return 0;

    return next_head;
}

void VarTrace::reset() 
{
    // reset all pointers and errors
    storage_->tail_ = 0;
    storage_->wrap_ = 0;
    storage_->head_ = 0;
    errorFlags_ = 0;
    isEmpty_ = true;
}

VarTrace::VarTrace(VarTrace * parent) :
    storage_(parent->storage_),
    errorFlags_(0),
    isEmpty_(parent->isEmpty_),
    isNested_(true),
    isWritingEnabled_(true),
    parent_(parent),
    getTimestamp(parent->getTimestamp)
{
}

VarTrace * VarTrace::createSubtrace(MessageIdType message_id)
{
    // create header for subtrace with timestamp and 0 data size
    subtraceStart_ = storage_->tail_;
    log(message_id, *this);
    // if new tail is smaller then the saved one then wrap happened,
    // save real position
    if (storage_->tail_ < subtraceStart_) {
	subtraceStart_ = storage_->tail_;
    }
    // create new trace object
    VarTrace * sub_trace = new VarTrace(this);
    isWritingEnabled_ = false;

    return sub_trace;
}

void VarTrace::subtraceClosed()
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
	reinterpret_cast<ShortestType*>(&storage_->data_[subtraceStart_]);
    // if *this is a top level trace skip over timestamp
    if (!isNested_) subtrace_header += sizeof(TimestampType);
    *(reinterpret_cast<LengthType*>(subtrace_header)) =
	subtrace_length*sizeof(AlignmentType);
}

}
