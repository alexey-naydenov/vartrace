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

#define MEASURE_TYPE(type) do {                                         \
    type val{};                                                         \
    cout << "\t" << std::setw(10) << #type << " "                       \
        << LogTimeToString(val, trace) << endl;                         \
  } while (false)

int main(int argc, char *argv[]) {
  int trace_size = 0x1000;
  int block_count = 4;

  VarTrace<>::Handle trace = VarTrace<>::Create(trace_size, block_count);

  cout << "Logging times:" << endl;
  MEASURE_TYPE(int8_t);
  MEASURE_TYPE(int32_t);
  MEASURE_TYPE(int64_t);
  MEASURE_TYPE(double);

  return 0;
}



