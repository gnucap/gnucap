/*$Id: m_interp.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * interpolation
 */
#include "constant.h"
#include "m_interp.h"
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
  if (begin == end){
    untested();
    error(bERROR, "interpolate table is empty\n");
  }
  --end;
  {if (begin == end){ // only 1 entry -- constant
    untested();
    f1 = (x < (*begin).first)
      ? ((below != NOT_INPUT) ? below : 0.)
      : ((above != NOT_INPUT) ? above : 0.);
    f0 = (*begin).second + (x - (*begin).first) * f1;
  }else{
    ++begin;
    std::pair<double,double> xx(x,BIGBIG);
    Iterator upper = upper_bound(begin, end, xx);
    Iterator lower = upper-1;
    {if ((upper == end) && (x > (*upper).first) && (above != NOT_INPUT)){
      lower = upper;
      if (above != 0.) {untested();}
      f1 = above;
    }else if ((upper == begin) && (x < (*lower).first) && (below!=NOT_INPUT)){
      untested();
      if (below != 0.) {untested();}
      f1 = below;
    }else if ((*upper).first <= (*lower).first){
      error(bERROR, "interpolate table is not sorted or has duplicate keys\n");
      f1 = 0.;
    }else{
      assert((*upper).first != (*lower).first);
      f1 = ((*upper).second-(*lower).second) / ((*upper).first-(*lower).first);
    }}
    f0 = (*lower).second + (x - (*lower).first) * f1;
  }}
  assert(f1 != NOT_VALID);
  assert(f0 != NOT_VALID);
  return FPOLY1(x, f0, f1);
}
/*--------------------------------------------------------------------------*/
#if defined(MANUAL_TEMPLATES)
template 
FPOLY1 interpolate(std::deque<std::pair<double,double> >::const_iterator,
		   std::deque<std::pair<double,double> >::const_iterator,
		   double, double, double);	      
template 
FPOLY1 interpolate(std::vector<std::pair<double,double> >::const_iterator,
		   std::vector<std::pair<double,double> >::const_iterator,
		   double, double, double);	      
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
