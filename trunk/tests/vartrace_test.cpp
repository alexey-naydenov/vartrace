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
    
    VarTraceTest() : trace(0x100000), t16(16), t32(32), t64(64) {}

    virtual void SetUp() {}

    virtual void TearDown() {}

    VarTrace trace;
    VarTrace t16;
    VarTrace t32;
    VarTrace t64;
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

    /* use message parser, test it also */
    MessageParser msg(trace.rawData());
    EXPECT_EQ(msg.dataSize, sizeof(a));
    EXPECT_EQ(msg.dataTypeId, 2);
    EXPECT_EQ(msg.messageId, 1);
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
    /* use message parser, test it also */
    MessageParser msg(trace.rawData());
    EXPECT_EQ(msg.dataSize, sizeof(a));
    EXPECT_EQ(msg.dataTypeId, 0x15);
    EXPECT_EQ(msg.messageId, 7);
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
    /* use message parser, test it also */
    MessageParser msg(trace.rawData());
    EXPECT_EQ(msg.dataSize, sizeof(c3));
    EXPECT_EQ(msg.dataTypeId, 0xc3);
    EXPECT_EQ(msg.messageId, 3);
}

TEST_F(VarTraceTest, UtilityFunct) 
{
    int i = 1;

    EXPECT_TRUE(trace.isEmpty());
    trace.log(1, i);
    EXPECT_FALSE(trace.isEmpty());
    EXPECT_TRUE(trace.isConsistent()) << "Error flags: " << trace.errorFlags();
}

TEST_F(VarTraceTest, SingleMessageTrace) 
{
    for (int i = 1; i < 3; ++i) {
	t16.log(i + 1, i);
	EXPECT_TRUE(t16.isConsistent()) << "Error flags: " << t16.errorFlags();
	MessageParser msg(t16.head());
	EXPECT_EQ(msg.dataSize, sizeof(i));
	EXPECT_EQ(msg.dataTypeId, 0x5);
	EXPECT_EQ(msg.messageId, i + 1)
	    << "Head pointer " << std::hex << t16.head()
	    << " data pointer " << t16.rawData();
    }
}

TEST_F(VarTraceTest, UniformTraceWrap) 
{
    for (int i = 0; i < 6; ++i) {
	t32.log(i + 1, i);
	EXPECT_TRUE(t32.isConsistent()) << "Error flags: " << t32.errorFlags();
	/* check if head is moving */
	if (i > 1) {
	    MessageParser msg(t32.head());
	    EXPECT_EQ(msg.dataSize, sizeof(i));
	    EXPECT_EQ(msg.dataTypeId, 0x5);
	    EXPECT_EQ(i - 1, msg.messageId)
		<< "Head pointer " << std::hex << t32.head()
		<< " data pointer " << t32.rawData();
	}
    }
}

TEST_F(VarTraceTest, WrappedTraceDump) 
{
    char buffer[256];

    // write 3 messages, logs fits only 2 
    for (int i = 0; i < 3; ++i) {
	t32.log(i + 1, i);
	EXPECT_TRUE(t32.isConsistent()) << "Error flags: " << t32.errorFlags();
    }
    // check dump: 2 messages starts from 2nd
    int rc = t32.dump(buffer, 256);
    EXPECT_EQ(2*(t32.HeaderSize + sizeof(int)), rc);
    MessageParser msg(buffer);
    EXPECT_EQ(1, msg.messageId);
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
