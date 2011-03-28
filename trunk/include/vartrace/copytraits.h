/* copytraits.h
   
   Copyright (C) 2011 Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file copytraits.h 
  Describe copy mechanizm for different types. 
*/

#ifndef TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_
#define TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_

#include "vartrace/tracetypes.h"
#include "vartrace/datatypeid.h"

namespace vartrace {
/// Sizeof memory segment should be copied into the trace.
struct SizeofCopyTag {};

/// Object will copy itself into the trace.
struct SelfCopyTag : public SizeofCopyTag {};

/// Object is a tuple, create subtrace.
struct TupleCopyTag : public SizeofCopyTag {};

/// Object can be copied through an assignment.
struct AssignmentCopyTag : public SizeofCopyTag {};

/// Default behaviour for adding an object into a trace.
template<typename T> struct CopyTraits {
    typedef SizeofCopyTag CopyCategory;
};
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_

