/*$Id: l_pmatch.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * string compare
 * compares characters until end of first string
 * any non-alpha character is a terminator
 * returns offset into string to legal start of next token
 * (skips trailing spaces and comma, if any)
 *     if they match so far, and enough matched,   else NO (0)
 * Characters in reference string in UPPER case must match.
 * Always requires at least one character to match.
 */
//testing=script,complete 2006.07.13
#include "ap.h"
/*--------------------------------------------------------------------------*/
/* pmatch: match str1 (under test) against str2 (reference)
 * if no match, returns 0
 * if match, returns the number of characters that match,
 *	---including whitespace---
 * return value is usually interpreted as a truth value:
 *	true if match exists.
 * str1 (being tested) is considered to be case-insensitive
 *	is terminated by a non-alpha character
 * str2 (reference) has special considerations:
 *	upper case characters must match, and must be present
 *	lower case characters must match if present, but may be omitted
 * strings are alpha only.
 */
int pmatch(const char* str1, const char* str2)
{
  CS cmd(str1);
  if (cmd.pmatch(str2)) {
    return cmd.cursor();
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
