//! \file containers_test.cc

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

//!  \brief Test container logging.

#include <vector>
#include <string>

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

using vartrace::VarTrace;
using vartrace::kInfoLevel;
using vartrace::kHiddenLevel;

//! Test suite class for testing containers.
class ContainerTestSuite : public ::testing::Test {
 public:
};

//! Test vector specialization.
TEST_F(ContainerTestSuite, VectorCasesTest) {
  VarTrace<> trace;
  std::vector<int> v(10);
  trace.Log(kInfoLevel, 1, v);
  int buffer_size = 0x1000;
  boost::shared_array<uint8_t> buffer(new uint8_t[buffer_size]);
  unsigned dumped_size = trace.DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  ASSERT_EQ(v.size()*sizeof(int), vt[0]->data_size());
}

//! Test string specialization.
TEST_F(ContainerTestSuite, StdStringTest) {
  VarTrace<> trace;
  std::string s("test string");
  trace.Log(kInfoLevel, 1, s);
  int buffer_size = 0x1000;
  boost::shared_array<uint8_t> buffer(new uint8_t[buffer_size]);
  unsigned dumped_size = trace.DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  ASSERT_EQ(s.size(), vt[0]->data_size());
}

//! Check that string and vector loggers are blocked by log level.
TEST_F(ContainerTestSuite, LogLevelTest) {
  VarTrace<> trace;
  std::string s("test string");
  trace.Log(kHiddenLevel, 1, s);
  int buffer_size = 0x1000;
  boost::shared_array<uint8_t> buffer(new uint8_t[buffer_size]);
  unsigned dumped_size = trace.DumpInto(buffer.get(), buffer_size);
  ASSERT_EQ(0, dumped_size);
  std::vector<int> v(10);
  trace.Log(kHiddenLevel, 1, v);
  dumped_size = trace.DumpInto(buffer.get(), buffer_size);
  ASSERT_EQ(0, dumped_size);
}
