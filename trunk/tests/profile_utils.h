//! \file profile_utils.h

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

//!  \brief Function for measuring logging time.

#ifndef TRUNK_TESTS_PROFILE_UTILS_H_
#define TRUNK_TESTS_PROFILE_UTILS_H_

#include <cmath>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <vartrace/vartrace.h>

typedef std::pair<std::string, double> NameNsRatio;

std::vector<NameNsRatio> kNameToRatio {
  NameNsRatio{"s", 1e9}, NameNsRatio{"ms", 1e6},
  NameNsRatio{"us", 1e3}, NameNsRatio{"ns", 1}};

std::string kSeparator{" "};

template <class Duration>
std::string DurationToString(const Duration &duration, std::size_t count) {
  std::ostringstream repr;
  repr << std::setprecision(3);
  double time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
      duration).count();
  time_ns /= count;
  for (auto name_ratio: kNameToRatio) {
    double time_in_units = time_ns/name_ratio.second;
    if (time_in_units < 1) {
      continue;
    }
    if (time_in_units < 100) {
      repr << time_in_units << kSeparator << name_ratio.first;
      break;
    }
    if (time_in_units < 1000) {
      repr << std::floor(time_in_units) << kSeparator << name_ratio.first;
      break;
    }
  }
  if (!repr.tellp()) {
    repr << time_ns << kSeparator << "ns";
  }
  return repr.str();
}

template <class L, typename D>
std::string LogTimeToString(const D &object, std::size_t count, L *logger) {
  auto begin = std::chrono::high_resolution_clock::now();
  for (std::size_t i = 0; i < count; ++i) {
    logger->Log(vartrace::kInfoLevel, 1, object);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = end - begin;
  return DurationToString(duration, count);
}

#endif  // TRUNK_TESTS_PROFILE_UTILS_H_
