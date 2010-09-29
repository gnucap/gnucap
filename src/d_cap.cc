/*$Id: d_cap.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * capacitor models
 * y.x = volts, y.f0 = coulombs, ev = y.f1 = farads
 * m.x = volts, m.c0 = amps,    acg = m.c1 = mhos.
 */
#include "ap.h"
#include "d_cap.h"
/*--------------------------------------------------------------------------*/
bool DEV_CAPACITANCE::do_tr()
{
  initial_condition = NOT_INPUT;
  {if (has_tr_eval()) {
    _m0.x = _i0.x = _y0.x = tr_involts_limited();
    tr_eval();
  }else{
    _m0.x = _i0.x = _y0.x = tr_involts();
    assert(_y0.f1 == value());
    _y0.f0 = _y0.x * _y0.f1;
    assert(converged());
  }}
  store_values();
  q_load();

  _q[0] = _y0;

  _i0.x  = _y0.x;
  _i0.f0 = integrate();
  _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);

  _m0 = CPOLY1(_i0);
  return converged();
}
/*--------------------------------------------------------------------------*/
double DEV_CAPACITANCE::tr_review()
{
  return review(_i0.f0, _it1.f0);
}
/*--------------------------------------------------------------------------*/
void DEV_CAPACITANCE::do_ac()
{
  {if (has_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == value());
  }}
  _acg = _ev * SIM::jomega;
  ac_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CAPACITANCE::tr_probe_num(CS& cmd)const
{
  {if (cmd.pmatch("Q") || cmd.pmatch("CHarge")) {
    return _q[0].f0;
  }else if (cmd.pmatch("Capacitance")) {
    return _q[0].f1;
  }else if (cmd.pmatch("DQDT")) {
    return (_q[0].f0 - _q[1].f0) / _dt;
  }else if (cmd.pmatch("DQ")) {
    return (_q[0].f0 - _q[1].f0);
  }else if (cmd.pmatch("TIMEFuture")) {
    return review(_i0.f0, _it1.f0);
  }else{
    return STORAGE::tr_probe_num(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool DEV_VCCAP::do_tr()
{
  initial_condition = NOT_INPUT;
  _y0.x = tr_involts_limited();
  tr_eval();

  store_values();
  q_load();

  _q[0].x = tr_outvolts();
  _q[0].f1 = _y0.f0;		 // self capacitance
  _q[0].f0 = _q[0].x * _q[0].f1; // charge

  _i0.x  = _q[0].x;
  _i0.f0 = integrate();
  _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);

  _m0.x  = _i0.x;
  _m0.c1 = _i0.f1;
  _m0.c0 = _i0.f0 - _i0.x * _i0.f1;
  return converged();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* DEV_POLY_CAP
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
/*--------------------------------------------------------------------------*/
DEV_POLY_CAP::DEV_POLY_CAP(const std::string& label, int n_ports,
		node_t nodes[], double vy0[], double vy1[], CARD *owner)
  :STORAGE(),
   _vy0(vy0),
   _vy1(vy1),
   _n_ports(n_ports) 
{
  untested();
  set_Label(label);
  set_owner(owner);
  _n = nodes;
  _vi0 = new double[_n_ports+1];
  _vi1 = new double[_n_ports+1];
}
/*--------------------------------------------------------------------------*/
bool DEV_POLY_CAP::do_tr()
{
  untested();
  set_converged(conchk(_time, SIM::time0));
  _time = SIM::time0;
  {for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_vy1[i], _vy0[i]));
  }}

  _q[0].x  = tr_outvolts();
  _q[0].f0 = _vy0[0];
  _q[0].f1 = _vy0[1];

  _i0.x  = _q[0].x;
  _vi0[0] = _i0.f0 = integrate();
  _vi0[1] = _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1); 

  {for (int i=1; i<=_n_ports; ++i) {
    _vi0[i] = tr_c_to_g(_vy0[i], _vi0[i]);
    _vi0[0] -= volts_limited(_n[2*i-2],_n[2*i-1]) * _vi0[i];
  }}
  _m0 = CPOLY1(0., _vi0[0], _vi0[1]);
  q_load();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_POLY_CAP::tr_load()
{
  untested();
  tr_load_passive();
  _vi1[0] = _vi0[0];
  _vi1[1] = _vi0[1];
  for (int i=2; i<=_n_ports; ++i) {
    tr_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1],
		     &(_vi0[i]), &(_vi1[i]));
  }
}
/*--------------------------------------------------------------------------*/
void DEV_POLY_CAP::tr_unload()
{
  untested();
  std::fill_n(_vi0, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_POLY_CAP::tr_amps()const
{
  untested();
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _vi0[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_POLY_CAP::ac_load()
{
  untested();
  _acg = _vy0[1] * SIM::jomega;
  ac_load_passive();
  for (int i=2; i<=_n_ports; ++i) {
    ac_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1],
		     _vy0[i] * SIM::jomega);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
