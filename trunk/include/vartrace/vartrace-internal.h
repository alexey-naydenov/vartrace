/* vartrace-internal.h
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

/*! \file vartrace-internal.h 
  Some function for internal use. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_INTERNAL_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_INTERNAL_H_

#include <vartrace/tracetypes.h>

namespace vartrace {

namespace internal {
TimestampType IncrementalTimestamp();
unsigned CalculateLog2Count(unsigned trace_size, unsigned block_count);
unsigned CalculateLog2Length(unsigned trace_size, unsigned block_count);
}  // namespace internal

}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INTERNAL_H_
