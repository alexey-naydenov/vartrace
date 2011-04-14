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

#include "vartrace/tracetypes.h"
#include "vartrace/messageparser.h"

#include <cstring>

namespace vartrace {

Message::Pointer Message::Parse(void *byte_stream, bool is_nested) {
  Pointer message(new Message(byte_stream, is_nested));
  return message;
}

Message::Message()
    : is_nested_(false), has_children_(false), timestamp_(0), data_type_id_(0),
      message_type_id_(0), data_size_(0), message_length_(0) {
}

Message::Message(void *byte_stream, bool is_nested)
    : is_nested_(is_nested) {
  ParseStream(byte_stream, is_nested);
}

template<typename T>
static uint8_t * ReadSimpleType(uint8_t *data, T *variable) {
  *variable = 0;
  std::memcpy(variable, data, sizeof(T));
  return (data + sizeof(T));
}

void Message::ParseStream(void *byte_stream, bool is_nested) {
  uint8_t *start_position = static_cast<uint8_t *>(byte_stream);
  uint8_t *parse_position = start_position;
  // parse timestamp if it is present
  if (!is_nested) {
    parse_position = ReadSimpleType(parse_position, &timestamp_);
  } else {
    timestamp_ = 0;
  };
  // get data size
  parse_position = ReadSimpleType(parse_position, &data_size_);
  // get message id
  parse_position = ReadSimpleType(parse_position, &message_type_id_);
  // get data type id
  parse_position = ReadSimpleType(parse_position, &data_type_id_);
  // get required storage length
  int data_length = CEIL_DIV(data_size_, sizeof(AlignmentType));
  data_.reset(new AlignmentType[data_length]);
  std::memcpy(data_.get(), parse_position, data_size_);
  // calculate message length
  message_length_ = CEIL_DIV(parse_position - start_position,
                             sizeof(AlignmentType)) + data_length;
}

ParsedVartrace::ParsedVartrace(void *byte_stream, size_t size) {
  ParseStream(byte_stream, size);
}

void ParsedVartrace::ParseStream(void *byte_stream, size_t size) {
  uint8_t *unparsed_position = static_cast<uint8_t *>(byte_stream);
  size_t parsed_size = 0;
  while (parsed_size < size) {
    Message::Pointer msg(new Message(unparsed_position, false));
    messages_.push_back(msg);
    parsed_size += msg->message_size();
    unparsed_position += msg->message_size();
  }
}
}  // namespace vartrace

