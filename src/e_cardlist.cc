/*$Id: e_cardlist.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 * This file contains functions that process a list of cards
 */
//testing=script 2006.07.10
#include "e_node.h"
#include "u_nodemap.h"
#include "e_model.h"
/*--------------------------------------------------------------------------*/
CARD_LIST::CARD_LIST()
  :_parent(NULL),
   _nm(new NODE_MAP),
   _params(NULL)
{
}
/*--------------------------------------------------------------------------*/
CARD_LIST::~CARD_LIST()
{
  erase_all();
  delete _nm;
  if (!_parent) {
    delete _params;
  }
}
/*--------------------------------------------------------------------------*/
PARAM_LIST* CARD_LIST::params()
{
  if (!_params) {
    assert(!_parent);
    _params = new PARAM_LIST;
  }
  return _params;
}
/*--------------------------------------------------------------------------*/
const PARAM_LIST* CARD_LIST::params()const
{
  if (_params) {
    return _params;
  }else{itested();
    static const PARAM_LIST empty_params;
    return &empty_params;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST::iterator CARD_LIST::find(const std::string& short_name)
{
  return notstd::find_ptr(begin(), end(), short_name);
}
/*--------------------------------------------------------------------------*/
CARD* CARD_LIST::operator[](const std::string& short_name)
{
  iterator i = find(short_name);
  if (i != end()) {
    return *i;
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST::const_iterator CARD_LIST::find(const std::string& short_name)const
{
  return notstd::find_ptr(begin(), end(), short_name);
}
/*--------------------------------------------------------------------------*/
const CARD* CARD_LIST::operator[](const std::string& short_name)const
{
  const_iterator i = find(short_name);
  if (i != end()) {
    return *i;
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(iterator ci)
{
  assert(ci != end());
  delete *ci;
  _cl.erase(ci);
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::erase(CARD* c)
{untested();
  delete c;
  _cl.remove(c);
  return *this;
}
/*--------------------------------------------------------------------------*/
/* erase_all: empty the list, destroy contents
 * Beware: something else may be pointing to them, leaving dangling ptr.
 */
CARD_LIST& CARD_LIST::erase_all()
{
  while (!_cl.empty()) {
    delete _cl.back();
    _cl.pop_back();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::set_owner(CARD* owner)
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).set_owner(owner);
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* set_slave: set a whole circuit to "slave" mode.
 * Only useful for subckts.
 */
CARD_LIST& CARD_LIST::set_slave()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).set_slave();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* expand: expand (flatten) a list of components (subckts)
 * Scan component list.  Expand each subckt: create actual elements
 * for flat representation to use for simulation.
 */
CARD_LIST& CARD_LIST::elabo2()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    if (!dynamic_cast<MODEL_CARD*>(*ci)) {
      (**ci).elabo1();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* map_nodes: create mapping between user node names and internal numbers
 */
CARD_LIST& CARD_LIST::map_nodes()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).map_nodes();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* precalc: precalculate anything that can be done up front for all modes
 * Also initializes some internal data (the polys, etc),
 * finds out what is constant, etc.
 */
CARD_LIST& CARD_LIST::precalc()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).precalc();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_iwant_matrix: allocate solution matrix
 * also sets some flags for mixed-mode
 */
CARD_LIST& CARD_LIST::tr_iwant_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_iwant_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dc_begin: first pass on a new DC simulation
 */
CARD_LIST& CARD_LIST::dc_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).dc_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_begin: first pass on a new transient simulation (initial DC)
 */
CARD_LIST& CARD_LIST::tr_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_restore: first pass on restarting a transient simulation
 */
CARD_LIST& CARD_LIST::tr_restore()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_restore();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dc_advance: first pass on a new step in a dc sweep
 */
CARD_LIST& CARD_LIST::dc_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).dc_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_advance: first pass on a new time step
 */
CARD_LIST& CARD_LIST::tr_advance()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_advance();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval: determine if anything needs to be evaluated
 */
bool CARD_LIST::tr_needs_eval()const
{
  for (const_iterator ci=begin(); ci!=end(); ++ci) {
    if ((**ci).tr_needs_eval()) {
      return true;
    }else{untested();
    }
  }
  untested();
  return false;
}
/*--------------------------------------------------------------------------*/
/* tr_queue_eval: build evaluator queue
 */
CARD_LIST& CARD_LIST::tr_queue_eval()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_queue_eval();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_eval: evaluate a list of models
 * evaluates a list (or sublist), checks convergence, etc.
 * does not load the matrix
 * argument is the head of the netlist.
 * recursively called to evaluate subcircuits
 */
bool CARD_LIST::do_tr()
{
  bool isconverged = true;
  if (OPT::bypass) {
    for (iterator ci=begin(); ci!=end(); ++ci) {
      if ((**ci).tr_needs_eval()) {
	isconverged &= (**ci).do_tr();
      }
    }
  }else{
    for (iterator ci=begin(); ci!=end(); ++ci) {
      isconverged &= (**ci).do_tr();
    }
  }
  return isconverged;
}
/*--------------------------------------------------------------------------*/
/* tr_load: load list of models to the matrix
 * recursively called to load subcircuits
 * Called only when either !OPT::traceload or !SIM::inc_mode
 */
CARD_LIST& CARD_LIST::tr_load()
{
  if (SIM::inc_mode) {untested();
    assert(!OPT::traceload);
    for (iterator ci=begin(); ci!=end(); ++ci) {
      CARD* brh = *ci;
      if (!brh->is_constant()) {itested();
	brh->tr_load();
      }else{itested();
      }
    }
  }else{
    for (iterator ci=begin(); ci!=end(); ++ci) {
      CARD* brh = *ci;
      brh->tr_load();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
DPAIR CARD_LIST::tr_review()
{
  double time_by_error_estimate = BIGBIG;
  double time_by_predicted_event = BIGBIG;
  for (iterator ci=begin(); ci!=end(); ++ci) {
    DPAIR thistime = (**ci).tr_review();
    if (thistime.first < time_by_error_estimate) {
      time_by_error_estimate = thistime.first;
    }
    if (thistime.second < time_by_predicted_event) {
      time_by_predicted_event = thistime.second;
    }
  }
  return DPAIR(time_by_error_estimate, time_by_predicted_event);
}
/*--------------------------------------------------------------------------*/
/* tr_accept: final acceptance of a time step, before moving on
 */
CARD_LIST& CARD_LIST::tr_accept()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_accept();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* tr_unload: remove a list of models from the matrix
 * recursively called to unload subcircuits
 */
CARD_LIST& CARD_LIST::tr_unload()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).tr_unload();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_iwant_matrix: allocate solution matrix
 */
CARD_LIST& CARD_LIST::ac_iwant_matrix()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_iwant_matrix();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_begin: first pass on a new ac simulation
 */
CARD_LIST& CARD_LIST::ac_begin()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_begin();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
CARD_LIST& CARD_LIST::do_ac()
{
  for (iterator ci=begin(); ci!=end(); ++ci) {
    if (!(**ci).evaluated()) {
      (**ci).do_ac();
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* ac_load: load list of models to the matrix
 * recursively called to load subcircuits
 */
CARD_LIST& CARD_LIST::ac_load()
{
  unreachable();
  for (iterator ci=begin(); ci!=end(); ++ci) {
    (**ci).ac_load();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
void CARD_LIST::attach_params(PARAM_LIST* p, const CARD_LIST* scope)
{
  //_params = p;
  if (!_params) {
    _params = new PARAM_LIST;
  }else{untested();
  }
  _params->eval_copy(*p, scope);
}
/*--------------------------------------------------------------------------*/
void CARD_LIST::shallow_copy(const CARD_LIST* p)
{
  assert(p);
  _parent = p;
  for (const_iterator ci = p->begin(); ci != p->end(); ++ci) {
    if ((**ci).is_device()) {
      CARD* copy = (**ci).clone();
      push_back(copy);
    }
  }
}
/*--------------------------------------------------------------------------*/
// set up the map of external to expanded node numbers
//#define DO_TRACE
//#include "io_trace.h"
void CARD_LIST::map_subckt_nodes(const CARD* model, const CARD* owner)
{
  assert(owner->subckt() == this);
  trace0(model->long_label().c_str());
  trace0(owner->long_label().c_str());

  int num_nodes_in_subckt = model->subckt()->nodes()->how_many();
  int* map = new int[num_nodes_in_subckt+1];
  {
    map[0] = 0;
    // self test: verify that port node numbering is correct
    for (int port = 0; port < model->net_nodes(); ++port) {
      assert(model->_n[port].e_() <= num_nodes_in_subckt);
      //assert(model->_n[port].e_() == port+1);
      trace3("ports", port, model->_n[port].e_(), owner->_n[port].t_());
    }
    {
      // take care of the "port" nodes (external connections)
      // map them to what the calling circuit wants
      int i=0;
      for (i=1; i <= model->net_nodes(); ++i) {
	map[i] = owner->_n[i-1].t_();
	trace3("ports", i, map[i], owner->_n[i-1].t_());
      }
    
      // get new node numbers, and assign them to the remaining
      for (assert(i==model->net_nodes() + 1); i <= num_nodes_in_subckt; ++i) {
	// for each remaining node in card_list
	map[i] = ::status.newnode_subckt();
	trace2("internal", i, map[i]);
      }
    }
  }
  // "map" now contains a translation list,
  // from subckt local numbers to matrix index numbers

  // scan the list, map the nodes
  for (CARD_LIST::iterator ci = begin(); ci != end(); ++ci) {
    // for each card in card_list
    assert((**ci).is_device());
    for (int ii = 0;  ii < (**ci).net_nodes();  ++ii) {
      // for each connection node in card
      (**ci)._n[ii].map_subckt_node(map);
    }
  }
  delete [] map;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
