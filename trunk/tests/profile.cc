/* profile_int.cc
 *
 * Copyright (C) 2012 Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file profile_int.cc 
  Log many integers to measure library performance. 
*/

#include <iostream>
#include <iomanip>

#include <vartrace/vartrace.h>
#include <profile_utils.h>

using std::cout;
using std::endl;

using vartrace::VarTrace;
using vartrace::kInfoLevel;

#define MEASURE_TYPE(type, count) do {                                  \
    type val{};                                                         \
    cout << std::setw(20) << #type << " "                               \
         << LogTimeToString(val, count, &trace) << endl;                \
  } while (false)

class SelfLogging {
 public:
  int ivar1;
  int ivar2;
  int ivar3;

  void LogItself(VarTrace<> *trace) const {
    trace->Log(kInfoLevel, 101, ivar1);
  }
};

struct CharArray8 {
  char cs[8];
};

struct CharArray16 {
  char cs[16];
};

struct CharArray32 {
  char cs[32];
};

struct CharArray64 {
  char cs[64];
};

VARTRACE_SET_SELFLOGGING(SelfLogging);

int main(int argc, char *argv[]) {
  int trace_size = 0x1000;
  int block_count = 4;
  std::size_t repetition_count = 1<<30;

  VarTrace<> trace = VarTrace<>(trace_size, block_count);

  cout << "Logging times:" << endl;

  MEASURE_TYPE(int8_t, repetition_count);
  MEASURE_TYPE(int32_t, repetition_count);
  MEASURE_TYPE(int64_t, repetition_count);
  MEASURE_TYPE(float, repetition_count);
  MEASURE_TYPE(double, repetition_count);
  cout << endl;
  MEASURE_TYPE(CharArray8, repetition_count);
  MEASURE_TYPE(CharArray16, repetition_count);
  MEASURE_TYPE(CharArray32, repetition_count);
  MEASURE_TYPE(CharArray64, repetition_count);
  cout << endl;
  MEASURE_TYPE(SelfLogging, repetition_count);

  return 0;
}



