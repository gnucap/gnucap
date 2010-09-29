/*$Id: d_vs.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * functions for fixed voltage sources
 * temporary kluge: it has resistance
 */
//testing=script 2006.07.17
#include "d_vs.h"
/*--------------------------------------------------------------------------*/
void DEV_VS::precalc()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  _y0.x  = 0.;
  _y0.f0 = 0.;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.x  = 0.;
  _m0.c0 = -_loss0 * _y0.f1;
  _m0.c1 = 0.;
  _m1 = _m0;
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VS::dc_begin()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  if (!using_tr_eval()) {
    if (_n[OUT2].m_() == 0) {
      set_limit(value());
    }else if (_n[OUT1].m_() == 0) {untested();
      set_limit(-value());
    }else{untested();
      // BUG: don't set limit
    }
    assert(_y0.x == 0.);
    assert(_y0.f0 == 0.);
    assert(_y0.f1 == value());
    assert(_m1 == _m0);
    assert(_m0.x == 0.);
    assert(_m0.c1 == 0.);
    _m0.c0 = -_loss0 * _y0.f1;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_VS::do_tr()
{
  assert(_m0.x == 0.);
  if (using_tr_eval()) {
    _y0.x = SIM::time0;
    tr_eval();
    if (_n[OUT2].m_() == 0) {
      set_limit(_y0.f1);
    }else if (_n[OUT1].m_() == 0) {
      set_limit(-_y0.f1);
    }else{
      // BUG: don't set limit
    }
    store_values();
    q_load();
    _m0.c0 = -_loss0 * _y0.f1;
    assert(_m0.c1 == 0.);
  }else{untested();
    assert(conchk(_loss0, 1./OPT::shortckt));
    assert(_y0.x == 0.);
    assert(_y0.f0 == 0.);
    assert(_y0.f1 == value());
    assert(_m0.x == 0.);
    assert(conchk(_m0.c0, -_loss0 * _y0.f1));
    assert(_m0.c1 == 0.);
    assert(_y1 == _y0);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VS::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{untested();
    assert(_acg == 0.);
  }
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
