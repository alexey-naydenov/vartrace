/* tracetypes.h
   
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

/*! \file tracetypes.h 
  Definition of types used for creating a trace. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_TRACETYPES_H_
#define TRUNK_INCLUDE_VARTRACE_TRACETYPES_H_

#include <stdint.h>

#ifdef __cplusplus
namespace vartrace {
#endif

//! Divide 2 ints and round up the result.
#define CEIL_DIV(num, div) ((num) + (div) - 1)/(div)

//! Sets type for minimal unit of data.
typedef uint8_t ShortestType;
//! Sets alingment of message boundaries and data fields.
typedef uint32_t AlignmentType;
//! Timestamp type, should be a similar to AlignmentType.
typedef uint32_t TimestampType;
//! Type to store message size.
typedef uint16_t LengthType;
//! Message type fields type
typedef uint8_t MessageIdType;
//! Data type fields type.
typedef uint8_t DataIdType;

//! Size of a header without a timestamp.
const int kNestedHeaderSize = sizeof(LengthType) + sizeof(MessageIdType)
    + sizeof(DataIdType);
//! Size of a header with a timestamp.
const int kHeaderSize = sizeof(TimestampType) + kNestedHeaderSize;
//! Length of a header without a timestamp.
const int kNestedHeaderLength = CEIL_DIV(kNestedHeaderSize,
                                         sizeof(AlignmentType));
//! Length of a header with a timestamp.
const int kHeaderLength = CEIL_DIV(kHeaderSize, sizeof(AlignmentType));

const int kMessageIdShift = 8*sizeof(LengthType);
const int kDataIdShift = 8*(sizeof(MessageIdType) + sizeof(LengthType));

//! Function type for timestamps.
typedef TimestampType (*TimestampFunctionType) ();

#ifdef __cplusplus
}  // namespace vartrace
#endif

#endif  // TRUNK_INCLUDE_VARTRACE_TRACETYPES_H_

