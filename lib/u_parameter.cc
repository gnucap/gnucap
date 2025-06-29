/*$Id: u_parameter.cc,v 26.119 2009/09/09 13:27:53 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script,sparse 2006.07.14
#include "l_stlextra.h"
#include "u_parameter.h"
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
void PARAM_LIST::parse(CS& cmd)
{
  (cmd >> "real |integer "); // ignore type
  size_t here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }else{itested();
    }
    std::string Name;
    PARAMETER<double> Value;
    cmd >> Name >> '=' >> Value;
    trace2("parsed", Value, Value.string());
    if (cmd.stuck(&here)) {untested();
      break;
    }else{itested();
    }
    if (OPT::case_insensitive) { untested();
      notstd::to_lower(&Name);
    }else{itested();
    }
    ref(Name) = Value;
  }
  cmd.check(bDANGER, "syntax error");
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::print(OMSTREAM& o, LANGUAGE* lang)const
{
  for (int i=0; i<int(_pv.size()); ++i) {
    if (_pv[i].second.has_hard_value()) {
      print_pair(o, lang, _pv[i].first, _pv[i].second);
    }else{ untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
bool PARAM_LIST::is_printable(int i)const
{
  if(i<0){ untested();
  }else if(i<int(_pv.size())){
    return _pv[i].second.has_hard_value();
  }else{ untested();
  }
  return false;
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::name(int i)const
{
  if(i<0){ untested();
  }else if(i<int(_pv.size())){
    return _pv[i].first;
  }else{ untested();
  }
  return "??";
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::value(int i)const
{
  if(i<0){ untested();
  }else if(i<int(_pv.size())){
    return _pv[i].second.string();
  }else{ untested();
  }
  return "??";
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::eval_copy(PARAM_LIST const& p, const PARAM_LIST* scope)
{
  assert(scope);
  assert(!_try_again);
  _try_again = p._try_again;
  _is_verilog = scope->_is_verilog;

  // BUG: wrong order.
  for (auto i = p._pv.begin(); i != p._pv.end(); ++i) {
    if (i->second.has_hard_value()) {
      auto j = _pi.find(i->first);
      if(j == _pi.end()){
	PARAM_INSTANCE& pi = ref(i->first); // create one.
	
	trace2("eval_copy not there", i->first, _pi.size());
	if(!_try_again){itested();
	}else{
	  const_iterator k = _try_again->find(i->first);
	  if(k == _try_again->end()){
	    trace0("not again");
	    // spice feature: create parameters from arglist
	    // should not get here in verilog mode
	    static PARAMETER<double> f;
	    pi = f; // what it used to be.
	  }else{
	    // get type from proto
	    pi = k.ref();
	  }

	}

	Base const* b = i->second.e_val(nullptr, scope);
	if(b && !b->is_NA()) {
	  pi.set_fixed(b->clone());
	}else{
	}

      }else{ itested();
	_pv[j->second-1].second.set_fixed(i->second.e_val(_pv[j->second-1].second.value(), scope));
      }
    }else{itested();
    }
  }
}
/*--------------------------------------------------------------------------*/
const PARAM_INSTANCE& PARAM_LIST::deep_lookup(std::string Name)const
{
  trace1("PARAM_LIST::deep_lookup", Name);
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  const_iterator i = find(Name);
  if (i!=end() && i.ref().has_hard_value()) {
    // found a value, return it
    return i.ref();
  }else if (_try_again) {
    // didn't find one, look in enclosing scope
    return _try_again->deep_lookup(Name);
  }else{
    // no enclosing scope to look in
    // really didn't find it, give up
    // return garbage value (NOT_INPUT)
    static PARAM_INSTANCE garbage;
    return garbage;
  }
}
/*--------------------------------------------------------------------------*/
Base const* PARAM_INSTANCE::e_val(Base const* def, const PARAM_LIST* scope) const
{
  static int recursion;
  if (++recursion > OPT::recursion) {itested();
    recursion = 0;
    throw Exception("recursion too deep");
  }else{
  }

  // try { untested();

  Base const* ret = nullptr;

  if(base()) {
    assert(recursion);
    ret = base()->e_val_(def, scope, recursion);
  }else{ untested();
  }

  --recursion;
  return ret;

  // }catch(Exception const& e){ untested();
  //   unreachable();
  //   return nullptr;
  // }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const double& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  Float v(Value);
  try{
    ref(Name).set_fixed(&v);
  }catch(Exception_Clash const&){ untested();
    (ref(Name) = "").set_fixed(&v);
    error(bTRACE, Name + " already set. replacing\n");
  }
}
/*--------------------------------------------------------------------------*/
int PARAM_LIST::set(std::string Name, const std::string& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  PARAM_INSTANCE& p = ref(Name);
  if(p.exists()){
    try{
      p = Value;
    }catch(Exception_Clash const&){
      (p = "") = Value;
      error(bTRACE, Name + " already set. replacing\n");
    }
  }else{
    trace2("PARAM_LIST::set", Name, Value);
    p = Value;
  }
  assert(_pi[Name]);
  return _pi[Name]-1;
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const PARAM_INSTANCE& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  try{
    ref(Name) = Value;
  }catch(Exception_Clash const&){ untested();
    (ref(Name) = "") = Value;
    error(bTRACE, Name + " already set. replacing\n");
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val)
{
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {
      cmd >> *val;
    }else{
      *val = true;
    }
    return true;
  }else if (cmd.umatch("no" + key)) {
    *val = false;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val)
{
  if (cmd.umatch(key + " {=}")) {
    *val = int(cmd.ctof());
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// similar in PARAMETER<T>
// make it all Base* and move to PARA_BASE?
void PARAM_INSTANCE::PARAM_ANY::lookup_solve(const PARAM_LIST* scope) const
{
  CS cmd(CS::_STRING, _s);
  Expression e(cmd);
  Expression reduced(e, scope);

  delete _v;
  _v = nullptr;
  {
    Base const* v = reduced.value();

    if(v && v->is_NA()) { untested();
    }else if(v){
      _v = v->clone();
      assert(_v);
    }else{
    }
  }

  if (_v) {
    // OK
  }else{
    Base const* b = scope->deep_lookup(_s).e_val(nullptr, scope);
    if(b && !b->is_NA()){ untested();
      error(bWARNING, "parameter " + _s +  "  specified\n");
      _v = b->clone();
    }else if(b){ untested();
      error(bWARNING, "parameter " + _s +  " not specified, using default\n");
    }else{
      error(bWARNING, "parameter " + _s +  " not specified, using default\n");
    }
  }
}
/*--------------------------------------------------------------------------*/
// duplicate of PARAMETER<T>::e_val_
// make it all Base* and move to PARA_BASE?
Base const* PARAM_INSTANCE::PARAM_ANY::e_val_(const Base* Def, const PARAM_LIST*
    scope, int recurse) const
{
  if (_s == "") {
    delete _v;
    _v = nullptr;
    // blank string means to use default value
    if(Def){itested();
      _v = Def->clone();
    }else{
    }
    if (recurse) {
      // error(bWARNING, "?parameter " + _s +  " not specified, using default\n");
    }else{ untested();
    }
  }else if (_s != "#") {
    // anything else means look up the value
    lookup_solve(scope);
    if (!_v || _v->is_NA()) {
      //BUG// needs to show scope
      //BUG// it is likely to have a numeric overflow resulting from the bad value
      error(bDANGER, "parameter " + _s + " value is \"NOT_INPUT\"\n");
      // throw Exception(": " + _s + " value is \"NOT_INPUT\"\n");
    }else if(!_v){ untested();
      error(bDANGER, "parameter " + _s + " value is \"NOT_INPUT\"\n");
    }else{
    }
  }else{ untested();
    // start with # means we have a final value
  }

  return _v;
}
/*--------------------------------------------------------------------------*/
PARAM_INSTANCE& PARAM_LIST::ref(std::string const& Name)
{
  if(int& idx = _pi[Name]){
    assert(_pv[idx-1].first == Name);
    return _pv[idx-1].second;
  }else{
    _pv.push_back(std::make_pair(Name, PARAM_INSTANCE()));
    idx = int(_pv.size());
    return _pv.back().second;
  }
}
/*--------------------------------------------------------------------------*/
PARAM_INSTANCE& PARAM_LIST::at(std::string const& Name)
{ untested();
  if(int& idx = _pi[Name]){ untested();
    assert(_pv[idx-1].first == Name);
    return _pv[idx-1].second;
  }else{ untested();
    throw std::out_of_range("");
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set_temperature(double x)
{
  set("$temperature", x);
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set_mfactor(double)
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set_scalefactor(double)
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
double PARAM_LIST::temperature() const
{
  PARAMETER<double> temperature_hier;
  temperature_hier = "$temperature";
  temperature_hier.e_val(NOT_INPUT, this);

  if(temperature_hier.has_good_value()) {
    return temperature_hier;
  }else{
    return NOT_INPUT;
  }
}
/*--------------------------------------------------------------------------*/
double PARAM_LIST::mfactor() const
{
  incomplete();
  return 0.;
}
/*--------------------------------------------------------------------------*/
double PARAM_LIST::scalefactor() const
{
  incomplete();
  return 0.;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
