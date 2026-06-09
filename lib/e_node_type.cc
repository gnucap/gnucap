/*                -*- C++ -*-
 * Copyright (C) 2026 Felix Salfelder
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
 * discipline and nature
 */
#include "globals.h"
#include "u_xprobe.h"
#include "e_node_type.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
NODE_TYPE::NODE_TYPE(std::string const& name)
  : NODE(name)
{
  set_type_number(int(node_dispatcher.size()));
  trace2("NODE_TYPE", name, type_number());
  assert(!_installer);
  _installer = new inst(&node_dispatcher, name, this);
}
/*--------------------------------------------------------------------------*/
NODE_TYPE::~NODE_TYPE()
{
  delete _installer;
  _installer = nullptr;
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
XPROBE NODE_TYPE::ac_probe_ext(const std::string&) const
{
  return XPROBE(NOT_VALID);
}
/*--------------------------------------------------------------------------*/
class ELECTRICAL : public NODE_TYPE {
  explicit ELECTRICAL(ELECTRICAL const& p) : NODE_TYPE(p) {}
public:
  explicit ELECTRICAL() : NODE_TYPE("electrical") {
    // set_potential("Voltage");
    // set_flow("Current");
    OPT::default_logic = this; // transition. need a type..
  }
  CARD* clone()const override { return new ELECTRICAL(*this);}
  NODE* deflate()override {
    return new LOGIC_NODE(); // TODO
  }
  // bool is_continuous()const override {untested(); return true; }
}electrical;
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class HYBRID : public NODE_TYPE {
  explicit HYBRID(HYBRID const& p) : NODE_TYPE(p) {untested();}
public:
  explicit HYBRID() : NODE_TYPE("hybrid") {
    // set_potential("Voltage");
    // set_flow("Current");
  }
  CARD* clone()const override {untested(); return new HYBRID(*this);}
  NODE* deflate()override { untested();
    return new LOGIC_NODE();
  }
  // bool is_mixed()const override {untested(); return true; }
}hybrid;
/*--------------------------------------------------------------------------*/
class LOGIC : public NODE_TYPE {
  explicit LOGIC(LOGIC const& p) : NODE_TYPE(p) {untested();}
public:
  explicit LOGIC() : NODE_TYPE("logic") {
    OPT::default_logic = this;
  }
  CARD* clone()const override {untested(); return new LOGIC(*this);}
  NODE* deflate()override { untested();
    return new LOGIC_NODE(); // TODO
  }
  // bool is_discrete()const override {untested(); return true; }
}logic;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
