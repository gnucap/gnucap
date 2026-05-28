/*                 -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
class CMD_GROUND : public CMD {
public:
  explicit CMD_GROUND() : CMD() {}
private:
  explicit CMD_GROUND(CMD_GROUND const&p) : CMD(p) {}
  CMD* clone()const override {return new CMD_GROUND(*this);}
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
    assert(Scope->nodes());
    std::string name;
    std::string full;
    full = "ground " + cmd.tail();
    cmd >> name >> ";";

    assert(CARD_LIST::card_list.nodes()->size());

    if(name == "0" && cmd.fullstring()[0]=='.'){ untested();
      // spice
    }else{
      set(full);

      // TODO: lang_verilog get_identifier
      if(name[0] == '\\'){
	name = name.substr(1);
      }else{
      }
    }

    NODE* n = Scope->nodes()->new_node(name);
    USER_NODE* np = prechecked_cast<USER_NODE*>(n);
    assert(np);
    trace2("ground node", name, np->user_number());
    np->set_to_ground();
    trace2("ground node", name, np->user_number());

    if(cmd.more()){ untested();
      cmd.warn(bDANGER, "trailing characters");
    }else{
    }
  }
} p12;
DISPATCHER<CMD>::INSTALL d12(&command_dispatcher, "ground|.ground", &p12);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
