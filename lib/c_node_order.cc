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
#include "u_sim_data.h"
#include "u_nodemap.h"
#include "e_cardlist.h"
#include "e_compon.h"
#include "e_logicnode.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* order_reverse: force ordering to reverse of user ordering
 *  subcircuits at beginning, results on border at the bottom
 */
class INTERFACE CMD_ORDER_REVERSE : public CMD {
  void do_it(CS&, CARD_LIST* scope)override {
    int seek = 0;
    do_it_recursive(scope, seek);
    for (int node = 1;  node <= _sim->_total_nodes;  ++node) {
      trace3("ordered", _sim->_total_nodes, node, _sim->_nm[node]);
      assert(node  == _sim->_nm[node]);
    }
  }
private:
  void do_it_recursive(CARD_LIST* scope, int& seek) {
    int* nm = _sim->_nm; // TODO: use scope
    assert(nm[0] == 0);
    assert(scope);
    for (CARD_LIST::reverse_iterator ci=scope->rbegin(); ci!=scope->rend(); ++ci) {
      if(CARD_LIST* s=(**ci).subckt()){
	do_it_recursive(s, seek);
      }else{
      }
      if(COMPONENT* c=dynamic_cast<COMPONENT*>(*ci)) {
	for(int i=c->ext_nodes()+c->int_nodes(); i>c->net_nodes();){
	  --i;
	  // former allocate
	  int flat = c->n_(i)->flat_number();
	  if(!c->n_(i).n_()) {
	    // link to another node. possibly further up. ignore
	  }else if(flat == 0){ untested();
	  }else if(flat == INVALID_NODE){ untested();
	  }else if(nm[flat] == INVALID_NODE){
	    nm[flat] = ++seek;
	  }else{ untested();
	  }
	}
      }else{
      }
    }
    assert(scope->nodes());
    NODE_MAP& n = *scope->nodes();
    for(int i = n.size(); i;) {
      --i;
      int flat = n[i]->flat_number();
      if(!n[i].n_()) {
	// link to another node. possibly further up. ignore
      }else if(flat == 0){
      }else if(flat == INVALID_NODE){ untested();
      }else if(nm[flat] == INVALID_NODE){
	nm[flat] = ++seek;
      }else{ untested();
      }
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
