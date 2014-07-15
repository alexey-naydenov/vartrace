/* type_codes.h
 
   Copyright (C) 2012 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
   
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

/*! \file type_codes.h 

  Constants that set ids of POD types.

  The constant are put in a separate file to simplify their extraction
  by trace parsing tools.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_
#define TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_

namespace vartrace {
//! Type ids of POD types.
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
  kTypeIdChar = 0xc,
  kTypeIdUnknown = 0xff
};
}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_TYPE_CODES_H_
