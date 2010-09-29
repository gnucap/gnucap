/*$Id: d_admit.cc,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
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
 * admittance devices:
 *	self-admittance (old Y device)
 *		y.x = volts, y.f0 = amps, ev  = y.f1 = mhos.
 *		m.x = volts, m.c0 = amps, acg = m.c1 = mhos.
 *	trans-admittance (VCCS, G device)
 *	voltage controlled admittance
 *		y.x  = volts(control), y.f0 = mhos, ev = y.f1 = mhos/volt
 *		m.x  = volts(control), m.c0 = 0,    acg = m.c1 = mhos
 *		_loss0 == 1/R. (mhos)
 */
//testing=script 2006.07.17
#include "d_admit.h"
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::precalc()
{
  _y0.f0 = LINEAR;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.c1 = _y0.f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
bool DEV_ADMITTANCE::do_tr()
{
  if (using_tr_eval()) {
    _y0.x = _m0.x = tr_involts_limited();
    // _y0.x = tr_input_limited();
    //assert(_y0.x == _m0.x);
    _y0.f0 = _m0.c1 * _m0.x + _m0.c0;	/* BUG:  patch for diode */
    tr_eval();
    assert(_y0.f0 != LINEAR);
    store_values();
    q_load();
    _m0 = CPOLY1(_y0);
  }else{
    assert(_y0.f0 == LINEAR);
    assert(_y0.f1 == value());
    assert(_m0.c1 == _y0.f1);
    assert(_m0.c0 == 0.);
    assert(_y1 == _y0);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::do_ac()
{
  if (using_ac_eval()) {untested();
    ac_eval();
    _acg = _ev;
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  assert(_acg == _ev);
  ac_load(); 
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VCG::precalc()
{
  _loss1 = _loss0 = 0;
  _y0.f0 = 0;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.c1 = 0.;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(!is_constant());
  set_not_converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_VCG::do_tr()
{
  _y0.x = tr_input_limited();
  tr_eval();
  assert(_y0.f0 != LINEAR);

  store_values();
  q_load();

  _loss0 = _y0.f0;
  _m0.x = tr_outvolts(); // fake
  _m0.c1 = _y0.f1 * tr_outvolts();
  _m0.c0 = -_y0.x * _m0.c1;
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCG::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = _ev * _m0.x;
    _ev *= _y0.x;
  }else{
    assert(_ev == _y0.f0);
    assert(_acg == _m0.c1);
  }
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* DEV_CPOLY_G
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G::DEV_CPOLY_G()
  :ELEMENT(),
   _values(0),
   _old_values(0),
   _n_ports(0),
   _time(NOT_VALID),
   _inputs(0)
{
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G::~DEV_CPOLY_G()
{
  delete [] _old_values;
  if (net_nodes() > NODES_PER_BRANCH) {
    delete [] _n;
  }else{
    // it is part of a base class
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_G::do_tr_con_chk_and_q()
{
  q_load();

  assert(_old_values);
  set_converged(conchk(_time, SIM::time0));
  _time = SIM::time0;
  for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_old_values[i], _values[i]));
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_G::do_tr()
{
  assert(_values);
  _m0 = CPOLY1(0., _values[0], _values[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
#if 0
bool DEV_FPOLY_G::do_tr()
{untested();
  assert(_values);
  double c0 = _values[0];
  if (_inputs) {untested();
    untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      c0 -= *(_inputs[i]) * _values[i];
      trace4("", i, *(_inputs[i]), _values[i], *(_inputs[i]) * _values[i]);
    }
  }else{untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      c0 -= volts_limited(_n[2*i-2],_n[2*i-1]) * _values[i];
      trace4("", i, volts_limited(_n[2*i-2],_n[2*i-1]), _values[i],
	     volts_limited(_n[2*i-2],_n[2*i-1]) * _values[i]);
    }
  }
  trace2("", _values[0], c0);
  _m0 = CPOLY1(0., c0, _values[1]);

  return do_tr_con_chk_and_q();
}
#endif
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::tr_load()
{
  tr_load_passive();
  _old_values[0] = _values[0];
  _old_values[1] = _values[1];
  for (int i=2; i<=_n_ports; ++i) {
    tr_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1],
		     &(_values[i]), &(_old_values[i]));
  }
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::tr_unload()
{
  
  std::fill_n(_values, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }else{
  }
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CPOLY_G::tr_amps()const
{
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _values[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::ac_load()
{
  _acg = _values[1];
  ac_load_passive();
  for (int i=2; i<=_n_ports; ++i) {
    ac_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1], _values[i]);
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void DEV_CPOLY_G::set_parameters(const std::string& Label, CARD *Owner,
				 COMMON_COMPONENT *Common, double Value,
				 int n_states, double states[],
				 int n_nodes, const node_t nodes[])
  //				 const double* inputs[])
{
  bool first_time = (net_nodes() == 0);

  set_Label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  if (first_time) {
    _n_ports = n_nodes/2; // sets num_nodes() = _n_ports*2
    assert(_n_ports == n_states-1);

    assert(!_old_values);
    _old_values = new double[n_states];

    if (net_nodes() > NODES_PER_BRANCH) {
      // allocate a bigger node list
      _n = new node_t[net_nodes()];
    }else{
      // use the default node list, already set
    }      
  }else{itested();
    assert(_n_ports == n_states-1);
    assert(_old_values);
    assert(net_nodes() == n_nodes);
    // assert could fail if changing the number of nodes after a run
  }

  //_inputs = inputs;
  _inputs = 0;
  _values = states;
  std::fill_n(_values, n_states, 0.);
  std::fill_n(_old_values, n_states, 0.);
  notstd::copy_n(nodes, net_nodes(), _n);
  assert(net_nodes() == _n_ports * 2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
