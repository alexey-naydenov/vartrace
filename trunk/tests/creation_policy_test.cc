/* creation_policy_test.cc
 *
 * Copyright (C) 2013 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file creation_policy_test.cc 
  Testsuite for creation policies. 
*/

#include <gtest/gtest.h>
#include <boost/shared_array.hpp>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

namespace vt = vartrace;

using vartrace::VarTrace;
using vartrace::User5LogLevel;
using vartrace::kInfoLevel;

typedef VarTrace<vt::InfoLogLevel, vt::ValueCreator> ValueVarTrace;
typedef VarTrace<vt::InfoLogLevel, vt::SingletonCreator> SingletonVarTrace;

class CreationPolicyTest : public ::testing::Test {
 public:
};

// Check default template constructor parameters.
TEST_F(CreationPolicyTest, NewCreatorTest) {
  int trace_size = 0x10000;
  int block_count = 4;
  VarTrace<>::Handle trace = VarTrace<>::Create();
  VarTrace<>::Handle trace2 = VarTrace<>::Create(trace_size, block_count);
  ASSERT_NE(trace.get(), trace2.get());
  ASSERT_TRUE(trace->is_initialized());
  ASSERT_TRUE(trace2->is_initialized());
  ASSERT_EQ(trace->block_count(), vartrace::internal::kDefaultBlockCount);
  ASSERT_EQ(trace2->block_count(), block_count);
  ASSERT_EQ(vartrace::internal::kDefaultTraceSize
            /vartrace::internal::kDefaultBlockCount, trace->block_size());
  ASSERT_EQ(trace_size/block_count, trace2->block_size());
}

void LogThroughPointer(ValueVarTrace::Pointer trace) {
  int16_t s = 2;
  trace->Log(kInfoLevel, 2, s);
}

void LogThroughReference(ValueVarTrace &trace) {
  uint16_t s = 3;
  trace.Log(kInfoLevel, 3, s);
}

// Check that statically created trace is working.
TEST_F(CreationPolicyTest, ValueCreator) {
  ValueVarTrace::Handle trace(ValueVarTrace::Create());
  int buffer_size = 0x1000;
  boost::shared_array<uint8_t> buffer(new uint8_t[buffer_size]);
  // log different types
  int8_t c = 1;
  trace.Log(kInfoLevel, 1, c);
  LogThroughPointer(&trace);
  LogThroughReference(trace);
  int32_t i = 4;
  trace.Log(kInfoLevel, 4, i);
  // check logged dumped values
  unsigned dumped_size = trace.DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check sizes
  ASSERT_EQ(sizeof(c), vt[0]->data_size());
  ASSERT_EQ(sizeof(int16_t), vt[1]->data_size());
  ASSERT_EQ(sizeof(uint16_t), vt[2]->data_size());
  ASSERT_EQ(sizeof(i), vt[3]->data_size());
  // check vales
  ASSERT_EQ(c, vt[0]->value<int8_t>());
  ASSERT_EQ(2, vt[1]->value<int16_t>());
  ASSERT_EQ(3, vt[2]->value<int16_t>());
  ASSERT_EQ(i, vt[3]->value<int32_t>());
}
