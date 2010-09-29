/*$Id: s_ac.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * ac analysis top
 */
#include "u_opt.h"
#include "u_status.h"
#include "s_ac.h"
/*--------------------------------------------------------------------------*/
//	void	AC::command(CS&);
/*--------------------------------------------------------------------------*/
void AC::command(CS& cmd)
{
  mode = sAC;
  reset_timers();
  STATUS::ac.reset().start();
  init();
  alloc_vectors();
  acx.allocate().set_min_pivot(OPT::pivtol);
  setup(cmd);
  STATUS::set_up.stop();
  switch (ENV::run_mode){
  case rINTERACTIVE:
  case rSCRIPT:
  case rBATCH:
    sweep();
    break;
  case rIGNORE:
  case rPRESET:
    /*nothing*/
    break;
  }
  acx.unallocate();
  unalloc_vectors();
  
  STATUS::ac.stop();
  STATUS::total.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
