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

TEST_F(IntStackTest, PushPop)
{
    s1.top();
    EXPECT_TRUE(s1.isError());
    s1.resetError();
    EXPECT_FALSE(s1.isError());

    s1.push(1);
    EXPECT_EQ(s1.top(), 1);
    EXPECT_EQ(s1.size(), 1);
    s1.pop();
    EXPECT_EQ(s1.size(), 0);
    EXPECT_FALSE(s1.isError());
}

TEST_F(IntStackTest, MultiPushPop) 
{
    for (int i = 0; i < s1.maxSize(); ++i) {
	s1.push(i);
	EXPECT_EQ(s1.top(), i);
	EXPECT_EQ(s1.size(), i + 1);
	EXPECT_FALSE(s1.isError());
    }
    for (int i = s1.size(); i > 0; --i) {
	EXPECT_EQ(s1.top(), i - 1);
	s1.pop();
	EXPECT_FALSE(s1.isError());
    }
}

TEST_F(IntStackTest, PushOverflow) 
{
    for (int i = 0; i < s3.maxSize(); ++i) {
	s3.push(i);
    }
    s3.push(1000);
    EXPECT_TRUE(s3.isError());
    s3.push(1000);
    EXPECT_TRUE(s3.isError());
    s3.resetError();
    for (int i = s3.size(); i > 0; --i) {
	EXPECT_EQ(s3.top(), i - 1);
	s3.pop();
	EXPECT_FALSE(s3.isError());
    }
}

}  /* namespace */


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
