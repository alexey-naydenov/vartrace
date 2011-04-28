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
#include <boost/thread.hpp>

#include <vector>

#include "vartrace/tracetypes.h"

namespace vartrace {

static const unsigned kDefaultBlockCount = 8;
static const unsigned kDefaultTraceSize = 0x1000;
static const unsigned kMinBlockCount = 4;

template <class T> struct SharedPtrCreator {
 public:
  typedef boost::shared_ptr<T> Pointer;
  static Pointer Create(int trace_size = kDefaultTraceSize,
                        int block_count = kDefaultBlockCount) {
    // find most significant bit of block_count that is 1
    int log2_count = 0;
    while ( (block_count >> log2_count) > 1) log2_count++;
    // block length not rounded down to 2^n
    int block_length = trace_size/sizeof(AlignmentType)/(1<<log2_count);
    // find most significant bit of block_length that is 1
    int log2_length = 0;
    while ((block_length >> log2_length) > 1) log2_length++;
    return Pointer(new T(log2_count, log2_length));
  }
 protected:
  ~SharedPtrCreator() {}
};

template <class T> struct SingletonCreator {
};

//! No locking.
template <class T> struct SingleThreaded {
 public:
  class Lock {
   public:
    Lock() {}
    explicit Lock(const T &obj) {}
  };
 protected:
  ~SingleThreaded() {}
};

//! Policy to allocate log storage through new operator.
struct SharedArrayAllocator {
 public:
  //! Type for storing trace data.
  typedef boost::shared_array<AlignmentType> StorageArrayType;
  //! Create new shared array of type AlingmentType.
  static StorageArrayType Allocate(int length) {
    return StorageArrayType(new AlignmentType[length]);
  }
 protected:
  ~SharedArrayAllocator() {}
};
}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_POLICIES_H_

