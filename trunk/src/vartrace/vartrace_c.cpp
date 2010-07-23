/* vartrace_c.cpp
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

/*! \file vartrace_c.cpp 
 * Function implementations for C interface. 
 */

#include "vartrace/vartrace.h"
#include "vartrace/vartrace_c.h"

VT_TraceHandle VT_create(unsigned size)
{
    return new vartrace::VarTrace(size);
}

VT_TraceHandle VT_createSubtrace(VT_TraceHandle trace, int message_id)
{
    return trace->createSubtrace(message_id);
}

void VT_destroy(VT_TraceHandle trace) 
{
    delete trace;
}

unsigned VT_dump(VT_TraceHandle trace, void *buffer, unsigned size) 
{
    return trace->dump(buffer, size);
}

int VT_isEmpty(VT_TraceHandle trace)
{
    return trace->isEmpty();
}

unsigned VT_errorFlags(VT_TraceHandle trace)
{
    return trace->errorFlags();
}

void VT_logInt(VT_TraceHandle trace, int message_id, int var)
{
    trace->log(message_id, var);
}

void VT_logUnsigned(VT_TraceHandle trace, unsigned message_id, unsigned var)
{
    trace->log(message_id, var);
}

void VT_logDouble(VT_TraceHandle trace, double message_id, double var)
{
    trace->log(message_id, var);
}

void VT_logIntArray(VT_TraceHandle trace, int message_id,
		    int * data, int length)
{
    trace->logArray(message_id, data, length);
}

