/*$Id: l_pmatch.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
#include "l_lib.h"
/*--------------------------------------------------------------------------*/
bool Umatch(const std::string& str1, const std::string& str2)
{
  CS cmd(CS::_STRING, str1); //call to CS member on string
  if (cmd.umatch(str2)) {
    return true;
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
