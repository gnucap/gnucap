/*$Id: d_subckt.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * subcircuit stuff
 * base class for other elements using internal subckts
 * netlist syntax:
 * device: Xxxxx <nodelist> <subckt-name> <args>
 * model:  .subckt <subckt-name> <nodelist>
 *	   (device cards)
 *	   .ends <subckt-name>
 * storage note ...
 * the .subckt always has a comment at the hook point, so a for loop works
 * the expansion (attact to the X) has all comments removed
 *	- need to process the entire ring - for doesn't work
 */
//testing=script 2006.07.17
#include "d_subckt.h"
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::_count = 0;
int COMMON_SUBCKT::_count = -1;
int MODEL_SUBCKT::_count = 0;
static COMMON_SUBCKT Default_SUBCKT(CC_STATIC);
/*--------------------------------------------------------------------------*/
bool COMMON_SUBCKT::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_SUBCKT* p = dynamic_cast<const COMMON_SUBCKT*>(&x);
  bool rv = p 
    && _params == p->_params
    && COMMON_COMPONENT::operator==(x);
  return rv;
}
/*--------------------------------------------------------------------------*/
bool COMMON_SUBCKT::parse_params(CS& cmd)
{
  _params.parse(cmd);
  return true;
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::print(OMSTREAM& o)const
{
  _params.print(o);
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::MODEL_SUBCKT()
  :COMPONENT(),
   _net_nodes(0)
{
  _n = _nodes;
  new_subckt();
  ++_count;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::~MODEL_SUBCKT()
{
  --_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_SUBCKT::parse_spice(CS& cmd)
{ 
  // parse the line
  cmd.reset();
  cmd.skiparg();	/* skip known ".subckt" */
  parse_label(cmd);
  _net_nodes = parse_nodes(cmd, max_nodes(), min_nodes(), 
			   0/*no unnamed par*/, 0/*start*/, true/*all new*/);
  cmd.check(bDANGER, "subckt default parameters not supported yet");
}
/*--------------------------------------------------------------------------*/
void MODEL_SUBCKT::print_spice(OMSTREAM& o, int)const
{
  o << ".subckt " << short_label() << ' ';
  printnodes(o);
  o << '\n';

  if (subckt()) {
    for (CARD_LIST::const_iterator 
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {
      (**ci).print_spice(o, false);
    }
  }else{untested();
  }
  o << ".ends " << short_label() << '\n';
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT(),
   _net_nodes(0)
{
  attach_common(&Default_SUBCKT);
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _net_nodes(p._net_nodes)
{
  //strcpy(modelname, p.modelname); in common
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::parse_spice(CS& cmd)
{
  assert(has_common());
  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(common()->clone());
  assert(c);
  assert(!c->has_model());

  parse_Label(cmd);
  _net_nodes = parse_nodes(cmd, max_nodes(), min_nodes(),
			   1/*subckt_name*/, 0/*start*/);
  c->parse_modelname(cmd);
  c->parse(cmd);
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::print_spice(OMSTREAM& o, int)const
{
  const COMMON_SUBCKT* c = prechecked_cast<const COMMON_SUBCKT*>(common());
  assert(c);
  o << short_label();
  printnodes(o);
  o << "  " << c->modelname();
  c->print(o);
  o << '\n';
}
/*--------------------------------------------------------------------------*/
inline const CARD* find_subckt_model(CARD* scope, const std::string& modelname)
{untested();
  const CARD* model = 0;
  {untested();
    model = scope->find_looking_out(modelname, bDEBUG);
    if (!model) {untested();
      error(bDANGER, "can't find subckt: " + modelname + '\n');
      return NULL;
    }else if(!dynamic_cast<const MODEL_SUBCKT*>(model)) {untested();
      error(bDANGER, modelname + " is not a subckt\n");
      return NULL;
    }else{untested();
    }
  }
  return model;
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::elabo1()
{ 
  BASE_SUBCKT::elabo1();
  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);

  const CARD* model = find_subckt_model(this /*scope*/, c->modelname());
  if (!model) {untested();
    error(bERROR, "no subckt\n");
  }else{
  }
  new_subckt();
  subckt()->attach_params(&(c->_params), scope());
  subckt()->shallow_copy(model->subckt());
  subckt()->set_owner(this);
  subckt()->map_subckt_nodes(model, this);
  subckt()->elabo2();

  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
double DEV_SUBCKT::tr_probe_num(CS& cmd)const
{untested();  
  if (cmd.pmatch("V")) {untested();
    int nn = cmd.ctoi();
    return (nn > 0 && nn <= net_nodes()) ? _n[nn-1].v0() : NOT_VALID;
  }else if (cmd.pmatch("P")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (cmd.pmatch("PD")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PD");
    }      
    return power;
  }else if (cmd.pmatch("PS")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PS");
    }      
    return power;
  }else{untested();
    return COMPONENT::tr_probe_num(cmd);
  }
  /*NOTREACHED*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
