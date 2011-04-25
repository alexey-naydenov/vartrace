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

#include <cstring>

namespace vartrace {

#define VAR_TRACE_TEMPLATE \
  template <template <class> class CP, template <class> class LP, class AP>

VAR_TRACE_TEMPLATE
VarTrace<CP, LP, AP>::VarTrace(int log2_count, int log2_length)
    : is_initialized_(false), is_nested_(false), can_log_(true),
      log2_block_count_(log2_count), log2_block_length_(log2_length),
      block_count_(1<<log2_block_count_), block_length_(1<<log2_block_length_),
      current_block_(0), current_index_(0),
      get_timestamp_(incremental_timestamp) {
  Initialize();
}

VAR_TRACE_TEMPLATE
VarTrace<CP, LP, AP>::VarTrace(VarTrace<CP, LP, AP> *ancestor)
    : is_initialized_(true), is_nested_(true), can_log_(true),
      log2_block_count_(ancestor->log2_block_count_),
      log2_block_length_(ancestor->log2_block_length_),
      block_count_(1<<log2_block_count_), block_length_(1<<log2_block_length_),
      current_block_(ancestor->current_block_),
      current_index_(ancestor->current_index_),
      get_timestamp_(ancestor->get_timestamp_),
      ancestor_(ancestor) {
  // get some info from ancestor
  index_mask_ = ancestor_->index_mask_;
  block_end_indices_ = ancestor_->block_end_indices_;
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::Initialize() {
  // check for double initialization
  if (is_initialized_) {return;}
  // check block count size
  if (block_count_ < kMinBlockCount) {return;}
  // try to allocate storage
  data_ = this->Allocate(block_count_*block_length_);
  if (data_) {
    is_initialized_ = true;
    // init blocks description variables
    index_mask_ = (block_count_*block_length_) - 1;
    block_end_indices_ = boost::shared_array<int>(new int[block_count_]);
    block_end_indices_[0] = 0; // start position of the cursor
    for (unsigned i = 1; i < block_count_; ++i) {
      block_end_indices_[i] = -1;
    }
  }
}

VAR_TRACE_TEMPLATE
VarTrace<CP, LP, AP>::~VarTrace() {
  if (is_nested_) {
    ancestor_->SubtraceDestruction(current_index_);
  }
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::IncrementCurrentIndex() {
  current_index_ = (current_index_ + 1) & index_mask_;
}

VAR_TRACE_TEMPLATE
int VarTrace<CP, LP, AP>::NextIndex(int index) {
  return (index + 1) & index_mask_;
}

VAR_TRACE_TEMPLATE
int VarTrace<CP, LP, AP>::PreviousIndex(int index) {
  // previous to the first block is the last one
  if (index == 0) {
    return block_count_ - 1;
  } else {
    return (index - 1) & index_mask_;
  }
}

VAR_TRACE_TEMPLATE
int VarTrace<CP, LP, AP>::NextBlock(int block_index) {
  return (block_index + 1) % block_count_;
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::CreateHeader(MessageIdType message_id,
                                        unsigned data_id,
                                        unsigned object_size) {
  // if top level record then add timestamp
  if (!is_nested_) {
    data_[current_index_] = get_timestamp_();
    IncrementCurrentIndex();
  }
  // write object size, message and data ids
  data_[current_index_] = object_size
      + (message_id << kMessageIdShift) + (data_id << kDataIdShift);
  IncrementCurrentIndex();
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::Log(MessageIdType message_id, const T &value) {
  DoLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
        DataTypeTraits<T>::DataTypeId, DataTypeTraits<T>::TypeSize);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                 const AssignmentCopyTag &copy_tag,
                                 unsigned data_id, unsigned object_size) {
  if (!can_log_) {return;}
  CreateHeader(message_id, data_id, object_size);
  data_[current_index_] = *value;
  IncrementCurrentIndex();
  current_block_ = current_index_ >> log2_block_length_;
  block_end_indices_[current_block_] = current_index_;
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                 const MultipleAssignmentsCopyTag &copy_tag,
                                 unsigned data_id, unsigned object_size) {
  if (!can_log_) {return;}
  CreateHeader(message_id, data_id, object_size);
  for (size_t i = 0; i < RoundSize(sizeof(T)); ++i) {
    data_[current_index_] = *(reinterpret_cast<const AlignmentType *>(value)
                              + i);
    IncrementCurrentIndex();
  }
  current_block_ = current_index_ >> log2_block_length_;
  block_end_indices_[current_block_] = current_index_;
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::DoLog(MessageIdType message_id, const T *value,
                                 const SizeofCopyTag &copy_tag,
                                 unsigned data_id, unsigned object_size) {
  if (!can_log_) {return;}
  CreateHeader(message_id, data_id, object_size);
  // check if data fits in space left in trace
  if ((block_count_*block_length_ - current_index_)*sizeof(AlignmentType)
      > object_size) {
    // copy using one function call
    std::memcpy(&data_[current_index_], value, object_size);
    // increment index
    current_index_ += RoundSize(object_size);
  } else {
    int copied_size = 0;
    // copy till the end of the trace
    int size_to_copy =
        (block_count_*block_length_ - current_index_)*sizeof(AlignmentType);
    std::memcpy(&data_[current_index_], value, size_to_copy);
    copied_size = size_to_copy;
    // copy rest of data to the begging of the trace buffer
    size_to_copy = object_size - copied_size;
    std::memcpy(&data_[0], reinterpret_cast<const uint8_t *>(value)
                + copied_size, size_to_copy);
    current_index_ = RoundSize(size_to_copy);
  }
  current_block_ = current_index_ >> log2_block_length_;
  block_end_indices_[current_block_] = current_index_;
}

VAR_TRACE_TEMPLATE
unsigned VarTrace<CP, LP, AP>::DumpInto(void *buffer, unsigned size) {
  // start copying from the end of the next block
  int copy_from = block_end_indices_[NextBlock(current_block_)];
  // if end index of the next block is -1 then the trace was not
  // filled even once, copy from index 0
  if (copy_from < 0) { copy_from = 0; }
  int copy_to = block_end_indices_[current_block_];
  // size of data copied in bytes
  int copied_size = 0;
  // check if block being copied wraps around
  if (copy_from > copy_to) { // wrapping
    // check the part before end of trace fits in buffer
    if ((block_length_*block_count_ - copy_from)*sizeof(AlignmentType)
        <= size) { // copy all from copy_from till end
      copied_size = (block_length_*block_count_ - copy_from)
          *sizeof(AlignmentType);
       memcpy(buffer, &data_[copy_from], copied_size);
       // try to c opy from index 0 till copy_to
       int leftover_to_copy = 0;
       if (copy_to*sizeof(AlignmentType) <= size - copied_size) {
         // everything fits
         leftover_to_copy = copy_to*sizeof(AlignmentType);
       } else { // only some part form 0 fits
         leftover_to_copy = size - copied_size;
       }
       memcpy(static_cast<uint8_t *>(buffer) + copied_size,
              &data_[0], leftover_to_copy);
       copied_size += leftover_to_copy;
    } else { // copy as much as possible from copy_from till end
      copied_size = size;
      memcpy(buffer, &data_[copy_from], copied_size);
    }
  } else { // no wrapping
    // copy at most size bytes
    if ((copy_to - copy_from)*sizeof(AlignmentType) <= size) {
      copied_size = (copy_to - copy_from)*sizeof(AlignmentType);
    } else {
      copied_size = size;
    }
    memcpy(buffer, &data_[copy_from], copied_size);
  }
  return copied_size;
}

VAR_TRACE_TEMPLATE typename VarTrace<CP, LP, AP>::Pointer
VarTrace<CP, LP, AP>::CreateSubtrace(MessageIdType subtrace_id) {
  // create header for the subtrace, subtrace data id = 0, size = 0 for now
  CreateHeader(subtrace_id, 0, 0);
  current_block_ = current_index_ >> log2_block_length_;
  block_end_indices_[current_block_] = current_index_;
  // block logging and subtrace creation and return pointer to subtrace object
  can_log_ = false;
  return typename VarTrace<CP, LP, AP>::Pointer(new VarTrace<CP, LP, AP>(this));
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::SubtraceDestruction(unsigned subtrace_index) {
  can_log_ = true;
  // calculate written size
  unsigned written_length = 0;
  if (subtrace_index < current_index_) {
    // trace buffer was wrapped around
    written_length = (block_count_*block_length_ - current_index_)
        + subtrace_index;
  } else {
    // no wrapping happenned
    written_length = subtrace_index - current_index_;
  }
  // update size of the message that contains subtrace
  data_[PreviousIndex(current_index_)] |= written_length*sizeof(AlignmentType);
  // update current index and current block
  current_index_ = subtrace_index;
  current_block_ = current_index_ >> log2_block_length_;
}
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
