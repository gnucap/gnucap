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
//testing=script 2023.10.25
#ifndef L_INDIRECT_H
#define L_INDIRECT_H
#include "md.h"
/*--------------------------------------------------------------------------*/
template <class T>
class INDIRECT {
 private:
  std::map<const void*, T> _map;
  INDIRECT(const INDIRECT&) = delete;
 public:
  INDIRECT() : _map() {_map[NULL];}
  ~INDIRECT() {}

  size_t count(const void* x)const {return _map.count(x);}
    
  T& operator[](const void* x) {
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
  const T& at(const void* x)const {
    if (!x) {untested();
      return _map.at(NULL);
    }else if (_map.count(x) == 0) {
      return _map.at(NULL);
    }else{
      return _map.at(x);
    }
  }
  size_t erase(void* b, void* e) {
    size_t c=0;
    assert(b <= e);
    for (void* i=b; i<=e; i=reinterpret_cast<bool*>(i)+1) {
      c += _map.erase(i);
    }
    return c;
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
