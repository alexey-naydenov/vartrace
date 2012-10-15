/* vartrace_c.h
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

/*! \file vartrace_c.h 
 * C interface for vartrace class. 
 */

#ifndef VARTRACE_C_H
#define VARTRACE_C_H

#include <vartrace/tracetypes.h>

// #ifdef __cplusplus
// namespace vartrace {
// class VarTrace;
// }
// typedef vartrace::VarTrace * VT_TraceHandle;
// using vartrace::TimestampFunctionType;
// #else
// typedef void * VT_TraceHandle;
// #endif

// #ifdef __cplusplus
// extern "C" {
// #endif

// VT_TraceHandle VT_create(unsigned size);
// VT_TraceHandle VT_createSubtrace(VT_TraceHandle trace, int message_id);
// void VT_destroy(VT_TraceHandle trace);
// unsigned VT_dump(VT_TraceHandle trace, void *buffer, unsigned size);
// int VT_isEmpty(VT_TraceHandle trace);
// unsigned VT_errorFlags(VT_TraceHandle trace);

// void VT_logInt(VT_TraceHandle trace, int message_id, int var);
// void VT_logUnsigned(VT_TraceHandle trace, unsigned message_id, unsigned var);
// void VT_logDouble(VT_TraceHandle trace, double message_id, double var);

// void VT_logIntArray(VT_TraceHandle trace, int message_id,
// 		    int * data, int length);

// void VT_setTimestampFunction(VT_TraceHandle trace,
// 			     TimestampFunctionType fnct);
// #ifdef __cplusplus
// } /* extern "C" */
// #endif


#endif /* VARTRACE_C_H */
