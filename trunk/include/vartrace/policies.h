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

#include <vartrace/tracetypes.h>
#include <vartrace/vartrace-internal.h>

namespace vartrace {

//! No locking policy.
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

//! Class level locking.
template <class T> struct ClassLevelLockable {
 public:
  class Lock {
   public:
    Lock() {AcquireLock();}
    explicit Lock(const T &obj) {AcquireLock();}
    ~Lock() {ReleaseLock();}
   private:
    static boost::recursive_mutex mutex_;
    void AcquireLock() {mutex_.lock();}
    void ReleaseLock() {mutex_.unlock();}
  };
 protected:
  ~ClassLevelLockable() {}
};

template<class T> boost::recursive_mutex ClassLevelLockable<T>::Lock::mutex_;

//! Class level locking.
template <class T> struct ObjectLevelLockable {
 public:
  class Lock {
   public:
    explicit Lock(T &obj) {
      object_ = &obj;
      object_->mutex_.lock();
    }
    ~Lock() {object_->mutex_.unlock();}
   protected:
    T *object_;
  };
 protected:
  ~ObjectLevelLockable() {}
  boost::recursive_mutex mutex_;
};

static const unsigned kDefaultBlockCount = 8;
static const unsigned kDefaultTraceSize = 0x1000;
static const unsigned kMinBlockCount = 4;

template <class T> struct SharedPtrCreator {
 public:
  typedef boost::shared_ptr<T> Pointer;
  static Pointer Create(int trace_size = kDefaultTraceSize,
                        int block_count = kDefaultBlockCount) {
    return Pointer(new T(
        internal::CalculateLog2Count(trace_size, block_count),
        internal::CalculateLog2Length(trace_size, block_count)));
  }
 protected:
  ~SharedPtrCreator() {}
};

template <class T> struct NoLockSingletonCreator {
 public:
  typedef boost::shared_ptr<T> Pointer;
  static Pointer Create(int trace_size = kDefaultTraceSize,
                        int block_count = kDefaultBlockCount) {
    if (!instance_) {
      instance_.reset(new T(
          internal::CalculateLog2Count(trace_size, block_count),
          internal::CalculateLog2Length(trace_size, block_count)));
    }
    return instance_;
  }
 private:
  ~NoLockSingletonCreator() {}
  static Pointer instance_;
};
template<class T> typename NoLockSingletonCreator<T>::Pointer
NoLockSingletonCreator<T>::instance_;

template <class T> struct LockableSingletonCreator
    : public ClassLevelLockable< LockableSingletonCreator<T> > {
 public:
  typedef boost::shared_ptr<T> Pointer;
  static Pointer Create(int trace_size = kDefaultTraceSize,
                        int block_count = kDefaultBlockCount) {
    if (!instance_) {
      typename ClassLevelLockable< LockableSingletonCreator<T> >::Lock guard();
      if (!instance_) {
        instance_.reset(new T(
            internal::CalculateLog2Count(trace_size, block_count),
            internal::CalculateLog2Length(trace_size, block_count)));
      }
    }
    return instance_;
  }
 private:
  ~LockableSingletonCreator() {}
  static Pointer instance_;
};
template<class T> typename LockableSingletonCreator<T>::Pointer
LockableSingletonCreator<T>::instance_;


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

