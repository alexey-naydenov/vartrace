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

#include <boost/program_options.hpp>

#include <string>
#include <iostream>

namespace po = boost::program_options;

po::variables_map parse_commandline(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  po::variables_map args = parse_commandline(argc, argv);
  if (args.count("help")) {
    return 0;
  }

  return 0;
}

po::variables_map parse_commandline(int argc, char *argv[]) {
  po::options_description desc("Create binary files with sample vartraces.");
  desc.add_options()
      ("help,h", "produce help message")
      ("directory,d", po::value<std::string>()->default_value(""),
       "output directory, current directory by default");
  po::variables_map args;
  po::store(po::parse_command_line(argc, argv, desc), args);
  po::notify(args);
  if (args.count("help")) {
    std::cout << desc << std::endl;
  }
  return args;
}
