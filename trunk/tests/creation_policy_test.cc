/* creation_policy_test.cc
 *
 * Copyright (C) 2013 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
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

/*! \file creation_policy_test.cc 
  Testsuite for creation policies. 
*/

#include <gtest/gtest.h>

#include <vartrace/vartrace.h>

namespace vt = vartrace;

using vartrace::VarTrace;
using vartrace::kInfoLevel;

class ValueCreationTest : public ::testing::Test {
 public:
  typedef VarTrace<vt::User5LogLevel, vt::ValueCreator,
                   vt::SingleThreaded, vt::SharedArrayAllocator> ValueVartrace;
};