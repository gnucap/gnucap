/*$Id: m_spline.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * piecewise polynomial interpolation
 * Constructor args:
 * table : a vector of xy pairs, all of the data points.
 * d0, dn: boundary conditions -- value of the first derivative at boundaries.
 * order : order of interpolating polynomial, must be in {0, 1, 2, 3}.
 *
 * Outside the range, the result is linearly extrapolated with slope d0, dn.
 * ("clamped" splines)
 * 
 * If d0 or dn is set to NOT_INPUT, it is a "natural" spline, with the 
 * derivative being determined by the data.
 * See any numerical analysis text for explanation.
 */
#ifndef M_SPLINE_H
#define M_SPLINE_H
#include "m_cpoly.h"
/*--------------------------------------------------------------------------*/
class SPLINE {
private:
  int _n;
  double* _x;
  double* _f0;
  double* _f1;
  double* _f2;
  double* _f3;
  int	  _order;
  double  _d0;

  void construct_order_3(double* h, double d0, double dn);
  void construct_order_2(double* h, double d0, double dn);
  void construct_order_1(double* h, double d0, double dn);
public:
  SPLINE(const std::vector<std::pair<double,double> >& table,
	 double d0, double dn, int order);
  ~SPLINE();
  FPOLY1 at(double x)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
