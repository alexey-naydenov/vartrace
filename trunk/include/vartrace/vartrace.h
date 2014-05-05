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
#include <vartrace/utility.h>
#include <vartrace/datatypeid.h>
#include <vartrace/policies.h>
#include <vartrace/log_level.h>

#include <cstring>
#include <cassert>
#include <vector>

namespace vartrace {

/*! Class for variable trace objects. */
// size of a block must be bigger then sizeof of biggest type + 8
// logging array or class must be smaller then block size
template <
  class LL = User5LogLevel,
  template <class> class CP = SharedPtrCreator, // creation policy
  template <class> class LP = SingleThreaded, // locking policy
  class AP = SharedArrayAllocator // storage allocation policy
  >
class VarTrace
    : public CP< VarTrace<LL, CP, LP, AP> >,
      public LP< VarTrace<LL, CP, LP, AP> >,
      public AP {
 public:
  typedef VarTrace * Pointer;

  VarTrace();
  //! Create a new trace with the given number of blocks and block size.
  /*! \note The constructor should not be called directly, use Create
    function provided by creation policy.
   */
  VarTrace(std::size_t trace_size, std::size_t block_count);
  void Initialize();
  ~VarTrace();

  //! Returns true after memory allocation.
  bool is_initialized() const { return is_initialized_; }
  //! Check if is in subtrace mode.
  bool is_subtrace() const { return is_top_level_ == 0; }
  //! Number of memory blocks used to store trace.
  unsigned block_count() const { return block_count_; }
  //! Approximate size of each block in bytes.
  unsigned block_size() const {
    return sizeof(AlignmentType)*block_length_;
  }

  //! Store a variable value in the trace.
  template <typename T>
  void Log(HiddenLogLevel log_level, MessageIdType message_id, const T &value);

  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const T &value);

  //! Empty log pointer specialization for suppressed log levels.
  template <typename T>
  void Log(HiddenLogLevel log_level, MessageIdType message_id, const T *value,
           unsigned length);
  //! Log an array of values.
  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const T *value,
           unsigned length);

  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const std::vector<T> &value);

  //! Copy trace information into a buffer.
  /*! \note Can not be called if there is an open subtrace.
   */
  unsigned DumpInto(void *buffer, unsigned size);

  void BeginSubtrace(MessageIdType subtrace_id);
  void EndSubtrace();

  void SetTimestampFunction(TimestampFunctionType timestamp_function);

 private:
  friend class CP< VarTrace<LL, CP, LP, AP> >;
  //! Conviniece typedef for locking.
  typedef typename LP< VarTrace<LL, CP, LP, AP> >::Lock Lock;

  //! Store object using memcpy.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
      unsigned length);
  //! Store a variable using assignment.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const AssignmentCopyTag &copy_tag, unsigned length);
  //! Class stores itself in a subtrace.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const SelfCopyTag &copy_tag, unsigned length);

  //! Store array using memcpy.
  template <typename T> void DoLogArray(
      MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
      unsigned length);
  //! Store array using self copy.
  template <typename T> void DoLogArray(
      MessageIdType message_id, const T *value, const SelfCopyTag &copy_tag,
      unsigned length);

  //! Update current block number and its end using current index.
  inline void UpdateBlock() {
    unsigned current_block = current_index_ >> log2_block_length_;
    message_end_indices_[current_block] = current_index_;
  }
  //! Form description word at given position.
  inline void FormDescription(MessageIdType message_id, DataIdType data_id,
                              unsigned object_size, unsigned position) {
    data_[position] = object_size + (message_id << kMessageIdShift)
        + (data_id << kDataIdShift);
  }
  //! Increment position for the next write.
  void IncrementCurrentIndex();
  //! Next wrapped around index.
  int NextIndex(int index);
  //! Next block index.
  int NextBlock(int block_index);
  //! Write message header.
  void CreateHeader(MessageIdType message_id, DataIdType data_id,
                    unsigned object_size);
  bool is_initialized_; //!< Set to true after memory allocation.
  unsigned is_top_level_;  //!< Set to 0 in the subtrace mode, 1 otherwise.
  std::vector<unsigned> subtrace_header_positions_;
  unsigned log2_block_length_; //!< Log2 of block length.
  unsigned block_count_; /*!< Total number of blocks, must be power of 2. */
  unsigned block_length_; /*!< Length of each block in AlignmentType units. */
  unsigned trace_length_; //! Length of the trace.
  unsigned index_mask_; /*!< Restricts array index to the range 0...2^n. */
  unsigned current_index_; /*!< Next array element to write to. */
  int *message_end_indices_; //!< Message boundaries.
  typename AP::StorageArrayType data_; /*!< Data array. */
  TimestampFunctionType get_timestamp_; /*!< Current timestamp function. */
  TimestampFunctionType real_timestamp_; /*!< Actual temestamp function. */
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
