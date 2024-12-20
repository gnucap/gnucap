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
#include "m_base_vams.h"
/*--------------------------------------------------------------------------*/
void Integer::parse(CS& File)
{
  if (File >> "NA") { untested();
    _input = false;
  }else{
    size_t here = File.cursor();
    File >> _data;
    if (File.stuck(&here)) {
      _input = false;
    }else if(File.peek() == '.'){ untested();
      trace1("Integer?", File.tail());
      // reject float literal.
      _input = false;
      File.reset_fail(here);
    }else{
      _input = true;
    }
  }
}
/*--------------------------------------------------------------------------*/
void Float::parse(CS& File)
{
  if (File >> "NA") {
    _data = ::NOT_INPUT;
  }else{
    size_t here = File.cursor();
    File >> _data;
    if (File.stuck(&here)) {
      _data = ::NOT_INPUT;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Name_String::parse(CS& File)
{
  File.skipbl();
  std::string data;
  if (File.is_pfloat()) {
    while (File.is_pfloat()) {
      data += File.ctoc();
    }
    if (File.match1("eE")) {
      data += File.ctoc();
      if (File.match1("+-")) {
	data += File.ctoc();
      }else{
      }
      while (File.is_digit()) {
	data += File.ctoc();
      }
    }else{
    }
    while (File.is_alpha()) {
      data += File.ctoc();
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
      data += File.ctoc();
    }
    if(bracket){
      File.warn(bDANGER, "missing ]?");
    }else{
    }
  }

  String::operator=(data);
  File.skipbl();
}
/*--------------------------------------------------------------------------*/
void Angled_String::parse(CS& File)
{ untested();
  File.skipbl();
  free(const_cast<char*>(_data));
  _data = nullptr;
  _data = strdup(File.ctos("", "<", ">").c_str());
  if(!_data){ untested();
    throw(Exception("strdup " + to_string(errno)));
  }else{
  }
  File.skipbl();
}
/*--------------------------------------------------------------------------*/
void vString::parse(CS& File)
{
  File.skipbl();
  size_t here = File.cursor();
  const char quote = '"';
  File.skip1(quote);
  std::string data;
  // TODO: extend ctos and use it.
  // TODO: remove '\0' characters, c.f. LRM
  for (;File;) {
    if (File.match1('\\')) {itested();
      data += File.ctoc();
      if (File.match1(quote)) {itested();
	data += File.ctoc();
      }else if (File.match1('\\')) {itested();
	data += File.ctoc();
      }else{itested();
      }
    }else if (File.skip1(quote)) {
      break;
    }else if (!File.ns_more()) {
      File.warn(bNOERROR, "end of file in quoted string");
      File.warn(bNOERROR, here, "string begins here");
      break;
    }else{
      data += File.ctoc();
      File.reset(File.cursor()); // set ok.
    }
  }
  if(File) {
    String::operator=(data);
    File.skipbl();
  }else{
    // not a vString.
    File.reset_fail(here);
  }
  trace1("stringparse", val_string());
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

  free(_data);
  _data = nullptr;
  char* data;
  _data = data = (char*) malloc(static_cast<size_t>(end-begin)+1);
  if(!_data){ untested();
    throw Exception("malloc " + to_string(errno));
  }else{
  }
  strncpy(data, begin, end-begin);
  data[end-begin] = '\0';
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
