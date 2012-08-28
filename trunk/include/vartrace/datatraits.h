
/* datatraits.h
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

/*! \file datatraits.h 
  Traits for describing data types and sizes.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_DATATRAITS_H_
#define TRUNK_INCLUDE_VARTRACE_DATATRAITS_H_

#include <vartrace/datatypeid.h>

namespace vartrace {

/*! Trait that assigns data type id and size to a type. */
template<typename T> struct DataTypeTraits {
  /*! Size and data type information. */
  enum {
    kDataTypeId = DataType2Int<T>::id,
    kTypeSize = sizeof(T)
  };
};
} /* vartrace */

#endif  // TRUNK_INCLUDE_VARTRACE_DATATRAITS_H_
