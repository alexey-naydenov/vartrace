/* profile_double.cc
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

/*! \file profile_double.cc 
  Program for testing performance of logging double variables.
*/

#include <vartrace/vartrace.h>

namespace vt = vartrace;

int main(int argc, char *argv[]) {
  // the number shown by test routine is time in nanoseconds per log operation
  int log_length = 100000000;
  int trace_size = 0x1000;
  int block_count = 4;

  vt::VarTrace<>::Pointer trace =
      vt::VarTrace<>::Create(trace_size, block_count);

  for (double i = 0; i < log_length; ++i) {
    trace->Log(1, i);
  }

  return 0;
}
