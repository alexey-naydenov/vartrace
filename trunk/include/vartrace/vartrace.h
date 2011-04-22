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
#include <vector>

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
// size of a block must be bigger then sizeof of biggest type + 8
// logging array or class must be smaller then block size
template <
  template <class> class CP = NewCreator, // creation policy
  template <class> class LP = NoLocker, // locking policy
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
  VarTrace(int log2_count, int log2_size);
  void Initialize();
  ~VarTrace();

  //! Returns true after memory allocation.
  bool is_initialized() const {return is_initialized_;}
  //! Number of memory blocks used to store trace.
  unsigned block_count() const {return block_count_;}
  //! Approximate size of each block in bytes.
  unsigned block_size() const {return sizeof(AlignmentType)*block_length_;}
  //! Check if the trace object can log data.
  bool can_log() const {return can_log_;}
  //! Store a variable value in the trace.
  /*! 
   */
  template <typename T> void Log(MessageIdType message_id, const T &value);

  //! Copy trace information into a buffer.
  /*! 
   */
  unsigned DumpInto(void *buffer, unsigned size);

  typename CP< VarTrace<CP, LP, AP> >::Pointer
  CreateSubtrace(MessageIdType subtrace_id);

 private:
  //! Subtrace constructor.
  VarTrace(typename CP< VarTrace<CP, LP, AP> >::Pointer ancestor);
  //! Calback for subtrace to indicate its destruction.
  void SubtraceDestruction(unsigned subtrace_index);
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
  unsigned log2_block_count_; /*!< Log base 2 of number of blocks. */
  unsigned log2_block_length_; /*!< Log base 2 of block length. */
  unsigned block_count_; /*!< Total number of blocks, must be power of 2. */
  unsigned block_length_; /*!< Length of each block in AlignmentType units. */
  unsigned index_mask_; /*!< Restricts array index to the range 0...2^n. */
  unsigned current_block_; /*!< Block currently being written into. */
  unsigned current_index_; /*!< Next array element to write to. */
  boost::shared_array<int> block_end_indices_; /*!< Block boundaries. */
  typename AP::StorageArrayType data_; /*!< Data storage. */
  TimestampFunctionType get_timestamp_; /*!< Pointer to a timestamp function. */
  //! Pointer to the ancestor of a subtrace.
  typename CP< VarTrace<CP, LP, AP> >::Pointer ancestor_;
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
