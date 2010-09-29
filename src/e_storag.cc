/*$Id: e_storag.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * Base class for storage elements of a circuit
 */
#include "ap.h"
#include "m_divdiff.h"
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
/* table for selecting local integraton method
 * Determines which one wins in a conflict.
 * "only" wins over non-only.  local (_method_u) wins over opt.
 */
//                     OPT::method    _method_u
METHOD STORAGE::method_select[meNUM_METHODS][meNUM_METHODS] = {
  /*vv OPT vv*/
  //local>>>EULER,EULERONLY,TRAP,TRAPONLY,GEAR2,GEAR2ONLY,TRAPGEAR,TRAPEULER
  /*meUNKNOWN*/
  {mTRAPGEAR,mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meEULER*/
  {mEULER,   mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meEULERONLY*/
  {mEULER,   mEULER,mEULER,mEULER,mTRAP,mEULER,mGEAR,mEULER,   mEULER},
  /*meTRAP*/
  {mTRAP,    mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meTRAPONLY*/
  {mTRAP,    mTRAP, mEULER,mTRAP, mTRAP,mTRAP, mGEAR,mTRAP,    mTRAP},
  /*meGEAR*/
  {mGEAR,    mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meGEAR2ONLY*/
  {mGEAR,    mGEAR, mEULER,mGEAR, mTRAP,mGEAR, mGEAR,mGEAR,    mGEAR},
  /*meTRAPGEAR*/
  {mTRAPGEAR,mEULER,mEULER,mTRAP, mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER},
  /*meTRAPEULER*/
  {mTRAPEULER,mEULER,mEULER,mTRAP,mTRAP,mGEAR, mGEAR,mTRAPGEAR,mTRAPEULER}
};
/*--------------------------------------------------------------------------*/
void STORAGE::precalc()
{
  _method_a = method_select[OPT::method][_method_u];
  _y0.f0 = LINEAR;
  _y0.f1 = value();
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  set_converged();
  assert(!constant()); /* because of integration */
  /* m0 and acg are frequency/time dependent and cannot be set here.
   * If this is a coupled inductor, there is a subckt, which is expanded
   * by the mutual pseudo-element.
   * Assigning the values here becomes unnecessary, but harmless.
   */
}
/*--------------------------------------------------------------------------*/
void STORAGE::dc_begin()
{
  _method_a = method_select[OPT::method][_method_u];
  if (!has_tr_eval()){
    //assert(_y0.f0 == LINEAR); f0 == f1 * x
    assert(_y0.f1 == value());
    assert(_loss0 == 0.);
    assert(_loss1 == 0.);
    assert(converged());
    assert(!constant());
  }
}
/*--------------------------------------------------------------------------*/
void STORAGE::dc_advance()
{
  _it1 = _i0;
  assert(SIM::time0 == 0.);
  assert(_keep_time_steps == 4);
  _time[0] = _time[1] = _time[2] = _time[3] = 0.;
  _dt = NOT_VALID;
  _c_mult = NOT_VALID;
}
/*--------------------------------------------------------------------------*/
void STORAGE::tr_advance()
{
  {if (_time[0] < SIM::time0){		// forward
    assert(_keep_time_steps == 4);
    _it1 = _i0;
    for (int i=_keep_time_steps-1; i>0; --i){
      _time[i] = _time[i-1];
      _q[i] = _q[i-1];
    }
  }else{				// else backward, don't save
    assert(_time[1] < SIM::time0);
  }}
  _time[0] = SIM::time0;
  _dt = _time[0] - _time[1];
  _c_mult = c_mult_num() / _dt;
}
/*--------------------------------------------------------------------------*/
double STORAGE::integrate()
{
  {if (SIM::mode == sDC  ||  SIM::mode == sOP) {
    return 0.;
  }else{
    assert(SIM::mode == sTRAN || SIM::mode == sFOURIER);
    {if (SIM::phase == SIM::pINIT_DC) {
      {if (_time[0] == 0.) {
	{if (SIM::uic && initial_condition != NOT_INPUT) {
	  set_limit(initial_condition);
	  return (_m0.x - initial_condition) / OPT::shortckt;
	}else{
	  return 0.;
	}}
      }else{
	assert(_time[0] > 0);
	/* leave it alone to restart from a previous solution */
	return _m0.c0 + _m0.x * _m0.c1; // BUG:: why is this needed??
      }}
    }else{
      assert(SIM::phase == SIM::pTRAN);
      switch (_method_a) {
      case mGEAR:
      case mTRAPGEAR:
      case mTRAPEULER:
      case mEULER:		// first order (stiff) (backward Euler)
	return (_q[0].f0 - _q[1].f0) * c_mult();
      case mTRAP:		// second order (non-stiff) (trapezoid)
	return (_q[0].f0 - _q[1].f0) * c_mult() - _it1.f0;
      }
      unreachable();
      return NOT_VALID;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
double STORAGE::tr_c_to_g(double c, double g)const
{
  {if (SIM::mode == sDC  ||  SIM::mode == sOP) {
    return 0.;
  }else{
    {if (SIM::phase == SIM::pINIT_DC) {
      {if (_time[0] == 0.) {
	{if (SIM::uic && initial_condition != NOT_INPUT) {
	  return 1./OPT::shortckt;
	}else{
	  return 0.;
	}}
      }else{
	return g;
	// no change, fake
      }}
    }else{
      assert(SIM::phase == SIM::pTRAN);
      return c * c_mult();
    }}
  }}
}
/*--------------------------------------------------------------------------*/
double STORAGE::review(double i0, double it1)const
{
  assert(order() <= _max_order);
  {if (_time[order()+1] <= 0. || _method_a == mEULER){
    return NEVER;
  }else{
    double c[_keep_time_steps];
    assert(_keep_time_steps == 4);
    {for (int i=0; i<_keep_time_steps; ++i) {
      c[i] = _q[i].f0;
    }}
    divided_differences(c, order()+2, _time);
    // now c[i] is i'th derivative (up to order-1)

    {if (c[order()+1] == 0){
      return NEVER;
    }else{
      double currenttol= OPT::abstol
	+ OPT::reltol * std::max(std::abs(i0), std::abs(it1));
      double chargetol = std::max(std::max(std::abs(_q[0].f0),
					   std::abs(_q[1].f0)),
			     OPT::chgtol) * OPT::reltol / _dt;
      double tol = OPT::trtol * std::max(currenttol,chargetol);
      double denom = error_factor() * std::abs(c[order()+1]);
      double timestep = pow((tol / denom), 1./order());
      
      if (timestep <= SIM::dtmin){
	untested();
	error(bDANGER,"step control error:%s %g\n",
	      long_label().c_str(),timestep);
	error(bTRACE, "q0=%g i0=%g dq0=%g\n", _q[0].f0, i0, c[1]);
	error(bTRACE, "it=%g qt=%g tol=%g\n", currenttol, chargetol, tol);
	timestep = SIM::dtmin;
      }
      {if (timestep < _dt * OPT::trreject){
	error(bTRACE, "step rejected:" + long_label() + '\n');
	error(bTRACE, "new=%g  old=%g  rej=%g\n",
	      timestep, _dt, _dt * OPT::trreject);
	return _time[1] + timestep;
      }else{
	return _time[0] + timestep;
      }}
    }}
  }}
}
/*--------------------------------------------------------------------------*/
double STORAGE::tr_probe_num(CS& cmd)const
{
  {if (cmd.pmatch("DT")) {
    untested();
    return _dt;
  }else if (cmd.pmatch("TIME")) {
    untested();
    return _time[0];
  }else if (cmd.pmatch("TIMEOld")) {
    untested();
    return _time[1];
  }else{
    return ELEMENT::tr_probe_num(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
