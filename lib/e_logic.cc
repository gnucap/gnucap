/*$Id: d_logic.cc  $ -*- C++ -*-
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
 * base class for logic commons
 */
//testing=?
#include "u_lang.h"
#include "e_logic.h"
#include "e_logicmod.h"
/*--------------------------------------------------------------------------*/
int COMMON_LOGIC::_count = -1;
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_LOGIC* p = dynamic_cast<const COMMON_LOGIC*>(&x);
  bool rv = p
    && _delay == p->_delay
    && COMMON_COMPONENT::operator==(x);
  if (rv) {
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC::set_param_by_index(int I, std::string& Value, int Offset)
{
  switch (I) {
  case 0:  _delay = Value; break;
  case 1:  _modelname = Value; break;
  default:untested(); COMMON_COMPONENT::set_param_by_index(I-2, Value, Offset+2); break;
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC::param_is_printable(int I)const
{
  switch (I) {
  case 0: return _delay.has_hard_value();
  case 1: return modelname().size() && OPT::language
      && OPT::language->name() != "spice" && OPT::language->name() != "acs";
  default: return COMMON_COMPONENT::param_is_printable(I-2);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC::param_name(int I)const
{
  switch (I) {
  case 0: return "delay";
  case 1: return "model";
  default: return COMMON_COMPONENT::param_name(I-2);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC::param_name(int I, int j)const
{
  if (j == 0) {
    return param_name(I);
  }else if (I < 2) {
    return "";
  }else{untested();
    return COMMON_COMPONENT::param_name(I-2, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_LOGIC::param_value(int I)const
{
  switch (I) {
  case 0: return _delay.string();
  case 1: return _modelname;
  default: return COMMON_COMPONENT::param_value(I-2);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC::precalc_first(PARAM_LIST const* scope)
{
  COMMON_COMPONENT::precalc_first(scope);
  if(modelname()=="") {
    error(bDEBUG, "falling back to default logic\n");
    set_modelname("logic");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_LOGIC::precalc_last(PARAM_LIST const* scope)
{
  _delay.e_val(1, scope);
  const MODEL_LOGIC* m = dynamic_cast<MODEL_LOGIC const*>(model());
  assert(m);
  _real_delay = m->delay * _delay;
  COMMON_COMPONENT::precalc_last(scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
