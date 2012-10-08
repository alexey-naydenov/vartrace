/* utility.h
   
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

/*! \file utility.h 
  Some utility macros and constants. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_UTILITY_H_
#define TRUNK_INCLUDE_VARTRACE_UTILITY_H_

#include <vartrace/tracetypes.h>

namespace vartrace {
//! Divide 2 ints and round up the result.
#define CEIL_DIV(num, div) ((num) + (div) - 1)/(div)

inline unsigned RoundSize(unsigned size) {
  return (size + sizeof(AlignmentType) - 1)/sizeof(AlignmentType);
}

//! Size of a header without a timestamp.
const unsigned kNestedHeaderSize = sizeof(LengthType) + sizeof(MessageIdType)
    + sizeof(DataIdType);
//! Size of a header with a timestamp.
const unsigned kHeaderSize = sizeof(TimestampType) + kNestedHeaderSize;
//! Length of a header without a timestamp.
const unsigned kNestedHeaderLength = CEIL_DIV(kNestedHeaderSize,
                                         sizeof(AlignmentType));
//! Length of a header with a timestamp.
const unsigned kHeaderLength = CEIL_DIV(kHeaderSize, sizeof(AlignmentType));

const unsigned kBitsPerByte = 8;
const unsigned kMessageIdShift = kBitsPerByte*sizeof(LengthType);
const unsigned kDataIdShift = kBitsPerByte*(sizeof(MessageIdType)
                                            + sizeof(LengthType));
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_UTILITY_H_
