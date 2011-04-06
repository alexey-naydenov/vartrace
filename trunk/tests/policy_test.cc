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

#include "vartrace/vartrace.h"

using vartrace::VarTrace;

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
  ASSERT_EQ(vartrace::kDefaultTraceSize/(1<<vartrace::kDefaultBlockCount)
            *sizeof(vartrace::AlignmentType), trace->block_size());
  ASSERT_EQ(trace_size/block_count, trace2->block_size());
}

//! Test logging of simple types.
TEST_F(PolicyTest, LogNoWrapTest) {
  int trace_size = 0x1000;
  int buffer_size = trace_size;
  int message_size = vartrace::kHeaderSize + sizeof(vartrace::AlignmentType);
  trace = VarTrace<vartrace::NewCreator>::Create(trace_size);
  boost::shared_array<char> buffer(new char[buffer_size]);
  int copied_size;
  // log simple variables and check dumped data, no overfill of the trace
  for (int i = 0; i < trace_size/message_size/3*2; ++i) {
    trace->Log(2*i, 3*i);
    ASSERT_EQ((i+1)*message_size, trace->DumpInto(buffer.get(), buffer_size));
  }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
