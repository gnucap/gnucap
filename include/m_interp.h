/*$Id: m_interp.h,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * interpolation on a sorted array
 *
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
 */
//testing=script,sparse 2006.07.13
#ifndef M_INTERP_H
#define M_INTERP_H
#include "m_cpoly.h"
/*--------------------------------------------------------------------------*/
/* interpolate:  linear interpolation on a table.
 * Keys must be sorted in increasing order.
 */
template <class Iterator>
FPOLY1 interpolate(Iterator begin, Iterator end, double x,
		   double below, double above, int order=1)
{
  double f1 = NOT_VALID;
  double f0 = NOT_VALID;
  if (begin == end) { untested();
    throw Exception("interpolate table is empty");
  }else{
    // x is the search key
    // xx is the search key converted to a "pair" as required by upper_bound
    // upper might point to a match for the key, exact match
    // if not, it points just above it, no exact match
    // lower points just below where a match would go
    // so the real match is between upper and lower
  }
  DPAIR    xx(x,BIGBIG);
  Iterator back = end;
  --back;
  if (begin == back) { // only 1 entry -- constant
    order = 0;
    if (x < begin->first) { untested();
      f1 = ((below != NOT_INPUT) ? below : 0.);
    }else{
      f1 = ((above != NOT_INPUT) ? above : 0.);
    }
    f0 = begin->second + (x - begin->first) * f1;
  }else if ((order == 1) && (x < begin->first || back->first < x)) {
    // below and above don't work with Neville
    Iterator lower = begin;
    if (x < begin->first){
      // x is out of bounds, below
      if (below != NOT_INPUT) { untested();
	f1 = below;
      }else{
	Iterator upper = begin;
	++upper;
	f1 = (upper->second - lower->second) / (upper->first - lower->first);
      }
      if (below != 0.) {
      }else{ untested();
      }
      lower = begin;
    }else if (x > back->first){
      // x is out of bounds, above
      Iterator upper = lower = back;
      if(above != NOT_INPUT) {
	f1 = above;
      }else{
	--lower;
	f1 = (upper->second - lower->second) / (upper->first - lower->first);
      }
      if (above != 0.) {
      }else{
      }
    }else{ untested();
      unreachable();
    }
    f0 = lower->second + (x - lower->first) * f1;
  }else{
    Iterator upper = upper_bound(begin, end, xx);
    Iterator orig_begin = begin;
    Iterator orig_begin2 = begin;
    ++orig_begin2;
    ++begin;
    if(end == upper){
      // extrapolating.
      --upper;
    }else{
    }
    Iterator seek = upper;

    class table {
      // double _data[OPT::_keep_time_steps][OPT::_keep_time_steps];
      double _data[OPT::_keep_time_steps];

    public:
      double& operator()(int i, int j) {
	assert(i>=j);
	assert(i-j < OPT::_keep_time_steps);
	return _data[i-j];
      }
    };
    table Y, D;
    double X[OPT::_keep_time_steps];

    X[0] = seek->first;
    Y(0, 0) = seek->second;
    D(0, 0) = 0.;

    for (int i = 1; i <= order; ++i) {
      if(seek == orig_begin){ untested();
	order = i-1;
	break;
      }else{
	--seek;
      }

      // i forward or backward?
      int n = i;
      X[n] = seek->first;
      Y(i, 0) = seek->second;
      D(i, 0) = 0.;

      if(X[i-1] <= X[i]){ untested();
	throw Exception("interpolate table is not sorted or has duplicate keys");
      }else{
      }

      for (int j = 1; j <= i; ++j) {
	double denom = X[i] - X[i-j];
	D(i, j) = ( Y(i,  j-1) + (x - X[i-j]) * D(i,   j-1)
	         -  Y(i-1,j-1) - (x - X[i  ]) * D(i-1, j-1)) / denom;

	Y(i, j) = ((x - X[i-j]) * Y(i,   j-1)
	         - (x - X[i  ]) * Y(i-1, j-1)) / denom;
      }
    }
    f0 = Y(order, order);
    f1 = D(order, order);
  }
  assert(f1 != NOT_VALID);
  assert(f0 != NOT_VALID);
  return FPOLY1(x, f0, f1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
