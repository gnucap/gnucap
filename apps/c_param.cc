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
void parse(CS& cmd, PARAM_LIST* pl)
{
  assert(pl);
  int type = 0;
  if(cmd >> "real"){ untested();
    type = 1;
  }else if(cmd >> "integer"){ untested();
    type = 2;
  }else{ untested();
  }
  size_t here = cmd.cursor();
  for (;;) { untested();
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) { untested();
      break;
    }else{ untested();
    }
    std::string Name;
    PARAM_INSTANCE par;
      PARAMETER<double> Value;
    switch(type){
    case 2: untested();
	    incomplete();
      par = PARAMETER<int>();
      break;
    default: untested();
      par = PARAMETER<double>();
      cmd >> Name >> '=' >> Value;
      break;
    }

    trace1("parsed", par.string());
    if (cmd.stuck(&here)) {untested();
      break;
    }else{ untested();
    }
    if (OPT::case_insensitive) { untested();
      notstd::to_lower(&Name);
    }else{ untested();
    }
    pl->set(Name, Value);
   // pl->set(Name, par);
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
      parse(cmd, pl);
    }
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "param|parameters|parameter", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
