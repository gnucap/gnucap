/*$Id: c_sim.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * command interface functions associated with the SIM base class
 */
//testing=script 2006.07.17
#include "u_sim_data.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_MARK : public CMD {
public:
  void do_it(CS&, CARD_LIST*)
  {itested();
    _sim->_freezetime = true;
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, "mark|freeze", &p6);
/*--------------------------------------------------------------------------*/
class CMD_UNMARK : public CMD {
public:
  void do_it(CS&, CARD_LIST*)
  {
    _sim->_freezetime = false;
  }
} p7;
DISPATCHER<CMD>::INSTALL d7(&command_dispatcher, "unmark|unfreeze", &p7);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
