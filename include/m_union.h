/*                                -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * union find
 */
#ifndef E_UNION_H
#define E_UNION_H
/*--------------------------------------------------------------------------*/
template<class T>
T find_subset(T x)
{
  if(!has_parent(x)) {
    return set_parent(x, x);
  }else if(parent(x) != x) {
    return set_parent(x, find_subset(parent(x)));
  }else{
    return x;
  }
}
/*--------------------------------------------------------------------------*/
template<class T>
T build_union(T x, T y)
{
  x = find_subset(x);
  y = find_subset(y);

  if(x == y){
    return x;
  }else if(rank(x) < rank(y)){ untested();
    return set_parent(x, y);
  }else if(rank(y) < rank(x)){ untested();
    return set_parent(y, x);
  }else{
    inc_rank(y); // new parent.
    return set_parent(x, y);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
