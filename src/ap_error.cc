/*$Id: ap_error.cc,v 26.122 2009/09/23 11:23:50 al Exp $ -*- C++ -*-
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
 */
//testing=script 2006.07.17
#include "u_opt.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
//	CS &	CS::check(int i, const string& s);
//	CS &	CS::warn(int i, int c, const string& s);
/*--------------------------------------------------------------------------*/
/* syntax_check: handle syntax errors
 * called on parsing an input string when nothing else matches.
 * if the rest of the line is nothing, just return
 * if comment, increment *cnt, so what is left is a valid comment string
 * otherwise, it is an error (the arrow pointing at the offending word)
 */
CS & CS::check(int badness, const std::string& message)
{
  skipbl();
  switch (peek()) {
  case '/': _ok = umatch("//"); skip(); break;
  case ';':
  case '\'':	itested(); _ok = true;  skip();     break;
  case '\0':		    _ok = true;		     break;
  default:	_ok = false; warn(badness, message); break;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* syntax_msg: print syntax error message
 * echo a portion of the input with an arrow pointing to the current place
 */
CS & CS::warn(int badness, unsigned spot, const std::string& message)
{
  if (badness >= OPT::picky) {
    if (spot < 40) {
      IO::error << _cmd.substr(0,70) << '\n';
      IO::error.tab(spot);
    }else{
      IO::error << _cmd.substr(0,15) << " ... " << _cmd.substr(spot-20, 56) << '\n';
      IO::error.tab(40);
    }
    IO::error << "^ ? " + message + '\n';
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
