/*$Id: base-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 */
#include "base.h"
/*--------------------------------------------------------------------------*/
Typ_Min_Max dataset = rTYP;
static char comment_char= '|';
/*--------------------------------------------------------------------------*/
bool skip_comment(CS& file)
{
  bool did_it = false;
  {for (;;) {
    file.skipbl();
    {if (file.match1(comment_char)) {
      file.skip();
      skip_tail(file);
      did_it = true;
    }else if (file.dmatch("[Comment_Char]")) {
      file >> comment_char;
      skip_tail(file);
      did_it = true;
    }else{
      break;
    }}
  }}
  return did_it;
}
/*--------------------------------------------------------------------------*/
void Float::parse(CS& file)
{
  {if (file.dmatch("NA")) {
    _data = NA;
  }else{
    int here = file.cursor();
    file >> _data;
    if (file.stuck(&here)) {
      _data = NA;
    }
  }}
}
/*--------------------------------------------------------------------------*/
void Vector_Item::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _x >> _y;
}
/*--------------------------------------------------------------------------*/
void Fraction::parse(CS& file)
{
  file >> _numerator;
  file.skip1b("/");
  file >> _denominator;
}
/*--------------------------------------------------------------------------*/
void Name_String::parse(CS& file)
{
  file.skipbl();
  _data = "";
  while (file.is_alpha() || file.is_digit() || file.match1("_[]")) {
    _data += file.ctoc();
  }
  file.skipbl();
}
/*--------------------------------------------------------------------------*/
void Quoted_String::parse(CS& file)
{
  file.skipbl();
  int here = file.cursor();
  char quote = file.ctoc();
  _data = "";
  {for (;;) {
    {if (file.skip1(quote)) {
      break;
    }else if (!file.more()) {
      file.warn(0, "end of file in quoted string");
      file.warn(0, here, "string begins here");
      break;
    }else{
      _data += file.ctoc();
    }}
  }}
  file.skipbl();
}
/*--------------------------------------------------------------------------*/
void Tail_String::parse(CS& file)
{
  static char end_marks[] = "|\n";
  end_marks[0] = comment_char;

  const char* begin = file.tail();
  file.skipto1(end_marks);
  const char* end = file.tail();
  assert(end >= begin);

  while ((--end >= begin) && (isspace(*end))) {
  }
  ++end;
  assert(end >= begin);

  _data = std::string(begin, end-begin);
}
/*--------------------------------------------------------------------------*/
void Text_Block::parse(CS& file)
{
  const char* begin = file.tail();
  {for (;;) {
    file.skipto1("\n").skip();
    if (file.peek() == '[') {
      break;
    }
  }}
  const char* end = file.tail();
  _data = std::string(begin, end-begin);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
