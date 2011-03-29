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
template <template <class> class CP, template <class> class LP, class AP>
VarTrace<CP, LP, AP>::VarTrace(int block_count, int block_size)
    : is_initialized_(false), block_count_(block_count),
      block_length_(block_size/sizeof(AlignmentType)) {
  Initialize();
}

template <template <class> class CP, template <class> class LP, class AP>
void VarTrace<CP, LP, AP>::Initialize() {
  // check for double initialization
  if (is_initialized_) { return; }
  // try to allocate storage, if success init success
  int allocated_length = 0;
  data_ = this->Allocate(block_count_*block_length_, &allocated_length);
  if (data_) {
    is_initialized_ = true;
    if (allocated_length != block_count_*block_length_) {
      block_length_ = allocated_length/block_count_;
    }
  }
}
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_VARTRACE_INL_H_
