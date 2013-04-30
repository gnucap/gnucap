/*$Id: l_denoise.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * functions to "de-noise" arithmetic
 * attempt to get rid of noise due to roundoff
 */
//testing=script,complete 2006.07.13
#ifndef L_DENOISE_H
#define L_DENOISE_H
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
/* fixzero: force numbers near zero to zero - changes thru pointer */
inline void fixzero(double *z, double r)
{
  assert(z);
  assert(*z == *z);
  assert(r == r);
  if (std::abs(*z) < std::abs(r*OPT::roundofftol)) {
    *z=0.;
  }
}
/*--------------------------------------------------------------------------*/
/* fixzero: force numbers near zero to zero - returns value */
inline double fixzero(double z, double r)
{
  assert(z == z);
  assert(r == r);
  return (std::abs(z) < std::abs(r*OPT::roundofftol)) ? 0. : z;
}
/*--------------------------------------------------------------------------*/
/* dn_diff: de-noised difference */
inline double dn_diff(double a, double b)
{
  assert(a == a);
  assert(b == b);
  return fixzero(a-b, a);
}
/*--------------------------------------------------------------------------*/
#if 0
/* dn_sum: de-noised sum */
inline double dn_sum(double a, double b)
{
  untested();
  assert(a == a);
  assert(b == b);
  return fixzero(a+b, a);
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
