/* log_level.h
   Copyright (C) 2012 Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
   
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
  Structures and objects that allow setting log levels for traces.
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
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_LOG_LEVEL_H_
