/* vartrace-impl.h
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

/*! \file vartrace-impl.h 
  Structure for storing vartrace data. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_IMPL_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_IMPL_H_

#include <boost/scoped_array.hpp>

#include "vartrace/tracetypes.h"

namespace vartrace {
//! Data for vartrace object hierarchy.
template<class AP> struct VarTraceImplementation {
  unsigned log2_block_count_; /*!< Log base 2 of number of blocks. */
  unsigned log2_block_length_; /*!< Log base 2 of block length. */
  unsigned block_count_; /*!< Total number of blocks, must be power of 2. */
  unsigned block_length_; /*!< Length of each block in AlignmentType units. */
  unsigned index_mask_; /*!< Restricts array index to the range 0...2^n. */
  unsigned current_block_; /*!< Block currently being written into. */
  unsigned current_index_; /*!< Next array element to write to. */
  boost::scoped_array<int> block_end_indices_; /*!< Block boundaries. */
  typename AP::StorageArrayType data_; /*!< Data array. */
  TimestampFunctionType get_timestamp_; /*!< Pointer to a timestamp function. */
};
}  // namespace vartrace
#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_IMPL_H_
