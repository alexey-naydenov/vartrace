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

#include "vartrace/vartrace.h"

using vartrace::VarTrace;

class PolicyTest : public ::testing::Test {
 public:

  VarTrace<>::Pointer trace;
  VarTrace<>::Pointer trace2;
  VarTrace<>::Pointer trace3;
};

//! NewCreator should create a new object every time (compiler test).
TEST_F(PolicyTest, NewCreatorTest) {
  trace = VarTrace<vartrace::NewCreator>::Create(3, 1000);
  trace2 = VarTrace<vartrace::NewCreator>::Create(3, 1000);
  EXPECT_NE(trace.get(), trace2.get());
}



int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
