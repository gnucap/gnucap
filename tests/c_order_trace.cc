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
#include "e_elemnt.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class INTERFACE CMD_ORDER_TRACE : public CMD {
  void do_it(CS&, CARD_LIST* scope)override {
    int seek = _sim->_total_nodes+1;
    do_it_recursive(scope, seek);
  }
protected:
  void do_it_recursive(CARD_LIST* scope, int& seek)const {
    assert(scope);
    for (CARD_LIST::reverse_iterator ci=scope->rbegin(); ci!=scope->rend(); ++ci) {
      if(CARD_LIST* s=(**ci).subckt()){
	do_it_recursive(s, seek);
      }else{
      }
      if(auto* e=dynamic_cast<ELEMENT*>(*ci)) {
        for(int i=0; i<e->ext_nodes()+e->int_nodes(); ++i){
          number(e->n_(i), seek);
        }
      }
    }
    assert(scope->nodes());
    NODE_MAP& n = *scope->nodes();
    for(int i = 0; i< n.size(); ++i) {
      if(!n[i].n_()){
      }else if(n[i].n_()->subckt()){
	trace2("fanout?", i, n[i]->subckt()->size());
      }
      number(n[i], seek);
    }
  }
  virtual int next(int& seek)const {
    return --seek;
  }
private:
  void number(node_t& n, int& seek)const {
    int flat = n->flat_number();
    int* nm = _sim->_nm; // TODO: use scope
   //  if(!n.n_()) {
   //    // link to another node. possibly further up. ignore
   //  }else
    if(flat == 0){
    }else if(flat == INVALID_NODE){
    }else if(nm[flat] == INVALID_NODE){
      nm[flat] = next(seek);
      if(n->subckt()){
	do_it_recursive(n->subckt(), seek);
      }else{
      }
    }else{
    }
  }
}p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "order_trace", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
