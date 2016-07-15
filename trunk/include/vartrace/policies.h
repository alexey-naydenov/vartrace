/* policies.h
   
   Copyright (C) 2011 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
   
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

  Locking policies passed as template argument to vartrace constructor.

  Classes defined here are used to customize trace object at compile
  time. They allow to specify behavior in multithreaded
  environment. The default policy is to do nothing that is no locking
  is done on trace access.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_POLICIES_H_
#define TRUNK_INCLUDE_VARTRACE_POLICIES_H_

namespace vartrace {
//! No locking policy.
template <class T> struct SingleThreaded {
 public:
  //! Actual lock for single threaded policy.
  class Lock {
   public:
    //! Default lock constructor, empty.
    Lock() {}
    //! Lock for particular object, empty.
    explicit Lock(const T &obj) {}
  };
 protected:
  ~SingleThreaded() {}
};

#ifdef HAS_STDLIB_THREADS
//! Lock object access using C++11 mutex.
template <class T> struct StdLocking {
 public:
  //! Mutex guard
};
#endif // HAS_STDLIB_THREADS

}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_POLICIES_H_

