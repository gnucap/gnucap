/*$Id: s_ac_slv.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * ac solution
 */
#include "e_cardlist.h"
#include "u_status.h"
#include "s_ac.h"
/*--------------------------------------------------------------------------*/
//	void	AC::solve(void);
//	void	AC::clear(void);
/*--------------------------------------------------------------------------*/
void AC::solve(void)
{
  clear();
  STATUS::load.start();
  ++STATUS::iter[iTOTAL];
  CARD_LIST::card_list.do_ac();
  STATUS::load.stop();
  acx.lu_decomp();
  acx.fbsub(ac);
}
/*--------------------------------------------------------------------------*/
void AC::clear(void)
{
  acx.zero();
  for (int ii=0;  ii <= STATUS::total_nodes;  ++ii){
    ac[ii] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
