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
VarTrace<CP, LP, AP>::VarTrace(int block_count, int block_size)
    : is_initialized_(false), block_count_(block_count),
      block_length_(block_size/sizeof(AlignmentType)) {
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
  data_ = this->Allocate(block_count_*block_length_, &allocated_length);
  if (data_ && allocated_length >= block_count_*kMinBlockLength) {
    is_initialized_ = true;
    // if less then required memory was allocated then decrease
    // block_length
    if (allocated_length != block_count_*block_length_) {
      block_length_ = allocated_length/block_count_;
    }
    // init trace description variables
    first_block_ = 0;
    last_block_ = 0;
    // init blocks description variables
    block_first_indices_ = boost::shared_array<int>(new int[block_count_]);
    block_last_indices_ = boost::shared_array<int>(new int[block_count_]);
    block_heads_ = boost::shared_array<int>(new int[block_count_]);
    block_tails_ = boost::shared_array<int>(new int[block_count_]);
    for (int i = 0; i < block_count_; ++i) {
      block_first_indices_[i] = i*block_length_;
      block_last_indices_[i] = (i+1)*block_length_ - 1;
      block_heads_[i] = 0;
      block_tails_[i] = 0;
    }
  }
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
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
