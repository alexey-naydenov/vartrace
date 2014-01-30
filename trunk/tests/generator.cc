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
  Utility to generate test file for vartools. 
*/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <cstdlib>
#include <string>
#include <iostream>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

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
    std::cout << desc << std::endl;
  }
  return args;
}

//! Strip string of specified characters.
std::string strip(const std::string &path, const std::string &chars=" \t\n\r") {
  std::size_t first_not_space = path.find_first_not_of(chars);
  if (first_not_space == std::string::npos) {
    return std::string();
  }
  std::size_t last_not_space = path.find_last_not_of(chars);
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
    std::cout << "ok" << std::endl;
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

int main(int argc, char *argv[]) {
  po::variables_map args = parse_commandline(argc, argv);
  if (args.count("help")) {
    return 0;
  }
  fs::path output_path(expand_tilde(args["directory"].as<std::string>()));
  if (!create_path(output_path)) {
    std::cerr << "ERROR: " << output_path.c_str() << " cannot be created"
              << std::endl;
    return 1;
  }


  return 0;
}
