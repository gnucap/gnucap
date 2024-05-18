/*$Id: l_indirect.h $ -*- C++ -*-
 * Copyright (C) 2023 Albert Davis
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
 * indirect storage of data
 */
//testing=script 2024.05.16
#ifndef L_INDIRECT_H
#define L_INDIRECT_H
#include "md.h"
/*--------------------------------------------------------------------------*/
typedef intptr_t tag_t;

template <class T>
class INDIRECT {
private:
  std::map<tag_t, T> _map;
  INDIRECT(const INDIRECT&) = delete;
public:
  INDIRECT() : _map() {_map[tag_t(0)];}
  ~INDIRECT() {}
  
  size_t count(tag_t x)const {return _map.count(x);}
    
  T& operator[](tag_t x) {
    if (!x) {untested();
      // null tag, not allowed here
    }else if (_map.count(x) == 0) {
      // first use of this tag
    }else{
      // repeat, adding or replacing
      // possible collision
    }
    assert(x);
    return _map[x];
  }
  const T& at(tag_t x)const {
    if (!x) {untested();
      return _map.at(tag_t(0));
    }else if (_map.count(x) == 0) {
      return _map.at(tag_t(0));
    }else{
      return _map.at(x);
    }
  }
  size_t erase(tag_t b, tag_t e) {
    size_t c=0;
    assert(b <= e);
    for (tag_t i=b; i<e; ++i) {
      c += _map.erase(i);
    }
    return c;
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
