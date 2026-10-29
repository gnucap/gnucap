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
#include "e_matrixnode.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
NODE_TYPE::NODE_TYPE(std::string const& name)
  : NODE(name)
{ untested();
  trace2("NODE_TYPE", name, type_number());
  assert(!_installer);
  set_label(name);
  install();
}
/*--------------------------------------------------------------------------*/
NODE_TYPE::~NODE_TYPE()
{
  if (!_installer){
  }else if(_sim) { untested();
    _sim->uninit();
  }else{ untested();
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
{ untested();
  if(_installer){ untested();
  }else{ untested();
    _installer = new inst(&node_dispatcher, short_label(), this);
    set_type_number(int(node_dispatcher.size())-1);
  }
}
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class WIRE : public NODE_TYPE {
  explicit WIRE(WIRE const& p) : NODE_TYPE(p) { untested();}
public:
  explicit WIRE() : NODE_TYPE() {
    set_label("wire");
    set_type_number(0);
  }
  ~WIRE() {}
  CARD* clone()const override { untested(); return new WIRE(*this);}
  NODE* deflate()override { untested();
    unreachable();
    return nullptr;
    // incomplete
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
    return new MATRIX_NODE();
  }
}electrical;
DISPATCHER<NODE>::INSTALL d1(&node_dispatcher, "electrical", &electrical);
/*--------------------------------------------------------------------------*/
class CONNECTRULES : public CARD {
  mutable std::vector<node_t> _n;
  int _net_nodes{0};
public:
  explicit CONNECTRULES() : CARD() {
    OPT::connect_rules = this;
  }
  void expand_first()override {itested();
    int n = num();
    _n.resize(n*n);

    for(auto a: node_dispatcher){itested();
      if(auto A = dynamic_cast<NODE_TYPE const*>(a.second)) {itested();
	int i = A->type_number();
	if(i == INVALID_NODE){ untested();
	}else{itested();
	  for(auto b: node_dispatcher){itested();
	    if(auto B = dynamic_cast<NODE_TYPE const*>(b.second)) {itested();
	      int j = B->type_number();
	      if(j == INVALID_NODE){ untested();
	      }else{itested();
		_n[i+j*n] = &electrical;
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
  int num()const {itested();return 4;}
  CARD* clone()const override { untested();unreachable(); return nullptr;}
  int net_nodes()const override {itested();return _net_nodes;}
  node_t& n_(int i)const override {itested();assert(i<num()); return _n[i*num()];}
} p3;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
