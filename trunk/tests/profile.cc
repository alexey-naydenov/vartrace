/* profile.cc
 *
 * Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
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

/*! \file profile.cc 
  Measure logging time of PODs, arrays and self logging classes.
*/

#include <iostream>
#include <iomanip>

#include <vartrace/vartrace.h>
#include <profile_utils.h>

using std::cout;
using std::endl;

using vartrace::VarTrace;
using vartrace::kInfoLevel;

//! Macro that create test logging time of a type and prints to cout.
#define MEASURE_TYPE(type, count) do {                                  \
    type val{};                                                         \
    cout << std::setw(20) << #type << " "                               \
         << LogTimeToString(val, count, &trace) << endl;                \
  } while (false)

//! Class for measure logging time of self logging class.
class SelfLogging {
 public:
  int ivar1; //!< Variable that is logged.
  int ivar2; //!< Unused variable.
  int ivar3; //!< Unused variable.
  //! Function that stores class in a log.
  void LogItself(VarTrace<> *trace) const {
    trace->Log(kInfoLevel, 101, ivar1);
  }
};
//! Structure for measuring logging time of 8 chars.
struct CharArray8 {
  //! Profile data array.
  char cs[8];
};
//! Structure for measuring logging time of 16 chars.
struct CharArray16 {
  //! Profile data array.
  char cs[16];
};
//! Structure for measuring logging time of 32 chars.
struct CharArray32 {
  //! Profile data array.
  char cs[32];
};
//! Structure for measuring logging time of 64 chars.
struct CharArray64 {
  //! Profile data array.
  char cs[64];
};
//! Register SelfLogging as self logging class.
VARTRACE_SET_SELFLOGGING(SelfLogging);

//! Measure and print logging time of PODs, arrays and self logging objects.
int main(int argc, char *argv[]) {
  int trace_size = 0x1000;
  int block_count = 4;
  std::size_t repetition_count = 1<<30;

  VarTrace<> trace(trace_size, block_count);

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



