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
#include "e_hsparam.h" // BUG
/*--------------------------------------------------------------------------*/
int COMMON_PARAMLIST::_count = -1;
/*--------------------------------------------------------------------------*/
int COMMON_PARAMLIST::compare(const COMMON_COMPONENT& x) const
{
  if(this == &x){
    return 0;
  }else if(int c = COMMON_COMPONENT::compare(x)) {
    return c;
  }else{
    const COMMON_PARAMLIST* p = prechecked_cast<const COMMON_PARAMLIST*>(&x);
    assert(p);
    return _params.compare(p->_params);
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_PARAMLIST::param_is_printable(int i)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (i < int(_params.size())) {
    return _params.is_printable(i);
  }else{
    return COMMON_COMPONENT::param_is_printable(i-int(_params.size()));
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_name(int i)const
{
  if (i < int(_params.size())) {
    return _params.name(i);
  }else{itested();
    return COMMON_COMPONENT::param_name(i-int(_params.size()));
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_name(int i, int j)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (j == 0) {untested();
    return param_name(i);
  }else if (i < int(_params.size())) {untested();
    return "";
  }else{
    return COMMON_COMPONENT::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_PARAMLIST::param_value(int i)const
{
  assert(i < COMMON_PARAMLIST::param_count());
  if (i < int(_params.size())) {
    return _params.value(i);
  }else{itested();
    return COMMON_COMPONENT::param_value(i-int(_params.size()));
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_PARAMLIST::precalc_first(const PARAM_LIST* Scope)
{
  // assert(Scope);
  COMMON_COMPONENT::precalc_first(Scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_PARAMLIST::precalc_last(const PARAM_LIST* Scope)
{
  // assert(Scope);
  COMMON_COMPONENT::precalc_last(Scope);

  for (int i=0; i<_params.size(); ++i){
    _params[i].e_val(nullptr, Scope);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
