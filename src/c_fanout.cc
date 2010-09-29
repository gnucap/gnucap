/*$Id: c_fanout.cc,v 24.2 2003/02/23 09:18:00 al Exp $ -*- C++ -*-
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
 * Prints out a list of all node connections.
 */
#include "ap.h"
#include "e_card.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::fanout(CS&);
static	void	checklist(const CARD_LIST&, const node_t&, OMSTREAM);
/*--------------------------------------------------------------------------*/
void CMD::fanout(CS& cmd)
{
  OMSTREAM out = IO::mstdout;
  int start = -1;
  int stop  = -1;
  for (;;){
    {if (cmd.is_digit()){
      int temp = cmd.ctoi();
      {if (cmd.match1('-')){
	untested();
	cmd.skip();
	start = temp;
      }else if (start < 0){
	start = temp;
	{if (stop < 0){
	  stop = start;
	}else{
	  untested();
	}}
      }else{
	untested();
	stop = temp;
      }}
    }else if (cmd.match1('-')){
      cmd.skip();
      cmd.skipbl();
      {if (cmd.is_digit()){
	untested();
	stop = cmd.ctoi();
      }else{
	untested();
      }}
    }else{
      outset(cmd,&out);
      cmd.check(bWARNING, "what's this");
      break;
    }}
  }
  SIM::init();

  if (start < 0){
    start = 0;
  }
  if (stop < 0  ||  stop > STATUS::total_nodes){
    stop = STATUS::user_nodes;
  }
  if (start>STATUS::total_nodes){
    untested();
    error(bERROR, "%u nodes\n", STATUS::total_nodes);
  }
  
  out << "Node:  Branches\n";
  for (int ii = start;  ii <= stop;  ++ii){
    out.form("%4u:",ii);
    checklist(CARD_LIST::card_list, node_t(ii), out);
    out << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/* checklist: print a list of all elements connecting to a node
 *	recursive, for subckts
 */
static void checklist(const CARD_LIST& cl, const node_t& node, OMSTREAM out)
{
  for (CARD_LIST::const_iterator ci = cl.begin(); ci != cl.end(); ++ci){
    if ((**ci).is_device()){
      if ((**ci).connects_to(node)){
	out << ' ' << (**ci).long_label();
      }
      checklist((**ci).subckt(), node, out);
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
