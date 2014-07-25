/* log_level.h
   
   Copyright (C) 2012 Alexey Naydenov <alexey.naydenovREMOVETHIS@linux.com>
   
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

/*! \file log_level.h

  Types and object declarations for setting log level.

  Weather an object is stored or not is determined at compile time. To
  implement this functionality a set of structures is used. Structures
  are organized into a chain through inheritance. The base
  HiddenLogLevel corresponds to lowest priority and prevents from
  storing information in a log. This type is for internal use only,
  the lowest level that can be used by an application is
  User5LogLevel. This level is set by default. To restrict output pass
  one of the types as first template argument to vartrace constructor.

  The first argument of Log function is an object inherited from
  HiddenLogLevel. There are two implementations of Log function: one
  that take HiddenLogLevel and does nothing; the other accept an
  object of the type passed as first template argument of the vartrace
  constructor. Thus if one pass an object inherited from type used in
  the constructor the data will be saved.
*/

#ifndef TRUNK_INCLUDE_VARTRACE_LOG_LEVEL_H_
#define TRUNK_INCLUDE_VARTRACE_LOG_LEVEL_H_

namespace vartrace {
//! Log function that takes object of this type does nothing.
struct HiddenLogLevel {};
//! Log level tag struct.
struct User5LogLevel : public HiddenLogLevel {};
//! Log level tag struct.
struct User4LogLevel : public User5LogLevel {};
//! Log level tag stuct.
struct User3LogLevel : public User4LogLevel {};
//! Log level tag struct.
struct User2LogLevel : public User3LogLevel {};
//! Log level tag struct.
struct User1LogLevel : public User2LogLevel {};
//! Log level tag struct.
struct DebugLogLevel : public User1LogLevel {};
//! Log level tag struct.
struct InfoLogLevel : public DebugLogLevel {};
//! Log level tag struct.
struct WarningLogLevel : public InfoLogLevel {};
//! Log level tag struct.
struct ErrorLogLevel : public WarningLogLevel {};
//! Log level tag struct.
struct FatalLogLevel : public ErrorLogLevel {};

//! Log function called with this object does nothing.
extern HiddenLogLevel kHiddenLevel;
//! Object that should be used in Log function call to indicate log level.
extern User5LogLevel kUser5Level;
//! Object that should be used in Log function call to indicate log level.
extern User4LogLevel kUser4Level;
//! Object that should be used in Log function call to indicate log level.
extern User3LogLevel kUser3Level;
//! Object that should be used in Log function call to indicate log level.
extern User2LogLevel kUser2Level;
//! Object that should be used in Log function call to indicate log level.
extern User1LogLevel kUser1Level;
//! Object that should be used in Log function call to indicate log level.
extern DebugLogLevel kDebugLevel;
//! Object that should be used in Log function call to indicate log level.
extern InfoLogLevel kInfoLevel;
//! Object that should be used in Log function call to indicate log level.
extern WarningLogLevel kWarningLevel;
//! Object that should be used in Log function call to indicate log level.
extern ErrorLogLevel kErrorLevel;
//! Object that should be used in Log function call to indicate log level.
extern FatalLogLevel kFatalLevel;
}  // namespace vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_LOG_LEVEL_H_
