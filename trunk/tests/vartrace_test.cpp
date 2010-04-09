#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vartrace/vartrace.h"

/*! \file vartrace_test.cpp
 * Main test for the vartrace project. */

namespace {

using vartrace::VarTrace;
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
    
}

TEST(AlignedSizeTest, SmallValues) 
{
    char c;
    short s;
    int i;
    long long l;
    double d;

    c3Type c3;
    c5Type c5;
    
    EXPECT_EQ(aligned_size<char>(c), 1);
    EXPECT_EQ(aligned_size<short>(s), 1);
    EXPECT_EQ(aligned_size<int>(i), 1);
    EXPECT_EQ(aligned_size<long long>(l), 2);
    EXPECT_EQ(aligned_size<double>(d), 2);
    EXPECT_EQ(aligned_size<c3Type>(c3), 1);
    EXPECT_EQ(aligned_size<c5Type>(c5), 2);
}

} /* namespace */

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
