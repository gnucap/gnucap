/* -*- C++ -*-
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
 * storage for attributes, not the same as parameters
 */
//testing=script 2023.10.25
#ifndef U_ATTRIB_H
#define U_ATTRIB_H
#include "ap.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
typedef ptrdiff_t tag_t;

class INTERFACE ATTRIB_LIST {
private:
  std::string _s;	// comma separated, key=value
  int _ref_count;
  ATTRIB_LIST* _up;	// up the hierarchy, for more.
  tag_t _owner;

  ATTRIB_LIST(const ATTRIB_LIST&) = delete;
  ATTRIB_LIST() = delete;
public:
  ATTRIB_LIST(const std::string& S, ATTRIB_LIST* UP, tag_t Owner) 
    :_s(S), _ref_count(0), _up(UP), _owner(Owner) {
    if (_up) {
      _up->inc_ref_count();
    }else{
    }
  }

  ~ATTRIB_LIST() {
    if (_up) {
      if (_up->dec_ref_count()==0) {
	delete _up;
	_up = NULL;
      }else{untested();
      }
    }else{
    }
    assert(_ref_count == 0);
  }

  int   inc_ref_count() {return ++_ref_count;}
  int   dec_ref_count() {assert(_ref_count>0); return --_ref_count;}
  //int   ref_count()const {untested(); return _ref_count;}

  const std::string string(tag_t Owner)const {
    if (Owner == _owner || !Owner) {
      if (_up) {
	return _up->string(Owner) + ", " + _s;
      }else{
	return _s;
      }
    }else{untested();
      return "";
    }
  }

  const std::string operator[] (const std::string& Key)const {untested();
    CS cmd(CS::_STRING, _s);
    bool found = false;
    std::string val("0");

    while (cmd.more()) {untested();
      if (cmd >> Key) {untested();
	if (cmd >> "=") {untested();
	  cmd >> val;
	}else{untested();
	  val = "1";
	}
	found = true;
	// keep looking in case there is another, which will supercede
	// finds right-most match using left-right search
      }else{untested();
	cmd.skiparg();
	if (cmd >> "=") {untested();
	  cmd.ctos();
	}else{untested();
	}
      }
    }

    if (found) {untested();
      return val;
    }else if (_up) {untested();
      return (*_up)[Key];
    }else{untested();
      return "0";
    }
  }
  ATTRIB_LIST& chown(tag_t Old, tag_t New) {
    if(_owner == Old){
      _owner = New;
      if(_up){untested();
	_up->chown(Old, New);
      }else{
      }
    }else{untested();
    }
    return *this;
  }
};
/*--------------------------------------------------------------------------*/
class INTERFACE ATTRIB_LIST_p {
private:
  ATTRIB_LIST* _p;
public:
  ATTRIB_LIST_p() :_p(NULL) {}

  ATTRIB_LIST_p(const ATTRIB_LIST_p& P) :_p(P._p) {untested();
    if (_p) {untested();
      _p->inc_ref_count();
    }else{untested();
    }
  }
  
  ~ATTRIB_LIST_p() {
    if (_p) {
      if (_p->dec_ref_count()==0) {
	delete _p;
	_p = NULL;
      }else{
      }
    }else{
    }
  }

  operator bool()const {return _p;}

  const std::string string(tag_t Owner)const {
    assert(_p);
    return _p->string(Owner);
  }

  const std::string operator[] (const std::string& Key)const {untested();
    return ((_p) ? (*_p)[Key] : "0");
  }

  ATTRIB_LIST_p& operator=(const ATTRIB_LIST_p& P) {
    if ((_p = P._p)) {
      _p->inc_ref_count();
    }else{untested();
    }
    return *this;
  }

  ATTRIB_LIST_p& add_to(const std::string& String, tag_t Owner) {
    if (_p) {
      _p->dec_ref_count();
    }else{
    }
    _p = new ATTRIB_LIST(String, _p, Owner);
    assert(_p);
    _p->inc_ref_count();
    return *this;
  }

  ATTRIB_LIST_p& chown(tag_t Old, tag_t New) {
    if (_p) {
      _p->chown(Old, New);
    }else{untested();
    }
    return *this;
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
