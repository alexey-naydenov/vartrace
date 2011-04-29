/* vartrace-internal.cc
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

/*! \file vartrace-internal.cc 
  Implementation of some internal functions. 
*/

#include "vartrace/vartrace-internal.h"

namespace vartrace {

namespace internal {

TimestampType incremental_timestamp = 0;
TimestampType IncrementalTimestamp() {
  return incremental_timestamp++;
}

unsigned CalculateLog2Count(unsigned trace_size, unsigned block_count) {
  // find most significant bit of block_count that is 1
  unsigned log2_count = 0;
  while ( (block_count >> log2_count) > 1) log2_count++;
  return log2_count;
}

unsigned CalculateLog2Length(unsigned trace_size, unsigned block_count) {
  unsigned log2_count = CalculateLog2Count(trace_size, block_count);
  unsigned block_length = trace_size/sizeof(AlignmentType)/(1<<log2_count);
  // find most significant bit of block_length that is 1
  unsigned log2_length = 0;
  while ((block_length >> log2_length) > 1) log2_length++;
  return log2_length;
}

}  // namespace internal

}  // namespace vartrace
