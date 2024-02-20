/*                         -*- C++ -*-
 * Copyright (C) 2023 Felix Salfelder
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
 */
//testing=script 2023.12.02
#ifndef IO_DIR_H
#define IO_DIR_H
#include "l_lib.h"
/*--------------------------------------------------------------------------*/
class DIRECTORY {
private:
  ::DIR* _d;

public:
  class const_iterator {
    ::DIR* _d;
    ::dirent* _de;
    friend class DIRECTORY;
  private:
    const_iterator(const_iterator const& o) : _d(o._d), _de(o._de) {untested();}
    const_iterator(::DIR* d) : _d(d) {
      if(d) {
	_de = ::readdir(d);
      }else{
	_de = NULL;
      }
    }
  public:
    explicit const_iterator() : _d(NULL), _de(NULL) {}
    ~const_iterator() {}
  public:
    ::dirent const* operator->() {return _de;}
    const_iterator& operator++() {assert(_d); _de = ::readdir(_d); return *this;}
    bool operator==(const_iterator const& o)const {return _de == o._de;}
    bool operator!=(const_iterator const& o)const {return !operator==(o);}
    const_iterator& operator=(const_iterator const& o) {
      _d = o._d;
      _de = o._de;
      return *this;
    }
  };

public:
  DIRECTORY() = delete;
  DIRECTORY(const DIRECTORY&) = delete;
  explicit DIRECTORY(const std::string& s) : _d(::opendir(s.c_str())) {}
  ~DIRECTORY() {
    if(_d) {
      ::closedir(_d);
      //delete(_d); // done by closedir?
      _d = NULL;
    }else{
    }
  }
  void rewinddir()const {::rewinddir(_d);}
  bool exists()const {return _d;}
  const_iterator begin()const {rewinddir(); return const_iterator(_d);}
  const_iterator end()const {return const_iterator(NULL);}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
