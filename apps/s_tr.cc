/*$Id: s_tr.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * tran and fourier commands -- top
 */
//testing=script,complete 2007.11.22
#include "u_sim_data.h"
#include "u_status.h"
#include "s_tr.h"
static TRANSIENT p5;
DISPATCHER<CMD>::INSTALL      d5(&command_dispatcher, "transient", &p5);
DISPATCHER<CKT_BASE>::INSTALL d6(&status_dispatcher,  "transient", &p5);
/*--------------------------------------------------------------------------*/
int TRANSIENT::steps_accepted_;
int TRANSIENT::steps_rejected_;
int TRANSIENT::steps_total_;
/*--------------------------------------------------------------------------*/
void TRANSIENT::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->set_command_tran();
  ::status.tran.reset().start();
  command_base(Cmd);
  ::status.tran.stop();
}
/*--------------------------------------------------------------------------*/
std::string TRANSIENT::status()const
{
  return "transient timesteps: accepted=" + to_string(steps_accepted())
    + ", rejected=" + to_string(steps_rejected())
    + ", total=" + to_string(steps_total()) + "\n";  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
