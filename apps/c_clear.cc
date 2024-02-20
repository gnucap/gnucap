/*$Id: c_clear.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
 * delete and clear commands
 */
//testing=script,complete 2006.07.16

#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/* cmd_clear: clear the whole circuit, including faults, etc
 *   equivalent to unfault; unkeep; delete all; title = (blank)
 */
class CMD_CLEAR : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)override {
    command("unfault", Scope);
    command("unmark", Scope);
    //command("ic clear", Scope);
    //command("nodeset clear", Scope);
    command("alarm clear", Scope);
    command("plot clear", Scope);
    command("print clear", Scope);
    command("delete all", Scope);
    command("title '", Scope);
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "clear", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
