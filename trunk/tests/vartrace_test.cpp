#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vartrace/vartrace.h"

/*! \file vartrace_test.cpp
 * Main test for the vartrace project. */

namespace {

using vartrace::VarTrace;
using vartrace::CopyTraits;
using vartrace::aligned_size;

typedef struct { char c[3]; } c3Type;
typedef struct { char c[5]; } c5Type;

class VarTraceTest : public ::testing::Test
{
public:
    
    VarTraceTest() : trace(0x100000) {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    VarTrace trace;
};

TEST_F(VarTraceTest, LogMessage) 
{
    int a = 0x123;
    unsigned b = 0x456;

    trace.logMessage(1, a, CopyTraits<int>::CopyCategory(),
		     CopyTraits<int>::DataTypeId,
		     CopyTraits<int>::ObjectSize,
		     CopyTraits<int>::ObjectLength);
    trace.logMessage(2, b, CopyTraits<unsigned>::CopyCategory(),
		     CopyTraits<unsigned>::DataTypeId,
		     CopyTraits<unsigned>::ObjectSize,
		     CopyTraits<unsigned>::ObjectLength);

    for (int i = 0; i < 10; ++i) {
	std::cout << std::hex << trace.data_[i] << std::endl;	
    }
}

TEST(AlignedSizeTest, SmallValues) 
{
    EXPECT_EQ(aligned_size<char>(), 1);
    EXPECT_EQ(aligned_size<short>(), 1);
    EXPECT_EQ(aligned_size<int>(), 1);
    EXPECT_EQ(aligned_size<long long>(), 2);
    EXPECT_EQ(aligned_size<double>(), 2);
    EXPECT_EQ(aligned_size<c3Type>(), 1);
    EXPECT_EQ(aligned_size<c5Type>(), 2);
}

} /* namespace */

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
