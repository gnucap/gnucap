/*$Id: d_res.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * functions for resistor.
 * y.x = amps,  y.f0 = volts, ev = y.f1 = ohms
 * m.x = volts, m.c0 = amps, acg = m.c1 = mhos.
 */
//testing=script,complete 2006.07.17
#include "d_res.h"
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::precalc()
{
  if (value() == 0. && !has_common()) {
    error(bPICKY, long_label() + ": short circuit\n");
  }else{
  }
  _y0.f0 = LINEAR;
  _y0.f1 = (value() != 0.) ? value() : OPT::shortckt;
  _y1 = _y0;
  _m0.c1 = 1./_y0.f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::dc_begin()
{
  if (!using_tr_eval()) {
    assert(_y0.f0 == LINEAR);
    assert(_y0.f1 == value() || _y0.f1 == OPT::shortckt);
    assert(conchk(_m0.c1, 1./_y0.f1));
    assert(_m0.c0 == 0.);
    assert(_m1 == _m0);
    assert(_loss0 == 0.);
    assert(_loss1 == 0.);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_RESISTANCE::do_tr()
{
  if (using_tr_eval()) {
    _m0.x = tr_involts_limited();
    _y0.x = tr_input_limited();;
    tr_eval();
    assert(_y0.f0 != LINEAR);
    if (_y0.f1 == 0.) {
      error(bPICKY, long_label() + ": short circuit\n");
      _y0.f1 = OPT::shortckt;
      set_converged(conv_check());
    }else{
    }
    store_values();
    q_load();
    _m0.c1 = 1./_y0.f1;
    _m0.c0 = _y0.x - _y0.f0 / _y0.f1;
  }else{
    assert(_y0.f0 == LINEAR);
    assert(_y0.f1 == value() || _y0.f1 == OPT::shortckt);
    assert(conchk(_m0.c1, 1./_y0.f1));
    assert(_m0.c0 == 0.);
    assert(_y1 == _y0);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    if (_ev == 0.) {
      error(bPICKY, long_label() + ": short circuit\n");
      _ev = OPT::shortckt;
    }else{
    }
    _acg = 1. / _ev;
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == double(value()) || _ev == OPT::shortckt);
  }
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
