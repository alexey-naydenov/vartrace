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

#include <boost/scoped_array.hpp>

#include "tracetypes.h"
#include "simplestack.h"

namespace vartrace {

/*! Class for variable trace objects. */
class VarTrace
{
public:
    enum { MaxMessageDepth = 20 };
	
    /*! Create a trace of a given size. */
    VarTrace(size_t size);
    /*! Destructor. */
    virtual ~VarTrace() {}

    template <typename T> void logMessage(const T& value);
    
private:
    /*! Length of the data array. */
    unsigned length_;
    /*! Pointer to the memory block that contains the trace. */
    boost::scoped_array<AlignmentType> data_;
    /*! Positions of heads for recursive messages. */
    SimpleStack<unsigned> heads_;
    /*! End of the last message. */
    unsigned tail_;
    /*! Position of the trace wrap. */
    unsigned wrap_;

    /*! Disabled default copy constructor. */
    VarTrace(const VarTrace&);
    /*! Disabled default assingment operator. */
    VarTrace& operator=(const VarTrace&);
};

/*! Calculates the number of AlingmentType elements required to store
    an object. */
template <typename T>
unsigned aligned_size(const T& value) 
{
    return sizeof(value)/sizeof(AlignmentType)
	+ (sizeof(value)%sizeof(AlignmentType) == 0 ? 0 : 1);
}

template <typename T>
void VarTrace::logMessage(const T& value) 
{
}

}

#endif /* VARTRACE_H */
