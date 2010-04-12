#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vartrace/vartrace.h"

/*! \file vartrace_test.cpp
 * Main test for the vartrace project. */

//namespace {

using vartrace::VarTrace;
using vartrace::CopyTraits;
using vartrace::aligned_size;

typedef struct { char c[3]; } c3Type;
typedef struct { char c[5]; } c5Type;

// typedef double MyDouble;
// template <> struct DataType2Int<MyDouble> {enum {id = 0xdd};};
namespace vartrace {
//template <> struct DataType2Int<c5Type> {enum {id = 0xc5};};
template<> struct DataTypeTraits<c5Type>
{
    enum
    {
	DataTypeId = 0xc5,
	TypeSize = 10
    };
};

}


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
    double d = 3.14;

    trace.log(1, a);
    trace.log(2, b);
    trace.log(3, d);

    for (int i = 0; i < 20; ++i) {
	std::cout << std::hex << trace.data_[i] << std::endl;	
    }
}

TEST_F(VarTraceTest, LogCustomType) 
{

    double d = 3.14;
    c5Type c5 = {{0x11,0x22,0x33,0x44,0x55}};
    int i = 0x42;
    int c1[] = {0xaa};
    int ct[] = {0x66, 0x77, 0x88, 0x99};
    int *ip;
    

    trace.log(3, c1);
    trace.log(5, i);
    trace.log(6, ct);

    for (int i = 0; i < 20; ++i) {
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

//} /* namespace */

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
