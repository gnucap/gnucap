/*$Id: e_paramlist.cc  2016/09/16  $ -*- C++ -*-
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
 */
//testing=script 2016.09.16
#include "e_paramlist.h"
/*--------------------------------------------------------------------------*/
int COMMON_PARAMLIST::_count = -1;
/*--------------------------------------------------------------------------*/
bool COMMON_PARAMLIST::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_PARAMLIST* p = dynamic_cast<const COMMON_PARAMLIST*>(&x);
  bool rv = p 
    && _params == p->_params
    && COMMON_COMPONENT::operator==(x);
  return rv;
}
/*--------------------------------------------------------------------------*/
bool COMMON_PARAMLIST::param_is_printable(int i)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.is_printable(COMMON_PARAMLIST::param_count() - 1 - i);
  }else{
    return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_name(int i)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.name(COMMON_PARAMLIST::param_count() - 1 - i);
  }else{untested();
    return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_name(int i, int j)const
{untested();
  assert(i < COMMON_PARAMLIST::param_count());
  if (j == 0) {untested();
    return param_name(i);
  }else if (i >= COMMON_COMPONENT::param_count()) {untested();
    return "";
  }else{untested();
    return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_value(int i)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.value(COMMON_PARAMLIST::param_count() - 1 - i);
  }else{untested();
    return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_PARAMLIST::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_first(Scope);
  _mfactor = _params.deep_lookup("m");
  //BUG//  _mfactor must be in precalc_first
}
/*--------------------------------------------------------------------------*/
void COMMON_PARAMLIST::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_last(Scope);

  for (PARAM_LIST::iterator i = _params.begin(); i != _params.end(); ++i) {
    i->second.e_val(NOT_INPUT,Scope);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
