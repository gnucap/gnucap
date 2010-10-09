/*$Id: c_measure.cc,v 26.113 2009/08/12 03:37:19 al Exp $ -*- C++ -*-
 * Copyright (C) 2008 Albert Davis
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
//testing=none
#include "u_parameter.h"
#include "u_function.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_MEASURE : public CMD {
public:
  void do_it(CS& Cmd, CARD_LIST* Scope)
  {
    std::string assign_to, function;
    Cmd >> assign_to >> '=' >> function >> '(';
    if (FUNCTION* f = measure_dispatcher[function]) {
      std::string value = f->eval(Cmd, Scope);
      if (!Cmd.skip1b(')')) {
	Cmd.warn(bWARNING, "need )");
      }else{
      }
      OMSTREAM out = IO::mstdout;
      outset(Cmd, &out);
      out << assign_to << '=' << value << '\n';
      PARAM_LIST* pl = Scope->params();
      pl->set(assign_to, value);
    }else{
      throw Exception_No_Match(function);
    }
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "measure", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
