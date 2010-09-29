/*$Id: ap_match.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * string compare
 */
#include "ap.h"
/*--------------------------------------------------------------------------*/
CS::MATCH_STYLE CS::_ms(msPARTIAL);
/*--------------------------------------------------------------------------*/
/* pmatch = partial match
 * compares characters until end of first string
 * any non-alpha character may be a terminator
 * Characters in reference string in UPPER case must match.
 * Always requires at least one character to match.
 */
CS& CS::pmatch(const std::string& s)
{
  int start = cursor();
  skipbl();
  int tokenstart = cursor();

  int begin_match = cursor();
  const char* str2 = s.c_str();
  while (*str2 && tolower(peek()) == tolower(*str2)) { // as far as you can
    skip();
    ++str2;
  }
  // something didn't match -- let's see what and why
  {if (strcmp(str2,"$$")==0) { // stop marker in reference -- accept
    skipcom();
    _ok = true;
  }else if (cursor() == tokenstart// didn't move
	    || is_alpha()	  // more letters in the string being tested
    	    || isspace(*str2)
    	    || isupper(*str2)	  // more caps (must match) in the reference
    	    || isdigit(*str2)	  // numbers (must match) in the reference
    	    || ispunct(*str2)) {  // punctuation (must match) in the reference
    reset(start);	// back up (don't consume)
    _ok = false;	// reject
  }else{	    // reached natural end, accept
    _begin_match = begin_match;
    _end_match = cursor();
    skipcom();
    _ok = true;
  }}
  return *this;
}
/*--------------------------------------------------------------------------*/
/* icmatch = ignore-case match
 * match of full string, ignoring case.
 */
CS& CS::icmatch(const std::string& s)
{
  int start = cursor();
  skipbl();

  int begin_match = cursor();
  const char* str2 = s.c_str();
  while (*str2 && tolower(peek()) == tolower(*str2)) { // as far as you can
    skip();
    ++str2;
  }
  {if (*str2 == '\0') {
    _begin_match = begin_match;
    _end_match = cursor();
    skipcom();
    _ok = true;
  }else{
    reset(start);
    _ok = false;
  }}
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ematch = exact match
 * match of full string, case sensitive
 */
CS& CS::ematch(const std::string& s)
{
  int start = cursor();
  skipbl();

  int begin_match = cursor();
  const char* str2 = s.c_str();
  while (*str2 && peek() == *str2) { // as far as you can
    skip();
    ++str2;
  }
  {if (*str2 == '\0') {
    _begin_match = begin_match;
    _end_match = cursor();
    skipcom();
    _ok = true;
  }else{
    reset(start);
    _ok = false;
  }}
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dmatch = default match
 * honor _ms
 */
CS& CS::dmatch(const std::string& s)
{
  switch (_ms) {
  case msPARTIAL:     return pmatch(s);
  case msIGNORE_CASE: return icmatch(s);
  case msEXACT:       return ematch(s);
  }
  unreachable();
  return *this;
}
/*--------------------------------------------------------------------------*/
CS& CS::pscan(const std::string& s)
{
  int start = cursor();
  for (;;) {
    {if (pmatch(s)) {	// found it
      _ok = true;
      break;
    }else if (!more()) {// ran out
      reset(start);
      _ok = false;
      break;
    }else{		// skip and try again
      skiparg();
    }}
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CS& CS::icscan(const std::string& s)
{
  int start = cursor();
  for (;;) {
    {if (icmatch(s)) {	// found it
      _ok = true;
      break;
    }else if (!more()) {// ran out
      reset(start);
      _ok = false;
      break;
    }else{		// skip and try again
      skiparg();
    }}
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CS& CS::escan(const std::string& s)
{
  int start = cursor();
  for (;;) {
    {if (ematch(s)) {	// found it
      _ok = true;
      break;
    }else if (!more()) {// ran out
      reset(start);
      _ok = false;
      break;
    }else{		// skip and try again
      skiparg();
    }}
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CS& CS::dscan(const std::string& s)
{
  int start = cursor();
  for (;;) {
    {if (dmatch(s)) {	// found it
      _ok = true;
      break;
    }else if (!more()) {// ran out
      reset(start);
      _ok = false;
      break;
    }else{		// skip and try again
      skiparg();
    }}
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
std::string CS::last_match()const
{
  return std::string(&(_cmd[_begin_match]), &(_cmd[_end_match]));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
