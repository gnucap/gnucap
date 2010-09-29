/*$Id: s__map.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Allocates space for the admittance matrix.  Allocation below the
 * diagonal is by row, above the diagonal is by column, and stored backwards.
 * This broken vector is stored.  The length of it increases with increasing
 * position.  The maximum length of the nth vector is 2n-1.  For a band matrix
 * only those elements that are non-zero or are nearer to the diagonal than a
 * non-zero element are stored.
 */
//testing=script 2006.07.14
#include "e_cardlist.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
//static void	 SIM::map_nodes();
//static void	 SIM::order_reverse();
//static void	 SIM::order_forward();
//static void	 SIM::order_auto();
/*--------------------------------------------------------------------------*/
/* map_nodes: map intermediate node number to internal node number.
 * Ideally, this function would find some near-optimal order
 * and squash out gaps.
 */
/*static*/ void SIM::map_nodes()
{
  nm = new int[::status.total_nodes+1];
  ::status.order.reset().start();
  switch (OPT::order) {
    default: untested();
      error(bWARNING, "invalid order spec: %d\n", OPT::order);
    case oAUTO:		       order_auto();    break;
    case oREVERSE: untested(); order_reverse(); break;
    case oFORWARD: untested(); order_forward(); break;
  }
  ::status.order.stop();
  CARD_LIST::card_list.map_nodes();
}
/*--------------------------------------------------------------------------*/
/* order_reverse: force ordering to reverse of user ordering
 *  subcircuits at beginning, results on border at the bottom
 */
/*static*/ void SIM::order_reverse()
{untested();
  nm[0] = 0;
  for (int node = 1;  node <= ::status.total_nodes;  ++node) {untested();
    nm[node] = ::status.total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/* order_forward: use user ordering, with subcircuits added to end
 * results in border at the top (worst possible if lots of subcircuits)
 */
/*static*/ void SIM::order_forward()
{untested();
  nm[0] = 0;
  for (int node = 1;  node <= ::status.total_nodes;  ++node) {untested();
    nm[node] = node;
  }
}
/*--------------------------------------------------------------------------*/
/* order_auto: full automatic ordering
 * reverse, for now
 */
/*static*/ void SIM::order_auto()
{
  nm[0] = 0;
  for (int node = 1;  node <= ::status.total_nodes;  ++node) {
    nm[node] = ::status.total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
