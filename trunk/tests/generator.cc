/* generator.cc
 *
 * Copyright (C) 2014 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
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

/*! \file generator.cc 
  Utility to generate test files for vartools. 
*/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <vartrace/vartrace.h>

#include <stdint.h>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::size_t;

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace vt = vartrace;

const size_t kTraceSize = 0x10000;
const size_t kDumpBufferSize = kTraceSize;

#define GENERATORS                               \
  ADD(empty)                                     \
  ADD(integer_count_1000)                        \
  ADD(assorted_types)                            \
  ADD(arrays_10_100_1000)                        \


//! Generate empty trace.
void empty(const vt::VarTrace<>::Pointer trace) {
}

//! Generate integer count from 0 to 999, mesage id 1.
void integer_count_1000(const vt::VarTrace<>::Pointer trace) {
  for (size_t i = 0; i < 1000; ++i) {
    trace->Log(vt::kInfoLevel, 1, i);
  } // loop i
}

//! Log min, 0, 123, max in the trace.
template <typename T>
uint8_t log_type_samples(const vt::VarTrace<>::Pointer trace, uint8_t id) {
  T value;
  value = std::numeric_limits<T>::min();
  trace->Log(vt::kInfoLevel, id++, value);
  value = 0;
  trace->Log(vt::kInfoLevel, id++, value);
  value = 123;
  trace->Log(vt::kInfoLevel, id++, value);
  value = std::numeric_limits<T>::max();
  trace->Log(vt::kInfoLevel, id++, value);
  return id;
}

//! Log different possible types 4 times: min, 0, 123, max (incremental id).
void assorted_types(const vt::VarTrace<>::Pointer trace) {
  uint8_t id = 0;
  id = log_type_samples<int8_t>(trace, id);
  id = log_type_samples<uint8_t>(trace, id);
  id = log_type_samples<int16_t>(trace, id);
  id = log_type_samples<uint16_t>(trace, id);
  id = log_type_samples<int32_t>(trace, id);
  id = log_type_samples<uint32_t>(trace, id);
  id = log_type_samples<int64_t>(trace, id);
  id = log_type_samples<uint64_t>(trace, id);
  id = log_type_samples<float>(trace, id);
  id = log_type_samples<double>(trace, id);
}

//! Log an array of given size that contains counter.
template <typename T>
void log_counter_array(const vt::VarTrace<>::Pointer trace, uint8_t id,
                       size_t length) {
  std::vector<T> array(length);
  for (std::size_t i = 0; i != array.size(); ++i) {
    array[i] = i;
  } // loop i
  cout << array.size() << endl;
  trace->Log(vt::kInfoLevel, id, &array[0], array.size());
}

void arrays_10_100_1000(const vt::VarTrace<>::Pointer trace) {
  log_counter_array<int>(trace, 0, 10);
  log_counter_array<int>(trace, 1, 100);
  log_counter_array<int>(trace, 2, 1000);
}

//! Buffer for trace serialization.
char dump_buffer[kDumpBufferSize];

typedef void (*SampleGenerator)(const vt::VarTrace<>::Pointer);

//! Fake generator, placed last in #kGenerators array.
void end_generator(const vt::VarTrace<>::Pointer trace) {
  assert(false);
}

#define ADD(x) 1 +
//! Number of functions for generating vartrace samples.
size_t kGeneratorsCount = GENERATORS 0;
#undef ADD
#define ADD(x) x,
//! Array of sample generation functions.
SampleGenerator kGenerators[] = {GENERATORS end_generator};
#undef ADD
#define ADD(x) #x,
//! Array of names of sample generation functions, used to name files.
const char *kGeneratorNames[] = {GENERATORS "end_generator"};
#undef ADD

//! Define and parse command line arguments, process help option.
po::variables_map parse_commandline(int argc, char *argv[]) {
  po::options_description desc("Create binary files with sample vartraces");
  desc.add_options()
      ("help,h", "produce help message")
      ("directory,d", po::value<std::string>()->default_value("."),
       ("output directory (tilde is expanded to home dir),"
        " current directory by default"));
  po::variables_map args;
  po::store(po::parse_command_line(argc, argv, desc), args);
  po::notify(args);
  if (args.count("help")) {
    cout << desc << endl;
  }
  return args;
}

//! Strip string of specified characters.
std::string strip(const std::string &path, const std::string &chars=" \t\n\r") {
  size_t first_not_space = path.find_first_not_of(chars);
  if (first_not_space == std::string::npos) {
    return std::string();
  }
  size_t last_not_space = path.find_last_not_of(chars);
  return path.substr(first_not_space, last_not_space - first_not_space + 1);
}

//! Remove spaces and replace tilde with home path.
std::string expand_tilde(const std::string &path) {
  std::string expanded_path(strip(path));
  if (expanded_path.empty() || expanded_path[0] != '~') {
    return expanded_path;
  }
  return expanded_path.replace(0, 1, std::getenv("HOME"));
}

//! Create path if it does not exist.
bool create_path(const fs::path &path) {
  if (path.empty()) {
    return true;
  }
  if (fs::exists(path)) {
    if (fs::is_directory(path)) {
      return true;
    } else {
      return false;
    }
  }
  if (!create_path(path.parent_path())) {
    return false;
  }
  return fs::create_directory(path);
}

//! Create trace, use generator to fill it and dump into file at given path.
bool generate(const std::string &file_path, SampleGenerator sample_generator) {
  cout << "Generating " << file_path << " ...\n";
  vt::VarTrace<>::Pointer trace =  vt::VarTrace<>::Create(kTraceSize);
  sample_generator(trace);
  size_t dumped_size = trace->DumpInto(dump_buffer, kDumpBufferSize);
  assert(dumped_size <= kDumpBufferSize);
  std::ofstream outfile(file_path.c_str(), std::ios::trunc | std::ios::binary);
  outfile.write(dump_buffer, dumped_size);
  outfile.close();
  return true;
}

int main(int argc, char *argv[]) {
  po::variables_map args = parse_commandline(argc, argv);
  if (args.count("help")) {
    return 0;
  }
  fs::path output_path(expand_tilde(args["directory"].as<std::string>()));
  if (!create_path(output_path)) {
    cerr << "ERROR: " << output_path.c_str() << " cannot be created"
         << endl;
    return 1;
  }
  for (size_t i = 0; i != kGeneratorsCount; ++i) {
    fs::path file_path(output_path);
    file_path /= kGeneratorNames[i];
    file_path.replace_extension("bin");
    if (!generate(file_path.string(), kGenerators[i])) {
      cerr << "ERROR: generation failed" << endl;
      return 1;
    }
  } // loop i
  return 0;
}
