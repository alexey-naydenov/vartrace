/* messageparser.h
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

/*! \file messageparser.h 
  Extracts fields from binary data stream. 
 */

#ifndef TRUNK_INCLUDE_VARTRACE_MESSAGEPARSER_H_
#define TRUNK_INCLUDE_VARTRACE_MESSAGEPARSER_H_

#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>

#include "vartrace/tracetypes.h"

namespace vartrace {

//! Holds information about a message.
class Message {
 public:
  typedef boost::shared_ptr<Message> Pointer;
  static Pointer Parse(void *byte_stream, bool is_nested = false);
  Message();
  Message(void *byte_stream, bool is_nested = false);
  ~Message() {}
  bool is_nested() const { return is_nested_; }
  bool has_children() const { return has_children_; }
  unsigned timestamp() const { return timestamp_; }
  int data_type_id() const { return data_type_id_; }
  int message_type_id() const { return message_type_id_; }
  int data_size() const { return data_size_; }
  int message_size() const { return sizeof(AlignmentType)*message_length_; }
  template <typename T> T value() const;
  template <typename T> T* pointer() const;
  const std::vector<Pointer>& children() const { return children_; }

 private:
  void ParseStream(void *byte_stream, bool is_nested = false);

  bool is_nested_;
  bool has_children_;
  TimestampType timestamp_;
  DataIdType data_type_id_;
  MessageIdType message_type_id_;
  LengthType data_size_;
  int message_length_;
  boost::scoped_array<AlignmentType> data_;
  std::vector<Pointer> children_;
};

template <typename T> T Message::value() const {
  T value;
  memset(&value, 0, sizeof(value));
  memcpy(&value, data_.get(), data_size_);
  return value;
}

template <typename T> T* Message::pointer() const {
}
} /* vartrace */

#endif  // TRUNK_INCLUDE_VARTRACE_MESSAGEPARSER_H_

