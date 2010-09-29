/*$Id: s_dc.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * dc analysis top
 */
//testing=script,complete 2006.07.14
#include "s_dc.h"
/*--------------------------------------------------------------------------*/
//	void	DC::command(CS&);
//	void	OP::command(CS&);
/*--------------------------------------------------------------------------*/
void DC::command(CS& cmd)
{
  time0 = 0.;
  mode = sDC;
  phase = pINIT_DC;
  ::status.dc.reset().start();
  command_base(cmd);
  ::status.dc.stop();
}
/*--------------------------------------------------------------------------*/
void OP::command(CS& cmd)
{
  time0 = 0.;
  mode = sOP;
  phase = pINIT_DC;
  ::status.op.reset().start();
  command_base(cmd);
  ::status.op.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
