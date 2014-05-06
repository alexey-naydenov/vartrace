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

#include <vartrace/tracetypes.h>
#include <vartrace/utility.h>
#include <cstddef>
#include <vector>

namespace vartrace {

//! Class for parsing and storing VarTrace messages.
class Message {
 public:
  //! Convenience typedef.
  typedef boost::shared_ptr<Message> Pointer;

  //! Read message from buffer and return pointer to it.
  static Pointer Parse(void *byte_stream, bool is_nested = false);

  //! Default constructor.
  Message();
  //! Construct message from stream.
  Message(void *byte_stream, bool is_nested = false);
  //! Empty destructor.
  ~Message() {}

  //! True if message is nested.
  bool is_nested() const {return is_nested_;}
  //! True if message contains other messages.
  bool has_children() const {return has_children_;}
  //! Return timestamp.
  unsigned timestamp() const {return timestamp_;}
  //! Return data type id.
  int data_type_id() const {return data_type_id_;}
  //! Return message type id.
  int message_type_id() const {return message_type_id_;}
  //! Return size of data in message.
  int data_size() const {return data_size_;}
  //! Return size of message with header and padding.
  int message_size() const {return sizeof(AlignmentType)*message_length_;}
  //! Interpret data as value of given type.
  template <typename T> T value() const;
  //! Interpret data as pointer to given type.
  template <typename T> T* pointer() const;
  //! Return vector of pointers to children.
  const std::vector<Pointer>& children() const {return children_;}

 private:
  //! Function that does actual stream parsing.
  void ParseStream(void *byte_stream, bool is_nested = false);

  bool is_nested_; //!< True if message is nested.
  bool has_children_; //!< True if message contains other messages.
  TimestampType timestamp_; //!< Message timestamp.
  DataIdType data_type_id_; //!< Data type id.
  MessageIdType message_type_id_; //!< Message type id
  LengthType data_size_; //!< Size of data.
  int message_length_; //!< Total message length, data and header.
  boost::scoped_array<AlignmentType> data_; //!< Message data.
  std::vector<Pointer> children_; //!< Pointers to children.
};

template <typename T> T Message::value() const {
  T val;
  memset(&val, 0, sizeof(val));
  memcpy(&val, data_.get(), data_size_);
  return val;
}

template <typename T> T* Message::pointer() const {
  T *ptr = reinterpret_cast<T *>(data_.get());
  return ptr;
}

//! Container for the whole trace, vector of messages.
class ParsedVartrace {
 public:
  //! Convenience typedef for pointer to a vartrace.
  typedef boost::shared_ptr<ParsedVartrace> Pointer;
  //! Parse trace from byte array.
  ParsedVartrace(void *byte_stream, std::size_t size);
  //! Empty destructor.
  ~ParsedVartrace() {}
  //! Access top level message.
  Message::Pointer operator[](std::size_t position) {
    return messages_[position];
  }
  //! Return vector of top level messages.
  const std::vector<Message::Pointer>& messages() const {return messages_;}
 private:
  //! Actual byte array parser.
  void ParseStream(void *byte_stream, std::size_t size);

  std::vector<Message::Pointer> messages_; //!< Top level messages.
};
} /* vartrace */

#endif  // TRUNK_INCLUDE_VARTRACE_MESSAGEPARSER_H_

