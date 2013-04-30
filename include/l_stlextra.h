/*$Id: l_stlextra.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * extra functions in the STL style
 * Things that ought to be there, but are not
 */
//testing=script,complete 2006.07.13
#ifndef L_STLEXTRA_H
#define L_STLEXTRA_H
#include "md.h"
/*--------------------------------------------------------------------------*/
namespace notstd {
/*--------------------------------------------------------------------------*/
template <class InputIter, class Size, class OutputIter>
void copy_n(InputIter first, Size count, OutputIter result)
{
  for ( ; count > 0; --count) {
    *result++ = *first++;
  }
}
/*--------------------------------------------------------------------------*/
/* find_ptr: like the stl find, except that the list contains pointers
   Dereference the pointer in the list, then compare */
template <class InputIterator, class T>
InputIterator find_ptr(InputIterator first,InputIterator last,const T& value)
{
  while (first != last && **first != value) {
    ++first;
  }
  return first;
}
/*--------------------------------------------------------------------------*/
inline void to_lower(std::string* s)
{
  assert(s);
  for (std::string::iterator i = s->begin(); i != s->end(); ++i) {
    *i = static_cast<char>(tolower(*i));
  }
}
/*--------------------------------------------------------------------------*/
inline void to_upper(std::string* s)
{
  assert(s);
  for (std::string::iterator i = s->begin(); i != s->end(); ++i) {
    *i = static_cast<char>(toupper(*i));
  }
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
