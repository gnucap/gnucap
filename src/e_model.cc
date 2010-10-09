/*$Id: e_model.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2006.07.12
#include "e_compon.h"
#include "e_model.h"
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const COMPONENT* p)
  :CARD(),
   _component_proto(p),
   _tnom_c(NOT_INPUT)
{
  _sim->uninit();
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const MODEL_CARD& p)
  :CARD(p),
   _component_proto(p._component_proto),
   _tnom_c(p._tnom_c)
{
  _sim->uninit();
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::~MODEL_CARD()
{
  _sim->uninit(); // disconnect models from devices
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_CARD::param_count() - 1 - i) {
  case 0: _tnom_c = value; break;
  default: CARD::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_CARD::param_is_printable(int i)const
{
  switch (MODEL_CARD::param_count() - 1 - i) {
  case 0: return true;
  default: return CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i)const
{
  switch (MODEL_CARD::param_count() - 1 - i) {
  case 0: return "tnom\0";
  default: return CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= CARD::param_count()) {
    return "";
  }else{
    return MODEL_CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_value(int i)const
{
  switch (MODEL_CARD::param_count() - 1 - i) {
  case 0: return _tnom_c.string();
  default: return CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::precalc_first()
{
  CARD::precalc_first();
  _tnom_c.e_val(OPT::tnom_c, scope());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
