/* vartrace.h
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

/*! \file vartrace.h 
  Template for a variable trace object. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_H_

#include <cstring>
#include <cassert>

#include "vartrace/copytraits.h"
#include "vartrace/tracetypes.h"
#include "vartrace/datatypeid.h"
#include "vartrace/datatraits.h"
#include "vartrace/policies.h"

namespace vartrace {

//! Divide 2 ints and round up the result.
#define CEIL_DIV(num, div) ((num) + (div) - 1)/(div)

/*! Calculates the number of AlingmentType elements required to store
 *  an object. */
template <typename T> unsigned aligned_size() {
  return CEIL_DIV(sizeof(T), sizeof(AlignmentType));
}

TimestampType incremental_timestamp() {
    static TimestampType timestamp = 0;
    return timestamp++;
}

/*! Class for variable trace objects. */
template <
  template <class> class CP = NewCreator,
  template <class> class LP = SingleThreadedLocker,
  class AP = NewAllocator
  >
class VarTrace
    : public CP< VarTrace<CP, LP, AP> >,
      public LP< VarTrace<CP, LP, AP> >,
      public AP {
 public:
  //! Shared pointer to the class, used for handling vartrace objects.
  //typedef boost::shared_ptr< VarTrace<CP, LP, AP> > Pointer;
  VarTrace(int block_count, int block_size) {}
 private:
  enum {
    //! Size of a header without a timestamp.
    NestedHeaderSize = (sizeof(LengthType) + sizeof(MessageIdType)
                        + sizeof(DataIdType)),
    //! Size of a header with a timestamp.
    HeaderSize = (sizeof(TimestampType) + sizeof(LengthType)
                  + sizeof(MessageIdType) + sizeof(DataIdType)),
    //! Length of a header without a timestamp.
    NestedHeaderLength = CEIL_DIV(NestedHeaderSize, sizeof(AlignmentType)),
    //! Length of a header with a timestamp.
    HeaderLength = CEIL_DIV(HeaderSize, sizeof(AlignmentType))
  };
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
