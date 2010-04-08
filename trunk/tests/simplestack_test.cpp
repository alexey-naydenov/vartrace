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
    IntStackTest() : s3(5) {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    SimpleStack<int> s1;
    SimpleStack<int> s2;
    SimpleStack<int> s3;
};

TEST_F(IntStackTest, CreateDelete)
{
}

}  /* namespace */


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
