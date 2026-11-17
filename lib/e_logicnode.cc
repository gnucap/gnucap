/*$Id: e_logicnode.cc $ -*- C++ -*-
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "e_logicnode.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
void LOGIC_NODE_BASE::tr_begin()
{
  _d_iter = -1;
  _a_iter = 0;
  _final_time = 0;
  _lastchange = 0;
  _old_lastchange = 0;
  _lv = _old_lv = lvXX;
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE_BASE::tr_probe_num(const std::string& x) const
{
  if (Umatch(x, "la{stchange} ")) {
    return _lastchange;
  }else if (Umatch(x, "fi{naltime} ")) {
    return final_time();
  }else if (Umatch(x, "di{ter} ")) {untested();
    return static_cast<double>(_d_iter);
  }else if (Umatch(x, "ai{ter} ")) {untested();
    return static_cast<double>(_a_iter);
  }else{
    return NODE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE_BASE::force_initial_value(LOGICVAL v)
{
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{untested();
  }
  if(_sim->_time0){
    assert(_sim->analysis_is_restore());
  }else{
    assert(!_sim->analysis_is_restore());
    assert(_sim->analysis_is_static());
  }
  // assert(is_unknown() || _lv == lvX0 || _lv == lvX1 || _lv == lvXZ);
  assert(is_digital());
  set_lv(v); // BUG ??
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE_BASE::set_event(double delay, LOGICVAL v, CARD* Device)
{
  if (Device){
  }else{ itested();
  }
  _lv.set_in_transition(v);
  if (_sim->analysis_is_tran_dynamic()  &&  in_transit()) {
    incomplete();
    // set_bad_quality("race");
  }else{
    // normal good quality event
    // leaving quality as it was
  }
  set_d_iter();
  double ft = CKT_BASE::_sim->new_event(_sim->_time0 + delay, Device?:this);
  trace4("set_event", _sim->_time0, delay, ft, _sim->_dtmin);
  set_final_time(ft);
  if (OPT::picky <= bTRACE) {
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  set_last_change_time();
  if(!subckt()){
  }else if(is_cm_hack()){ untested();
    subckt()->tr_accept();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
int LOGIC_NODE_BASE::matrix_number() const
{
  return _sim->_nm[flat_number()]; // TODO: -> MATRIX_NODE
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE_BASE::propagate()
{
  assert(in_transit());
  if (lv().is_rising()) {
    set_lv(lv11);
  }else if (lv().is_falling()) {
    set_lv(lv00);
  }else{
    // lv no change
  }
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
  assert(!(in_transit()));
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE_BASE::unpropagate()
{
  set_final_time(last_change_time());
  set_last_change_time(old_last_change_time());
  set_lv(old_lv());
  set_d_iter();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
