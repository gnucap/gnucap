/*$Id: s_tr.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * tran and fourier commands -- top
 */
//testing=script,complete 2005.09.17
#include "u_status.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
int TRANSIENT::steps_accepted_;
int TRANSIENT::steps_rejected_;
int TRANSIENT::steps_total_;
/*--------------------------------------------------------------------------*/
void TRANSIENT::command(CS& cmd)
{
  mode = sTRAN;
  ::status.tran.reset().start();
  command_base(cmd);
  ::status.tran.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
