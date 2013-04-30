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
		   double below, double above)
{
  double f1 = NOT_VALID;
  double f0 = NOT_VALID;
  if (begin == end) {
    untested();
    throw Exception("interpolate table is empty");
  }
  --end;
  if (begin == end) { // only 1 entry -- constant
    untested();
    f1 = (x < (*begin).first)
      ? ((below != NOT_INPUT) ? below : 0.)
      : ((above != NOT_INPUT) ? above : 0.);
    f0 = (*begin).second + (x - (*begin).first) * f1;
  }else{
    ++begin;

    // x is the search key
    // xx is the search key converted to a "pair" as required by upper_bound
    // upper might point to a match for the key, exact match
    // if not, it points just above it, no exact match
    // lower points just below where a match would go
    // so the real match is between upper and lower
    DPAIR    xx(x,BIGBIG);
    Iterator upper = upper_bound(begin, end, xx);
    Iterator lower = upper-1;

    // set f1 (derivative)
    if ((upper == end) && (x > (*upper).first) && (above != NOT_INPUT)) {
      // x is out of bounds, above
      lower = upper;
      if (above != 0.) {
	untested();
      }
      f1 = above;
    }else if ((upper == begin) && (x < (*lower).first) && (below!=NOT_INPUT)) {
      // x is out of bounds, below
      untested();
      if (below != 0.) {
	untested();
      }else{
	untested();
      }
      f1 = below;
    }else if ((*upper).first <= (*lower).first) {untested();
      throw Exception("interpolate table is not sorted or has duplicate keys");
      f1 = 0.;
    }else{
      // ordinary interpolation
      assert((*upper).first != (*lower).first);
      f1 = ((*upper).second-(*lower).second) / ((*upper).first-(*lower).first);
    }

    f0 = (*lower).second + (x - (*lower).first) * f1;
  }
  assert(f1 != NOT_VALID);
  assert(f0 != NOT_VALID);
  return FPOLY1(x, f0, f1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
