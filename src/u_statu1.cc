/*$Id: u_statu1.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * place to store all kinds of statistics -- initialization
 */
#include "u_status.h"

TIMER	STATUS::get("get");
TIMER	STATUS::op("op");
TIMER	STATUS::dc("dc");
TIMER	STATUS::tran("tran");
TIMER	STATUS::four("fourier");
TIMER	STATUS::ac("ac");
TIMER	STATUS::set_up("setup");
TIMER	STATUS::order("order");
TIMER	STATUS::advance("advance");
TIMER	STATUS::evaluate("evaluate");
TIMER	STATUS::load("load");
TIMER	STATUS::lud("lu");
TIMER	STATUS::back("back");
TIMER	STATUS::review("review");
TIMER	STATUS::accept("accept");
TIMER	STATUS::output("output");
TIMER	STATUS::overhead("overhead");
TIMER	STATUS::aux1("aux1");
TIMER	STATUS::aux2("aux2");
TIMER	STATUS::aux3("aux3");
TIMER	STATUS::total("total");
int	STATUS::user_nodes = 0;
int	STATUS::subckt_nodes = 0;
int	STATUS::model_nodes = 0;
int	STATUS::total_nodes = 0;
int	STATUS::control[cCOUNT] = {0};
int	STATUS::iter[iCOUNT] = {0};
