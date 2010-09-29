/*$Id: d_cap.cc,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
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
 * capacitor models
 * y.x = volts, y.f0 = coulombs, ev = y.f1 = farads
 * q = y history in time
 * i.x = volts, i.f0 = amps,	      i.f1 = mhos
 * m.x = volts, m.c0 = amps,    acg = m.c1 = mhos
 */
//testing=script 2006.07.17
#include "d_cap.h"
/*--------------------------------------------------------------------------*/
bool DEV_CAPACITANCE::do_tr()
{
  if (using_tr_eval()) {
    _y0.x = tr_input_limited();
    tr_eval();
  }else{
    _y0.x = tr_input(); // tr_involts();
    assert(_y0.f1 == value());
    _y0.f0 = _y0.x * _y0.f1;
    assert(converged());
  }
  store_values();
  q_load();

  _q[0] = _y0;

  _i0.x  = _y0.x;
  _i0.f0 = differentiate();
  _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);
  _m0 = CPOLY1(_i0);
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CAPACITANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  _acg = _ev * SIM::jomega;
  ac_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CAPACITANCE::tr_probe_num(CS& cmd)const
{
  if (cmd.pmatch("Q") || cmd.pmatch("CHarge")) {
    return _q[0].f0;
  }else if (cmd.pmatch("Capacitance")) {
    return _q[0].f1;
  }else if (cmd.pmatch("DCDT")) {untested();
    return (_q[0].f1 - _q[1].f1) / _dt;
  }else if (cmd.pmatch("DC")) {untested();
    return (_q[0].f1 - _q[1].f1);
  }else if (cmd.pmatch("DQDT")) {
    return (_q[0].f0 - _q[1].f0) / _dt;
  }else if (cmd.pmatch("DQ")) {
    return (_q[0].f0 - _q[1].f0);
  }else{
    return STORAGE::tr_probe_num(cmd);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool DEV_VCCAP::do_tr()
{
  _y0.x = tr_input_limited();
  tr_eval();

  store_values();
  q_load();

  _q[0].x = tr_outvolts();
  _q[0].f1 = _y0.f0;		 // self capacitance
  _q[0].f0 = _q[0].x * _q[0].f1; // charge

  _i0.x  = _q[0].x;
  _i0.f0 = differentiate();
  _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);

  _m0.x  = _i0.x;
  _m0.c1 = _i0.f1;
  _m0.c0 = _i0.f0 - _i0.x * _i0.f1;
  return converged();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* DEV_CPOLY_CAP
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
/*--------------------------------------------------------------------------*/
DEV_CPOLY_CAP::DEV_CPOLY_CAP()
  :STORAGE(),
   _vy0(0),
   _vy1(0),
   _vi0(0),
   _vi1(0),
   _n_ports(0),
   _load_time(NOT_VALID),
   _inputs(0)
{
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_CAP::~DEV_CPOLY_CAP() 
{
  delete [] _vy1;
  delete [] _vi0;
  delete [] _vi1;
  if (net_nodes() > NODES_PER_BRANCH) {untested();
    delete [] _n;
  }else{
    // it is part of a base class
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_CAP::do_tr_con_chk_and_q()
{
  q_load();

  assert(_vy1);
  set_converged(conchk(_load_time, SIM::time0));
  _load_time = SIM::time0;
  for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_vy1[i], _vy0[i]));
  }
  set_converged();
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_CAP::do_tr()
{untested();
  incomplete();
  _m0 = CPOLY1(0., _vi0[0], _vi0[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
bool DEV_FPOLY_CAP::do_tr()
{
  assert((_time[0] == 0) || (_vy0[0] == _vy0[0]));

  _q[0].x  = tr_outvolts();
  _q[0].f0 = _vy0[0];
  _q[0].f1 = _vy0[1];
  
  _i0.x  = _q[0].x;
  _vi0[0] = _i0.f0 = differentiate();
  _vi0[1] = _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);
  assert(_vi0[0] == _vi0[0]);
  
  if (_inputs) {untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      _vi0[i] = tr_c_to_g(_vy0[i], _vi0[i]);
      _vi0[0] -= *(_inputs[i]) * _vi0[i];
    }
  }else{
    for (int i=1; i<=_n_ports; ++i) {
      _vi0[i] = tr_c_to_g(_vy0[i], _vi0[i]);
      _vi0[0] -= volts_limited(_n[2*i-2],_n[2*i-1]) * _vi0[i];
      assert(_vi0[i] == _vi0[i]);
      assert(_vi0[0] == _vi0[0]);
    }
  }
  for (int i=0; i<=_n_ports; ++i) {
    assert(_vi0[i] == _vi0[i]);
  }
  
  _m0 = CPOLY1(0., _vi0[0], _vi0[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::tr_load()
{
  for (int i=0; i<=_n_ports; ++i) {
    assert(_vi0[i] == _vi0[i]);
  }
  tr_load_passive();
  _vi1[0] = _vi0[0];
  _vi1[1] = _vi0[1];
  for (int i=2; i<=_n_ports; ++i) {
    tr_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1],
		     &(_vi0[i]), &(_vi1[i]));
  }
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::tr_unload()
{untested();
  std::fill_n(_vi0, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  if (SIM::inc_mode) {untested();
    SIM::inc_mode = tsBAD;
  }else{untested();
  }
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CPOLY_CAP::tr_amps()const
{untested();
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {untested();
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _vi0[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::ac_load()
{
  _acg = _vy0[1] * SIM::jomega;
  ac_load_passive();
  for (int i=2; i<=_n_ports; ++i) {
    ac_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1],
		     _vy0[i] * SIM::jomega);
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void DEV_CPOLY_CAP::set_parameters(const std::string& Label, CARD *Owner,
				   COMMON_COMPONENT *Common, double Value,
				   int n_states, double states[],
				   int n_nodes, const node_t nodes[])
  //				   const double* inputs[])
{
  bool first_time = (net_nodes() == 0);

  set_Label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  if (first_time) {
    _n_ports = n_nodes/2; // sets num_nodes() = _n_ports*2
    assert(_n_ports == n_states-1);

    assert(!_vy1);
    assert(!_vi0);
    assert(!_vi1);
    _vy1 = new double[n_states]; 
    _vi0 = new double[n_states];
    _vi1 = new double[n_states];

    if (net_nodes() > NODES_PER_BRANCH) {untested();
      // allocate a bigger node list
      _n = new node_t[net_nodes()];
    }else{
      // use the default node list, already set
    }      
  }else{itested();
    assert(_n_ports == n_states-1);
    assert(_vy1);
    assert(_vi0);
    assert(_vi1);
    assert(net_nodes() == n_nodes);
    // assert could fail if changing the number of nodes after a run
  }

  //_inputs = inputs;
  _inputs = 0;
  _vy0 = states;
  std::fill_n(_vy0, n_states, 0.);
  std::fill_n(_vy1, n_states, 0.);
  std::fill_n(_vi0, n_states, 0.);
  std::fill_n(_vi1, n_states, 0.);
  notstd::copy_n(nodes, net_nodes(), _n);
  assert(net_nodes() == _n_ports * 2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
