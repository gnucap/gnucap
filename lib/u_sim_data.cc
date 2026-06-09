/*$Id: u_sim_data.cc 2016/03/23 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
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
 * aux functions associated with the SIM class
 */
//testing=script 2015.01.28
#include "m_wave.h"
#include "m_random.h"
#include "u_nodemap.h"
#include "u_status.h"
#include "e_cardlist.h"
#include "e_usernode.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
SIM_DATA::SIM_DATA()
  :_time0(0.),
   _freq(0.),
   _damp(0.),
   _dtmin(0.),
   _genout(0.),
   _bypass_ok(true),
   _fulldamp(false),
   _last_time(0.),
   _freezetime(false),
   _user_nodes(0),
   _subckt_nodes(0),
   _model_nodes(0),
   _total_nodes(0),
   _jomega(0.,0.),
   _limiting(true),
   _vmax(0.),
   _vmin(0.),
   _uic(false),
   _inc_mode(tsNO),
   _mode(s_NONE),
   _phase(p_NONE),
   _nm(nullptr),
   _i(nullptr),
   _v0(nullptr),
   _vt1(nullptr),
   _ac(nullptr),
   _noise(nullptr),
   _nstat(false),
   _vdc(nullptr),
   _aa(),
   _lu(_aa), // alias.
   _acx(),
   _eq(),
   _loadq(),
   _acceptq(),
   _evalq1(),
   _evalq2(),
   _late_evalq(),
   _evalq(nullptr),
   _evalq_uc(nullptr),
   _waves(nullptr),
   _has_op(s_NONE)
{
  _evalq = &_evalq1;
  _evalq_uc = &_evalq2;
  std::fill_n(_iter, iCOUNT, 0);
}
/*--------------------------------------------------------------------------*/
SIM_DATA::~SIM_DATA()
{
  if (_nm) {unreachable();
    delete [] _nm;
    _nm = nullptr;
  }else{
  }
  if (_i) {unreachable();
    delete [] _i;
    _i = nullptr;
  }else{
  }
  if (_v0) {unreachable();
    delete [] _v0;
    _v0 = nullptr;
  }else{
  }
  if (_vt1) {unreachable();
    delete [] _vt1;
    _vt1 = nullptr;
  }else{
  }
  if (_ac) {unreachable();
    delete [] _ac;
    _ac = nullptr;
  }else{
  }
  if (_noise) {unreachable();
    delete [] _noise;
    _noise = nullptr;
  }else{
  }
  if (_nstat) {unreachable();
    _nstat = false;
  }else{
  }
  if (_vdc) {unreachable();
    delete [] _vdc;
    _vdc = nullptr;
  }else{
  }
  //assert(_eq.empty()); //not empty means an analysis ended with an unhandled event
			 // could be DC, could be tran with event time past the end
  //assert(_loadq.empty());
  if(_loadq.empty()){
  }else{
    incomplete();
  }
  assert(_acceptq.empty());
  assert(_evalq1.empty());
  assert(_evalq2.empty());
  assert(_late_evalq.empty());
  assert(_evalq);
  assert(_evalq_uc);
  _evalq = nullptr;
  _evalq_uc = nullptr;

  if (_waves) {
    delete [] _waves;
    _waves = nullptr;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SIM_DATA::set_limit()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    set_limit(_v0[ii]);
  }
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::set_limit(double v)
{
  if (v+.4 > _vmax) {
    _vmax = v+.5;
    error(bTRACE, "new max = %g, new limit = %g\n", v, _vmax);
  }
  if (v-.4 < _vmin) {
    _vmin = v-.5;
    error(bTRACE, "new min = %g, new limit = %g\n", v, _vmin);
  }
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::clear_limit()
{
  _vmax = OPT::vmax;
  _vmin = OPT::vmin;
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::keep_voltages()
{
  if (!_freezetime) {
    for (int ii = 1;  ii <= _total_nodes;  ++ii) {
      _vdc[ii] = _v0[ii];
    }
    _last_time = (_time0 > 0.) ? _time0 : 0.;
  }else{untested();
    //BUG// probably incorrect
  }
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::restore_voltages()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    _vt1[ii] = _v0[ii] = _vdc[ii];
  }
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::zero_voltages()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    _vt1[ii] = _v0[ii] = _vdc[ii] = _i[ii] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* map__nodes: map intermediate node number to internal node number.
 * Ideally, this function would find some near-optimal order
 * and squash out gaps.
 */
void SIM_DATA::map__nodes(CARD_LIST* scope)
{
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }
  assert(!_nm);
  _nm = new int[_total_nodes+1];
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {
    _nm[node] = INVALID_NODE;
  }
  ::status.order.reset().start();
  try{
    CMD::command("order_" + OPT::order, scope);
  }catch(Exception const&){ untested();
    error(bDANGER, "invalid order spec " + OPT::order);
  }
  ::status.order.stop();
}
/*--------------------------------------------------------------------------*/
extern NODE ground_node;
/* prepare top level for node mapping
 * reset top level device ports to what was read in.
 */
extern NODE electrical;
static void clear_top_nodes(CARD_LIST* scope)
{
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }
  assert(scope->nodes());
  NODE_MAP& top_nodes = *scope->nodes();
  // assert(top_nodes[0].n_() == &ground_node);
  for (NODE_MAP::iterator p = top_nodes.begin(); p != top_nodes.end(); ++p ){
    NODE* n = (*p).second;
    USER_NODE* un = prechecked_cast<USER_NODE*>(n);
    assert(un);
    assert(n->net_nodes()==1);

    n->n_(0).clear(); // TODO: set type.
  }

  for (int i=0; i<top_nodes.size(); ++i) {
    node_t none;
    top_nodes[i] = none;
    top_nodes[i].set_type(&electrical);
  }
#if 0
  top_nodes[0].set_to_ground(nullptr); // link_to(top_nodes["0"]);
#else
  top_nodes[0] = &ground_node;
  assert(ground_node.matrix_number()==0);
  // assert(top_nodes[0].m_()==0);
  top_nodes[0].map();
  assert(top_nodes[0].m_()==0);
#endif
}
/*--------------------------------------------------------------------------*/
/* map USER_NODEs (top level only)
 */
static void map_user_nodes(CARD_LIST* scope)
{
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }
  assert(scope->nodes());
  NODE_MAP& top_nodes = *scope->nodes();
  for (NODE_MAP::iterator p = top_nodes.begin(); p != top_nodes.end(); ++p ){
    NODE* n = (*p).second;
    USER_NODE* un = prechecked_cast<USER_NODE*>(n);
    assert(un);
    assert(n->net_nodes()==1);

    if(un->user_number()==0){
      n->n_(0).map_subckt_node(&top_nodes[0], nullptr);
    }else if(un->is_global()){
      // node has link. must be global.. map as usual
      n->n_(0).map_subckt_node(&top_nodes[0], nullptr);
    }else{
      // just link, but do not mark for allocation.
      int u = n->user_number();
      assert(u == n->n_(0).e_());
      n->n_(0).clear();
      n->n_(0).link_to(&top_nodes[u]);
    }
    assert((*p).first == n->short_label()); // BUG: redundant storage.
					    // use std::set and c++14?
  }

}
/*--------------------------------------------------------------------------*/
/* init: allocate, set up, etc ... for any type of simulation
 * also called by status and probe for access to internals and subckts
 */
void SIM_DATA::init(CARD_LIST* scope)
{
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }
  if (is_first_expand()) {
    uninit();
    init_node_count(0, 0, 0);
    clear_top_nodes(scope);
    scope->precalc_first();
    scope->expand_first();
    scope->map_subckt_nodes(nullptr, nullptr);
    map_user_nodes(scope);
    scope->expand_();
    expand_last();
    alloc_hold_vectors(scope);
    scope->make_fanout();
    map__nodes(scope);
    scope->map_nodes();
    _aa.reinit(_total_nodes);
    _acx.reinit(_total_nodes);
    scope->tr_iwant_matrix();
    scope->ac_iwant_matrix();
    _last_time = 0;
  }else{
    random_seeds.clear();
    scope->precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::expand_last()
{
  while (!_explast_q.empty()) {itested();
    _explast_q.front()->expand_last();
    _explast_q.pop_front();
  }
}
/*--------------------------------------------------------------------------*/
/* alloc_hold_vectors:
 * allocate space to hold data between commands.
 * for restart, convergence assistance, bias for AC, post-processing, etc.
 * must be done BEFORE deciding what array elements to allocate,
 * but after mapping
 * if they already exist, leave them alone to save data
 */
extern NODE ground_node;
void SIM_DATA::alloc_hold_vectors(CARD_LIST* scope)
{
  assert(is_first_expand());
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }
  assert(scope->nodes());
  NODE_MAP& top_nodes = *scope->nodes();

  assert(!_nstat);
  _nstat = true;

  assert(top_nodes.size());
  for (int ii=top_nodes.size(); --ii;) {
    top_nodes[ii].allocate(1 /*bump user node count*/);
  }

  assert(ground_node.owner() == nullptr);
  assert(top_nodes[0].n_() == &ground_node);
  for(auto p : top_nodes) {
    int idx = p.second->user_number();
    assert(idx!=INVALID_NODE);
    if( top_nodes[idx].n_()){
      top_nodes[idx].n_()->set_label(p.first);
      // assert(&top_nodes[idx] == &top_nodes[idx].root());
      p.second->n_(0).link_to(&top_nodes[idx]); // too late?
    }else{
    }
  }

  assert(!_vdc);
  _vdc = new double[_total_nodes+1];
  std::fill_n(_vdc, _total_nodes+1, 0);

  assert(_nstat);
  assert(_vdc);

  top_nodes[0].map();
  assert(top_nodes[0].m_()==0);
}
/*--------------------------------------------------------------------------*/
/* alloc_vectors:
 * these are new with every run and are discarded after the run.
 */
void SIM_DATA::alloc_vectors()
{
  assert(_evalq1.empty());
  assert(_evalq2.empty());
  assert(_evalq != _evalq_uc);

  assert(!_ac);
  assert(!_i);
  assert(!_v0);
  assert(!_vt1);

  _ac = new COMPLEX[_total_nodes+1];
  _noise = new COMPLEX[_total_nodes+1];
  _i   = new double[_total_nodes+1];
  _v0  = new double[_total_nodes+1];
  _vt1 = new double[_total_nodes+1];
  std::fill_n(_ac, _total_nodes+1, 0);
  std::fill_n(_noise, _total_nodes+1, 0);
  std::fill_n(_i,  _total_nodes+1, 0);
  std::fill_n(_v0, _total_nodes+1, 0);
  std::fill_n(_vt1,_total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
void SIM_DATA::unalloc_vectors()
{
  _evalq1.clear();
  _evalq2.clear();
  delete [] _i;
  _i = nullptr;
  delete [] _v0;
  _v0 = nullptr;
  delete [] _vt1;
  _vt1 = nullptr;
  delete [] _ac;
  _ac = nullptr;
  delete [] _noise;
  _noise = nullptr;
}
/*--------------------------------------------------------------------------*/
/* uninit: undo all the allocation associated with any simulation
 * called when the circuit changes after a run, so it needs a restart
 * may be called multiple times without damage to make sure it is clean
 */
void SIM_DATA::uninit()
{
  if (_vdc) {
    _acx.reinit(0);
    _aa.reinit(0);
    delete [] _vdc;
    _vdc = nullptr;
    _nstat = false;
    delete [] _nm;
    _nm = nullptr;
  }else{
    assert(_acx.size() == 0);
    assert(_aa.size() == 0);
    assert(!_nstat);
    assert(!_nm);
  }
  _has_op = s_NONE;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
