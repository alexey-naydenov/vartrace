/* policy_test.cc
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

/*! \file policy_test.cc 
  Test policy for vartrace class.. 
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/shared_array.hpp>

#include <cstdio>

#include "vartrace/vartrace.h"
#include "vartrace/messageparser.h"

using vartrace::VarTrace;
using vartrace::Message;

class PolicyTest : public ::testing::Test {
 public:

  VarTrace<>::Pointer trace;
  VarTrace<>::Pointer trace2;
  VarTrace<>::Pointer trace3;
};

//! NewCreator should create different objects (compiler test).
TEST_F(PolicyTest, NewCreatorTest) {
  int trace_size = 0x1000;
  int block_count = 4;
  trace = VarTrace<vartrace::NewCreator>::Create();
  trace2 = VarTrace<vartrace::NewCreator>::Create(trace_size, block_count);
  ASSERT_TRUE(trace);
  ASSERT_TRUE(trace2);
  ASSERT_NE(trace.get(), trace2.get());
  ASSERT_TRUE(trace->is_initialized());
  ASSERT_TRUE(trace2->is_initialized());
  ASSERT_EQ(trace->block_count(), vartrace::kDefaultBlockCount);
  ASSERT_EQ(trace2->block_count(), block_count);
  ASSERT_EQ(vartrace::kDefaultTraceSize/vartrace::kDefaultBlockCount,
            trace->block_size());
  ASSERT_EQ(trace_size/block_count, trace2->block_size());
}

//! Test logging of many integers.
TEST_F(PolicyTest, LogIntegersTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  int message_size = vartrace::kHeaderSize + sizeof(vartrace::AlignmentType);
  int message_length = 3;
  trace = VarTrace<vartrace::NewCreator>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // write 5 times the trace capacity
  for (int i = 0; i < 5*trace_size/message_size; ++i) {
    trace->Log(2*i, 3*i);
    // dump trace
    int dumped_size = trace->DumpInto(buffer.get(), buffer_size);
    ASSERT_GE(buffer_size, dumped_size);
    // if less then trace capacity of data was written
    if (i < 3*trace_size/message_size/4) {
      // check total dumped size of the dump in the predictable case
      ASSERT_EQ((i+1)*message_size, dumped_size);
      // first logged message must be present
      ASSERT_EQ(0, buffer[0]); // timestamp
      ASSERT_EQ(0, (buffer[1]>>16) & 0xff); // message id
      ASSERT_EQ(0, buffer[2]); // value
    }
    // check all dumped messaged
    for (int j = 0; j < dumped_size/message_size; ++j) {
      // timestamp is the message couter
      int message_counter = buffer[j*message_length];
      // check size of the message
      ASSERT_EQ(sizeof(i), buffer[j*message_length + 1] & 0xffff);
      // check message id of the message
      ASSERT_EQ((2*message_counter)%256,
                (buffer[j*message_length + 1]>>16) & 0xff);
      // check data type of the message
      ASSERT_EQ(vartrace::DataType2Int<int>::id,
                (buffer[j*message_length + 1]>>24) & 0xff);
      // check the value of the message
      ASSERT_EQ(3*message_counter, buffer[j*message_length + 2]);
    }
  }
}

//! Test logging of different types.
TEST_F(PolicyTest, LogDifferentTypesTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = VarTrace<vartrace::NewCreator>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // log different types
  char c = 1;
  trace->Log(1, c);
  short s = 2;
  trace->Log(2, s);
  unsigned short us = 3;
  trace->Log(3, us);
  int i = 4;
  trace->Log(4, i);
  unsigned ui = 5;
  trace->Log(5, ui);
  // check logged dumped values
  trace->DumpInto(buffer.get(), buffer_size);
  // parse dumped data
  int offset = 0;
  // check char message
  Message cmsg(&buffer[offset]);
  offset += cmsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(1, cmsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<char>::id, cmsg.data_type_id());
  ASSERT_EQ(sizeof(c), cmsg.data_size());
  // check short message
  Message smsg(&buffer[offset]);
  offset += smsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(2, smsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<short>::id, smsg.data_type_id());
  ASSERT_EQ(sizeof(s), smsg.data_size());
  // check unsigned short message
  Message usmsg(&buffer[offset]);
  offset += usmsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(3, usmsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<unsigned short>::id, usmsg.data_type_id());
  ASSERT_EQ(sizeof(us), usmsg.data_size());
  // check int message
  Message imsg(&buffer[offset]);
  offset += imsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(4, imsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<int>::id, imsg.data_type_id());
  ASSERT_EQ(sizeof(i), imsg.data_size());
  // check unsigned int message
  Message uimsg(&buffer[offset]);
  offset += uimsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(5, uimsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<unsigned>::id, uimsg.data_type_id());
  ASSERT_EQ(sizeof(ui), uimsg.data_size());
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
