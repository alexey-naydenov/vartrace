//! \file utils_test.cc

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

//!  \brief Internal utilities tests.


#include <gtest/gtest.h>

#include <vartrace/utility.h>

using vartrace::FillWithOnes;
using vartrace::CeilPower2;
using vartrace::FloorPower2;
using vartrace::CeilLog2;
using vartrace::FloorLog2;
using vartrace::AlignmentType;
using vartrace::AlignPointer;

class UtilsTestSuite : public ::testing::Test {
};

TEST_F(UtilsTestSuite, FillWithOnesTest) {
  ASSERT_EQ(FillWithOnes(0), 0);
  ASSERT_EQ(FillWithOnes(1), 1);
  ASSERT_EQ(FillWithOnes(2), 3);
  ASSERT_EQ(FillWithOnes(3), 3);
  ASSERT_EQ(FillWithOnes(4), 7);
  ASSERT_EQ(FillWithOnes(0xFFFFFFFFu), 0xFFFFFFFFu);
  ASSERT_EQ(FillWithOnes(0x80000000u), 0xFFFFFFFFu);
  ASSERT_EQ(FillWithOnes(0x81234567u), 0xFFFFFFFFu);
}

TEST_F(UtilsTestSuite, CeilPower2Test) {
  ASSERT_EQ(CeilPower2(1), 1);
  ASSERT_EQ(CeilPower2(3), 4);
  ASSERT_EQ(CeilPower2(4), 4);
  ASSERT_EQ(CeilPower2(0x80000000u), 0x80000000u);
  ASSERT_EQ(CeilPower2(0x7FFFFFFFu), 0x80000000u);
}

TEST_F(UtilsTestSuite, FloorPower2Test) {
  ASSERT_EQ(FloorPower2(1), 1);
  ASSERT_EQ(FloorPower2(2), 2);
  ASSERT_EQ(FloorPower2(3), 2);
  ASSERT_EQ(FloorPower2(4), 4);
  ASSERT_EQ(FloorPower2(5), 4);
  ASSERT_EQ(FloorPower2(0x80000000u), 0x80000000u);
  ASSERT_EQ(FloorPower2(0x7FFFFFFFu), 0x40000000u);
}

TEST_F(UtilsTestSuite, CeilLog2Test) {
  ASSERT_EQ(CeilLog2(1), 0);
  ASSERT_EQ(CeilLog2(2), 1);
  ASSERT_EQ(CeilLog2(3), 2);
  ASSERT_EQ(CeilLog2(4), 2);
  ASSERT_EQ(CeilLog2(5), 3);
  ASSERT_EQ(CeilLog2(0x80000000u), 31);
  ASSERT_EQ(CeilLog2(0x7FFFFFFFu), 31);
}

TEST_F(UtilsTestSuite, FloorLog2Test) {
  ASSERT_EQ(FloorLog2(1), 0);
  ASSERT_EQ(FloorLog2(2), 1);
  ASSERT_EQ(FloorLog2(3), 1);
  ASSERT_EQ(FloorLog2(4), 2);
  ASSERT_EQ(FloorLog2(5), 2);
  ASSERT_EQ(FloorLog2(0x80000000u), 31);
  ASSERT_EQ(FloorLog2(0x7FFFFFFFu), 30);
}

TEST_F(UtilsTestSuite, AlignPointerTest) {
  ASSERT_EQ(reinterpret_cast<AlignmentType *>(0),
            AlignPointer(reinterpret_cast<void *>(0)).first);
  ASSERT_EQ(reinterpret_cast<AlignmentType *>(0) + 1,
            AlignPointer(reinterpret_cast<void *>(1)).first);
  ASSERT_EQ(reinterpret_cast<AlignmentType *>(0) + 1,
            AlignPointer(reinterpret_cast<void *>(
                sizeof(AlignmentType))).first);
  ASSERT_EQ(reinterpret_cast<AlignmentType *>(0) + 111,
            AlignPointer(reinterpret_cast<void *>(
                111*sizeof(AlignmentType))).first);
  ASSERT_EQ(0, AlignPointer(reinterpret_cast<void *>(
      111*sizeof(AlignmentType))).second);
  for (unsigned i = 1; i < sizeof(AlignmentType); ++i) {
    ASSERT_EQ(
        reinterpret_cast<AlignmentType *>(0) + 11 + 1,
        AlignPointer(reinterpret_cast<void *>(
            11*sizeof(AlignmentType) + i)).first);
    ASSERT_EQ(sizeof(AlignmentType) - i,
              AlignPointer(reinterpret_cast<void *>(
                  11*sizeof(AlignmentType) + i)).second);
  }
}
