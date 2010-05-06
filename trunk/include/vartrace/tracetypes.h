/* tracetypes.h
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

/*! \file tracetypes.h 
 * Type definitions for internal use. 
 */

#ifndef TRACETYPES_H
#define TRACETYPES_H

#ifdef __cplusplus
namespace vartrace {
#endif

/*! Sets type for minimal unit of data. */
typedef unsigned char ShortestType;
/*! Sets alingment of message boundaries and data fields. */
typedef unsigned AlignmentType;
/*! Timestamp type. */
typedef unsigned TimestampType;
/*! Type to store message size. */
typedef unsigned short LengthType;
/*! Message type fields type. */
typedef unsigned char MessageIdType;
/*! Data type fields type. */
typedef unsigned char DataIdType;

/*! Function type for timestamps. */
typedef TimestampType (*TimestampFunctionType) ();

#ifdef __cplusplus
} /* namespace vartrace */
#endif

#endif /* TRACETYPES_H */
