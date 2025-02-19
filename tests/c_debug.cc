/*                                   -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder <felix@salfelder.org>
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
#include "e_cardlist.h"
#include "u_lang.h"
#include "c_comand.h"
#include "globals.h"
#include "e_node.h"
#include "e_subckt.h"
#include "u_nodemap.h"
//#include "io_matrix.h"
#include "u_sim_data.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
/*--------------------------------------------------------------------------*/
class CMD_NL : public CMD {
  int _indent{0};
public:
  void print(OMSTREAM, const CARD_LIST*);
  void do_it(CS& cmd, CARD_LIST* )override {
    OMSTREAM o = IO::mstdout;
    o.setfloatwidth(3);
    outset(cmd, &o);

    o << "name   ...\n";

    CKT_BASE::_sim->init(&CARD_LIST::card_list);

    o << "have " << _sim->_total_nodes 
      << " nodes. model: " << _sim->_model_nodes 
      << " sckt: " << _sim->_subckt_nodes
      << "\n";
    for(int k=0; k<=_sim->_total_nodes; ++k){
      trace2( "CMD_NL::do_it", k, _sim->_nm[k]);
    }
    o << "\n";
    print(o, &CARD_LIST::card_list);
    o.reset();
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, "portdump", &p6);
/*--------------------------------------------------------------------------*/
void CMD_NL::print( OMSTREAM o, const CARD_LIST* scope)
{
  std::string ind(" ", _indent);
 // const NODE_MAP * nm = scope->nodes();
  for (auto d : *scope){
    o << ind << d->long_label();
    if(auto c = dynamic_cast<COMPONENT const*>(d)){
      o << " " << c->net_nodes() << " " << c->ext_nodes() << " " << c->int_nodes() << "\n";
      for (int ii = 0; ii < c->ext_nodes()+c->int_nodes(); ++ii) {
	o << std::string(" ", _indent) << ii << " " << c->n_(ii).m_() << "\n";
      }
      if (c->subckt()){
	_indent += 2;
	print(o, c->subckt());
	_indent -= 2;
      }
    }else{
      o << "\n";
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
