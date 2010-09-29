/*$Id: d_switch.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * voltage (and current) controlled switch.
 * netlist syntax:
 * device:  Sxxxx n+ n- vc+ vc- mname <on>|<off> <model-card-args>
 * model:   .model mname SW <args>
 * current controlled switch
 * device:  Wxxxx n+ n- controlelement mname <on>|<off> <model-card-args>
 * model:   .model mname CSW <args>
 */
//testing=script 2006.06.14
#include "d_switch.h"
/*--------------------------------------------------------------------------*/
static COMMON_SWITCH Default_SWITCH(CC_STATIC);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool COMMON_SWITCH::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_SWITCH* p = dynamic_cast<const COMMON_SWITCH*>(&x);
  bool rv = p
    && COMMON_COMPONENT::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SWITCH::_default_vt = 0.;
double const MODEL_SWITCH::_default_vh = 0.;
double const MODEL_SWITCH::_default_ron = 1.;
double const MODEL_SWITCH::_default_roff = 1e12;
/*--------------------------------------------------------------------------*/
MODEL_SWITCH::MODEL_SWITCH()
  :MODEL_CARD(),
   vt(_default_vt),
   vh(_default_vh),
   ron(_default_ron),
   roff(_default_roff),
   von(_default_vt + _default_vh),
   voff(_default_vt - _default_vh),
   type(VOLTAGE)
{
}
/*--------------------------------------------------------------------------*/
void MODEL_SWITCH::parse_spice(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();		// skip known ".model"
  parse_label(cmd);
  int here = cmd.cursor();
  ONE_OF
    || set(cmd, "SW",  &type, VOLTAGE)
    || set(cmd, "CSW", &type, CURRENT)
    ;
  if (cmd.stuck(&here)) {
    untested();
    cmd.warn(bWARNING, "need sw or csw");
  }
  cmd.skip1b('(');
  cmd.stuck(&here);
  do{
    ONE_OF
      || get(cmd, "VT",   &vt)
      || get(cmd, "VH",   &vh)	//,  mPOSITIVE)
      || get(cmd, "IT",   &vt)
      || get(cmd, "IH",   &vh)	//,  mPOSITIVE)
      || get(cmd, "RON",  &ron)
      || get(cmd, "ROFF", &roff)
      || get(cmd, "VON",  &von)
      || get(cmd, "VOFF", &voff)
      || get(cmd, "ION",  &von)
      || get(cmd, "IOFF", &voff)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skip1b(')');
  cmd.check(bWARNING, "what's this?");
}
/*--------------------------------------------------------------------------*/
void MODEL_SWITCH::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_CARD::elabo1();
    vt.e_val(_default_vt, par_scope);
    vh.e_val(_default_vh, par_scope);
    ron.e_val(_default_ron, par_scope);
    roff.e_val(_default_roff, par_scope);
    von.e_val(vt + vh, par_scope);
    voff.e_val(vt - vh, par_scope);
  }else{
    untested();
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_SWITCH::print_spice(OMSTREAM& where, int)const
{
  where.setfloatwidth(7);
  switch (type) {
  case VOLTAGE:
    where << ".model " << short_label() << " sw ("
	  <<   "vt=" << vt
	  << "  vh=" << vh
	  << "  von="  << von
	  << "  voff=" << voff;
    break;
  case CURRENT:
    where << ".model " << short_label() << " csw ("
	  <<   "it=" << vt
	  << "  ih=" << vh
	  << "  ion="  << von
	  << "  ioff=" << voff;
    break;
  }
  where << "  ron="  << ron
	<< "  roff=" << roff
	<< ")\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SWITCH_BASE::SWITCH_BASE()
  :ELEMENT(),
   _input_label(),
   _input(0),
   _ic(_UNKNOWN),
   _time_future(0.)
{
  attach_common(&Default_SWITCH);
  std::fill_n(_time, int(_keep_time_steps), 0.);
  std::fill_n(_in, int(_keep_time_steps), 0.);
  std::fill_n(_state, int(_keep_time_steps), _UNKNOWN);
}
/*--------------------------------------------------------------------------*/
SWITCH_BASE::SWITCH_BASE(const SWITCH_BASE& p)
  :ELEMENT(p),
   _input_label(p._input_label),
   _input(0),
   _ic(p._ic),
   _time_future(p._time_future)
{
  untested();
  notstd::copy_n(p._time, int(_keep_time_steps), _time);
  notstd::copy_n(p._in, int(_keep_time_steps), _in);
  notstd::copy_n(p._state, int(_keep_time_steps), _state);  
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::parse_spice(CS& cmd)
{
  assert(has_common());
  COMMON_SWITCH* c = prechecked_cast<COMMON_SWITCH*>(common()->clone());
  assert(c);
  assert(!c->has_model());

  parse_Label(cmd);
  parse_nodes(cmd, max_nodes(), min_nodes(), 1/*model_name*/, 0/*start*/);
  if (typeid(*this) == typeid(DEV_CSWITCH)) {
    _input_label = cmd.ctos(TOKENTERM);
    _input_label[0] = toupper(_input_label[0]);
  }else{
    assert(typeid(*this) == typeid(DEV_VSWITCH));
  }
  c->parse_modelname(cmd);
  int here = cmd.cursor();
  ONE_OF
    || set(cmd, "OFF",    &_ic, _OFF)
    || set(cmd, "ON",	  &_ic, _ON)
    || set(cmd, "UNKNOWN",&_ic, _UNKNOWN)
    ;
  if (cmd.stuck(&here)) {
    cmd.check(bWARNING, "need off, on, or unknown");
  }else{
    cmd.check(bWARNING, "what's this?");
    untested();
  }
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::print_spice(OMSTREAM& where, int)const
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  where << short_label();
  printnodes(where);
  if (_input) {
    where << "  " << _input->short_label();
  }else{
    where << "  " << _input_label;
  }
  where << ' ' << c->modelname();

  switch (_ic) {
    case _OFF:     untested();	where << " off"; break;
    case _ON:      untested();	where << " on";  break;
    case _UNKNOWN: break;
  }
  where << '\n';
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::elabo1()
{
  ELEMENT::elabo1();
  const MODEL_SWITCH* m = dynamic_cast<const MODEL_SWITCH*>(attach_model());
  if (!m) {
    untested();
    assert(has_common());
    error(bERROR, long_label() + ": model " + common()->modelname()
	  + " is not a switch (SW or CSW)\n");
  }
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::precalc()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);

  set_value(m->ron);
  _y0.f0 = LINEAR;
  _y0.f1 = (_ic == _ON) ? m->ron : m->roff;	/* unknown is off */
  _m0.c1 = 1./_y0.f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  _state[1] = _state[0] = _ic;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  assert(!is_constant()); /* depends on input */
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::tr_begin()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);

  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  assert(_y0.f0 == LINEAR);
  _y0.f1 = ((_ic == _ON) ? m->ron : m->roff);  /* unknown is off */
  _m0.c1 = 1./_y0.f1;
  assert(_m0.c0 == 0.);
  _m1 = _m0;
  _state[0] = _state[0] = _ic;
  _time[0] = _time[1] = 0.;
  set_converged();
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::dc_advance()
{ 
  _state[1] = _state[0];
  _time[0] = _time[1] = 0.;
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::tr_advance()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);
  
  if (_time[0] < SIM::time0) {		// forward
    assert(_time[1] < _time[0] || _time[0] == 0.);
    _time[1] = _time[0];
    _state[1] = _state[0];
    _y0.x = _in[1] = _in[0];
    
    if (_y0.x >= m->von) {
      _state[0] = _ON;
    }else if (_y0.x <= m->voff) {
      _state[0] = _OFF;
    }else{
      _state[0] = _state[1];
    }
    
    if (_state[1] != _state[0]) {
      _y0.f1 = (_state[0] == _ON) ? m->ron : m->roff;	/* unknown is off */
      _m0.c1 = 1./_y0.f1;
      q_eval();
    }
  }else{
    assert(_time[1] < SIM::time0);
    assert(_time[0] >= SIM::time0);
    assert(_time[1] < _time[0] || _time[0] == 0.);
    // else backward, don't do anything
  }
  assert(_y0.f1 == ((_state[0] == _ON) ? m->ron : m->roff));
  assert(_y0.f0 == LINEAR);
  assert(_m0.c1 == 1./_y0.f1);
  assert(_m0.c0 == 0.);
  set_converged();
  _time[0] = SIM::time0;
}
/*--------------------------------------------------------------------------*/
bool SWITCH_BASE::do_tr()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);
  
  if (SIM::phase != SIM::pTRAN) {
    _y0.x = (_input)			/* _y0.x is controlling value */
      ? CARD::probe(_input,"I")		/* current controlled */
      : _n[IN1].v0() - _n[IN2].v0();	/* voltage controlled */
    
    state_t new_state;
    if (_y0.x > m->von) {
      new_state = _ON;
    }else if (_y0.x < m->voff) {
      new_state = _OFF;
    }else{
      new_state = _state[1];
    }
    
    if (new_state != _state[0]) {
      _y0.f1 = (new_state == _ON) ? m->ron : m->roff;	/* unknown is off */
      _state[0] = new_state;
      _m0.c1 = 1./_y0.f1;
      q_load();
      store_values();
      set_not_converged();
    }else{
      set_converged();
    }
  }else{
    if (_state[0] != _state[1]) {
      q_load();
      store_values();
    }else{
      // gets here only on "nobypass"
    }
    assert(converged());
  }
  assert(_y0.f1 == ((_state[0] == _ON) ? m->ron : m->roff));
  assert(_y0.f0 == LINEAR);
  //assert(_m0.c1 == 1./_y0.f1);
  assert(_m0.c0 == 0.);
  return converged();
}
/*--------------------------------------------------------------------------*/
DPAIR SWITCH_BASE::tr_review()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);
  
  _in[0] = (_input)
    ? CARD::probe(_input,"I")
    : _n[IN1].v0() - _n[IN2].v0();

  double dt = _time[0] - _time[1];
  double dv = _in[0] - _in[1];
  double dvdt = dv / dt;
  if (_state[0] != _ON  &&  dvdt > 0) {
    double new_dv = m->von - _in[1];
    _time_future = _time[1] + new_dv / dvdt;
  }else if (_state[0] != _OFF  &&  dvdt < 0) {
    double new_dv = m->voff - _in[1];
    _time_future = _time[1] + new_dv / dvdt;
  }else{
    _time_future = NEVER;
  }
  if (_time_future < _time[1] + 2*SIM::dtmin) {
    _time_future = _time[1] + 2*SIM::dtmin;
  }
  if (conchk(_time_future, _time[0], 2*SIM::dtmin, 0.)) {
    _time_future = _time[0] + 2*SIM::dtmin;
  }
  return DPAIR(NEVER, _time_future);
}
/*--------------------------------------------------------------------------*/
double SWITCH_BASE::tr_probe_num(CS& cmd)const
{
  if (cmd.pmatch("DT")) {
    untested();
    return _time[0] - _time[1];
  }else if (cmd.pmatch("TIME")) {
    untested();
    return _time[0];
  }else if (cmd.pmatch("TIMEOld")) {
    untested();
    return _time[1];
  }else if (cmd.pmatch("TIMEFuture")) {
    return _time_future;
  }else{
    return ELEMENT::tr_probe_num(cmd);
  }
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::do_ac()
{
  assert(_ev  == _y0.f1);
  assert(_acg == _m0.c1);
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_CSWITCH::elabo1()
{
  SWITCH_BASE::elabo1();
  _input = dynamic_cast<const ELEMENT*>(
	   find_in_my_scope(_input_label, bERROR));
  if (!_input) {
    untested();
    error(bERROR,
	  long_label() + ": " + _input_label + " cannot be used as input\n");
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
