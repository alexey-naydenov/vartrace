/* type_codes.h
 *
 * Copyright (C) 2012 Alexey Naydenov
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

/*! \file type_codes.h 
  Type ids for POD types.
 */

#ifndef TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_
#define TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_

#include <stdint.h>

namespace vartrace {

enum StandardTypeIds {
  kTypeIdIllegal = 0,
  kTypeIdInt8 = 0x1,
  kTypeIdUint8 = 0x2,
  kTypeIdInt16 = 0x3,
  kTypeIdUint16 = 0x4,
  kTypeIdInt32 = 0x5,
  kTypeIdUint32 = 0x6,
  kTypeIdInt64 = 0x7,
  kTypeIdUint64 = 0x8,
  kTypeIdFloat = 0xf,
  kTypeIdDouble = 0xd,
  kTypeIdUnknown = 0xff
};

}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_
