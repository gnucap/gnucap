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
  INDIRECT() : _map() {}
  ~INDIRECT() {}
    
  T& at(void* x) {
    assert(x);
    return _map[x];
  }
  const T& at(const void* x) {
    if (!x) {untested();
      return _map[NULL];
    }else if (_map.count(x) == 0) {
      return _map[NULL];
    }else{
      return _map[x];
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
