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
    tail_(0), wrap_(length_),
    getTimestamp(incremental_timestamp)
{
    heads_.push(0);
}

}
