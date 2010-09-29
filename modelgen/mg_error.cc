/*$Id: mg_error.cc,v 25.92 2006/06/28 15:03:12 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
#include "ap.h"
extern int errorcount;
/*--------------------------------------------------------------------------*/
void error(int, const std::string& message)
{
  std::cout << message << '\n';
  exit(2);
}
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
    case '\'':	_ok = true;  skip();	   break;
    case '\0':	_ok = true;		   break;
    default:	_ok = false; warn(badness, message); break;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
static void tab(int n)
{
  for (int i=0; i<n; ++i) {
    std::cout << ' ';
  }
}
/*--------------------------------------------------------------------------*/
/* syntax_msg: print syntax error message
 * echo a portion of the input with an arrow pointing to the current place
 */
CS & CS::warn(int badness, int spot, const std::string& message)
{
  if (badness >= 0) {
    ++errorcount;
    int linestart = spot;
    assert(linestart >= 0);
    for (;;) {
      {if (linestart == 0) {
	break;
      }else if (_cmd[linestart] == '\n') {
	++linestart;
	break;
      }else{
	--linestart;
      }}
    }
    int lineno = 1;
    for (int i=0; i<linestart; ++i) {
      if (_cmd[i] == '\n') {
	++lineno;
      }
    }
    std::cout << _name << ':' << lineno << ":\n";
    {if (spot-linestart < 20) {
      for (int i=linestart; _cmd[i] && _cmd[i]!='\n'; ++i) {
	std::cout << _cmd[i];
      }
      std::cout << '\n';
      tab(spot-linestart);
    }else{
      std::cout << "..";
      for (int i=spot-15; _cmd[i] && _cmd[i]!='\n'; ++i) {
	std::cout << _cmd[i];
      }
      std::cout << '\n';
      tab(17);
    }}
    std::cout << "^ ? " << message << '\n';
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
