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

  Traits that select copy mechanism for different types.
  VarTrace::Log() template function calls appropriate overloaded
  VarTrace::DoLog() function that does copying. The logging function
  is chosen based on type trait. If the template structure CopyTraits
  is not specialized for a type then objects of that type are copied
  through memory copy. Logging of small POD types are sped up by using
  assignment. User defined types can be copied through user defined
  function.

 */

#ifndef TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_
#define TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_

#include <vector>
#include <string>

#include <vartrace/tracetypes.h>
#include <vartrace/datatypeid.h>

namespace vartrace {
//! Select default logging mechanism through memcopy.
struct SizeofCopyTag {};

//! Object is stored by calling LogItself member function.
struct SelfCopyTag : public SizeofCopyTag {};

//! Object is copied into a log by function vartrace::Log.
struct CustomCopyTag : public SizeofCopyTag {};

//! Object is copied by using assignment.
struct AssignmentCopyTag : public SizeofCopyTag {};

//! Default implementation, memory occupied by an object copied into a trace.
template<typename T, typename U = void> struct CopyTraits {
  //! Tag typedef.
  typedef SizeofCopyTag CopyCategory;
};

//! Macro that simplifies setting assignment copy trait.
#define VARTRACE_SET_ASSIGNMENTCOPY(Type)                         \
  template<> struct CopyTraits<Type> {                            \
    typedef AssignmentCopyTag CopyCategory;                       \
  }

//! Specialize CopyTraits to copy int8_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(int8_t);
//! Specialize CopyTraits to copy uint8_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(uint8_t);
//! Specialize CopyTraits to copy int16_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(int16_t);
//! Specialize CopyTraits to copy uint16_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(uint16_t);
//! Specialize CopyTraits to copy int32_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(int32_t);
//! Specialize CopyTraits to copy uint32_t through assignment.
VARTRACE_SET_ASSIGNMENTCOPY(uint32_t);
}  // vartrace

//! Macro for setting self logging policy for custom types.
#define VARTRACE_SET_SELFLOGGING(Type)                  \
  namespace vartrace {                                  \
  template<> struct CopyTraits<Type> {                  \
    typedef SelfCopyTag CopyCategory;                   \
  };                                                    \
  }

//! Macro that specializes trait so that a type is copied by calling a function.
#define VARTRACE_SET_LOG_FUNCTION(Type)                 \
  namespace vartrace {                                  \
  template<> struct CopyTraits<Type> {                  \
    typedef CustomCopyTag CopyCategory;                 \
  };                                                    \
  }

#endif  // TRUNK_INCLUDE_VARTRACE_COPYTRAITS_H_

