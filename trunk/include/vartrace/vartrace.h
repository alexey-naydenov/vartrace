/* vartrace.h

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

/*! \file vartrace.h 
  Contains main template class.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_H_

#include <vartrace/copytraits.h>
#include <vartrace/tracetypes.h>
#include <vartrace/utility.h>
#include <vartrace/datatypeid.h>
#include <vartrace/policies.h>
#include <vartrace/log_level.h>

#include <cstring>
#include <cassert>
#include <string>
#include <vector>

namespace vartrace {

namespace internal {
//! Number of blocks used to split a trace by default.
const unsigned kDefaultBlockCount = 8;
//! Minimum number of blocks that a trace can be split into.
const unsigned kMinBlockCount = 4;
//! Default trace size.
const unsigned kDefaultTraceSize = 0x1000;
} // namespace internal

//! Guard class to ensure that a subtrace is opened and closed properly.
/*! Every call of BeginSubtrace() must be matched by a closing call to
  EndSubtrace(). This class starts a subtrace in the constructor and
  closes it in the destructor.
*/
template <class T> class SubtraceGuard {
 public:
  //! Begin subtrace and store trace pointer.
  SubtraceGuard(T *trace, MessageIdType subtrace_id) : trace_(trace) {
    trace_->BeginSubtrace(subtrace_id);
  }
  //! End subtrace.
  ~SubtraceGuard() {
    trace_->EndSubtrace();
  }
 private:
  //! Pointer to trace in which subtrace was created.
  T *trace_;
};

//! Class that stores values and timestamp in a circular buffer.
template <
  class LL = User5LogLevel, // log level selection
  template <class> class LP = SingleThreaded // locking policy
  >
class VarTrace
    : public LP< VarTrace<LL, LP> > {
 public:
  //! Create a new trace with the given number of blocks and block size.
  /*! Last parameter can be used to specify preallocated storage space.
   */
  VarTrace(std::size_t trace_size = internal::kDefaultTraceSize,
           std::size_t block_count = internal::kDefaultBlockCount,
           void *storage = NULL);
  //! Free memory.
  ~VarTrace();

  //! Returns true after memory allocation.
  bool is_initialized() const { return is_initialized_; }
  //! Check if is in subtrace mode.
  bool is_subtrace() const { return is_top_level_ == 0; }
  //! Number of memory blocks used to store trace.
  unsigned block_count() const { return block_count_; }
  //! Size of each block in bytes.
  unsigned block_size() const {
    return sizeof(AlignmentType)*block_length_;
  }

  //! Empty Log overload used for messages below log level.
  template <typename T>
  void Log(HiddenLogLevel log_level, MessageIdType message_id, const T &value);
  //! Logging function used for objects, PODs and arrays.
  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const T &value);
  //! Empty array Log overload for suppressed log levels.
  template <typename T>
  void Log(HiddenLogLevel log_level, MessageIdType message_id, const T *value,
           unsigned length);
  //! Array logging function.
  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const T *value,
           unsigned length);
  //! Log overload for vector.
  template <typename T>
  void Log(LL log_level, MessageIdType message_id, const std::vector<T> &value);
  //! Log overload for std::string.
  void Log(LL log_level, MessageIdType message_id, const std::string &value);

  //! Copy trace information into a buffer.
  /*! \note Can not be called if there is an open subtrace.
   */
  unsigned DumpInto(void *buffer, unsigned size);
  //! Start subtrace.
  void BeginSubtrace(MessageIdType subtrace_id);
  //! End subtrace.
  void EndSubtrace();

  //! Assign timestamp function.
  void SetTimestampFunction(TimestampFunctionType timestamp_function);

 private:
  //! Convenience typedef for locking.
  typedef typename LP< VarTrace<LL, LP> >::Lock Lock;

  //! Disabled copy constructor.
  VarTrace(const VarTrace &);
  //! Disabled assignment.
  VarTrace operator=(const VarTrace &);

  //! Initialize memory and counters.
  void Initialize();

  //! Overloading of actual logging function that uses memcpy.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
      unsigned length);
  //! Overloading of logging function that copies through assignment.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const AssignmentCopyTag &copy_tag, unsigned length);
  //! Overloading of logging function that calls class method for logging.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const SelfCopyTag &copy_tag, unsigned length);
  //! Overloading of logging function that calls custom function for copying.
  template <typename T> void DoLog(
      MessageIdType message_id, const T *value,
      const CustomCopyTag &copy_tag, unsigned length);

  //! Force array copy through memcpy for types that copied through assignment.
  template <typename T> void DoLogArray(
      MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
      unsigned length);
  //! Store array of objects that have custom logging member function.
  template <typename T> void DoLogArray(
      MessageIdType message_id, const T *value, const SelfCopyTag &copy_tag,
      unsigned length);
  //! Store array of objects that have custom logging function.
  template <typename T> void DoLogArray(
      MessageIdType message_id, const T *value, const CustomCopyTag &copy_tag,
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
  inline void IncrementCurrentIndex();
  //! Next wrapped around index.
  inline uint_fast32_t NextIndex(uint_fast32_t index);
  //! Next block index.
  inline uint_fast32_t  NextBlock(uint_fast32_t block_index);
  //! Write message header.
  inline void CreateHeader(MessageIdType message_id, DataIdType data_id,
                           unsigned object_size);
  bool is_initialized_; //!< Set to true after memory allocation.
  uint_fast8_t is_top_level_;  //!< Set to 0 in the subtrace mode, 1 otherwise.
  bool is_memory_managed_; //!< Is memory allocated or provided.
  //! Last header positions inside each trace block.
  std::vector<unsigned> subtrace_header_positions_;
  uint_fast16_t log2_block_length_; //!< Log2 of block length.
  uint_fast16_t block_count_; //!< Total number of blocks, must be power of 2.
  uint_fast32_t block_length_; //!< Length of each block in AlignmentType units.
  uint_fast32_t trace_length_; //!< Length of the trace.
  uint_fast32_t index_mask_; //!< Restricts array index to the range 0...2^n.
  uint_fast32_t current_index_; //!< Next array element to write to.
  int *message_end_indices_; //!< Message boundaries.
  AlignmentType *data_; //!< Data array.
  TimestampFunctionType get_timestamp_; //!< Current timestamp function.
  TimestampFunctionType real_timestamp_; //!< Actual temestamp function.
};
}  // vartrace

#include "vartrace/vartrace-inl.h"

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_H_
