/*$Id: c_param.cc,v 26.130 2009/11/15 21:51:59 al Exp $ -*- C++ -*-
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
 */
//testing=script,complete 2006.07.17
#include "c_comand.h"
#include "u_parameter.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
void parse(CS& cmd, PARAM_LIST* pl, CARD_LIST* Scope)
{
  assert(pl);
  assert(Scope);
  PARAM_INSTANCE par;
  int type = 0;
  if(cmd >> "real"){
    type = 1;
  }else if(cmd >> "integer"){
    type = 2;
  }else if(cmd >> "string"){
    type = 3;
  }else{
  }
  assert(Scope);
  if(Scope->is_verilog_math()){
    type += 10;
    if(Scope == &CARD_LIST::card_list) {
      error(bLOG, "Top level parameter. Not within spec, use with care\n");
    }else{ untested();
    }
  }else if(type == 0){
    error(bNOERROR, "Untyped top level parameter. Assuming Float\n");
  }

  switch(type){
  case 1: untested();
	  par = PARAMETER<Float>();
	  break;
  case 11:
	  par = PARAMETER<vReal>();
	  break;
  case 2:
	  par = PARAMETER<Integer>();
	  break;
  case 12:
	  par = PARAMETER<vInteger>();
	  break;
  case 3:
	  par = PARAMETER<vString>(); // !
	  break;
  case 13:
	  par = PARAMETER<vString>();
	  break;
  case 10:
	  par = PARAMETER<vReal>();
	  // fallback. see lang_verilog.
	  break;
  default:
	  par = PARAMETER<Float>();
	  break;
  }

  size_t here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }else{
    }
    std::string Name;

    cmd >> Name >> '=';
    par.obsolete_parse(cmd);

    trace2("parsed", Name, par.string());
    if (cmd.stuck(&here)) {untested();
      break;
    }else{
    }
    if (OPT::case_insensitive) {
      notstd::to_lower(&Name);
    }else{
    }
    pl->set(Name, par);
  }
  cmd.check(bDANGER, "syntax error");
}
/*--------------------------------------------------------------------------*/
class CMD_PARAM : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    PARAM_LIST* pl = Scope->params();
    if (cmd.is_end()) {
      pl->print(IO::mstdout, OPT::language);
      IO::mstdout << '\n';
    }else{
      parse(cmd, pl, Scope);
    }
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "param|parameters|parameter", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
