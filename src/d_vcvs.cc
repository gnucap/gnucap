/*$Id: d_vcvs.cc,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * functions for vcvs
 * temporary kluge: it has resistance
 */
#include "d_vcvs.h"
/*--------------------------------------------------------------------------*/
void DEV_VCVS::precalc()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  _y0.f0 = LINEAR;
  _y0.f1 = value();
  _y1 = _y0;
  _m0.c1 = -_loss0 * _y0.f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::dc_begin()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  if (!using_tr_eval()){
    assert(_y0.f0 == LINEAR);
    assert(_y0.f1 == value());
    _m0.c1 = -_loss0 * _y0.f1;
    assert(_m0.c0 == 0.);
    assert(_m1 == _m0);
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_VCVS::do_tr()
{
  {if (using_tr_eval()){
    _m0.x = tr_involts_limited();
    _y0.x = _m0.x;
    tr_eval();
    assert(_y0.f0 != LINEAR);
    store_values();
    q_load();
    _m0 = CPOLY1(_y0);
    _m0 *= -_loss0;
  }else{
    assert(conchk(_loss0, 1./OPT::shortckt));
    assert(_y0.f0 == LINEAR);
    assert(_y0.f1 == value());
    assert(conchk(_m0.c1, -_loss0 * _y0.f1));
    assert(_m0.c0 == 0.);
    assert(_y1 == _y0);
    assert(converged());
  }}
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::ac_begin()
{
  _loss1 = _loss0 = 1./OPT::shortckt;
  _ev = _y0.f1;
  _acg = -_loss0 * _ev;
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::do_ac()
{
  {if (has_ac_eval()){
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{
    assert(_ev == _y0.f1);
    assert(has_tr_eval() || _ev == value());
  }}
  ac_load(); 
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
