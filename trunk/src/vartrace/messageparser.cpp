/* messageparser.cpp
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

/*! \file messageparser.cpp 
 * Definitions for MessageParser class. 
 */

#include "tracetypes.h"
#include "messageparser.h"

namespace vartrace {

MessageParser::MessageParser(void *datastream)
{
    ShortestType *tail = reinterpret_cast<ShortestType*>(datastream);
    timestamp = *(reinterpret_cast<TimestampType*>(tail));
    tail += sizeof(TimestampType);
    dataSize = *(reinterpret_cast<LengthType*>(tail));
    tail += sizeof(LengthType);
    messageId = *(reinterpret_cast<MessageIdType*>(tail));
    tail += sizeof(MessageIdType);
    dataTypeId = *(reinterpret_cast<DataIdType*>(tail));
    tail += sizeof(DataIdType);
    data = tail;
}

} /* vartrace */

