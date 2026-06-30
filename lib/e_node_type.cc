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
  trace2("NODE_TYPE", name, type_number());
  assert(!_installer);
  set_label(name);
  install();
}
/*--------------------------------------------------------------------------*/
NODE_TYPE::~NODE_TYPE()
{
  if (!_installer){
  }else if(_sim) {
    _sim->uninit();
  }else{
  }
  delete _installer;
  _installer = nullptr;
}
/*--------------------------------------------------------------------------*/
XPROBE NODE_TYPE::ac_probe_ext(const std::string&) const
{ untested();
  return XPROBE(NOT_VALID);
}
/*--------------------------------------------------------------------------*/
void NODE_TYPE::install()
{
  if(_installer){ untested();
  }else{
    _installer = new inst(&node_dispatcher, short_label(), this);
    set_type_number(int(node_dispatcher.size())-1);
  }
}
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class WIRE : public NODE_TYPE {
  explicit WIRE(WIRE const& p) : NODE_TYPE(p) {}
public:
  explicit WIRE() : NODE_TYPE() {
    set_label("wire");
    set_type_number(0);
  }
  ~WIRE() {}
  CARD* clone()const override { return new WIRE(*this);}
  NODE* deflate()override {
    return new LOGIC_NODE(); // TODO
  }
}wire;
DISPATCHER<NODE>::INSTALL d0(&node_dispatcher, "wire", &wire);
/*--------------------------------------------------------------------------*/
class ELECTRICAL : public NODE_TYPE {
  explicit ELECTRICAL(ELECTRICAL const& p) : NODE_TYPE(p) {}
public:
  explicit ELECTRICAL() : NODE_TYPE() {
    set_label("electrical");
    set_type_number(1);
    // set_potential("Voltage");
    // set_flow("Current");
    set_analog();
  }
  ~ELECTRICAL() {}
  CARD* clone()const override { return new ELECTRICAL(*this);}
  NODE* deflate()override {
    return new LOGIC_NODE(); // TODO
  }
}electrical;
DISPATCHER<NODE>::INSTALL d1(&node_dispatcher, "electrical", &electrical);
/*--------------------------------------------------------------------------*/
class HYBRID : public NODE_TYPE {
  explicit HYBRID(HYBRID const& p) : NODE_TYPE(p) {}
public:
  explicit HYBRID() : NODE_TYPE() {
    set_label("hybrid");
    set_type_number(2);
    // set_potential("Voltage");
    // set_flow("Current");
    set_mixed();
  }
  ~HYBRID() {}
  CARD* clone()const override { return new HYBRID(*this);}
  NODE* deflate()override {
    return new LOGIC_NODE();
  }
}hybrid;
DISPATCHER<NODE>::INSTALL d2(&node_dispatcher, "hybrid", &hybrid);
/*--------------------------------------------------------------------------*/
class LOGIC : public NODE_TYPE {
  explicit LOGIC(LOGIC const& p) : NODE_TYPE(p) {}
public:
  explicit LOGIC() : NODE_TYPE() {
    set_label("logic");
    set_type_number(3);
    set_digital();
    OPT::default_logic = this;
  }
  ~LOGIC() {}
  CARD* clone()const override { return new LOGIC(*this);}
  NODE* deflate()override {
    return new LOGIC_NODE(); // TODO
  }
}logic;
DISPATCHER<NODE>::INSTALL d3(&node_dispatcher, "logic", &logic);
/*--------------------------------------------------------------------------*/
class CONNECTRULES : public CARD {
  mutable std::vector<node_t> _n;
  int _net_nodes{0};
public:
  explicit CONNECTRULES() : CARD() {
    OPT::connect_rules = this;
  }
  void expand_first()override {
    int n = num();
    _n.resize(n*n);

    // {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
    NODE* dom[16];
    dom[0+0*4] = &wire;
    dom[0+1*4] = &electrical;
    dom[0+2*4] = &logic;
    dom[0+3*4] = &hybrid;
    dom[1+0*4] = &electrical;
    dom[1+1*4] = &electrical;
    dom[1+2*4] = &hybrid;
    dom[1+3*4] = &hybrid;
    dom[2+0*4] = &logic;
    dom[2+1*4] = &hybrid;
    dom[2+2*4] = &logic;
    dom[2+3*4] = &hybrid;
    dom[3+0*4] = &hybrid;
    dom[3+1*4] = &hybrid;
    dom[3+2*4] = &hybrid;
    dom[3+3*4] = &hybrid;

    for(auto a: node_dispatcher){
      if(auto A = dynamic_cast<NODE_TYPE const*>(a.second)) {
	int i = A->type_number();
	if(i == INVALID_NODE){ untested();
	}else{
	  for(auto b: node_dispatcher){
	    if(auto B = dynamic_cast<NODE_TYPE const*>(b.second)) {
	      int j = B->type_number();
	      if(j == INVALID_NODE){ untested();
	      }else{
		int x = A->domain();
		int y = B->domain();
		assert(dom[x+4*y]);
		_n[i+j*n] = dom[x+4*y];
	      }
	    }else{ itested();
	    }
	  }
	}
      }else{ itested();
      }
    }

    _net_nodes = num();
  }
private:
  int num()const {return int(node_dispatcher.size());}
  CARD* clone()const override { untested();unreachable(); return nullptr;}
  int net_nodes()const override {return _net_nodes;}
  node_t& n_(int i)const override {assert(i<num()); return _n[i*num()];}
} p3;
/*--------------------------------------------------------------------------*/
class CONNECTRULES : public CARD {
  mutable std::vector<node_t> _n;
  int _net_nodes{0};
public:
  explicit CONNECTRULES() : CARD() {
    OPT::connect_rules = this;
  }
  void expand_first()override {
    int n = num();
    _n.resize(n*n);

    // {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
    NODE* dom[16];
    dom[0+0*4] = &wire;
    dom[0+1*4] = &electrical;
    dom[0+2*4] = &logic;
    dom[0+3*4] = &hybrid;
    dom[1+0*4] = &electrical;
    dom[1+1*4] = &electrical;
    dom[1+2*4] = &hybrid;
    dom[1+3*4] = &hybrid;
    dom[2+0*4] = &logic;
    dom[2+1*4] = &hybrid;
    dom[2+2*4] = &logic;
    dom[2+3*4] = &hybrid;
    dom[3+0*4] = &hybrid;
    dom[3+1*4] = &hybrid;
    dom[3+2*4] = &hybrid;
    dom[3+3*4] = &hybrid;

    for(auto a: node_dispatcher){
      if(auto A = dynamic_cast<NODE_TYPE const*>(a.second)) {
	int i = A->type_number();
	if(i == INVALID_NODE){ untested();
	}else{
	  for(auto b: node_dispatcher){
	    if(auto B = dynamic_cast<NODE_TYPE const*>(b.second)) {
	      int j = B->type_number();
	      if(j == INVALID_NODE){ untested();
	      }else{
		int x = A->domain();
		int y = B->domain();
		assert(dom[x+4*y]);
		_n[i+j*n] = dom[x+4*y];
	      }
	    }else{ untested();
	    }
	  }
	}
      }else{ untested();
      }
    }

    _net_nodes = num();
  }
private:
  int num()const {return int(node_dispatcher.size());}
  CARD* clone()const override { untested();unreachable(); return nullptr;}
  int net_nodes()const override {return _net_nodes;}
  node_t& n_(int i)const override { untested();assert(i<num()); return _n[i*num()];}
} p3;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
