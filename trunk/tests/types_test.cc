/* types_test.cc
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

/*! \file types_test.cc 
  Test logging different pods, structures and arrays.
*/

#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

#include <vartrace/vartrace.h>
#include <vartrace/messageparser.h>

#include <limits>

using vartrace::VarTrace;
using vartrace::kInfoLevel;
using vartrace::Message;

//! Test class for pods and array logging testing.
class TypesTest : public ::testing::Test {
 public:
  boost::shared_ptr<VarTrace<> > trace;
};

//! Overflow trace with integers and check result.
TEST_F(TypesTest, LogIntegersTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  int message_size = vartrace::kHeaderSize + sizeof(vartrace::AlignmentType);
  int message_length = 3;
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // write 5 times the trace capacity
  for (int i = 0; i < 5*trace_size/message_size; ++i) {
    trace->Log(kInfoLevel, 2*i, 3*i);
    // dump trace
    int dumped_size = trace->DumpInto(buffer.get(), buffer_size);
    ASSERT_GE(buffer_size, dumped_size);
    // if less then trace capacity of data was written
    if (i < 3*trace_size/message_size/4) {
      // check total dumped size of the dump in the predictable case
      ASSERT_EQ((i+1)*message_size, dumped_size);
      // first logged message must be present
      ASSERT_EQ(0, buffer[0]); // timestamp
      ASSERT_EQ(0, (buffer[1]>>16) & 0xff); // message id
      ASSERT_EQ(0, buffer[2]); // value
    }
    // check all dumped messaged
    for (int j = 0; j < dumped_size/message_size; ++j) {
      // timestamp is the message counter
      int message_counter = buffer[j*message_length];
      // check size of the message
      ASSERT_EQ(sizeof(i), buffer[j*message_length + 1] & 0xffff);
      // check message id of the message
      ASSERT_EQ((2*message_counter)%256,
                (buffer[j*message_length + 1]>>16) & 0xff);
      // check data type of the message
      ASSERT_EQ(vartrace::DataType2Int<int>::id,
                (buffer[j*message_length + 1]>>24) & 0xff);
      // check the value of the message
      ASSERT_EQ(3*message_counter, buffer[j*message_length + 2]);
    }
  }
}

//! Check correct logging of short POD types..
TEST_F(TypesTest, LogDifferentTypesTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // log different types
  char c = 1;
  trace->Log(kInfoLevel, 1, c);
  int16_t s = 2;
  trace->Log(kInfoLevel, 2, s);
  uint16_t us = 3;
  trace->Log(kInfoLevel, 3, us);
  int i = 4;
  trace->Log(kInfoLevel, 4, i);
  unsigned ui = 5;
  trace->Log(kInfoLevel, 5, ui);
  // check logged dumped values
  trace->DumpInto(buffer.get(), buffer_size);
  // parse dumped data
  int offset = 0;
  // check char message
  Message cmsg(&buffer[offset]);
  offset += cmsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(1, cmsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<char>::id, cmsg.data_type_id());
  ASSERT_EQ(sizeof(c), cmsg.data_size());
  // check short message
  Message smsg(&buffer[offset]);
  offset += smsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(2, smsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<int16_t>::id, smsg.data_type_id());
  ASSERT_EQ(sizeof(s), smsg.data_size());
  // check unsigned short message
  Message usmsg(&buffer[offset]);
  offset += usmsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(3, usmsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<uint16_t>::id, usmsg.data_type_id());
  ASSERT_EQ(sizeof(us), usmsg.data_size());
  // check int message
  Message imsg(&buffer[offset]);
  offset += imsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(4, imsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<int>::id, imsg.data_type_id());
  ASSERT_EQ(sizeof(i), imsg.data_size());
  // check unsigned int message
  Message uimsg(&buffer[offset]);
  offset += uimsg.message_size()/sizeof(vartrace::AlignmentType);
  ASSERT_EQ(5, uimsg.message_type_id());
  ASSERT_EQ(vartrace::DataType2Int<unsigned>::id, uimsg.data_type_id());
  ASSERT_EQ(sizeof(ui), uimsg.data_size());
}

//! Check that max/min int values stored and restored correctly.
TEST_F(TypesTest, LogIntLimitsTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // init and log int values
  int minint = std::numeric_limits<int>::min();
  int maxint = std::numeric_limits<int>::max();
  unsigned maxuint = std::numeric_limits<unsigned>::max();
  // log values
  trace->Log(kInfoLevel, 1, minint);
  trace->Log(kInfoLevel, 2, maxint);
  trace->Log(kInfoLevel, 3, maxuint);
  // parse trace
  trace->DumpInto(buffer.get(), buffer_size);
  int offset = 0;
  Message minmsg(&buffer[offset]);
  offset += minmsg.message_size()/sizeof(vartrace::AlignmentType);
  Message maxmsg(&buffer[offset]);
  offset += maxmsg.message_size()/sizeof(vartrace::AlignmentType);
  Message umaxmsg(&buffer[offset]);
  // check results
  ASSERT_EQ(minint, minmsg.value<int>());
  ASSERT_EQ(maxint, maxmsg.value<int>());
  ASSERT_EQ(maxuint, umaxmsg.value<unsigned>());
  // try to get max int through unsigned template
  ASSERT_EQ(maxint, maxmsg.value<unsigned>());
}

//! Check logging types longer then int.
TEST_F(TypesTest, MultipleAssignmentTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // initialize some long variables
  int64_t lmin = std::numeric_limits<int64_t>::min();
  int64_t lmax = std::numeric_limits<int64_t>::max();
  uint64_t ulmax = std::numeric_limits<uint64_t>::max();
  float f = 0.123456e-22;
  double d = 0.789123e-44;
  // log everything
  trace->Log(kInfoLevel, 1, lmin);
  trace->Log(kInfoLevel, 2, lmax);
  trace->Log(kInfoLevel, 3, ulmax);
  trace->Log(kInfoLevel, 7, f);
  trace->Log(kInfoLevel, 8, d);
  // parse trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check stored size
  ASSERT_EQ(sizeof(lmin), vt[0]->data_size());
  ASSERT_EQ(sizeof(f), vt[3]->data_size());
  ASSERT_EQ(sizeof(d), vt[4]->data_size());
  // check values
  ASSERT_EQ(lmin, vt[0]->value<int64_t>());
  ASSERT_EQ(lmax, vt[1]->value<int64_t>());
  ASSERT_EQ(ulmax, vt[2]->value<uint64_t>());
  ASSERT_EQ(f, vt[3]->value<float>());
  ASSERT_EQ(d, vt[4]->value<double>());
}

//! Check char array logging.
TEST_F(TypesTest, LogCharArrayTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create an array and log it
  char anarray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  trace->Log(kInfoLevel, 1, anarray);
  // dump trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check message
  ASSERT_EQ(vartrace::DataType2Int<char[1]>::id, vt[0]->data_type_id());
  ASSERT_EQ(1, vt[0]->message_type_id());
  ASSERT_EQ(sizeof(anarray), vt[0]->data_size());
  char *logged_data = vt[0]->pointer<char>();
  for (size_t i = 0; i < sizeof(anarray)/sizeof(anarray[0]); ++i) {
    ASSERT_EQ(anarray[i], logged_data[i]);
  }
}

//! Check multiple char array logging.
TEST_F(TypesTest, LogMultipleCharArrayTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>(trace_size));
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create an array and log it 100 times
  char anarray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  for (int i = 0; i < 1000; ++i) {
    trace->Log(kInfoLevel, i, anarray);
    // dump trace
    size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
    vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
    // first message id
    int message_id =  vt[0]->message_type_id();
    // check messages
    for (std::vector<vartrace::Message::Pointer>::const_iterator pos
             = vt.messages().begin(); pos != vt.messages().end(); ++pos) {
      ASSERT_EQ(vartrace::DataType2Int<char[1]>::id, (*pos)->data_type_id());
      ASSERT_EQ(message_id, (*pos)->message_type_id());
      message_id += 1;
      message_id = message_id%256;
      ASSERT_EQ(sizeof(anarray), (*pos)->data_size());
      char *logged_data = (*pos)->pointer<char>();
      for (size_t i = 0; i < sizeof(anarray)/sizeof(anarray[0]); ++i) {
        ASSERT_EQ(anarray[i], logged_data[i]);
      }
    }
  }
}

//! Check correct logging of an array of doubles.
TEST_F(TypesTest, LogDoubleArrayTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create an array and log it
  double anarray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  trace->Log(kInfoLevel, 1, anarray);
  // dump trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check message
  ASSERT_EQ(vartrace::DataType2Int<double[1]>::id, vt[0]->data_type_id());
  ASSERT_EQ(1, vt[0]->message_type_id());
  ASSERT_EQ(sizeof(anarray), vt[0]->data_size());
  double *logged_data = vt[0]->pointer<double>();
  for (size_t i = 0; i < sizeof(anarray)/sizeof(anarray[0]); ++i) {
    ASSERT_EQ(anarray[i], logged_data[i]);
  }
}

//! Structure for testing custom type logging.
struct LogTestStructure {
  char cvar;
  int ivar;
  double dvar;
  char anarray[13];
};

//! Define data type id for custom structure.
VARTRACE_SET_TYPEID(LogTestStructure, 40);

//! Check if a structure is stored correctly along with its data type id.
TEST_F(TypesTest, LogCustomStructureTest) {
  int trace_size = 0x100;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create and fill some custom structure
  LogTestStructure ts;
  ts.cvar = 'a';
  ts.ivar = 12345;
  ts.dvar = 12e-34;
  for (size_t i = 0; i < sizeof(ts.anarray)/sizeof(ts.anarray[0]); ++i) {
    ts.anarray[i] = 2*i;
  }
  // log structure
  trace->Log(kInfoLevel, 11, ts);
  // dump and parse trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check message parameters
  ASSERT_EQ(vartrace::DataType2Int<LogTestStructure>::id,
            vt[0]->data_type_id());
  ASSERT_EQ(40, vt[0]->data_type_id());
  ASSERT_EQ(11, vt[0]->message_type_id());
  ASSERT_EQ(sizeof(LogTestStructure), vt[0]->data_size());
  // check parsed structure content
  LogTestStructure *result = vt[0]->pointer<LogTestStructure>();
  ASSERT_EQ(ts.cvar, result->cvar);
  ASSERT_EQ(ts.ivar, result->ivar);
  ASSERT_EQ(ts.dvar, result->dvar);
  for (size_t i = 0; i < sizeof(ts.anarray)/sizeof(ts.anarray[0]); ++i) {
    ASSERT_EQ(ts.anarray[i], result->anarray[i]);
  }
}

//! Check logging of array of structures.
TEST_F(TypesTest, LogCustomStructureArrayTest) {
  int trace_size = 0x1000;
  int buffer_length = trace_size/sizeof(vartrace::AlignmentType);
  int buffer_size = buffer_length*sizeof(vartrace::AlignmentType);
  trace = boost::shared_ptr<VarTrace<> >(new VarTrace<>());
  boost::shared_array<vartrace::AlignmentType> buffer(
      new vartrace::AlignmentType[buffer_length]);
  // create and fill some custom structure
  const unsigned kArrayLength = 7;
  LogTestStructure static_array[kArrayLength];
  boost::shared_array<LogTestStructure> shared_array(
      new LogTestStructure[kArrayLength]);
  for (unsigned i = 0; i < kArrayLength; ++i) {
    static_array[i].cvar = 2*i;
    static_array[i].ivar = 2*i*i;
    shared_array[i].cvar = i;
    shared_array[i].ivar = i*i;
  }
  // log static array of structures
  trace->Log(kInfoLevel, 11, static_array);
  // log pointer to a structure, 1 structure stored by default
  trace->Log(kInfoLevel, 12, shared_array.get(), 1);
  // log dynamically allocated array of structures
  trace->Log(kInfoLevel, 13, shared_array.get(), kArrayLength);
  // dump and parse trace
  size_t dumped_size = trace->DumpInto(buffer.get(), buffer_size);
  vartrace::ParsedVartrace vt(buffer.get(), dumped_size);
  // check static array message parameters
  ASSERT_EQ(vartrace::DataType2Int<LogTestStructure>::id,
            vt[0]->data_type_id());
  ASSERT_EQ(sizeof(static_array), vt[0]->data_size());
  ASSERT_EQ(sizeof(LogTestStructure), vt[1]->data_size());
  ASSERT_EQ(kArrayLength*sizeof(LogTestStructure), vt[2]->data_size());
  // check logged values of static array
  LogTestStructure *pointer = vt[0]->pointer<LogTestStructure>();
  for (unsigned i = 0; i < kArrayLength; ++i) {
    ASSERT_EQ(2*i, pointer[i].cvar);
    ASSERT_EQ(2*i*i, pointer[i].ivar);
  }
  // check logged values of dynamic array
  pointer = vt[2]->pointer<LogTestStructure>();
  for (unsigned i = 0; i < kArrayLength; ++i) {
    ASSERT_EQ(i, pointer[i].cvar);
    ASSERT_EQ(i*i, pointer[i].ivar);
  }
}


