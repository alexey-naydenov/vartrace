/* messageparser.h
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

/*! \file messageparser.h 
 * Extracts fields from binary data stream. 
 */

#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

namespace vartrace {

/*! Converts binary stream into a set of fields. */
class MessageParser
{
public:
    MessageParser(void *datastream);
    virtual ~MessageParser() {};

    unsigned timestamp;
    unsigned dataSize;
    unsigned dataTypeId;
    unsigned messageId;
    void *data;
    
private:
    /*! Disabled default copy constructor. */
    MessageParser(const MessageParser&);
    /*! Disabled default assingment operator. */
    MessageParser& operator=(const MessageParser&);
};

} /* vartrace */

#endif /* MESSAGEPARSER_H */
