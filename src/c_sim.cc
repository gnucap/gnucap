/*$Id: c_sim.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * command interface functions associated with the SIM base class
 */
#include "s_ac.h"
#include "s_dc.h"
#include "s_tr.h"
#include "s_fo.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
void CMD::ac(CS& cmd)		{static AC ac;		ac.command(cmd);}
void CMD::dc(CS& cmd)		{static DC dc;		dc.command(cmd);}
void CMD::fourier(CS& cmd)	{static FOURIER fo;	fo.command(cmd);}
void CMD::op(CS& cmd)		{static OP op;		op.command(cmd);}
void CMD::tr(CS& cmd)		{static TRANSIENT tr;	tr.command(cmd);}
void CMD::mark(CS&)		{SIM::freezetime = true;}
void CMD::unmark(CS&)		{SIM::freezetime = false;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
