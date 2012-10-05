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

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <vartrace/copytraits.h>
#include <vartrace/tracetypes.h>
#include <vartrace/datatypeid.h>
#include <vartrace/datatraits.h>
#include <vartrace/policies.h>
#include <vartrace/vartrace-impl.h>

#include <cstring>
#include <cassert>
#include <vector>

namespace vartrace {

/*! Calculates the number of AlingmentType elements required to store
 *  an object. */
template <typename T> unsigned aligned_size() {
  return CEIL_DIV(sizeof(T), sizeof(AlignmentType));
}

struct LowestLogLevel {
};

struct InfoLogLevel : public LowestLogLevel {
};

struct ErrorLogLevel : public InfoLogLevel {
};

/*! Class for variable trace objects. */
// size of a block must be bigger then sizeof of biggest type + 8
// logging array or class must be smaller then block size
template <
  class LL = InfoLogLevel,
  template <class> class CP = SharedPtrCreator, // creation policy
  template <class> class LP = SingleThreaded, // locking policy
  class AP = SharedArrayAllocator // storage allocation policy
  >
class VarTrace
    : public CP< VarTrace<LL, CP, LP, AP> >,
      public LP< VarTrace<LL, CP, LP, AP> >,
      public AP {
 public:
  void Initialize();
  ~VarTrace();

  //! Returns true after memory allocation.
  bool is_initialized() const {return is_initialized_;}
  //! Number of memory blocks used to store trace.
  unsigned block_count() const {return block_count_;}
  //! Approximate size of each block in bytes.
  unsigned block_size() const {
    return sizeof(AlignmentType)*block_length_;
  }
  //! Check if the trace object can log data.
  bool can_log() const {return can_log_;}
  //! Store a variable value in the trace.
  /*! 
   */
  template <typename T> void Log(LowestLogLevel log_level,
                                 MessageIdType message_id, const T &value);

  template <typename T> void Log(LL log_level,
                                 MessageIdType message_id, const T &value);
  
  template <typename T> void LogPointer(MessageIdType message_id,
                                        const T *value, unsigned length = 1);

  //! Copy trace information into a buffer.
  /*! \note Can not be called if there is an open subtrace.
   */
  unsigned DumpInto(void *buffer, unsigned size);

  typename VarTrace<LL, CP, LP, AP>::Pointer
  CreateSubtrace(MessageIdType subtrace_id);

 private:
  //! Disabled default constructor.
  VarTrace();
  // allow creation policy access to constructors
  friend class CP< VarTrace<LL, CP, LP, AP> >;
  //! Conviniece typedef for locking.
  typedef typename LP< VarTrace<LL, CP, LP, AP> >::Lock Lock;
  //! Create a new trace with the given number of blocks and block size.
  /*! \note The constructor should not be called directly, use Create
    function provided by creation policy.
   */
  VarTrace(int log2_count, int log2_size);
  //! Subtrace constructor.
  explicit VarTrace(VarTrace<LL, CP, LP, AP> *ancestor);
  //! Calback to ancestor to indicate the subtrace destruction.
  void SubtraceDestruction(unsigned subtrace_current_index);
  //! Store object using memcpy.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
      unsigned data_id, unsigned object_size);
  //! Store a variable using assignment.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const AssignmentCopyTag &copy_tag, unsigned data_id,
      unsigned object_size);
  //! Store a variable using multiple assignment.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const MultipleAssignmentsCopyTag &copy_tag, unsigned data_id,
      unsigned object_size);
  //! Class stores itself in a subtrace.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const SelfCopyTag &copy_tag, unsigned data_id,
      unsigned object_size);

  //! Increment position for the next write.
  void IncrementCurrentIndex();
  //! Next wrapped around index.
  int NextIndex(int index);
  //! Previous wrapped index.
  int PreviousIndex(int index);
  //! Next block index.
  int NextBlock(int block_index);
  //! Write message header.
  void CreateHeader(MessageIdType message_id, unsigned data_id,
                    unsigned object_size);
  bool is_initialized_; /*!< True after memory allocation. */
  bool is_nested_; /*!< True if trace object is not top level one. */
  bool can_log_; /*!< True if the object can write in its trace */
  //! Stores position of a subtrace to store its size after destruction.
  unsigned subtrace_start_index_;
  unsigned log2_block_count_; /*!< Log base 2 of number of blocks. */
  unsigned log2_block_length_; /*!< Log base 2 of block length. */
  unsigned block_count_; /*!< Total number of blocks, must be power of 2. */
  unsigned block_length_; /*!< Length of each block in AlignmentType units. */
  unsigned index_mask_; /*!< Restricts array index to the range 0...2^n. */
  unsigned current_block_; /*!< Block currently being written into. */
  unsigned current_index_; /*!< Next array element to write to. */
  boost::shared_array<int> block_end_indices_; /*!< Block boundaries. */
  typename AP::StorageArrayType data_; /*!< Data array. */
  TimestampFunctionType get_timestamp_; /*!< Pointer to a timestamp function. */
  //! Pointer to the ancestor if the object is a subtrace.
  VarTrace<LL, CP, LP, AP> *ancestor_;
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
