/*$Id: u_sim_data.cc,v 26.135 2009/12/02 09:26:53 al Exp $ -*- C++ -*-
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
//testing=obsolete
#include "e_node.h"
#include "u_nodemap.h"
#include "e_cardlist.h"
#include "u_status.h"
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
void SIM_DATA::map__nodes()
{
  _nm = new int[_total_nodes+1];
  ::status.order.reset().start();
  switch (OPT::order) {
    default: untested();
      error(bWARNING, "invalid order spec: %d\n", OPT::order);
    case oAUTO:		       order_auto();    break;
    case oREVERSE: untested(); order_reverse(); break;
    case oFORWARD: untested(); order_forward(); break;
  }
  ::status.order.stop();
}
/*--------------------------------------------------------------------------*/
/* order_reverse: force ordering to reverse of user ordering
 *  subcircuits at beginning, results on border at the bottom
 */
void SIM_DATA::order_reverse()
{untested();
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {untested();
    _nm[node] = _total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/* order_forward: use user ordering, with subcircuits added to end
 * results in border at the top (worst possible if lots of subcircuits)
 */
void SIM_DATA::order_forward()
{untested();
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {untested();
    _nm[node] = node;
  }
}
/*--------------------------------------------------------------------------*/
/* order_auto: full automatic ordering
 * reverse, for now
 */
void SIM_DATA::order_auto()
{
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {
    _nm[node] = _total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* init: allocate, set up, etc ... for any type of simulation
 * also called by status and probe for access to internals and subckts
 */
void SIM_DATA::init()
{
  if (is_first_expand()) {
    uninit();
    init_node_count(CARD_LIST::card_list.nodes()->how_many(), 0, 0);
    CARD_LIST::card_list.expand();
    CARD_LIST::card_list.precalc_last();
    map__nodes();
    CARD_LIST::card_list.map_nodes();
    alloc_hold_vectors();
    _aa.reinit(_total_nodes);
    _lu.reinit(_total_nodes);
    _acx.reinit(_total_nodes);
    CARD_LIST::card_list.tr_iwant_matrix();
    CARD_LIST::card_list.ac_iwant_matrix();
    _last_time = 0;
  }else{
    CARD_LIST::card_list.precalc_first();
    CARD_LIST::card_list.precalc_last();
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
void SIM_DATA::alloc_hold_vectors()
{
  assert(is_first_expand());

  assert(!_nstat);
  _nstat = new LOGIC_NODE[_total_nodes+1];
  for (int ii=0;  ii <= _total_nodes;  ++ii) {
    _nstat[_nm[ii]].set_user_number(ii);
  }

  assert(!_vdc);
  _vdc = new double[_total_nodes+1];
  std::fill_n(_vdc, _total_nodes+1, 0);

  assert(_nstat);
  assert(_vdc);
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
  _i   = new double[_total_nodes+1];
  _v0  = new double[_total_nodes+1];
  _vt1 = new double[_total_nodes+1];
  std::fill_n(_ac, _total_nodes+1, 0);
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
  _i = NULL;
  delete [] _v0;
  _v0 = NULL;
  delete [] _vt1;
  _vt1 = NULL;
  delete [] _ac;
  _ac = NULL;
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
    _lu.reinit(0);
    _aa.reinit(0);
    delete [] _vdc;
    _vdc = NULL;
    delete [] _nstat;
    _nstat = NULL;
    delete [] _nm;
    _nm = NULL;
  }else{
    assert(_acx.size() == 0);
    assert(_lu.size() == 0);
    assert(_aa.size() == 0);
    assert(!_nstat);
    assert(!_nm);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
