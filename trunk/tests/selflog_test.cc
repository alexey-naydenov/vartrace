//! \file selflog_test.cc

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

//!  \brief Self logging object tests.

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

#include <iostream>

using std::cout;
using std::endl;

using vartrace::VarTrace;
using vartrace::kInfoLevel;
using vartrace::Message;

class SelfLoggingTestSuite : public ::testing::Test {
 public:
  VarTrace<>::Handle trace;
  VarTrace<>::Handle trace2;
  VarTrace<>::Handle trace3;
};

namespace test {
class SelfLogClass {
 public:
  int ivar;
  double dvar;
  double dont_log_array[10];

  void LogItself(VarTrace<>::Pointer trace) const {
    trace->Log(kInfoLevel, 101, ivar);
    trace->Log(kInfoLevel, 102, dvar);
  }
};
}  // namespace test

VARTRACE_SET_SELFLOGGING(test::SelfLogClass);

//! Check self logging class.
TEST_F(SelfLoggingTestSuite, SelfLogTest) {
  std::size_t buffer_size = 0x1000;
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_size]);
  trace = VarTrace<>::Create();
  // create and populate some objects
  test::SelfLogClass cls;
  cls.ivar = 1234;
  cls.dvar = 12e-34;
  // log
  trace->Log(kInfoLevel, 1, cls);
  // dump this stuff
  unsigned dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check results
  ASSERT_EQ(2*vartrace::kNestedHeaderSize + sizeof(cls.ivar) + sizeof(cls.dvar),
            vt[0]->data_size());
  ASSERT_EQ(0, vt[0]->data_type_id());
  ASSERT_EQ(1, vt[0]->message_type_id());
}

//! Check logging of an array of self logging classes.
TEST_F(SelfLoggingTestSuite, SelfLogArrayTest) {
  int buffer_size = 0x1000;
  trace = VarTrace<>::Create();
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_size]);
  // create and populate some objects
  const unsigned kArrayLength = 7;
  boost::shared_array<test::SelfLogClass> anarray(
      new test::SelfLogClass[kArrayLength]);
  for (unsigned i = 0; i < kArrayLength; ++i) {
    anarray[i].ivar = i;
    anarray[i].dvar = i*i;
  }
  // log arrays
  trace->Log(kInfoLevel, 1, anarray.get(), kArrayLength);
  // dump this stuff
  unsigned dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check results
  ASSERT_EQ(kArrayLength*(2*vartrace::kNestedHeaderSize
                          + sizeof(anarray[0].ivar) + sizeof(anarray[0].dvar)),
            vt[0]->data_size());
  ASSERT_EQ(2*kArrayLength, vt[0]->children().size());
  for (unsigned i = 0; i < kArrayLength; ++i) {
    ASSERT_EQ(101, vt[0]->children()[2*i]->message_type_id());
    ASSERT_EQ(i, vt[0]->children()[2*i]->value<int>());
    ASSERT_EQ(102, vt[0]->children()[2*i + 1]->message_type_id());
    ASSERT_EQ(i*i, vt[0]->children()[2*i + 1]->value<double>());
  }
}

class SelfLogTemplateClass {
 public:
  int ivar;
  double dvar;
  double dont_log_array[10];

  template<class LoggerPointer>
  void LogItself(LoggerPointer trace) const {
    trace->Log(kInfoLevel, 101, ivar);
    trace->Log(kInfoLevel, 102, dvar);
  }
};

VARTRACE_SET_SELFLOGGING(SelfLogTemplateClass);

//! Check self logging template class.
TEST_F(SelfLoggingTestSuite, SelfLogTemplateTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = VarTrace<>::Create(trace_size);
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create and populate some objects
  SelfLogTemplateClass cls;
  cls.ivar = 1234;
  cls.dvar = 12e-34;
  // log
  trace->Log(kInfoLevel, 1, cls);
  // dump this stuff
  unsigned dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check results
  ASSERT_EQ(2*vartrace::kNestedHeaderSize + sizeof(cls.ivar) + sizeof(cls.dvar),
            vt[0]->data_size());
  ASSERT_EQ(0, vt[0]->data_type_id());
  ASSERT_EQ(1, vt[0]->message_type_id());
}

