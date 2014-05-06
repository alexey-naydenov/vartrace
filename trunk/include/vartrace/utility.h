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

#include <cassert>
#include <cstdlib>
#include <utility>

#include <vartrace/tracetypes.h>

namespace vartrace {
//! Divide 2 ints and round up the result.
#define CEIL_DIV(num, div) ((num) + (div) - 1)/(div)

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

//! Constant to get rid of magic numbers.
const unsigned kBitsPerByte = 8;
//! Position of message id inside header.
const unsigned kMessageIdShift = kBitsPerByte*sizeof(LengthType);
//! Position of data id inside header.
const unsigned kDataIdShift = kBitsPerByte*(sizeof(MessageIdType)
                                            + sizeof(LengthType));

//! Default timestamp function that returns consecutive integers.
TimestampType IncrementalTimestamp();
//! Function used for subtrace timestamp, returns 0.
TimestampType ZeroTimestamp();

//! Round size to AlignmentType boundary.
inline unsigned RoundSize(unsigned size) {
  return CEIL_DIV(size, sizeof(AlignmentType));
}

//! Return pointer >= then given aligned to AlignemntType boundary and shift.
inline std::pair<AlignmentType *, std::size_t> AlignPointer(void *ptr) {
  std::size_t addr = reinterpret_cast<std::size_t>(ptr);
  std::size_t mask = sizeof(AlignmentType) - 1;
  std::size_t aligned_addr = (addr & (~mask))
      + sizeof(AlignmentType)*((addr & mask) > 0);
  return std::make_pair(reinterpret_cast<AlignmentType *>(aligned_addr),
                        aligned_addr - addr);
}

//! Fill an integer with ones from MSB to 0.
template <typename T> T FillWithOnes(T value) {
  for (unsigned shift = 1; shift != 8*sizeof(value); shift <<= 1) {
    value |= value >> shift;
  }
  return value;
}

//! Calculate power of 2 bigger or equal to given value.
/*! \note Not defined for <= 0 as there is no n such that 2^n <= 0. */
template <typename T> T CeilPower2(T value) {
  assert(value > 0);
  return FillWithOnes(value - 1) + 1;
}

//! Calculate power of 2 smaller or equal to given value.
/*! \note Not defined for <= 0 as there is no n such that 2^n <= 0. */
template <typename T> T FloorPower2(T value) {
  assert(value > 0);
  return FillWithOnes(value >> 1) + 1;
}

//! Calculate ceiling of log2 of value.
/*! \note Not defined for <= 0 as there is no n such that 2^n <= 0. */
template <typename T> unsigned CeilLog2(T value) {
  assert(value > 0);
  unsigned power = 0;
  value--;
  while (value > 0) {
    power++;
    value >>= 1;
  }
  return power;
}

//! Calculate floor of log2 of value.
/*! \note Not defined for <= 0 as there is no n such that 2^n <= 0. */
template <typename T> unsigned FloorLog2(T value) {
  assert(value > 0);
  unsigned power = 0;
  value >>= 1;
  while (value > 0) {
    power++;
    value >>= 1;
  }
  return power;
}

}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_UTILITY_H_
