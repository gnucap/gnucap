/*$Id: l_wmatch.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * wmatch: string match with wild cards
 * s1 may have wild cards: ? any character matches; * any repeated 0 or more
 * returns true or false
 * normally not case sensitive,
 *	but \ before any letter in s1 forces exact match
 * recursive
 */
#include <ctype.h>
/*--------------------------------------------------------------------------*/
	bool	wmatch(const char*,const char*);
/*--------------------------------------------------------------------------*/
bool wmatch(const char *s2, const char *s1)
{
  {if (!*s2 && !*s1) {			// both end together -- match
    return true;
  }else if (!*s2 || !*s1) {		// ends don't match
    return false;
  }else if (tolower(*s2) == tolower(*s1)) { // one char matches - move on
    return wmatch(s2+1, s1+1);
  }else if (*s1 == '?') {		// ? wild card match - move on
    return wmatch(s2+1, s1+1);
  }else if (*s1 == '*') {		// * (repeat) wild card match
    {if (wmatch(s2+1, s1)) {		// match 1, try for 2
      return true;
    }else if (wmatch(s2, s1+1)) {	// match 0 - continue
      return true;
    }else{				// match 1, only 1
      return wmatch(s2+1, s1+1);
    }}
  }else{				// mismatch
    return false;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
