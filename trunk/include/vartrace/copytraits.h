/* copytraits.h
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

/*! \file copytraits.h 
 * Traits that describe copying data into log. 
 */

#ifndef COPYTRAITS_H
#define COPYTRAITS_H

#include "tracetypes.h"
#include "datatypeid.h"

namespace vartrace {
/*! Sizeof memory segment should be copied into the trace. */
struct SizeofCopyTag {};

/*! Object will copy itself into the trace. */
struct SelfCopyTag : public SizeofCopyTag {};

/*! Object can be copied through an assignment. */
struct AssignmentCopyTag : public SizeofCopyTag {};
    
/*! Trait class that describes how to add an object into the trace. */
template<typename T> struct CopyTraits
{
    /*! Describes how to copy an object into the log. */
    typedef SizeofCopyTag CopyCategory;
    /*! Size and data type information. */
    enum
    {
	DataTypeId = DataType2Int<T>::id,
	ObjectSize = sizeof(T),
	ObjectLength = (sizeof(T)/sizeof(AlignmentType)
			 + (sizeof(T)%sizeof(AlignmentType) == 0 ? 0 : 1))
    };
};

}

#endif /* COPYTRAITS_H */
