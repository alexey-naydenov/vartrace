/* vartrace-inl.h
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

/*! \file vartrace-inl.h 
  Function implementations for vartrace class. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
#define TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_

#include <cstddef>
#include <cstring>
#include <algorithm>

namespace vartrace {

namespace internal {
const unsigned kInitialSubtraceDepth = 8;
}  // namespace internal

#define VAR_TRACE_TEMPLATE                              \
  template <class LL, template <class> class CP,        \
            template <class> class LP, class AP>

VAR_TRACE_TEMPLATE
VarTrace<LL, CP, LP, AP>::VarTrace()
    : is_initialized_(false) {
}

VAR_TRACE_TEMPLATE
VarTrace<LL, CP, LP, AP>::VarTrace(std::size_t trace_size,
                                   std::size_t block_count)
    : is_initialized_(false), is_top_level_(1), current_index_(0),
      get_timestamp_(IncrementalTimestamp), real_timestamp_(get_timestamp_) {
  block_count_ = FloorPower2(block_count);
  block_length_ = FloorPower2(trace_size/sizeof(AlignmentType)/block_count_);
  log2_block_length_ = CeilLog2(block_length_);
  trace_length_ = block_count_*block_length_;
  // check parameters and allocate memory
  Initialize();
}

VAR_TRACE_TEMPLATE
void VarTrace<LL, CP, LP, AP>::Initialize() {
  Lock guard(*this);
  // check for double initialization
  if (is_initialized_) {return;}
  // check block count size
  if (block_count_ < internal::kMinBlockCount) {return;}
  // try to allocate storage
  subtrace_header_positions_.reserve(internal::kInitialSubtraceDepth);
  message_end_indices_ = new int[block_count_];
  data_ = this->Allocate(trace_length_);
  if (subtrace_header_positions_.capacity() > 0 && message_end_indices_
      && data_) {
    is_initialized_ = true;
    // init blocks description variables
    index_mask_ = (trace_length_) - 1;
    message_end_indices_[0] = 0; // start position of the cursor
    for (unsigned i = 1; i != block_count_; ++i) {
      message_end_indices_[i] = -1;
    }
  }
}

VAR_TRACE_TEMPLATE
VarTrace<LL, CP, LP, AP>::~VarTrace() {
  if (is_initialized_) {
    delete[] message_end_indices_;
  }
}

VAR_TRACE_TEMPLATE
void VarTrace<LL, CP, LP, AP>::IncrementCurrentIndex() {
  current_index_ = (current_index_ + 1) & index_mask_;
}

VAR_TRACE_TEMPLATE
int VarTrace<LL, CP, LP, AP>::NextIndex(int index) {
  return (index + 1) & index_mask_;
}

VAR_TRACE_TEMPLATE
int VarTrace<LL, CP, LP, AP>::NextBlock(int block_index) {
  return (block_index + 1) % block_count_;
}

VAR_TRACE_TEMPLATE
void VarTrace<LL, CP, LP, AP>::CreateHeader(MessageIdType message_id,
                                            DataIdType data_id,
                                            unsigned object_size) {
  // if not top level then timestamp will be overwritten
  data_[current_index_] = (get_timestamp_)();
  current_index_ = (current_index_ + is_top_level_) & index_mask_;
  // write object size, message and data ids
  FormDescription(message_id, data_id, object_size, current_index_);
  IncrementCurrentIndex();
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::Log(HiddenLogLevel log_level,
                                   MessageIdType message_id, const T &value) {
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::Log(LL log_level,
                                   MessageIdType message_id, const T &value) {
  DoLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
        DataType2Int<T>::id, 1);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::LogPointer(HiddenLogLevel log_level,
                                          MessageIdType message_id,
                                          const T *value, unsigned length) {
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::LogPointer(LL log_level,
                                          MessageIdType message_id,
                                          const T *value, unsigned length) {
  LogPointerHelper(message_id, value, typename CopyTraits<T>::CopyCategory(),
                   length);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::LogPointerHelper(
    MessageIdType message_id, const T *value, const SelfCopyTag &copy_tag,
    unsigned length) {
  DoLog(message_id, value, copy_tag, DataType2Int<T>::id, length);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::LogPointerHelper(
    MessageIdType message_id, const T *value, const SizeofCopyTag &copy_tag,
    unsigned length) {
  DoLog(message_id, value, SizeofCopyTag(), DataType2Int<T>::id, length);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                     const AssignmentCopyTag &copy_tag,
                                     DataIdType data_id, unsigned length) {
  Lock guard(*this);
  CreateHeader(message_id, data_id, sizeof(T));
  data_[current_index_] = *value;
  IncrementCurrentIndex();
  UpdateBlock();
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                     const SizeofCopyTag &copy_tag,
                                     DataIdType data_id, unsigned length) {
  assert(current_index_ < trace_length_);
  Lock guard(*this);
  unsigned object_size = length*sizeof(T);
  CreateHeader(message_id, data_id, object_size);
  // check if data fits in space left in trace
  if ((trace_length_ - current_index_)
      *sizeof(AlignmentType) > object_size) {
    // copy using one function call
    std::memcpy(&(data_[current_index_]), value, object_size);
    // increment index
    current_index_ += RoundSize(object_size);
    current_index_ = current_index_ & index_mask_;
  } else {
    int copied_size = 0;
    // copy till the end of the trace
    int size_to_copy =
        (trace_length_ - current_index_)
        *sizeof(AlignmentType);
    std::memcpy(&(data_[current_index_]), value, size_to_copy);
    copied_size = size_to_copy;
    // copy rest of data to the begging of the trace buffer
    size_to_copy = object_size - copied_size;
    std::memcpy(&(data_[0]), reinterpret_cast<const uint8_t *>(value)
                + copied_size, size_to_copy);
    current_index_ = RoundSize(size_to_copy);
  }
  UpdateBlock();
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<LL, CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                     const SelfCopyTag &copy_tag,
                                     DataIdType data_id, unsigned length) {
  Lock guard(*this);
  BeginSubtrace(message_id);
  for (std::size_t i = 0; i < length; ++i) {
    value[i].LogItself(this);
  }
  EndSubtrace();
}

VAR_TRACE_TEMPLATE
unsigned VarTrace<LL, CP, LP, AP>::DumpInto(void *buffer, unsigned size) {
  Lock guard(*this);
  if (!is_top_level_) {
    // trace can not be parsed because subtrace size is written when
    // subtrace is closed
    return 0;
  }
  // start copying from the end of the next block
  unsigned current_block = current_index_ >> log2_block_length_;
  int copy_from = message_end_indices_[NextBlock(current_block)];
  // if end index of the next block is -1 then the trace was not
  // filled even once, copy from index 0
  if (copy_from < 0) { copy_from = 0; }
  int copy_to = message_end_indices_[current_block];
  // size of data copied in bytes
  int copied_size = 0;
  // check if block being copied wraps around
  if (copy_from > copy_to) { // wrapping
    // check the part before end of trace fits in buffer
    if ((block_length_*block_count_ - copy_from)
        *sizeof(AlignmentType) <= size) { // copy all from copy_from till end
      copied_size = (block_length_*block_count_ - copy_from)
          *sizeof(AlignmentType);
      std::memcpy(buffer, &(data_[copy_from]), copied_size);
       // try to c opy from index 0 till copy_to
       int leftover_to_copy = 0;
       if (copy_to*sizeof(AlignmentType) <= size - copied_size) {
         // everything fits
         leftover_to_copy = copy_to*sizeof(AlignmentType);
       } else { // only some part form 0 fits
         leftover_to_copy = size - copied_size;
       }
       std::memcpy(static_cast<uint8_t *>(buffer) + copied_size,
              &(data_[0]), leftover_to_copy);
       copied_size += leftover_to_copy;
    } else { // copy as much as possible from copy_from till end
      copied_size = size;
      std::memcpy(buffer, &(data_[copy_from]), copied_size);
    }
  } else { // no wrapping
    // copy at most size bytes
    if ((copy_to - copy_from)*sizeof(AlignmentType) <= size) {
      copied_size = (copy_to - copy_from)*sizeof(AlignmentType);
    } else {
      copied_size = size;
    }
    std::memcpy(buffer, &(data_[copy_from]), copied_size);
  }
  return copied_size;
}  // function DumpInto

VAR_TRACE_TEMPLATE
void VarTrace<LL, CP, LP, AP>::SetTimestampFunction(
    TimestampFunctionType timestamp_function) {
  assert(timestamp_function != 0);
  Lock guard(*this);
  real_timestamp_ = timestamp_function;
  if (is_top_level_) {
    get_timestamp_ = real_timestamp_;
  }
}

VAR_TRACE_TEMPLATE
void VarTrace<LL, CP, LP, AP>::BeginSubtrace(MessageIdType subtrace_id) {
  Lock guard(*this);
  // create temporary subtrace header and store its position
  subtrace_header_positions_.push_back(current_index_);
  CreateHeader(subtrace_id, 0, 0);
  // switch to subtrace state
  is_top_level_ = 0;
  // subtrace has no timestamp so replace by cheaper function
  get_timestamp_ = ZeroTimestamp;
}  // function BeginSubtrace

VAR_TRACE_TEMPLATE void VarTrace<LL, CP, LP, AP>::EndSubtrace() {
  Lock guard(*this);
  if (subtrace_header_positions_.empty()) {
    return;
  }
  unsigned subtrace_description_index = subtrace_header_positions_.back();
  subtrace_header_positions_.pop_back();
  // switch state to top level if necessary
  if (subtrace_header_positions_.empty()) {
    is_top_level_ = 1;
    get_timestamp_ = real_timestamp_;
    // top level adds timestamp
    subtrace_description_index = NextIndex(subtrace_description_index);
  }
  unsigned subtrace_start_index = NextIndex(subtrace_description_index);
  unsigned written_length = 0;
  if (current_index_ < subtrace_start_index) {
    // trace buffer wrapped around
    written_length = trace_length_ - subtrace_start_index
        + current_index_;
  } else {
    // no wrapping happened
    written_length = current_index_ - subtrace_start_index;
  }
  // change size field of subtrace header
  data_[subtrace_description_index] |= written_length*sizeof(AlignmentType);
  UpdateBlock(); // in case of empty subtrace
}  //function EndSubtrace
}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
