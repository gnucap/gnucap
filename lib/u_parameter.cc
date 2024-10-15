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
    }else{ untested();
    }
    std::string Name;
    PARAMETER<double> Value;
    assert(0);
    cmd >> Name >> '=' >> Value;
    trace2("parsed", Value, Value.string());
    if (cmd.stuck(&here)) {untested();
      break;
    }else{ untested();
    }
    if (OPT::case_insensitive) { untested();
      notstd::to_lower(&Name);
    }else{ untested();
    }
    _pl[Name] = Value;
    trace4("assigned", _pl[Name].string(), Value.string(), &_pl[Name], _pl[Name].operator->());
  }
  cmd.check(bDANGER, "syntax error");
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::print(OMSTREAM& o, LANGUAGE* lang)const
{
  for (const_iterator i = _pl.begin(); i != _pl.end(); ++i) {
    if (i->second.has_hard_value()) {
      print_pair(o, lang, i->first, i->second);
    }else{ untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
bool PARAM_LIST::is_printable(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.has_hard_value();
    }else{
    }
  }
  return false;
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::name(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->first;
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::value(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.string();
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::eval_copy(PARAM_LIST const& p, const CARD_LIST* scope)
{
  assert(scope);
  assert(!_try_again);
  _try_again = p._try_again;

  for (const_iterator i = p._pl.begin(); i != p._pl.end(); ++i) {
    if (i->second.has_hard_value()) {
      auto j = _pl.find(i->first);
      if(j == _pl.end()){
	PARAM_INSTANCE& pi = _pl[i->first]; // create one.
	
	trace2("eval_copy not there", i->first, _pl.size());
	if(!_try_again){itested();
	}else{
	  auto k = _try_again->find(i->first);
	  if(k == _try_again->end()){
	    trace0("not again");
	    // spice feature: create parameters from arglist
	    // should not get here in verilog mode
	    static PARAMETER<double> f;
	    pi = f; // what it used to be.
	  }else{
	    trace1("got one", i->first);
	    // get type from proto
	    pi = k->second;
	  }

	}

	Base const* b = i->second.e_val(nullptr, scope);
	trace1("eval_copy value", typeid(*b).name());

	pi.set_fixed(b);

      }else if(j->second.has_hard_value()) {untested();
	j->second.set_fixed(i->second.e_val(j->second.value(), scope));
      }else{ untested();
	// this is not needed.
      }
    }else{ untested();
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
  const_iterator i = _pl.find(Name);
  if (i!=_pl.end() && i->second.has_hard_value()) {
    // found a value, return it
    return i->second;
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
Base const* PARAM_INSTANCE::e_val(Base const* def, const CARD_LIST* scope) const
{
  static int recursion;
  if (++recursion > OPT::recursion) { untested();
    recursion = 0;
    throw Exception("recursion too deep");
  }else{
  }
  if(dynamic_cast<PARA_NONE const*>(base())) {
    assert(!base()->e_val_(def, scope, 1));
  }else{
  }

  // try { untested();

  Base const* ret = nullptr;

  if(base()) {
    ret = base()->e_val_(def, scope, 1);
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
    _pl[Name].set_fixed(&v);
  }catch(Exception_Clash const&){ untested();
    (_pl[Name] = "").set_fixed(&v);
    error(bTRACE, Name + " already set. replacing\n");
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const std::string& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  PARAM_INSTANCE& p = _pl[Name];
  if(p.exists()){
    try{
      p = Value;
    }catch(Exception_Clash const&){
      (p = "") = Value;
      error(bTRACE, Name + " already set. replacing\n");
    }
  }else{
    // spice fallback.
    p = PARAMETER<double>();
    p = Value;
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const PARAM_INSTANCE& Value)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&Name);
  }else{
  }
  try{
    _pl[Name] = Value;
  }catch(Exception_Clash const&){ untested();
    (_pl[Name] = "") = Value;
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
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
