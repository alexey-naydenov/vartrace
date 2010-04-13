#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "vartrace/vartrace.h"
#include "vartrace/messageparser.h"

/*! \file vartrace_test.cpp
 * Main test for the vartrace project. */

using vartrace::VarTrace;
using vartrace::CopyTraits;
using vartrace::MessageParser;
using vartrace::aligned_size;

typedef struct { char c[3]; } c3Type;
typedef struct { char c[5]; } c5Type;

namespace vartrace {

template<> struct DataType2Int<c3Type> { enum {id = 0xc3}; };

template<> struct DataTypeTraits<c5Type>
{
    enum
    {
	DataTypeId = 0xc5,
	TypeSize = 3
    };
};

}


class VarTraceTest : public ::testing::Test
{
public:
    
    VarTraceTest() : trace(0x100000), small(16), medium(64) {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    VarTrace trace;
    VarTrace small;
    VarTrace medium;
};

TEST_F(VarTraceTest, LogSimpleType) 
{
    unsigned char a = 0x11;
    unsigned b = 0x222222;
    vartrace::AlignmentType *p = trace.rawData();

    trace.log(1, a);
    trace.log(2, b);

    EXPECT_TRUE(trace.isConsistent()) << "Error flags: " << trace.errorFlags();
    
    EXPECT_EQ(p[2] & 0xff, a);
    EXPECT_EQ(p[5], b);
}

TEST_F(VarTraceTest, LogArray) 
{
    int a[] = {0x1111, 0x2222, 0x3333};
    vartrace::AlignmentType *p = trace.rawData();

    trace.log(7, a);
    EXPECT_TRUE(trace.isConsistent()) << "Error flags: " << trace.errorFlags();

    for (int i = 0; i < sizeof(a)/sizeof(a[0]); ++i) {
	EXPECT_EQ(p[2 + i], a[i])
	    << "Element " << i << "of an array was not copied correctly";
    }
}

TEST_F(VarTraceTest, LogCustomType) 
{
    c3Type c3 = {{0xaa, 0xbb}};
    c5Type c5 = {{0x11, 0x22, 0x33, 0x44, 0x55}};
    vartrace::AlignmentType *p = trace.rawData();
    
    trace.log(3, c3);
    trace.log(5, c5);
    EXPECT_TRUE(trace.isConsistent()) << "Error flags: " << trace.errorFlags();
    
    EXPECT_EQ((p[1] & 0xff000000)>>24, 0xc3)
	<< "Data type id was not set correctly";
    EXPECT_EQ((p[4] & 0xff000000)>>24, 0xc5)
	<< "Data type id was not set correctly";
    EXPECT_EQ(p[5] & 0xffffff, c5.c[0] + (c5.c[1]<<8) + (c5.c[2]<<16))
	<< "Data was not copied correctly";
}


TEST_F(VarTraceTest, UtilityFunct) 
{
    int i = 1;

    EXPECT_TRUE(trace.isEmpty());
    trace.log(1, i);
    EXPECT_FALSE(trace.isEmpty());
    EXPECT_TRUE(trace.isConsistent()) << "Error flags: " << trace.errorFlags();
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

int main (int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
