/*$Id: l_dispatcher.h  al -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 * dispatcher -- for dynamically loaded modules
 *
 * To address the issue of unknown static construction order:
 * Must not have a constructor or destructor, or any parts that do.
 * Also must not have any virtual functions, because the v-table has a constructor.
 * "construction" will be done on demand by check_init.
 *
 * This is suitable for static objects only!
 */
//testing=script 2020.04.06
#ifndef L_DISPATCHER_H
#define L_DISPATCHER_H
#include "e_base.h"
#include "l_stlextra.h"
#include "u_opt.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
class DISPATCHER_BASE {
protected:
  std::map<std::string, CKT_BASE*> * _map;
public:
  typedef std::map<std::string, CKT_BASE*>::const_iterator const_iterator;

  const_iterator begin()const		{assert(_map); return _map->begin();}
  const_iterator end()const		{assert(_map); return _map->end();}
public:
  CKT_BASE* operator[](std::string s);
private:
  void      uninstall(CKT_BASE* p);
  void	    install(const std::string& s, CKT_BASE* p);
  void      check_init();
public:
  class INSTALL {
  private:
    const std::string _name;
    DISPATCHER_BASE* _d;
    CKT_BASE* _p;
  public:
    INSTALL(DISPATCHER_BASE* d, const std::string& name, CKT_BASE* p) :
      _name(name),
      _d(d),
      _p(p)
    {
      assert(_d);
      assert(p);
      _d->install(_name, p);
    }

    ~INSTALL() {
      _d->uninstall(_p);
    }
  };
};
/*--------------------------------------------------------------------------*/
template <class TT>
class INTERFACE DISPATCHER : public DISPATCHER_BASE {
public:
  TT* operator[](std::string s);
  TT* operator[](CS& cmd);
  TT* clone(std::string s);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class TT>
TT* DISPATCHER<TT>::operator[](std::string s)
{
  assert(_map);
  CKT_BASE* rv = (*_map)[s];
  if (!rv && OPT::case_insensitive) {
    notstd::to_lower(&s);
    rv = (*_map)[s];
  }else{
  }
  return prechecked_cast<TT*>(rv);
}
/*--------------------------------------------------------------------------*/
template <class TT>
TT* DISPATCHER<TT>::operator[](CS& cmd)
{
  unsigned here = cmd.cursor();
  std::string s;
  cmd >> s;
  //------------------------
  TT* p = (*this)[s];
  //------------------------
  if (!p) {
    cmd.reset(here);
  }else{
  }
  return p;
}
/*--------------------------------------------------------------------------*/
template <class TT>
TT* DISPATCHER<TT>::clone(std::string s)
{
  TT* proto = (*this)[s];
  if (proto) {
    TT* c=prechecked_cast<TT*>(proto->clone());
    assert(c);
    return c;
  }else{untested();
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
