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

namespace vartrace {

#define VAR_TRACE_TEMPLATE \
  template <template <class> class CP, template <class> class LP, class AP>

VAR_TRACE_TEMPLATE
VarTrace<CP, LP, AP>::VarTrace(int log2_count, int log2_length)
    : is_initialized_(false), is_nested_(false),
      log2_block_count_(log2_count), log2_block_length_(log2_length),
      block_count_(1<<log2_block_count_), block_length_(1<<log2_block_length_),
      current_block_(0), current_index_(0),
      get_timestamp_(incremental_timestamp) {
  Initialize();
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::Initialize() {
  // check for double initialization
  if (is_initialized_) { return; }
  // check block count size
  if (block_count_ < kMinBlockCount) { return; }
  // try to allocate storage
  int allocated_length = 0;
  data_ = this->Allocate(block_count_*block_length_);
  if (data_) {
    is_initialized_ = true;
    // init blocks description variables
    index_mask_ = (block_count_*block_length_) - 1;
    block_end_indices_ = boost::shared_array<int>(new int[block_count_]);
    for (int i = 0; i < block_count_; ++i) {
      block_end_indices_[i] = (i + 1)*block_length_;
    }
  }
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::IncrementIndex() {
  current_index_ = (current_index_ + 1) & index_mask_;
}

VAR_TRACE_TEMPLATE
void VarTrace<CP, LP, AP>::CreateHeader(MessageIdType message_id,
                                        unsigned data_id,
                                        unsigned object_size) {
  if (!is_nested_) {
    data_[current_index_] = get_timestamp_();
    IncrementIndex();
  }
  data_[current_index_] = (object_size << kSizeShift)
      + (message_id << kMessageIdShift) + data_id;
  IncrementIndex();
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::log(MessageIdType message_id, const T &value) {
  doLog(message_id, &value, typename CopyTraits<T>::CopyCategory(),
        DataTypeTraits<T>::DataTypeId, DataTypeTraits<T>::TypeSize);
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::doLog(MessageIdType message_id, const T *value,
                                 const SizeofCopyTag &copy_tag,
                                 unsigned data_id, unsigned object_size) {
}

VAR_TRACE_TEMPLATE template <typename T>
void VarTrace<CP, LP, AP>::doLog(MessageIdType message_id, const T *value,
                                 const AssignmentCopyTag &copy_tag,
                                 unsigned data_id, unsigned object_size) {
  CreateHeader(message_id, data_id, object_size);
  data_[current_index_] = reinterpret_cast<AlignmentType>(*value);
  block_end_indices_[current_index_ >> log2_block_length_] = current_index_;
  IncrementIndex();
}
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
