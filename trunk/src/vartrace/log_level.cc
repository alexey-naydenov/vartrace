/* log_level.cc
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

/*! \file log_level.cc
  Objects used for setting log level of a message.
*/

#include <vartrace/log_level.h>

namespace vartrace {
HiddenLogLevel kHiddenMessage;
User5LogLevel kUser5Message;
User4LogLevel kUser4Message;
User3LogLevel kUser3Message;
User2LogLevel kUser2Message;
User1LogLevel kUser1Message;
DebugLogLevel kDebugMessage;
InfoLogLevel kInfoMessage;
WarningLogLevel kWarningMessage;
ErrorLogLevel kErrorMessage;
FatalLogLevel kFatalMessage;
}  // vartrace
