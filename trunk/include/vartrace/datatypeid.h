/* datatypeid.h
 *
 * Copyright (C) 2010 Alexey Naydenov
 *
 * Author: Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*! \file datatypeid.h 
 * Type to type id mappings. 
 */

#ifndef TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_
#define TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_

namespace vartrace {

template <typename T, unsigned L = 1> struct DataType2Int {enum {id = 0xff};};

template <> struct DataType2Int<char> {enum {id = 0x1};};
template <> struct DataType2Int<unsigned char> {enum {id = 0x2};};
template <> struct DataType2Int<short> {enum {id = 0x3};};
template <> struct DataType2Int<unsigned short> {enum {id = 0x4};};
template <> struct DataType2Int<int> {enum {id = 0x5};};
template <> struct DataType2Int<unsigned int> {enum {id = 0x6};};
template <> struct DataType2Int<long> {enum {id = 0x7};};
template <> struct DataType2Int<unsigned long> {enum {id = 0x8};};
template <> struct DataType2Int<long long> {enum {id = 0x9};};
template <> struct DataType2Int<unsigned long long> {enum {id = 0xa};};

template <> struct DataType2Int<float> {enum {id = 0xf};};
template <> struct DataType2Int<double> {enum {id = 0xd};};

template <typename T, unsigned L>
struct DataType2Int<T[L]> {
  enum {
    id = DataType2Int<T>::id
  };
};

#define REGISTER_VARTRACE_TYPE(Type, type_id)           \
  namespace vartrace {                                  \
  template<> struct DataType2Int<Type> {                \
    enum {id = type_id};                                \
  };                                                    \
  }
}  // vatrace

#endif  // TRUNK_INCLUDE_VARTRACE_DATATYPEID_H_
