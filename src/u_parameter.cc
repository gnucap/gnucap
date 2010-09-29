/*$Id: u_parameter.cc,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
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
 * Used for .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script,sparse 2006.07.14
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
void PARAM_LIST::parse(CS& cmd)
{
  int here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }
    std::string name;
    PARAMETER<double> value;
    cmd >> name >> '=' >> value;
    if (cmd.stuck(&here)) {untested();
      break;
    }
    _pl[name] = value;      
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::print(OMSTREAM& o)const
{
  for (const_iterator i = _pl.begin(); i != _pl.end(); ++i) {
    if (i->second.has_value()) {
      o << "  " << i->first << '=' << i->second;
    }else{untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::eval_copy(PARAM_LIST& p, const CARD_LIST* scope)
{
  for (iterator i = p._pl.begin(); i != p._pl.end(); ++i) {
    if (i->second.has_value()) {
      _pl[i->first] = i->second.e_val(_pl[i->first], scope);
    }else{untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
PARAMETER<double> PARAM_LIST::operator[](const std::string& i)
{
  PARAMETER<double> & rv = _pl[i];
  if (rv.has_value()) {
    // found a value, return it
    return rv;
  }else if (try_again) {untested();
    // didn't find one, look in enclosing scope
    return (*try_again)[i];
  }else{itested();
    // no enclosing scope to look in
    // really didn't find it, give up
    // return garbage value (NOT_INPUT)
    return rv;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, PARAMETER<bool>* val)
{
  if (cmd.dmatch(key)) {
    if (cmd.skip1b('=')) {
      cmd >> *val;
    }else{
      *val = true;
    }
    return true;
  }else if (cmd.dmatch("NO" + key)) {
    *val = false;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, PARAMETER<int>* val)
{
  if (cmd.dmatch(key)) {
    cmd.skip1b('=');
    *val = int(cmd.ctof());
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
