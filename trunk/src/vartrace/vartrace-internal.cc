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

#include <vartrace/vartrace-internal.h>

namespace vartrace {
namespace internal {
//! Store incremental timestamp.
TimestampType incremental_timestamp = 0;
TimestampType IncrementalTimestamp() {
  return incremental_timestamp++;
}
}  // namespace internal
}  // namespace vartrace
