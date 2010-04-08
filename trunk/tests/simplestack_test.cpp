#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vartrace/simplestack.h"

/*! \file simplestack_test.cpp
 * Tests for a simple implementation of a stack. */

namespace {

using vartrace::SimpleStack;

class IntStackTest : public ::testing::Test
{
public:
    enum {TestStackSize = 5};

    IntStackTest() : s3(TestStackSize) {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    SimpleStack<int> s1;
    SimpleStack<int> s2;
    SimpleStack<int> s3;
};

TEST_F(IntStackTest, StateAfterCreation)
{
    EXPECT_EQ(s1.maxSize(), s1.DefaultStackSize);
    EXPECT_EQ(s3.maxSize(), TestStackSize);
    EXPECT_EQ(s1.size(), 0);
    EXPECT_EQ(s3.size(), 0);
}

}  /* namespace */


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
