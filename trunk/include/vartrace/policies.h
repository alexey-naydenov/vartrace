/* policies.h
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

/*! \file policies.h 
  Common policies for VarTrace. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_POLICIES_H_
#define TRUNK_INCLUDE_VARTRACE_POLICIES_H_

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>

#iclude "vartrace/tracetypes.h"

namespace vartrace {

static int kDefaultBlockCount = 3;
static int kDefaultBlockSize = 0x1000;

//! Object creation policy using new.
template <class T> struct CreateNewObject {
  static boost::shared_ptr<T> Create(int block_count = kDefaultBlockCount,
                                     int block_size = kDefaultBlockSize) {
    return boost::shared_ptr<T>(new T(block_count, block_size));
  }
 protected:
  ~CreateNewObject() {}
};

//! No lock policy.
template <class T> struct LockSingleThread {
 protected:
  ~LockSingleThread() {}
};

//! Policy to allocate log storage through new operator.
struct AllocateNewStorage {
 protected:
  ~AllocateNewStorage() {}
};
}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_POLICIES_H_

