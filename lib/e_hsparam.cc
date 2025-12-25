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
#include "u_sim_data.h"
#include "u_lang.h" // print_pair.
#include "e_hsparam.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
bool HS_PARAM::operator==(const COMMON_COMPONENT& x) const
{
#ifndef NDEBUG
  if(this == &x){ untested();
    incomplete();
    unreachable();
  }else{
  }
#endif

  auto* p = dynamic_cast<HS_PARAM const*>(&x);
  bool ret = (p
      && _mfactor == p->_mfactor
      && _xposition == p->_xposition
      && _yposition == p->_yposition
      && _zposition == p->_zposition
      && _hflip == p->_hflip
      && _vflip == p->_vflip
      && _zflip == p->_zflip
      && _angle == p->_angle
      && _method == p->_method
      && _temperature == p->_temperature
      && _dtemp == p->_dtemp
      && _tnom == p->_tnom
      && _temp_c == p->_temp_c
      && _tnom_c == p->_tnom_c
      && COMMON_PARAMLIST::operator==(x));
  return ret;
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
  case 8:
    return _method.has_hard_value();
  case 9:
    return _temperature.has_hard_value();
  case 10:
    return _dtemp.has_hard_value();
  case 11:
    return _tnom.has_hard_value();
  case 12:
    return _temp_c.has_hard_value();
  case 13:
    return _tnom_c.has_hard_value();
  default:
    return COMMON_PARAMLIST::param_is_printable(I-sysparams_count);
  }
}

#if 0
  if(name[0] != '$') { untested();
    return nullptr;
  }else{ untested();
    if(name == "$mfactor") { untested();
      return new vReal(1.);
    }else if(name == "$xposition") { untested();
      return new vReal(0.);
    }else if(name == "$yposition") { untested();
      return new vReal(0.);
    }else if(name == "$zposition") { untested();
      return new vReal(0.);
    }else if(name == "$angle") { untested();
      return new vReal(0.);
    }else if(name == "$hflip") { untested();
      return new vInteger(1);
    }else if(name == "$vflip") { untested();
      return new vInteger(1);
    }else if(name == "$zflip") { untested();
      return new vInteger(1);
    }else if(name == "$temperature") { untested();
      return new vReal(P_CELSIUS0 + _sim->_temp_c);
    }else if(name == "$vt") { untested();
      double t = P_CELSIUS0 + _sim->_temp_c;
      double vt = P_K * t / P_Q;
      trace2("new_hs", t, vt);
      return new vReal(vt);
      //  }else if(name == "$dtemp") { untested();
      //    return new vReal(0.);
    }else if(name == "$tnom") { untested();
      return new vReal(0.);
    }else if(name == "$vt") { untested();
      return new vReal(0.);
    }else if(name == "$method") { untested();
      return new vString("");
    }else{ untested();
      return nullptr;
    }
#endif
/*--------------------------------------------------------------------------*/
void HS_PARAM::set_param_by_index(int I, std::string& V, int Offset)
{
  switch(I) {
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
  case 8:
    if(!V.size()){ untested();
      _method = "";
    }else{
      _method = V;
      trace3("hs::spbn", V, _method, _method.string());
    }
    break;
  case 9:
    _temperature = V;
    break;
  case 10:
    _dtemp = V;
    break;
  case 11: untested();
    _tnom = V;
    break;
  case 12:
    _temp_c = V;
    break;
  case 13: untested();
    _tnom_c = V;
    break;
  default: untested();
    return COMMON_PARAMLIST::set_param_by_index(I-sysparams_count, V, Offset+sysparams_count);
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
    }else{
    }
  }
  if(which>=0){
    set_param_by_index(which, Value, 0);
    return which;
  }else if(Name[0] == '$'){
    return _params.set(Name, Value) + sysparams_count;
  }else{ untested();
    trace2("hsp::spbn base", Name, Value);
    incomplete();
    return COMMON_COMPONENT::set_param_by_name(Name, Value) + sysparams_count;
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
    "$angle",
    "$method",
    "$temperature",
    "$dtemp",
    "$tnom",
    "$temp_c",
    "$tnom_c"
  };
  assert(I>=0);
  if(I<sysparams_count){
    return hspname[I];
  }else{
    return COMMON_PARAMLIST::param_name(I-sysparams_count);
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
  case 8:
    return _method.string();
  case 9:
    return _temperature.string();
  case 10:
    return _dtemp.string();
  case 11:
    return _tnom.string();
  case 12:
    return _temp_c.string();
  case 13:
    return _tnom_c.string();
  default:
    return COMMON_PARAMLIST::param_value(I-sysparams_count);
  }
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::expand(COMPONENT const* c)
{
  assert(c);
  CARD const* owner = c->owner();
  CARD_LIST const* scope;
  if(auto comp = dynamic_cast<COMPONENT const*>(owner)){
    scope = comp->scope();
  }else{
    scope = &CARD_LIST::card_list;
  }

  _xposition.e_val(0., scope->params());
  _yposition.e_val(0., scope->params());
  _zposition.e_val(0., scope->params());
  _hflip.e_val(1, scope->params());
  _vflip.e_val(1, scope->params());
  _zflip.e_val(1, scope->params());
  _angle.e_val(0., scope->params());
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  print_pair(o, lang, "tnom", _tnom_c,  _tnom_c.has_hard_value());
  print_pair(o, lang, "dtemp",_dtemp,   _dtemp.has_hard_value());
  print_pair(o, lang, "temp", _temp_c,  _temp_c.has_hard_value());
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::precalc_last(PARAM_LIST const* Scope)
{
  // BUG: e_val is not really const.
  //      this code looks misleading...
  _tnom.e_val(OPT::temp_k, Scope);
  _tnom_c.e_val(OPT::temp_k - P_CELSIUS0, Scope);

  _mfactor.e_val(1., Scope);     // incoming from arg
  _temperature.e_val(OPT::temp_k, Scope);
  _dtemp.e_val(0., Scope);
  _temp_c.e_val(OPT::temp_k - P_CELSIUS0, Scope);

  _method.e_val(vString(std::string("\"unknown\"")), Scope);
}
/*--------------------------------------------------------------------------*/
void HS_PARAM::precalc_hierarchy(PARAM_LIST const* Src, PARAM_LIST* Tgt) const
{
  assert(Src);
  assert(Tgt);
  assert(Src!=Tgt);

  double mfactor_fixed = precalc_mfactor(Src);
  trace1("precalc_hierarchy", mfactor_fixed);
  if(mfactor_fixed!=NOT_INPUT){
    Tgt->set("$mfactor", mfactor_fixed);
  }else{ untested();
  }

  method_t method_fixed = precalc_method(Src);
  if(method_fixed != meUNKNOWN){
    Tgt->set("$method", method_fixed);
  }else{ untested();
  }

  double temperature_fixed = precalc_temperature(Src);
  if(temperature_fixed != NOT_INPUT){
    Tgt->set("$temperature", temperature_fixed);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
double HS_PARAM::precalc_mfactor(PARAM_LIST const* Scope) const
{
  trace1("precalc_mfactor0", _mfactor.string());
  PARAMETER<double> mfactor_hier;
  mfactor_hier = "$mfactor";
  mfactor_hier.e_val(1., Scope); // incoming, from parent scope

  trace2("precalc_mfactor", _mfactor, mfactor_hier);
  return _mfactor * mfactor_hier;
}
/*--------------------------------------------------------------------------*/
double HS_PARAM::mfactor(PARAM_LIST const* Scope) const
{
  double mfactor_fixed = precalc_mfactor(Scope);
  trace1("mfactor(Scope)", mfactor_fixed);
  if(mfactor_fixed == NOT_INPUT) {
    return 1.;
  }else{
    // assert(_mfactor_fixed);
    return mfactor_fixed;
  }
}
/*--------------------------------------------------------------------------*/
double HS_PARAM::precalc_temperature(PARAM_LIST const* Scope) const
{
  PARAMETER<double> temperature_hier;
  temperature_hier = "$temperature";
  temperature_hier.e_val(NOT_INPUT, Scope);
  double temperature_fixed = NOT_INPUT;

  if(temperature_hier.has_good_value()) {
    temperature_fixed = temperature_hier;
  }else{
    temperature_fixed = NOT_INPUT;
  }

  if(_temperature.has_hard_value()) {
    trace1("hsp temp hard", _temperature);
    temperature_fixed = _temperature;
    if(_temp_c.has_hard_value()) { untested();
      error(bWARNING, "temp conflict, using $temperature 1\n");
    }else{
    }
    trace1("hsp temp0", temperature_fixed);
   //  _temp_c.set_default(_temperature - P_CELSIUS0);
  }else if(_dtemp.has_hard_value()) {
    temperature_fixed = temperature_hier + _dtemp;

    trace1("hsp dtemp hard", temperature_fixed);
    if(_temp_c.has_hard_value()) { untested();
      error(bWARNING, "temp conflict, using $dtemp\n");
    }else{
    }
  }else if(_temp_c.has_hard_value()) {
    temperature_fixed = _temp_c + P_CELSIUS0;
  }else{
  }

#if 0 // is this needed?
  if(_tnom.has_hard_value()) { untested();
    _params.set("$tnom", double(_tnom));
    if(_tnom_c.has_hard_value()) { untested();
      error(bWARNING, "tnom conflict, using $tnom\n");
    }else{ untested();
    }
  }else if(_tnom_c.has_hard_value()) { untested();
    _params.set("$tnom", double(_tnom_c - P_CELSIUS0));
  }else{
  }
#endif

  return temperature_fixed;
}
/*--------------------------------------------------------------------------*/
double HS_PARAM::temp_k(PARAM_LIST const* Scope) const
{
  double temperature_fixed = precalc_temperature(Scope);
  if(temperature_fixed == NOT_INPUT){
    return OPT::temp_k;
  }else{
    return temperature_fixed;
  }
}
/*--------------------------------------------------------------------------*/
double HS_PARAM::temp_diff(PARAM_LIST const* Scope) const
{
  // bug? duplicate precalc_temperature call.
  double temperature_fixed = precalc_temperature(Scope);
  return temperature_fixed - _tnom;
}
/*--------------------------------------------------------------------------*/
method_t HS_PARAM::precalc_method(PARAM_LIST const* Scope) const
{
  PARAMETER<int> method_hier{meUNKNOWN};
  method_hier = "$method";
  method_hier.e_val(OPT::method, Scope);

  method_t method_fixed;
  if(_method.has_hard_value()) {
    CS cmd(CS::_STRING, std::string("= ") + std::string(String(_method)));
    Get(cmd, "", &method_fixed);
  }else{
    method_fixed = meUNKNOWN;
  }

  method_fixed = method_propagate(method_t(int(method_hier)), method_fixed);
  trace1("precalc_method3 set", method_fixed);
  return method_fixed;
}
/*--------------------------------------------------------------------------*/
method_t HS_PARAM::method(PARAM_LIST const* Scope) const
{
  method_t method_fixed = precalc_method(Scope);
  if(method_fixed){
    return method_fixed;
  }else{ untested();
    return OPT::method;
  }
}
/*--------------------------------------------------------------------------*/
method_t HS_PARAM::method_propagate(method_t env, method_t here) const
{
  method_t prop[meNUM_METHODS][meNUM_METHODS] = { //
    /*vv OPT vv*/
    //local>>>EULER,      EULERONLY,  TRAP,     TRAPONLY, GEAR2,GEAR2ONLY,TRAPGEAR,TRAPEULER
    /*meUNKNOWN*/
    {meUNKNOWN,meEULER,   meEULERONLY,meTRAP,       meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER},
    /*meEULER*/
    {meEULER,   meEULER,   meEULERONLY,meTRAP,      meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER},
    /*meEULERONLY*/
    {meEULER,   meEULER,   meEULERONLY,meEULERONLY, meTRAPONLY,meEULER,   meGEAR2ONLY,meEULERONLY,meEULER},
    /*meTRAP*/
    {meTRAP,    meEULER,   meEULERONLY,meTRAP,      meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER},
    /*meTRAPONLY*/
    {meTRAPONLY,meTRAPONLY,meEULERONLY,meTRAPONLY,  meTRAPONLY,meTRAPONLY,meGEAR2ONLY,meTRAPONLY, meTRAPONLY},
    /*meGEAR*/
    {meGEAR2,   meEULER,   meEULERONLY,meTRAP,      meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER},
    /*meGEAR2ONLY*/
    {meGEAR2,   meGEAR2,   meEULERONLY,meGEAR2ONLY, meTRAPONLY,meGEAR2,   meGEAR2ONLY,meGEAR2ONLY,meGEAR2},
    /*meTRAPGEAR*/
    {meTRAPGEAR,meEULER,   meEULERONLY,meTRAP,      meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER},
    /*meTRAPEULER*/
    {meTRAPEULER,meEULER,  meEULERONLY,meTRAP,      meTRAPONLY,meGEAR2,   meGEAR2ONLY,meTRAPGEAR, meTRAPEULER} //
  };
  assert(env<meNUM_METHODS);
  assert(here<meNUM_METHODS);
  return prop[env][here];
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
