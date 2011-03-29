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
  template <class> class CP = NewCreator, // creation policy
  template <class> class LP = SingleThreadedLocker, // locking policy
  class AP = NewAllocator // storage allocation policy
  >
class VarTrace
    : public CP< VarTrace<CP, LP, AP> >,
      public LP< VarTrace<CP, LP, AP> >,
      public AP {
 public:
  //! Create a new trace with the given number of blocks and block size.
  /*! \note The constructor should not be called directly, use Create
   *  function provided by creation policy.
   */
  VarTrace(int block_count, int block_size);
  void Initialize();

  bool is_initialized() const {
    return is_initialized_;
  }

 private:
  bool is_initialized_;
  int block_count_;
  int block_length_;
  typename AP::StorageArrayType data_;
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
