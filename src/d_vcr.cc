/*$Id: d_vcr.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * voltage controlled resistor.
 * y.x  = volts(control), ev = y.f0 = ohms,       y.f1 = ohms/volt
 * m.x  = volts(control),      m.c0 = 0,    acg = m.c1 = mhos
 * _loss0 == 1/R. (mhos)
 */
#include "d_vcr.h"
/*--------------------------------------------------------------------------*/
void DEV_VCR::precalc()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  _y0.f0 = LINEAR;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.c1 = 0.;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(!constant());
  set_not_converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_VCR::do_tr()
{
  _y0.x = tr_involts_limited();
  tr_eval();
  trace3("vcr", _y0.x, _y0.f0, _y0.f1);
  assert(_y0.f0 != LINEAR);
  if (_y0.f0 == 0.){
    error(bDEBUG, long_label() + ": short circuit\n");
    _y0.f0 = OPT::shortckt;
    set_converged(conv_check());
  }
  store_values();
  q_load();

  _loss0 = 1./_y0.f0;
  _m0.x = tr_outvolts(); // fake
  _m0.c1 = -_y0.f1 * _loss0 * _loss0 * tr_outvolts();
  _m0.c0 = -_y0.x * _m0.c1;
  trace3("vcr", _loss0, _m0.c0, _m0.c1);
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCR::ac_begin()
{
  _ev  = _y0.f0;
  _acg = _m0.c1;
  trace4("vcr-ac_begin", _y0.f0, _y0.f1, _m0.c0, _m0.c1);
  trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
  trace1("", _loss0);
}
/*--------------------------------------------------------------------------*/
void DEV_VCR::do_ac()
{
  {if (has_ac_eval()){
    ac_eval();
    _acg = -_ev * _loss0 * _loss0 * _m0.x;
    trace4("vcr-do_ac(eval)", _y0.f0, _y0.f1, _m0.c0, _m0.c1);
    trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
    _ev *= _y0.x;
    trace4("", _ev.real(), _ev.imag(), _loss0, _m0.x);
  }else{
    trace4("vcr-do_ac", _y0.f0, _y0.f1, _m0.c0, _m0.c1);
    trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
    trace1("", _loss0);
    assert(_ev == _y0.f0);
    assert(_acg == _m0.c1);
  }}
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
