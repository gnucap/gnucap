/*$Id: c_fanout.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Prints out a list of all node connections.
 */
//testing=none 2006.07.17
#include "e_card.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::fanout(CS&);
static	void	checklist(const CARD_LIST*, const node_t&, OMSTREAM);
/*--------------------------------------------------------------------------*/
void CMD::fanout(CS& cmd)
{
  error(bERROR, "doesn't work with named nodes\n");
  OMSTREAM out = IO::mstdout;
  int start = -1;
  int stop  = -1;
  for (;;) {
    {if (cmd.is_digit()) {
      int temp = cmd.ctoi();
      {if (cmd.match1('-')) {
	untested();
	cmd.skip();
	start = temp;
      }else if (start < 0) {
	start = temp;
	{if (stop < 0) {
	  stop = start;
	}else{
	  untested();
	}}
      }else{
	untested();
	stop = temp;
      }}
    }else if (cmd.match1('-')) {
      cmd.skip();
      cmd.skipbl();
      {if (cmd.is_digit()) {
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
  //SIM::init();

  if (start < 0) {
    start = 0;
  }
  if (stop < 0  ||  stop > ::status.total_nodes) {
    stop = ::status.user_nodes;
  }
  if (start>::status.total_nodes) {
    untested();
    error(bERROR, "%u nodes\n", ::status.total_nodes);
  }
  
  out << "Node:  Branches\n";
  for (int ii = start;  ii <= stop;  ++ii) {
    out.form("%4u:",ii);
    //checklist(&CARD_LIST::card_list, node_t(ii), out);
    out << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/* checklist: print a list of all elements connecting to a node
 *	recursive for subckts
 */
static void checklist(const CARD_LIST* cl, const node_t& node, OMSTREAM out)
{
  untested();
  assert(cl);

  for (CARD_LIST::const_iterator ii = cl->begin(); ii != cl->end(); ++ii) {
    const CARD* brh = *ii;
    assert(brh);

    if (brh->is_device()) {
      if (brh->connects_to(node)) {
	out << ' ' << brh->long_label();
      }
      if (brh->subckt()) {
	checklist(brh->subckt(), node, out);
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
