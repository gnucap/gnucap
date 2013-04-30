/*$Id: m_spline.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
 * piecewise polynomial interpolation
 */
//testing=script 2006.04.18
#include "m_cpoly.h"
#include "l_denoise.h"
#include "m_spline.h"
/*--------------------------------------------------------------------------*/
SPLINE::SPLINE(const std::vector<DPAIR>& table,
	       double d0, double dn, int order)
  :_n(static_cast<int>(table.size())-1),
   _x( new double[_n+1]),
   _f0(new double[_n+1]),
   _f1(0),
   _f2(0),
   _f3(0),
   _order(order)
{untested();
  if (_n < 0) {untested();
    throw Exception("no points in spline");
  }else{untested();
  }

  for (int i=0; i<=_n; ++i) {untested();
    _x[i]  = table[static_cast<unsigned>(i)].first;
    _f0[i] = table[static_cast<unsigned>(i)].second;
  }

  // set up h --------------------------------------------------
  double* h = new double[_n+1];
  for (int i=0; i<_n; ++i) {
    h[i] = _x[i+1] - _x[i];
    if (h[i] == 0.) {untested();
      throw Exception("duplicate points in spline: " 
		      + to_string(_x[i]) +  ", " + to_string(_x[i+1]));
    }else{untested();
    }
  }
  h[_n] = NOT_VALID;

  switch (_order) {
  case 3: construct_order_3(h, d0, dn); break;
  case 2: construct_order_2(h, d0, dn); break;
  case 1: construct_order_1(h, d0, dn); break;
  case 0: untested(); /* nothing to do */   break;
  default:
    untested();
    error(bDANGER, "illegal spline order (%d), must be 0, 1, 2, 3\n", _order);
    break;
  }
}
/*--------------------------------------------------------------------------*/
SPLINE::SPLINE(const std::vector<std::pair<PARAMETER<double>, PARAMETER<double> > >& table,
	       double d0, double dn, int order)
  :_n(static_cast<int>(table.size())-1),
   _x( new double[_n+1]),
   _f0(new double[_n+1]),
   _f1(0),
   _f2(0),
   _f3(0),
   _order(order)
{
  if (_n < 0) {
    throw Exception("no points in spline");
  }else{
  }

  for (int i=0; i<=_n; ++i) {
    _x[i]  = table[static_cast<unsigned>(i)].first;
    _f0[i] = table[static_cast<unsigned>(i)].second;
  }

  // set up h --------------------------------------------------
  double* h = new double[_n+1];
  for (int i=0; i<_n; ++i) {
    h[i] = _x[i+1] - _x[i];
    if (h[i] == 0.) {
      throw Exception("duplicate points in spline: " 
	    + to_string(_x[i]) +  ", " + to_string(_x[i+1]));
    }else{
    }
  }
  h[_n] = NOT_VALID;

  switch (_order) {
  case 3: construct_order_3(h, d0, dn); break;
  case 2: construct_order_2(h, d0, dn); break;
  case 1: construct_order_1(h, d0, dn); break;
  case 0: untested(); /* nothing to do */   break;
  default:
    untested();
    error(bDANGER, "illegal spline order (%d), must be 0, 1, 2, 3\n", _order);
    break;
  }
}
/*--------------------------------------------------------------------------*/
void SPLINE::construct_order_1(double* h, double d0, double dn)
{
  assert(_n >= 0);

  _f1 = h; // reuse storage
  for (int i=0; i<_n; ++i) {
    _f1[i] = (_f0[i+1] - _f0[i]) / h[i];
  }
  //h = 0;

  if (d0 == NOT_INPUT) {
    _d0 = _f1[0];
  }else{untested();
    _d0 = d0;
  }
  if (dn == NOT_INPUT) {
    _f1[_n] = _f1[_n-1];
  }else{untested();
    _f1[_n] = dn;
  }
}
/*--------------------------------------------------------------------------*/
void SPLINE::construct_order_2(double* h, double d0, double dn)
{
  assert(_n >= 0);

  _f1 = new double[_n+1];
  if (d0 != NOT_INPUT && dn == NOT_INPUT) {
    _d0 = _f1[0] = d0;
    for (int i=0; i<_n; ++i) {
      _f1[i+1] = 2*(_f0[i+1]-_f0[i])/h[i] - _f1[i];
    }
  }else{
    if (dn == NOT_INPUT) {
      // neither bc .. must guess
      _f1[_n] = (_f0[_n] - _f0[_n-1]) / h[_n-1];
    }else{
      _f1[_n] = dn;
    }
    for (int i=_n-1; i>=0; --i) {
      _f1[i] = 2*(_f0[i+1]-_f0[i])/h[i] - _f1[i+1];
    }
    if (d0 == NOT_INPUT) {
      _d0 = _f1[0];
    }else{
      // both bc ... discontinuous derivative at _x[0]
      _d0 = d0;
    }
  }

  // second derivative -- piecewise constant
  _f2 = h; // reuse storage
  for (int i=0; i<_n; ++i) {
    _f2[i] = .5 * (_f1[i+1] - _f1[i]) / h[i];
  }
  _f2[_n] = 0.;
  //h = 0;
}
/*--------------------------------------------------------------------------*/
void SPLINE::construct_order_3(double* h, double d0, double dn)
{
  assert(_n >= 0);

  // set up right side -----------------------------------------
  double* b = new double[_n+1]; // b as in Ax=b
  for (int i=1; i<_n; ++i) {
    double num = _f0[i+1]*h[i-1] -_f0[i]*(h[i]+h[i-1]) +_f0[i-1]*h[i];
    double den = h[i-1] * h[i];
    b[i] = 3 * num / den;
  }
  // boundary conditions
  if (d0 == NOT_INPUT) {
    b[0] = 0.;
  }else{
    b[0] = 3 * ((_f0[1]-_f0[0])/h[0] - d0);
  }
  if (dn == NOT_INPUT) {
    b[_n] = 0.;
  }else{
    b[_n] = 3 * (dn - (_f0[_n]-_f0[_n-1])/h[_n-1]);
  }

  // fill, LU decomp, fwd sub ----------------------------------
  double* u = new double[_n+1];
  double* z = b; // reuse storage.
  if (d0 == NOT_INPUT) {
    u[0] = 0.;
    z[0] = 0.;
  }else{
    u[0] = .5;	// h[0] / (2*h[0])
    z[0] = b[0] / (2*h[0]);
  }
  for (int i=1; i<_n; ++i) {
    double p = 2*(h[i]+h[i-1]) - h[i-1]*u[i-1]; // pivot
    u[i] = h[i] / p;
    z[i] = (b[i] - z[i-1]*h[i-1]) / p;
  }
  if (dn == NOT_INPUT) { // natural
    z[_n] = 0;
  }else{ // clamped
    double p = h[_n-1] * (2.-u[_n-1]);
    z[_n] = (b[_n] - z[_n-1]*h[_n-1]) / p;
  }
  u[_n] = NOT_VALID;
  //b = 0;
  
  // back sub --------------------------------------------------
  _f1 = u; // reuse storage.
  _f2 = z;
  _f3 = h;
  _f2[_n] = z[_n];
  _f3[_n] = 0.;
  for (int i=_n-1; i>=0; --i) {
    _f2[i] = z[i] - u[i]*_f2[i+1];
    _f1[i] = (_f0[i+1]-_f0[i])/h[i] - h[i]*(_f2[i+1]+2*_f2[i])/3;
    _f3[i] = (_f2[i+1]-_f2[i])/(3*h[i]);
    trace4("", i, _f1[i], _f2[i], _f3[i]);
  }

  _d0 = fixzero(_f1[0], _f1[1]);  //_f1[0];
  assert(d0 == NOT_INPUT  ||  _d0 == d0);

  // set up last slot for extrapolation above. -----------------
  if (dn == NOT_INPUT) { // natural
    _f1[_n] = _f1[_n-1] + (_x[_n]-_x[_n-1])*_f2[_n-1];
  }else{ // clamped
    _f1[_n] = dn;
  }
  _f2[_n] = 0.;
  _f3[_n] = 0.;
  //u = z = h = 0;
}
/*--------------------------------------------------------------------------*/
SPLINE::~SPLINE()
{
  delete [] _x;
  delete [] _f0;
  delete [] _f1;
  delete [] _f2;
  delete [] _f3;
}
/*--------------------------------------------------------------------------*/
FPOLY1 SPLINE::at(double x)const
{
  assert(_n >= 0);

  double* here = std::upper_bound(_x, _x+_n+1, x);

  if (here == _x) {	// x out of range, below
    if (_order == 0) {untested();
      return FPOLY1(x, _f0[0], 0.);
    }else{
      double dx = x - _x[0];
      double f0 = _f0[0] + dx*_d0;
      return FPOLY1(x, f0, _d0);
    }
  }else{		// x in range, or out of range above
    int i = static_cast<int>(here - _x - 1);
    double dx = x - _x[i];
    switch (_order) {
    case 3:{
      double f0 = _f0[i] + dx*(_f1[i] + dx*(_f2[i] + dx*_f3[i]));
      double f1 = _f1[i] + dx*(2*_f2[i] + 3*dx*_f3[i]);
      return FPOLY1(x, f0, f1);
    }
    case 2:{
      double f0 = _f0[i] + dx*(_f1[i] + dx*_f2[i]);
      double f1 = _f1[i] + dx*(2*_f2[i]);
      return FPOLY1(x, f0, f1);
    }
    case 1:{
      double f0 = _f0[i] + dx*_f1[i];
      double f1 = _f1[i];
      return FPOLY1(x, f0, f1);
    }
    case 0:
      untested();
      return FPOLY1(x, _f0[i], 0.);
    default:
      untested();
      assert(!"spline problem");
      return FPOLY1();
    }
    untested();
    //trace4("", x, _x[i], dx, i);
    //trace4("", _f0[i],   _f1[i],   _f2[i],   _f3[i]);
    //trace4("", _f0[i+1], _f1[i+1], _f2[i+1], _f3[i+1]);
    //trace2("", f0, f1);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
