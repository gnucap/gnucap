/*                   -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * hierarchical system parameters
 */
/*--------------------------------------------------------------------------*/
#include "e_hsparam.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
bool HS_PARAM::operator==(const COMMON_COMPONENT& x) const
{
  auto* p = dynamic_cast<HS_PARAM const*>(&x);
  return (p
      && _mfactor == p->_mfactor
      && _xposition == p->_xposition
      && _yposition == p->_yposition
      && _zposition == p->_zposition
      && _hflip == p->_hflip
      && _vflip == p->_vflip
      && _zflip == p->_zflip
      && _angle == p->_angle
      && _mfactor_fixed == p->_mfactor_fixed // needed? must be in device anyway
      && COMMON_PARAMLIST::operator==(x));
}
/*--------------------------------------------------------------------------*/
bool HS_PARAM::param_is_printable(int I) const
{
  switch(I) {
  case 0:
    return _mfactor.has_hard_value();
  case 1:
    return _xposition.has_hard_value();
  case 2:
    return _yposition.has_hard_value();
  case 3:
    return _zposition.has_hard_value();
  case 4:
    return _hflip.has_hard_value();
  case 5:
    return _vflip.has_hard_value();
  case 6:
    return _zflip.has_hard_value();
  case 7:
    return _angle.has_hard_value();
  default:
    unreachable();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::set_param_by_index(int i, std::string& V, int)
{
  switch(i) {
  case 0:
    _mfactor = V;
    break;
  case 1:
    _xposition = V;
    break;
  case 2:
    _yposition = V;
    break;
  case 3: untested();
    _zposition = V;
    break;
  case 4: untested();
    _hflip = V;
    break;
  case 5: untested();
    _vflip = V;
    break;
  case 6: untested();
    _zflip = V;
    break;
  case 7: untested();
    _angle = V;
    break;
  default: untested();
    throw Exception_Too_Many(i, HS_PARAM::param_count(), 0);
  }
}
/*--------------------------------------------------------------------------*/
int HS_PARAM::set_param_by_name(std::string Name, std::string Value)
{
  int which = -1;
  for(int i = 0; i<sysparams_count; ++i){
    if(Name == param_name(i)) {
      which = i;
      break;
    }else{ untested();
    }
  }
  if(which>=0){
    set_param_by_index(which, Value, 0);
    return which;
  }else{ untested();
    trace2("hsp::spbn base", Name, Value);
    return COMMON_COMPONENT::set_param_by_name(Name, Value);
  }
}
/*--------------------------------------------------------------------------*/
std::string HS_PARAM::param_name(int I) const
{
  static std::string hspname[sysparams_count] { //
    "$mfactor",
    "$xposition",
    "$yposition",
    "$zposition",
    "$hflip",
    "$vflip",
    "$zflip",
    "$angle"
  };
  assert(I>=0);
  if(I<sysparams_count){
    return hspname[I];
  }else{ untested();
    unreachable();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
std::string HS_PARAM::param_value(int I) const
{
  switch(I) {
  case 0:
    return _mfactor.string();
  case 1:
    return _xposition.string();
  case 2:
    return _yposition.string();
  case 3: untested();
    return _zposition.string();
  case 4: untested();
    return _hflip.string();
  case 5: untested();
    return _vflip.string();
  case 6: untested();
    return _zflip.string();
  case 7: untested();
    return _angle.string();
  default: untested();
	   unreachable();
	   return "???";
  }
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::expand(COMPONENT const* c)
{
  assert(c);
  CARD const* owner = c->owner();
  CARD_LIST const* scope;
  double mfactor_hier;
  if(auto comp = dynamic_cast<COMPONENT const*>(owner)){
    scope = owner->scope();
    if(comp->hsparam()){
      mfactor_hier = comp->hsparam()->mfactor();
    }else{ untested();
      mfactor_hier = 1.; // assert?
    }
  }else{
    scope = &CARD_LIST::card_list;
    mfactor_hier = 1.;
  }

  _mfactor.e_val(1., scope->params());
  _mfactor_fixed = _mfactor * mfactor_hier;
  trace4("hsp expand", c->long_label(), scope, _mfactor, _mfactor_fixed);

  _xposition.e_val(0., scope->params());
  _yposition.e_val(0., scope->params());
  _zposition.e_val(0., scope->params());
  _hflip.e_val(1, scope->params());
  _vflip.e_val(1, scope->params());
  _zflip.e_val(1, scope->params());
  _angle.e_val(0., scope->params());
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::precalc_last(PARAM_LIST const* Scope)
{
  PARAMETER<double> mfactor_hier;
  mfactor_hier = "$mfactor_hier"; // $mfactor?

  mfactor_hier.e_val(1., Scope);
  _mfactor.e_val(1., Scope);

  _mfactor_fixed = _mfactor * mfactor_hier;

 // _params.set("$mfactor_hier", _mfactor_fixed);
  trace4("HSP::precalc_last", _mfactor.string(), _mfactor, mfactor_hier, _mfactor_fixed);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
