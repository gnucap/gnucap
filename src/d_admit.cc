/*$Id: d_admit.cc,v 20.11 2001/10/07 05:22:34 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
  {if (has_tr_eval()) {
    _m0.x = tr_involts_limited();
    _y0.x = _m0.x;
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
  }}
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::do_ac()
{
  {if (has_ac_eval()) {
    ac_eval();
    _acg = _ev;
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == value());
  }}
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
  assert(!constant());
  set_not_converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_VCG::do_tr()
{
  _y0.x = tr_involts_limited();
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
  {if (has_ac_eval()) {
    ac_eval();
    _acg = _ev * _m0.x;
    _ev *= _y0.x;
  }else{
    assert(_ev == _y0.f0);
    assert(_acg == _m0.c1);
  }}
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* DEV_POLY_G
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
/*--------------------------------------------------------------------------*/
DEV_POLY_G::DEV_POLY_G()
  :ELEMENT(),
   _values(0),
   _old_values(0),
   _n_ports(0) 
{
}
/*--------------------------------------------------------------------------*/
#if 0
DEV_POLY_G::DEV_POLY_G(const std::string& label, int n_ports,
		       const node_t nodes[], double val[], CARD *owner)
  :ELEMENT(),
   _values(val),
   _old_values(0),
   _n_ports(n_ports) 
{
  std::fill_n(_values, n_ports+1, 0.);
  _old_values = new double[n_ports+1];
  std::fill_n(_old_values, n_ports+1, 0.);
  set_Label(label);
  set_owner(owner);
  set_port_count(n_ports*2);
  _n = new node_t[numnodes()];
  std::copy_n(nodes, numnodes(), _n);
}
#endif
/*--------------------------------------------------------------------------*/
bool DEV_POLY_G::do_tr()
{
  assert(_values);
  assert(_old_values);
  set_converged(conchk(_time, SIM::time0));
  _time = SIM::time0;
  {for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_old_values[i], _values[i]));
  }}
  double c0 = _values[0];
  {for (int i=1; i<=_n_ports; ++i) {
    c0 -= volts_limited(_n[2*i-2],_n[2*i-1]) * _values[i];
  }}
  _m0 = CPOLY1(0., c0, _values[1]);
  q_load();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_POLY_G::tr_load()
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
void DEV_POLY_G::tr_unload()
{
  //untested();
  std::fill_n(_values, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_POLY_G::tr_amps()const
{
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _values[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_POLY_G::ac_load()
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
void DEV_POLY_G::set_parameters(const std::string& Label, CARD *Owner,
				COMMON_COMPONENT *Common, double Value,
				int n_states, double states[],
				int n_nodes, const node_t nodes[])
{
  COMPONENT::set_parameters(Label, Owner, Common, Value, 0, 0, n_nodes, nodes);
  _values = states;
  _n_ports = n_nodes/2;
  assert(_n_ports == n_states-1);
  std::fill_n(_values, n_states, 0.);
  _old_values = new double[n_states];
  std::fill_n(_old_values, n_states, 0.);
  set_port_count(n_nodes);
  _n = new node_t[numnodes()];
  std::copy_n(nodes, numnodes(), _n);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
