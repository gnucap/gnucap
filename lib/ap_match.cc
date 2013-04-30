/*$Id: ap_match.cc,v 26.131 2009/11/20 08:22:10 al Exp $ -*- C++ -*-
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
 * string compare
 */
//testing=script 2006.07.17
#include "u_opt.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
/* umatch = universal match
 */
CS& CS::umatch(const std::string& s)
{
  unsigned start = cursor();
  skipbl();
  unsigned begin_match = cursor();
  const char* str2 = s.c_str();
  bool optional = 0;

  for (;;) {
    if ((!*str2) || (*str2 == '|')) {
      _ok = true;
      break;
    }else if ((str2[0] == '\\') && (peek() == str2[1])) {
      skip();
      str2 += 2;
    }else if ((!optional) && (*str2 == '{')) {
      ++str2;
      optional = true;
    }else if ((optional) && (*str2 == '}')) {
      ++str2;
      optional = false;
    }else if ((*str2 == ' ') && is_term()) {
      // blank in ref string matches anything that delimits tokens
      skipbl();
      ++str2;
    }else if (peek() == *str2) {
      skip();
      ++str2;
    }else if ((OPT::case_insensitive) && (tolower(peek()) == tolower(*str2))) {
      skip();
      ++str2;
    }else if (optional) {
      while (*str2 != '}') {
	++str2;
      }
    }else{
      // mismatch
      const char* bar = strchr(str2, '|');
      if (bar && (bar[-1] != '\\')) {
	str2 = bar+1;
	reset(start);
      }else{
	_ok = false;
	break;
      }
    }
  }

  if (_ok) {
    _begin_match = begin_match;
    _end_match = cursor();
    skipcom();
    _ok = true;
  }else{
    reset(start);
    _ok = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CS& CS::scan(const std::string& s)
{
  unsigned start = cursor();
  for (;;) {
    if (umatch(s)) {	 // found it
      _ok = true;
      break;
    }else if (!more()) { // ran out
      reset(start);
      _ok = false;
      break;
    }else{		 // skip and try again
      skiparg();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
std::string CS::last_match()const
{
  return _cmd.substr(_begin_match, _end_match-_begin_match);  
}
/*--------------------------------------------------------------------------*/
std::string CS::trimmed_last_match(const std::string& suf)const
{
  unsigned real_end = _end_match;
  while (strchr(suf.c_str(), _cmd[real_end-1])) {
    --real_end;
  }
  return _cmd.substr(_begin_match, real_end-_begin_match);  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
