/*$Id: c_param.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 */
//testing=script,complete 2006.07.17
#include "u_parameter.h"
#include "s__.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void CMD::param(CS&cmd)
{
  PARAM_LIST* pl = CARD_LIST::card_list.params();
  if (cmd.is_end()) {
    pl->print(IO::mstdout);
    IO::mstdout << '\n';
  }else{
    SIM::uninit();
    pl->parse(cmd);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
