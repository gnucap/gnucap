/*$Id: c_insert.cc,v 20.7 2001/09/29 05:31:06 al Exp $ -*- C++ -*-
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
 * insert command
 * insert new nodes
 * Doesn't really work.
 * Appears to insert into net list, but does not update anything else.
 * It may go away.
 */
#include "e_card.h"
#include "u_status.h"
#include "ap.h"
#include "s__.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::insert(CS&);
/*--------------------------------------------------------------------------*/
void CMD::insert(CS& cmd)
{
  int nod;		/* first node number to insert */
  int ncnt;		/* how many nodes to insert */
  
  SIM::uninit();
  nod = cmd.ctoi();
  if (nod <= 0)
    error(bERROR, "insert which node?\n");
  if (nod > STATUS::total_nodes)
    error(bERROR, "%u nodes\n", STATUS::total_nodes);
  
  ncnt = cmd.ctoi();
  if (ncnt <= 0)
    ncnt = 1;
  
  untested();
  for (CARD_LIST::iterator 
	 ci=CARD_LIST::card_list.begin();ci!=CARD_LIST::card_list.end();++ci) {
    CARD* brh = *ci;
    if (brh->is_device()) {
      for (int ii = 0;  ii < brh->port_count();  ++ii) {
	untested();
	if (brh->_n[ii].e >= nod)		/* move them up */
	  brh->_n[ii].e += ncnt;
      }
    }
  untested();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
