/* datatypeid.h
 
   Copyright (C) 2010 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
   
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

/*! \file datatypeid.h 
  
  Map type to a numeric value.

  During logging type information is stored automatically in the output
  stream. This file contains templates that map standard C types to
  integer values. The templates have two parameters: typename and
  length. This enables logging static arrays without having to specify
  array size and type.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_
#define TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_

#include <vartrace/type_codes.h>

namespace vartrace {

//! Id assigned by default to unknown types.
template <typename T, unsigned L = 1> struct DataType2Int {
  enum {
    id = kTypeIdUnknown
  };
};

//! Type to id mapping for static arrays.
template <typename T, unsigned L>
struct DataType2Int<T[L]> {
  enum {
    id = DataType2Int<T>::id
  };
};
}  // namespace vatrace

//! Macros that simplifies setting id for a type.
/*! In order to assign an id to a user type one has to put the
  following statement outside all namespaces:

  \code
  VARTRACE_SET_TYPEID(UserType, 0xXX);
  \endcode
*/
#define VARTRACE_SET_TYPEID(Type, type_id)              \
  namespace vartrace {                                  \
  template<> struct DataType2Int<Type> {                \
    enum {id = type_id};                                \
  };                                                    \
  }

//! Specialize DataType2Int to assign data id to int8_t.
VARTRACE_SET_TYPEID(int8_t, kTypeIdInt8);
//! Specialize DataType2Int to assign data id to uint8_t.
VARTRACE_SET_TYPEID(uint8_t, kTypeIdUint8);
//! Specialize DataType2Int to assign data id to int16_t.
VARTRACE_SET_TYPEID(int16_t, kTypeIdInt16);
//! Specialize DataType2Int to assign data id to uint16_t.
VARTRACE_SET_TYPEID(uint16_t, kTypeIdUint16);
//! Specialize DataType2Int to assign data id to int32_t.
VARTRACE_SET_TYPEID(int32_t, kTypeIdInt32);
//! Specialize DataType2Int to assign data id to uint32_t.
VARTRACE_SET_TYPEID(uint32_t, kTypeIdUint32);
//! Specialize DataType2Int to assign data id to int64_t.
VARTRACE_SET_TYPEID(int64_t, kTypeIdInt64);
//! Specialize DataType2Int to assign data id to uint64_t.
VARTRACE_SET_TYPEID(uint64_t, kTypeIdUint64);
//! Specialize DataType2Int to assign data id to float.
VARTRACE_SET_TYPEID(float, kTypeIdFloat);
//! Specialize DataType2Int to assign data id to double.
VARTRACE_SET_TYPEID(double, kTypeIdDouble);
//! Specialize DataType2Int to assign data id to char.
VARTRACE_SET_TYPEID(char, kTypeIdChar);

#endif  // TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_
