/*$Id: io_error.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * Error handler.
 * Collection of functions to handle all types of errors
 * including user interrupts, system errors, overflow, etc.
 */
//testing=script,sparse 2006.07.17
#include "ap.h"
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
	void	error(int,const char*,...);
	void	error(int,const std::string&);
/*--------------------------------------------------------------------------*/
#if 0
Exception_CS::Exception_CS(const std::string& Message, const CS& cmd, unsigned cursor)
  :Exception(Message),
   _cmd(cmd.fullstring()),
   _cursor(cursor)
{untested();
}
#endif
/*--------------------------------------------------------------------------*/
Exception_CS::Exception_CS(const std::string& Message, const CS& cmd)
  :Exception(Message),
   _cmd(cmd.fullstring()),
   _cursor(cmd.cursor())
{itested();
}
/*--------------------------------------------------------------------------*/
const std::string Exception_CS::message()const
{itested();
  std::string s;
  if (_cursor < 40) {itested();
    s = _cmd.substr(0,60)
      + '\n' + std::string(_cursor, ' ') + "^ ? " + Exception::message();
  }else{untested();
    s = "... " + _cmd.substr(_cursor-36, 56)
      + "\n                                        ^ ? " + Exception::message();
  }
  return s;
}
/*--------------------------------------------------------------------------*/
/* error: error message printer
 * print it, if severe enough
 * terminate command, if really bad
 */
void error(int badness, const char* fmt, ...)
{
  if (badness >= OPT::picky) {
    char buffer[BIGBUFLEN] = "";
    va_list arg_ptr;
    va_start(arg_ptr,fmt);
    vsprintf(buffer,fmt,arg_ptr);
    va_end(arg_ptr);
    IO::error << buffer;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void error(int badness, const std::string& message)
{
  if (badness >= OPT::picky) {
    IO::error << message;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
