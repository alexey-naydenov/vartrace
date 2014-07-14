//! \file profile_int.cc

// Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//! \brief Program for profiling using operf.

#include <stdint.h>

#include <vartrace/vartrace.h>

using vartrace::VarTrace;
using vartrace::kInfoLevel;

int main() {
  uint32_t value = 123;
  VarTrace<> trace = VarTrace<>(0x10000, 4);
  for (std::size_t i = 0; i < 100000000; ++i) {
    trace.Log(kInfoLevel, 1, value);
  }

  return 0;
}


