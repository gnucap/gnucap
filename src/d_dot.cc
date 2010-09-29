/*$Id: d_dot.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * processing for DOTCARD netlist item (pseudo-device)
 * "parsing" a "dotcard" will execute it.
 * What actually happens (usually either to preset or really do)
 * depends on the option "run_mode", which depends on what command is running.
 */
//testing=script 2006.07.17
#include "c_comand.h"
#include "d_dot.h"
/*--------------------------------------------------------------------------*/
//	void	DEV_DOT::parse_spice(CS& cmd);
//	void	DEV_DOT::print(int,int)const;
/*--------------------------------------------------------------------------*/
void DEV_DOT::parse_spice(CS& cmd)
{
  _s = cmd.fullstring();
  switch (ENV::run_mode) {
  case rIGNORE:
    unreachable();
    /*nothing*/
    break;
  case rBATCH: untested();
  case rINTERACTIVE: untested();
  case rPRESET:
  case rSCRIPT:
    cmd.skip1b(".");
    CMD::cmdproc(cmd.tail());
    break;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_DOT::print_spice(OMSTREAM& where, int)const
{
  where << _s << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
