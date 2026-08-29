/*                 -*- C++ -*-
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
 * ground command
 */
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "e_node.h"
#include "u_nodemap.h"
#include "e_cardlist.h"
#include "m_union.h"
#include "e_usernode.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_GLOBAL : public CMD {
public:
  explicit CMD_GLOBAL() : CMD() {}
private:
  explicit CMD_GLOBAL(CMD_GLOBAL const&p) : CMD(p) {}
  CMD* clone()const override {return new CMD_GLOBAL(*this);}
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
    assert(Scope->nodes());
    std::string name;
    size_t here = cmd.cursor();
    cmd >> name;
    CMD::set(cmd.fullstring());

    assert(CARD_LIST::card_list.nodes()->size());

    NODE* np = Scope->nodes()->new_node(name);
    USER_NODE* un = prechecked_cast<USER_NODE*>(np);
    assert(un);
    if(Scope==&CARD_LIST::card_list){
      un->set_global(); // needed?
    }else{
      NODE* topnode = CARD_LIST::card_list.nodes()->new_node(name);
      USER_NODE* ut = prechecked_cast<USER_NODE*>(topnode);
      assert(ut);
      if(ut->is_global()){
	np->n_(0).link_to(&ut->n_(0));
      }else{
	cmd.warn(bDANGER, here, "not a global node");
      }

    }

    if(cmd.more()){ untested();
      cmd.warn(bDANGER, "trailing characters");
    }else{
    }
  }
} p12;
DISPATCHER<CMD>::INSTALL d12(&command_dispatcher, ".global", &p12);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
