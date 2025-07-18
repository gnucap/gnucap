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
 * random sampling
 */
/*--------------------------------------------------------------------------*/
#include "l_indirect.h"
#include "u_opt.h"
#include "m_random.h"
/*--------------------------------------------------------------------------*/
INDIRECT<int32_t, int32_t> random_seeds;
/*--------------------------------------------------------------------------*/
int32_t& random_seed(int32_t slot)
{
  int32_t& s = random_seeds[slot];
  if(!s) {
    s = slot + OPT::rndseed;
  }else{
  }
  return s;
}
/*--------------------------------------------------------------------------*/
// .. from IEE1364, adopted to C++ from Icarus (GPLv2+)
namespace rdist {
/*--------------------------------------------------------------------------*/
double uniform( int32_t& seed, double start, double end )
{
  double d = 0.00000011920928955078125;
  if ((seed) == 0){
    seed = 259341593;
  }else{
  }

  if (start >= end) { untested();
    start = 0.0;
    end = 2147483647.0;
  } else {
  }
  seed = 69069 * seed + 1;
  u_int32_t newseed = seed;

  double c = 1.0 + (newseed >> 9) * 0.00000011920928955078125;
  c = c + c*d;
  c = ((end - start) * (c - 1.)) + start;
  return c;
}
/*--------------------------------------------------------------------------*/
double normal(int32_t& seed, double mean, double deviation)
{
  double v1,v2;
  double s = 1.;
  while((s >= 1.) || (s == 0.)) {
    v1 = uniform(seed, -1., 1.);
    v2 = uniform(seed, -1., 1.);
    s = v1 * v1 + v2 * v2;
  }
  s = v1 * sqrt(-2.0 * log(s) / s);
  return(s * deviation + mean);
}
/*--------------------------------------------------------------------------*/
double exponential(int32_t& seed, double mean)
{
  double n = uniform(seed, 0., 1.);
  if(n != 0) {
    n = -log(n) * mean;
  }else{ untested();
  }
  return n;
}
/*--------------------------------------------------------------------------*/
double poisson(int32_t& seed, double mean)
{
  int n = 0;
  double p = exp(-mean);
  double q = uniform(seed, 0., 1.);
  while(p < q) { untested();
    n++;
    q = uniform(seed, 0., 1.) * q;
  }
  return(n);
}
/*--------------------------------------------------------------------------*/
double chi_square(int32_t& seed, int deg_of_free)
{
  double x;
  int k;
  if(deg_of_free % 2) {
    x = normal(seed, 0, 1);
    x = x * x;
  } else { untested();
    x = 0.0;
  }
  for(k = 2; k <= deg_of_free; k = k + 2) { untested();
    x = x + 2 * exponential(seed, 1.);
  }
  return(x);
}
/*--------------------------------------------------------------------------*/
double t(int32_t& seed, int deg_of_free)
{
  double x;
  double chi2 = chi_square(seed, deg_of_free);
  double div = chi2 / (double)deg_of_free;
  double root = sqrt(div);
  x = normal(seed, 0., 1.) / root;
  return(x);
}
/*--------------------------------------------------------------------------*/
double erlangian(int32_t& seed, double k, double mean)
{
  double x = 1.;
  for(int i=0; i<k; ++i) {
    x = x * uniform(seed,0,1);
  }
  x = - mean*log(x)/k;
  return(x);
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
