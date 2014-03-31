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

//!  \brief Test container loggin

#include <vector>
#include <string>

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

using vartrace::VarTrace;
using vartrace::kInfoLevel;

class ContainerTestSuite : public ::testing::Test {
 public:
};


TEST_F(ContainerTestSuite, SimplestCasesTest) {
  VarTrace<>::Handle trace = VarTrace<>::Create();
  std::vector<int> v(10);
  trace->Log(kInfoLevel, 1, v);
}

TEST_F(ContainerTestSuite, StdStringTest) {
  VarTrace<>::Handle trace = VarTrace<>::Create();
  std::string s("test string");
  trace->Log(kInfoLevel, 1, s);
}