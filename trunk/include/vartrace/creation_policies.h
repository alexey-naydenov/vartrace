//! \file creation_policies.h

// Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//!  \brief Creation policies.

#ifndef TRUNK_INCLUDE_VARTRACE_CREATION_POLICIES_H_
#define TRUNK_INCLUDE_VARTRACE_CREATION_POLICIES_H_

#include <boost/shared_ptr.hpp>

namespace vartrace {

namespace internal {
const unsigned kDefaultBlockCount = 8;
const unsigned kDefaultTraceSize = 0x1000;
const unsigned kMinBlockCount = 4;
} // internal namespace

template <class T> struct SharedPtrCreator {
 public:
  typedef boost::shared_ptr<T> Handle;
  static Handle Create(std::size_t trace_size = internal::kDefaultTraceSize,
                       std::size_t block_count = internal::kDefaultBlockCount,
                       AlignmentType *storage = NULL) {
    return Handle(new T(trace_size, block_count, storage));
  }
 protected:
  ~SharedPtrCreator() {}
};

template <class T> struct SingletonCreator {
 public:
  typedef boost::shared_ptr<T> Handle;
  static Handle Create(std::size_t trace_size = internal::kDefaultTraceSize,
                       std::size_t block_count = internal::kDefaultBlockCount,
                       AlignmentType *storage = NULL) {
    if (!instance_) {
      instance_.reset(new T(trace_size, block_count, storage));
    }
    return instance_;
  }
 private:
  ~SingletonCreator() {}
  static Handle instance_;
};
template<class T> typename SingletonCreator<T>::Handle
SingletonCreator<T>::instance_;

template <class T> struct ValueCreator {
 public:
  typedef T Handle;
  static Handle Create(std::size_t trace_size = internal::kDefaultTraceSize,
                       std::size_t block_count = internal::kDefaultBlockCount,
                       AlignmentType *storage = NULL) {
    return T(trace_size, block_count, storage);
  }

 protected:
  ~ValueCreator() {}
};

}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_CREATION_POLICIES_H_
