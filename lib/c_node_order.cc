/*                     -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
/*--------------------------------------------------------------------------*/
#include "globals.h"
#include "c_comand.h"
#include "u_sim_data.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* order_reverse: force ordering to reverse of user ordering
 *  subcircuits at beginning, results on border at the bottom
 */
class INTERFACE CMD_ORDER_REVERSE : public CMD {
  void do_it(CS&, CARD_LIST*)override {
    int* nm = _sim->_nm; // TODO: use scope
    int total_nodes = _sim->_total_nodes;
    nm[0] = 0;
    //  (it is already reversed. leave it like that.
    for (int node = 1;  node <= total_nodes;  ++node) {
      nm[node] = node;
    }
  }
}p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "order_reverse|order_auto", &p0);
/*--------------------------------------------------------------------------*/
/* order_forward: use user ordering, with subcircuits added to end
 * results in border at the top (worst possible if lots of subcircuits)
 */
class INTERFACE CMD_ORDER_FORWARD : public CMD {
  void do_it(CS&, CARD_LIST*)override {
    int* nm = _sim->_nm; // TODO: use scope
    int total_nodes = _sim->_total_nodes;
    nm[0] = 0;
    // need to reverse to obtain forward ordering.
    for (int node = 1;  node <= total_nodes;  ++node) {
      nm[node] = total_nodes - node + 1;
    }
  }
}p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "order_forward", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
