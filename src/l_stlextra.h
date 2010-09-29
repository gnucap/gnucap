/*$Id: l_stlextra.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * extra functions in the STL style
 * Things that ought to be there, but are not
 */
#ifndef L_STLEXTRA_H
#define L_STLEXTRA_H
#include "md.h"
/*--------------------------------------------------------------------------*/
/* find_ptr: like the stl find, except that the list contains pointers
   Dereference the pointer in the list, then compare */
template <class InputIterator, class T>
InputIterator find_ptr(InputIterator first,InputIterator last,const T& value)
{
  while (first != last && **first != value)
    ++first;
  return first;
}
/*--------------------------------------------------------------------------*/
inline void to_lower(std::string* s)
{
  assert(s);
  for (std::string::iterator i = s->begin(); i != s->end(); ++i) {
    *i = tolower(*i);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
