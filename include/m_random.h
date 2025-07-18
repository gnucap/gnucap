/*                        -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
 * Author: Felix Salfelder
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
 * IEEE1364 random sampling adopted to C++ from Icarus (GPLv2+)
 */
#ifndef GNUCAP_RDIST_VAMS_H
#define GNUCAP_RDIST_VAMS_H
/*--------------------------------------------------------------------------*/
#include "md.h"
/*--------------------------------------------------------------------------*/
namespace rdist_vams {
/*--------------------------------------------------------------------------*/
inline double uniform( int32_t& seed, double start, double end )
{
  union u_s {
    float s;
    unsigned stemp;
  } u;
  double d = 0.00000011920928955078125;
  double a, b;
  if ((seed) == 0){
    seed = 259341593;
  }else{
  }

  if (start >= end) {
    start = 0.0;
    end = 2147483647.0;
  } else {
  }
  seed = 69069 * seed + 1;
  uint_32t newseed = seed;

  double c = 1.0 + (newseed >> 9) * 0.00000011920928955078125;
  c = c + c*d;
  c = ((end - start) * (c - 1.0)) + start;
  return c;
}
/*--------------------------------------------------------------------------*/
inline double normal(int32_t& seed, double mean, double deviation)
{
  double v1,v2;
  double s = 1.;
  while((s >= 1.0) || (s == 0.0)) {
    v1 = uniform(seed, -1., 1.);
    v2 = uniform(seed, -1., 1.);
    s = v1 * v1 + v2 * v2;
  }
  s = v1 * sqrt(-2.0 * log(s) / s);
  return(s * deviation + mean);
}
/*--------------------------------------------------------------------------*/
inline double exponential(int32_t& seed, double mean)
{
  double n = uniform(seed, 0., 1.);
  if(n != 0) {
    n = -log(n) * mean;
  }else{
  }
  return n;
}
/*--------------------------------------------------------------------------*/
inline double poisson(int32_t& seed, double mean)
{
  int n = 0;
  double p = exp(-mean);
  double q = uniform(seed,0.,1.);
  while(p < q) {
    n++;
    q = uniform(seed,0.,1.) * q;
  }
  return(n);
}
/*--------------------------------------------------------------------------*/
inline double chi_square(int32_t& seed, int deg_of_free)
{
  double x;
  long k;
  if(deg_of_free % 2) {
    x = normal(seed, 0, 1);
    x = x * x;
  } else {
    x = 0.0;
  }
  for(k = 2; k <= deg_of_free; k = k + 2) {
    x = x + 2 * exponential(seed,1);
  }
  return(x);
}
/*--------------------------------------------------------------------------*/
inline double t(int32_t& seed, int deg_of_free)
{
  double x;
  double chi2 = chi_square(seed, deg_of_free);
  double div = chi2 / (double)deg_of_free;
  double root = sqrt(div);
  x = normal(seed,0,1) / root;
  return(x);
}
/*--------------------------------------------------------------------------*/
inline double erlangian(int32_t& seed, double k, double mean)
{
  double x = 1.;
  for(int i=0; i<k; ++i) { untested();
    x = x * uniform(seed,0,1);
  }
  x = - mean*log(x)/k;
  return(x);
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
