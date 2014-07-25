/* level_test.cc

   Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
   
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

/*! \file level_test.cc 
  Test log level functionality.
*/

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>

using vartrace::VarTrace;

//! Test suite for log level functionality.
class LogLevelTestSuite : public ::testing::Test {
};

//! Check that PODs with low log level are not stored.
TEST_F(LogLevelTestSuite, LevelPODTest) {
  VarTrace<vartrace::InfoLogLevel> trace;
  uint8_t buffer[100];
  int value = 123;
  trace.Log(vartrace::kDebugLevel, 1, value);
  std::size_t dump_size = trace.DumpInto(buffer, 100);
  ASSERT_EQ(0, dump_size);
  trace.Log(vartrace::kInfoLevel, 1, value);
  dump_size = trace.DumpInto(buffer, 100);
  ASSERT_LT(0, dump_size);
}
