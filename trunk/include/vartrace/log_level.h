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

struct HiddenLogLevel {};
struct User5LogLevel : public HiddenLogLevel {};
struct User4LogLevel : public User5LogLevel {};
struct User3LogLevel : public User4LogLevel {};
struct User2LogLevel : public User3LogLevel {};
struct User1LogLevel : public User2LogLevel {};
struct DebugLogLevel : public User1LogLevel {};
struct InfoLogLevel : public DebugLogLevel {};
struct WarningLogLevel : public InfoLogLevel {};
struct ErrorLogLevel : public WarningLogLevel {};
struct FatalLogLevel : public ErrorLogLevel {};

extern HiddenLogLevel kHiddenLevel;
extern User5LogLevel kUser5Level;
extern User4LogLevel kUser4Level;
extern User3LogLevel kUser3Level;
extern User2LogLevel kUser2Level;
extern User1LogLevel kUser1Level;
extern DebugLogLevel kDebugLevel;
extern InfoLogLevel kInfoLevel;
extern WarningLogLevel kWarningLevel;
extern ErrorLogLevel kErrorLevel;
extern FatalLogLevel kFatalLevel;
}  // vartrace

#endif  // TRUNK_INCLUDE_VARTRACE_LOG_LEVEL_H_
