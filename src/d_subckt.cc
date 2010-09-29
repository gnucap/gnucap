/*$Id: d_subckt.cc,v 22.21 2002/10/06 07:21:50 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * subcircuit stuff
 * base class for other elements using internal subckts
 * netlist syntax:
 * device: Xxxxx <nodelist> <subckt-name>
 * model:  .subckt <subckt-name> <nodelist>
 *	   (device cards)
 *	   .ends <subckt-name>
 * storage note ...
 * the .subckt always has a comment at the hook point, so a for loop works
 * the expansion (attact to the X) has all comments removed
 *	- need to process the entire ring - for doesn't work
 */
#include "l_stlextra.h"
#include "c_comand.h" // ".ends"
#include "ap.h"
#include "e_model.h"
#include "d_subckt.h"
/*--------------------------------------------------------------------------*/
//		MODEL_SUBCKT::MODEL_SUBCKT(const char*);
//	void	MODEL_SUBCKT::parse(CS& cmd);
// 	void	MODEL_SUBCKT::print(int,int)const;

//	void	CMD::ends(CS&);

//		DEV_SUBCKT::DEV_SUBCKT();
//		DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p);
//	void	DEV_SUBCKT::parse(CS& cmd);
// 	void	DEV_SUBCKT::print(int,int)const;
//	void	DEV_SUBCKT::expand();
//	void    COMPONENT::expandsubckt(const string&);
//	double	DEV_SUBCKT::tr_probe_num(CS&)const;
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::_count = 0;
int COMMON_SUBCKT::_count = -1;
int MODEL_SUBCKT::_count = 0;
static COMMON_SUBCKT Default_SUBCKT(CC_STATIC);

struct SSNODE {
  std::string _name;
  CARD_LIST::fat_iterator _pb;
  explicit SSNODE() :_name(), _pb(&CARD_LIST::card_list) {}
  explicit SSNODE(const std::string& Name, CARD_LIST::fat_iterator c)
    :_name(Name), _pb(c) {}
  bool operator==(const SSNODE&)const{incomplete();unreachable();return false;}
  bool operator!=(const SSNODE&)const{incomplete();unreachable();return false;}
  bool operator<(const SSNODE&)const {incomplete();unreachable();return false;}
  bool operator>(const SSNODE&)const {incomplete();unreachable();return false;}
  // comparisons here because MS list requires them, even though not used here.
};
static std::list<SSNODE> substack;
/*--------------------------------------------------------------------------*/
bool COMMON_SUBCKT::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_SUBCKT* p = dynamic_cast<const COMMON_SUBCKT*>(&x);
  bool rv = p && is_equal(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::MODEL_SUBCKT()
  :COMPONENT(),
   _num_nodes(0)
{
  _n = _nodes;
  //set_label(name);
  root_model_list.push_back(this);
  ++_count;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::~MODEL_SUBCKT()
{
  --_count;
  root_model_list.erase(
	remove(root_model_list.begin(), root_model_list.end(), this),
	root_model_list.end());
}
/*--------------------------------------------------------------------------*/
void MODEL_SUBCKT::parse(CS& cmd)
{ 
  // parse the line
  cmd.reset();
  cmd.skiparg();	/* skip known ".subckt" */
  parse_label(cmd);
  _num_nodes = parse_nodes(cmd, max_nodes(), min_nodes());

  // "push" the return place, so when the subckt ends, we can go back
  substack.push_back(SSNODE(short_label(), CARD_LIST::putbefore));
  // until further notice, attach to the subckt.
  CARD_LIST::putbefore = CARD_LIST::fat_iterator(&(subckt()), subckt().end());
}
/*--------------------------------------------------------------------------*/
void MODEL_SUBCKT::print(OMSTREAM& where, int)const
{
  where << ".subckt " << short_label() << ' ';
  printnodes(where);
  where << '\n';

  {if (subckt().exists()) {
    for (CARD_LIST::const_iterator 
	   ci = subckt().begin(); ci != subckt().end(); ++ci) {
      (**ci).print(where, false);
    }
  }else{
    untested(); 
  }}
  where << "*+ends " << short_label() << '\n';
}
/*--------------------------------------------------------------------------*/
void CMD::ends(CS& cmd)
{
  SSNODE t;
  {if (substack.empty()) {
    untested(); 
    error(bWARNING, "ends not in subckt\n");
  }else{
    t = substack.back();
    substack.pop_back();
  }}
  if (cmd.more()) {
    if (!cmd.pmatch(t._name)) {
      untested(); 
      error(bERROR, std::string("ends tag [") + cmd.tail()
	    + "] does not match subckt [" + t._name + "]\n");
    }
  }
  CARD_LIST::putbefore = t._pb;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT(),
   _num_nodes(0)
{
  attach_common(&Default_SUBCKT);
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _num_nodes(p._num_nodes)
{
  //strcpy(modelname, p.modelname); in common
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::parse(CS& cmd)
{
  assert(has_common());
  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(common()->clone());
  assert(c);
  assert(!c->has_model());

  parse_Label(cmd);
  _num_nodes = parse_nodes(cmd, max_nodes(), min_nodes());
  c->parse_modelname(cmd);
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::print(OMSTREAM& where, int)const
{
  const COMMON_SUBCKT* c = prechecked_cast<const COMMON_SUBCKT*>(common());
  assert(c);
  where << short_label();
  printnodes(where);
  where << "  " << c->modelname() << '\n';
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  const COMMON_SUBCKT* c = prechecked_cast<const COMMON_SUBCKT*>(common());
  assert(c);
  expandsubckt(c->modelname());
  if (subckt().empty()) {
    untested(); 
    error(bERROR, "no subckt\n");
  }
  subckt().expand();
  assert(!constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
void COMPONENT::expandsubckt(const std::string& modelname)
{
  enum {UNUSED=0, USED=-3};

  // get rid of the old one, so we start clean
  if (!subckt().empty()) {
    subckt().destroy();
  }
  assert(subckt().empty());

  // find the prototype (.subckt)
  const CARD* model = 0;
  {
    std::list<CARD*>::const_iterator mi =
      notstd::find_ptr(root_model_list.begin(), root_model_list.end(),
		       modelname);
    {if (mi == root_model_list.end()) {
      error(bDANGER, "can't find subckt: " + modelname + '\n');
      assert(subckt().empty());
      return;
    }else if(!dynamic_cast<MODEL_SUBCKT*>(*mi)) {
      untested();
      error(bDANGER, modelname + " is not a subckt\n");
      assert(subckt().empty());
      return;
    }}
    model = *mi;
  }
  assert(model);
  
  // set up the map of external to expanded node numbers
  int map[NODESPERSUBCKT];
  {
    // initialize - all nodes unused
    for (int i = 0; i < NODESPERSUBCKT; ++i) {
      map[i] = UNUSED;
    }

    // scan the subckt netlist and mark which nodes are used
    // this is used to pack out the holes
    for (CARD_LIST::const_iterator
	   ci = model->subckt().begin(); ci != model->subckt().end(); ++ci) {
      if ((**ci).is_device()) {
	for (int ii = 0;  ii < (**ci).net_nodes();  ++ii) {
	  if ((**ci)._n[ii].e_() >= NODESPERSUBCKT) {
	    untested(); 
	    error(bERROR, model->long_label() + ": too many internal nodes\n");
	  }
	  map[(**ci)._n[ii].e_()] = USED;
	}
      }
    }

    // map is a set of flags showing USED or UNUSED
    // now we want to turn it into numbers
    // UNUSED will remain 0, like ground
    // USED will turn into numbers

    // first, take care of the "port" nodes (external connections)
    // map them to what the calling circuit wants
    map[0] = 0;
    for (int port = 0; port < model->net_nodes(); ++port) {
      if (model->_n[port].e_() >= NODESPERSUBCKT) {
	untested(); 
	error(bERROR, "internal error: subckt node out of range: "
	      + model->long_label() + '\n');
      }
      map[model->_n[port].e_()] = _n[port].t_();
    }
    // map now has node numbers for ports only -- others still USED or UNUSED

    // get new node numbers, and assign them to the remaining USED
    for (int ii = 0;  ii < NODESPERSUBCKT;  ++ii) {
      if (map[ii] == USED) {
	map[ii] = STATUS::newnode_subckt();
      }
    }
  }
  // done. map now full of internal node numbers, with all UNUSED == 0
  
  // make a shallow copy of the prototype
  assert(subckt().empty());
  error(bTRACE, long_label() + ": expanding\n");
  for (CARD_LIST::const_iterator
	 ci = model->subckt().begin(); ci != model->subckt().end(); ++ci) {
    if ((**ci).is_device()) {
      CARD* copy = (**ci).clone();
      copy->set_owner(this);
      subckt().push_front(copy);
    }
  }
  assert(!subckt().empty());
  
  // map the nodes
  for (CARD_LIST::iterator
	 ci = subckt().begin(); ci != subckt().end(); ++ci) {
    assert((**ci).is_device());
    for (int ii = 0;  ii < (**ci).net_nodes();  ++ii) {
      (**ci)._n[ii].map_subckt_node(map);
    }
  }
}
/*--------------------------------------------------------------------------*/
double DEV_SUBCKT::tr_probe_num(CS& cmd)const
{
  untested();
  assert(subckt().exists());
  
  {if (cmd.pmatch("V")) {
    untested(); 
    int nn = cmd.ctoi();
    return (nn > 0 && nn <= net_nodes()) ? _n[nn-1].v0() : NOT_VALID;
  }else if (cmd.pmatch("P")) {
    untested(); 
    double power = 0.;
    untested();
    for (CARD_LIST::const_iterator
	   ci = subckt().begin(); ci != subckt().end(); ++ci) {
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (cmd.pmatch("PD")) {
    untested(); 
    double power = 0.;
    untested();
    for (CARD_LIST::const_iterator
	   ci = subckt().begin(); ci != subckt().end(); ++ci) {
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (cmd.pmatch("PS")) {
    untested(); 
    double power = 0.;
    untested();
    for (CARD_LIST::const_iterator
	   ci = subckt().begin(); ci != subckt().end(); ++ci) {
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else{
    untested(); 
    return COMPONENT::tr_probe_num(cmd);
  }}
  /*NOTREACHED*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
