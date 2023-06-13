/*$Id: m_base_in.cc,v 26.125 2009/10/15 20:58:21 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 */
//testing=script,sparse 2009.08.13
#include "m_base.h"
/*--------------------------------------------------------------------------*/
void Float::parse(CS& File)
{
  if (File >> "NA") {
    _data = NOT_INPUT;
  }else{
    size_t here = File.cursor();
    File >> _data;
    if (File.stuck(&here)) {untested();
      _data = NOT_INPUT;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Name_String::parse(CS& File)
{
  File.skipbl();
  _data = "";
  if (File.is_pfloat()) {
    while (File.is_pfloat()) {
      _data += File.ctoc();
    }
    if (File.match1("eE")) {
      _data += File.ctoc();
      if (File.match1("+-")) {
	_data += File.ctoc();
      }else{
      }
      while (File.is_digit()) {
	_data += File.ctoc();
      }
    }else{
    }
    while (File.is_alpha()) {
      _data += File.ctoc();
    }
  }else{
    int bracket = 0;
    while (true) {
      if (File.is_alpha() || File.is_pfloat() || File.match1("_$")) {
      }else if (File.match1("[")) {
	++bracket;
      }else if (bracket && File.match1("]")) {
	--bracket;
      }else{
	break;
      }
      _data += File.ctoc();
    }
    if(bracket){
      File.warn(bDANGER, "missing ]?");
    }else{
    }
  }
  File.skipbl();
}
/*--------------------------------------------------------------------------*/
void Quoted_String::parse(CS& File)
{
  File.skipbl();
  size_t here = File.cursor();
  char quote = File.ctoc();
  _data = "";
  for (;;) {
    if (File.skip1(quote)) {
      break;
    }else if (!File.more()) {untested();
      File.warn(0, "end of file in quoted string");
      File.warn(0, here, "string begins here");
      break;
    }else{
      _data += File.ctoc();
    }
  }
  File.skipbl();
}
/*--------------------------------------------------------------------------*/
void Tail_String::parse(CS& File)
{untested(); // c_str
  static char end_marks[] = "\n";

  const char* begin = File.tail().c_str();
  File.skipto1(end_marks);
  const char* end = File.tail().c_str();
  assert(end >= begin);

  while ((--end >= begin) && (isspace(*end))) {untested();
  }
  ++end;
  assert(end >= begin);

  _data = std::string(begin, static_cast<size_t>(end-begin));
}
/*--------------------------------------------------------------------------*/
#if 0
void Text_Block::parse(CS& File)
{untested(); // c_str
  const char* begin = File.tail().c_str();
  for (;;) {untested();
    File.skipto1("\n").skip();
    if (File.peek() == '[') {untested();
      break;
    }else{untested();
    }
  }
  const char* end = File.tail().c_str();
  _data = std::string(begin, static_cast<size_t>(end-begin));
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
