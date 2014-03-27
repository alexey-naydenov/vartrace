//! \file subtrace_test.cc

// Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//!  \brief Subtrace handling tests.

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

using vartrace::VarTrace;
using vartrace::kInfoLevel;

class SubtraceTestSuite : public ::testing::Test {
 public:
};

TEST_F(SubtraceTestSuite, BeginEndTest) {
  VarTrace<>::Handle trace = VarTrace<>::Create();
  trace->BeginSubtrace(1);
  trace->EndSubtrace();
}

// //! Check subtrace storage explicitly.
TEST_F(SubtraceTestSuite, LogBasicSubtraceTest) {
  int buffer_size = 0x100;
  VarTrace<>::Handle trace = VarTrace<>::Create(buffer_size);
  boost::shared_array<uint32_t> buffer(new uint32_t[buffer_size]);
  // create subtrace
  int m1 = 0x1234;
  int m2 = 0x5678;
  int m3 = 0x9012;
  {
    trace->BeginSubtrace(1);
    ASSERT_TRUE(trace->is_subtrace());
    trace->Log(kInfoLevel, 2, m1);
    trace->Log(kInfoLevel, 3, m2);
    trace->EndSubtrace();
  }
  ASSERT_FALSE(trace->is_subtrace()); // logging reenabled
  trace->Log(kInfoLevel, 4, m3);
  // dump and parse trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  // check results
  ASSERT_EQ(36, dumped_size);
  ASSERT_EQ((1<<16) | 16, buffer[1]); // subtrace header
  ASSERT_EQ(m1, buffer[3]); // first value
  ASSERT_EQ(m2, buffer[5]); // second value
  ASSERT_EQ(m3, buffer[8]); // third value
}

//! Check basic subtrace parsing.
TEST_F(SubtraceTestSuite, ParsingBasicSubtraceTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  VarTrace<>::Handle trace = VarTrace<>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create subtrace
  int m1 = 0x1234;
  int m2 = 0x5678;
  int m3 = 0x9012;
  {
    trace->BeginSubtrace(1);
    trace->Log(kInfoLevel, 2, m1);
    trace->Log(kInfoLevel, 3, m2);
    trace->EndSubtrace();
  }
  trace->Log(kInfoLevel, 4, m3);
  // dump and parse trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check complex message
  ASSERT_FALSE(vt[0]->is_nested());
  ASSERT_TRUE(vt[0]->has_children());
  ASSERT_EQ(0, vt[0]->data_type_id());
  ASSERT_EQ(1, vt[0]->message_type_id());
  ASSERT_EQ(16, vt[0]->data_size());
  // check submessages
  ASSERT_TRUE(vt[0]->children()[0]->is_nested());
  ASSERT_FALSE(vt[0]->children()[0]->has_children());
  ASSERT_EQ(vartrace::DataType2Int<int>::id,
            vt[0]->children()[0]->data_type_id());
  ASSERT_EQ(2, vt[0]->children()[0]->message_type_id());
  ASSERT_EQ(4, vt[0]->children()[0]->data_size());
  ASSERT_EQ(8, vt[0]->children()[0]->message_size());
  ASSERT_EQ(m1, vt[0]->children()[0]->value<int>());
  ASSERT_TRUE(vt[0]->children()[1]->is_nested());
  ASSERT_FALSE(vt[0]->children()[1]->has_children());
  ASSERT_EQ(vartrace::DataType2Int<int>::id,
            vt[0]->children()[1]->data_type_id());
  ASSERT_EQ(3, vt[0]->children()[1]->message_type_id());
  ASSERT_EQ(4, vt[0]->children()[1]->data_size());
  ASSERT_EQ(8, vt[0]->children()[1]->message_size());
  ASSERT_EQ(m2, vt[0]->children()[1]->value<int>());
  // check simple message
  ASSERT_FALSE(vt[1]->is_nested());
  ASSERT_FALSE(vt[1]->has_children());
  ASSERT_EQ(vartrace::DataType2Int<int>::id, vt[1]->data_type_id());
  ASSERT_EQ(4, vt[1]->message_type_id());
  ASSERT_EQ(4, vt[1]->data_size());
  ASSERT_EQ(m3, vt[1]->value<int>());
}

//! Check writing many nested subtraces.
TEST_F(SubtraceTestSuite, DeepSubtraceTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  VarTrace<>::Handle trace = VarTrace<>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // variables to hold subtrace data
  // size of the toplevel messge must be less then 3/4 of the trace size
  const int kMaxDepth = 45;
  // begin toplevel message
  trace->BeginSubtrace(1);
  ASSERT_TRUE(trace->is_subtrace());
  // create all other subtraces
  for (std::size_t i = 1; i < kMaxDepth; ++i) {
    trace->BeginSubtrace(2*i + 1);
  }
  // dump trace with all subtraces open, cannot parse it
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  ASSERT_EQ(0, dumped_size);
  // finish subtraces
  for (std::size_t i = kMaxDepth; i != 0; --i) {
    trace->EndSubtrace();
  }
  ASSERT_FALSE(trace->is_subtrace());
  // dump this stuff
  dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check dumped with closed subtraces
  ASSERT_EQ(vartrace::kHeaderSize + (kMaxDepth - 1)*vartrace::kNestedHeaderSize,
            dumped_size);
  // check top level subtrace header
  vartrace::Message::Pointer msg = vt[0];
  ASSERT_FALSE(msg->is_nested());
  ASSERT_TRUE(msg->has_children());
  ASSERT_EQ(0, msg->data_type_id());
  ASSERT_EQ(1, msg->message_type_id());
  ASSERT_EQ((kMaxDepth - 1)*vartrace::kNestedHeaderSize, msg->data_size());
  // check all other headers
  for (int i = 1; i < kMaxDepth; ++i) {
    msg = msg->children()[0];
    ASSERT_TRUE(msg->is_nested());
    if (i < kMaxDepth - 1) {
      ASSERT_TRUE(msg->has_children());
    } else {
      ASSERT_FALSE(msg->has_children());
    }
    ASSERT_EQ(0, msg->data_type_id());
    ASSERT_EQ(2*i + 1, msg->message_type_id());
    ASSERT_EQ((kMaxDepth - i - 1)*vartrace::kNestedHeaderSize,
              msg->data_size());
  }
}

//! Check writing multiple nested subtraces with some data.
TEST_F(SubtraceTestSuite, DeepSubtraceDoubleTest) {
  int trace_size = 0x800;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  VarTrace<>::Handle trace = VarTrace<>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // variables to hold subtrace data
  // size of the toplevel messge must be less then 3/4 of the trace size
  const int kMaxDepth = 40;
  double d;
  // create toplevel message
  trace->BeginSubtrace(1);
  d = 0;
  trace->Log(kInfoLevel, 10, d);
  // create all other subtraces
  for (std::size_t i = 1; i < kMaxDepth; ++i) {
    trace->BeginSubtrace(2*i + 1);
    d = i*i*i;
    trace->Log(kInfoLevel, 10 + i, d);
  }
  // end subtraces
  for (int i = kMaxDepth - 1; i >= 0; --i) {
    d = 2*i*i*i;
    trace->Log(kInfoLevel, 100 + i, d);
    trace->EndSubtrace();
  }
  // dump this stuff
  std::size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check dumped with closed subtraces and two doubles per trace level
  ASSERT_EQ(vartrace::kHeaderSize + (kMaxDepth - 1)*vartrace::kNestedHeaderSize
            + 2*kMaxDepth*12, dumped_size);
  // check top level subtrace header
  vartrace::Message::Pointer msg = vt[0];
  ASSERT_TRUE(msg->has_children());
  ASSERT_EQ(0, msg->data_type_id());
  ASSERT_EQ(1, msg->message_type_id());
  ASSERT_EQ((kMaxDepth - 1)*vartrace::kNestedHeaderSize + 2*kMaxDepth*12,
            msg->data_size());
  // check all other headers
  for (int i = 1; i < kMaxDepth; ++i) {
    // get to the next level of nesting
    msg = msg->children()[1];
    // check messages with doubles
    ASSERT_EQ(10 + i, (msg->children()[0])->message_type_id());
    ASSERT_EQ(i*i*i, (msg->children()[0])->value<double>());
    if (i < kMaxDepth - 1) {
      ASSERT_EQ(100 + i, (msg->children()[2])->message_type_id());
      ASSERT_EQ(2*i*i*i, (msg->children()[2])->value<double>());
    } else {
      ASSERT_EQ(100 + i, (msg->children()[1])->message_type_id());
      ASSERT_EQ(2*i*i*i, (msg->children()[1])->value<double>());
    }
    // check next nested subtrace parameters
    ASSERT_EQ(2*i + 1, msg->message_type_id());
    ASSERT_EQ((kMaxDepth - i - 1)*vartrace::kNestedHeaderSize +
              2*(kMaxDepth - i)*12, msg->data_size());
  }
}
