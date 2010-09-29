/*$Id: l_pmatch.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * compares characters until end of first string
 * any non-alpha character is a terminator
 * returns offset into string to legal start of next token
 * (skips trailing spaces and comma, if any)
 *     if they match so far, and enough matched,   else NO (0)
 * Characters in reference string in UPPER case must match.
 * Always requires at least one character to match.
 */
#include "ap.h"
/*--------------------------------------------------------------------------*/
	int	pmatch(const char*,const char*);
/*--------------------------------------------------------------------------*/
static inline int is_alpha(int c){return isalpha(toascii(c));}
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
  {if (cmd.pmatch(str2)){
    return cmd.cursor();
  }else{
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
