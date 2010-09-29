/*$Id: u_parameter.h,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * A class for parameterized values
 * Used for spice compatible .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script 2006.07.14
#ifndef U_PARAMETER_H
#define U_PARAMETER_H
#include "u_opt.h"
#include "e_cardlist.h"
#include "constant.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
template <class T>
class PARAMETER {
private:
  mutable T _v;
  std::string _s;
public:
  explicit PARAMETER() :_v(NOT_INPUT), _s() {}
  PARAMETER(const PARAMETER<double>& p) :_v(p._v), _s(p._s) {}
  explicit PARAMETER(T v) :_v(v), _s() {}
  //explicit PARAMETER(T v, const std::string& s) :_v(v), _s(s) {untested();}
  ~PARAMETER() {}
  
  operator T()const {return _v;}
  bool	has_value()const {return (_s != "");}
  T	e_val(T def, const CARD_LIST* scope)const;
  void	parse(CS& cmd);
  void	print(OMSTREAM& o)const {
    if (_s == "#") {
      o << _v;
    }else if (_s == "") {
      o << "NA(" << _v << ")";
    }else{itested();
      o << _s;
    }
  }
  void	operator=(T v)	 {_v = v; _s = "#";}
  void	operator=(const std::string& s) {_s = s;}
  bool  operator==(const PARAMETER& p) {
    bool a = _v==p._v;
    bool b = _s==p._s;
    if (!a) {untested();
    }
    if (!b) {untested();
    }
    if (!(a && b)) {untested();
    }
    if (a ^ b) {untested();
    }
    return a && b;
  }
  T*	pointer_hack()	 {return &_v;}
};
/*--------------------------------------------------------------------------*/
class PARAM_LIST {
private:
  std::map<const std::string, PARAMETER<double> > _pl;
  PARAM_LIST* try_again;
public:
  typedef std::map<const std::string, PARAMETER<double> >::const_iterator
		const_iterator;
  typedef std::map<const std::string, PARAMETER<double> >::iterator
		iterator;
  explicit PARAM_LIST() :try_again(NULL) {}
  explicit PARAM_LIST(const PARAM_LIST& p) 
				:_pl(p._pl), try_again(p.try_again) {}
  //explicit PARAM_LIST(PARAM_LIST* ta) :try_again(ta) {untested();}
  ~PARAM_LIST() {}
  void	parse(CS& cmd);
  void	print(OMSTREAM&)const;
  void	eval_copy(PARAM_LIST&, const CARD_LIST*);
  bool  operator==(const PARAM_LIST& p)const {return _pl == p._pl;}
  PARAMETER<double> operator[](const std::string& i);
};
/*--------------------------------------------------------------------------*/
template <class T>
inline T PARAMETER<T>::e_val(T def, const CARD_LIST* scope)const
{
  assert(scope);

  static int recursion=0;
  static const std::string* first_name = NULL;
  if (recursion == 0) {
    first_name = &_s;
  }
  assert(first_name);

  ++recursion;
  if (_s == "") {
    // blank string means to use default value
    _v = def;
    if (recursion > 1) {itested();
      error(bWARNING, "parameter " + *first_name + " has no value\n");
    }
  }else if (_s != "#") {
    // anything else means look up the value
    if (recursion <= OPT::recursion) {
      PARAM_LIST* pl = const_cast<PARAM_LIST*>(scope->params());
      _v = T((*pl)[_s].e_val(def, scope));
    }else{untested();
      _v = def;
      error(bDANGER, "parameter " + *first_name + " recursion too deep\n");
    }
  }else{
    // start with # means we have a final value
  }
  --recursion;
  return _v;
}
/*--------------------------------------------------------------------------*/
template <>
inline void PARAMETER<bool>::parse(CS& cmd) 
{
  bool new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{untested();
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"'", "'}\"'");
    if (cmd) {untested();
      if (name == "NA") {untested();
	_s = "";
      }else{untested();
	_s = name;
      }
    }else{untested();
    }
  }
  if (!cmd) {untested();
    _v = true;
    _s = "#";
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline void PARAMETER<T>::parse(CS& cmd) 
{
  T new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"'", "'}\"'");
    if (cmd) {
      if (name == "NA") {untested();
	_s = "";
      }else{
	_s = name;
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, PARAMETER<bool>* val);
bool get(CS& cmd, const std::string& key, PARAMETER<int>* val);
/*--------------------------------------------------------------------------*/
template <class T>
inline OMSTREAM& operator<<(OMSTREAM& o, const PARAMETER<T> p)
{
  p.print(o);
  return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
