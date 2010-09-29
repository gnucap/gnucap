/*$Id: d_cs.cc,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * functions for fixed current source
 * x = 0, y.f0 = nothing, ev = y.f1 = amps.
 */
#include "d_cs.h"
/*--------------------------------------------------------------------------*/
void DEV_CS::precalc()
{
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  _y0.x  = 0.;
  _y0.f0 = 0.;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.x  = 0.;
  _m0.c0 = _y0.f1;
  _m0.c1 = 0.;
  _m1 = _m0;
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_CS::dc_begin()
{
  if (!using_tr_eval()){
    assert(_loss0 == 0.);
    assert(_loss1 == 0.);
    assert(_y0.x  == 0.);
    assert(_y0.f0 == 0.);
    assert(_y0.f1 == value());
    assert(_m0.x  == 0.);
    assert(_m0.c0 == _y0.f1);
    assert(_m0.c1 == 0.);
    assert(_m1 == _m0);
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_CS::do_tr()
{
  assert(_m0.x == 0.);
  {if (using_tr_eval()){
    _y0.x = SIM::time0;
    tr_eval();
    store_values();
    q_load();
    _m0.c0 = _y0.f1;
    assert(_m0.c1 == 0.);
  }else{
    untested();
    assert(_y0.x  == 0.);
    assert(_y0.f0 == 0.);
    assert(_y0.f1 == value());
    assert(_m0.x  == 0.);
    assert(_m0.c0 == _y0.f1);
    assert(_m0.c1 == 0.);
    assert(_y1 == _y0);
    assert(converged());
  }}
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CS::do_ac()
{
  {if (has_ac_eval()){
    ac_eval();
    _acg = _ev;
  }else{
    assert(_acg == 0.);
  }}
  ac_load();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
