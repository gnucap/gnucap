/*$Id: d_coment.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * processing for COMMENT netlist item (pseudo-device)
 * Old C style hack:
 * uses the space in a "card" struct, overwrites the leftover
 * Maybe the class hierarchy needs another level
 */
//testing=script,complete 2006.07.17
#include "d_coment.h"
/*--------------------------------------------------------------------------*/
//	void	DEV_COMMENT::parse_spice(CS& cmd);
//	void	DEV_COMMENT::print(int,int)const;
/*--------------------------------------------------------------------------*/
void DEV_COMMENT::parse_spice(CS& cmd)
{
  _s = cmd.fullstring();
}
/*--------------------------------------------------------------------------*/
void DEV_COMMENT::print_spice(OMSTREAM& where, int)const
{
  if (_s[0] != '*'  ||  _s[1] != '+') {
    where << _s << '\n';
  }else{
  }
  // Suppress printing of comment lines starting with "*+".
  // These are generated as a way to display calculated values.
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
