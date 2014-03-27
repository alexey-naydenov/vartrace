/* profile_subtrace.cc
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

/*! \file profile_subtrace.cc 
  Program for profiling subtrace creation.
*/

#include <vartrace/vartrace.h>

using vartrace::VarTrace;
using vartrace::kInfoLevel;

namespace test {
class SelfLogClass {
 public:
  int ivar1;
  int ivar2;
  int ivar3;

  void LogItself(VarTrace<>::Pointer trace) const {
    trace->Log(kInfoLevel, 101, ivar1);
    trace->Log(kInfoLevel, 102, ivar2);
    trace->Log(kInfoLevel, 103, ivar3);
  }
};
}  // namespace test

VARTRACE_SET_SELFLOGGING(test::SelfLogClass);

int main(int argc, char *argv[]) {
  // the number shown by test routine is time in nanoseconds per log operation
  int log_length = 100000000;
  int trace_size = 0x1000;
  int block_count = 4;

  VarTrace<>::Handle trace = VarTrace<>::Create(trace_size, block_count);

  test::SelfLogClass obj;
  for (int i = 0; i < log_length; ++i) {
    obj.ivar1 = i;
    obj.ivar2 = i;
    obj.ivar3 = i;
    trace->Log(kInfoLevel, 1, obj);
  }

  return 0;
}

