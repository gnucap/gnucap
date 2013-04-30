/*$Id: m_fft.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * fast fourier transform
 */
//testing=script 2006.07.13
#include "constant.h"
#include "declare.h"	/* self */
/*--------------------------------------------------------------------------*/
void fft(COMPLEX* x, int n, int inv)
{
  int s = (inv) ? 1 : -1;
  int nxp, nxp2;
  for (nxp=n;  (nxp2=nxp/2) > 0;  nxp=nxp2) {
    double wpwr = M_TWO_PI / nxp;
    for (int m = 0;  m < nxp2;  ++m) {
      double argg = m * wpwr;
      COMPLEX w(cos(argg), s*sin(argg));
      for (int jj1 = m;  jj1+nxp-m <= n;  jj1 += nxp) {
	int jj2 = jj1 + nxp2;
	COMPLEX t = x[jj1] - x[jj2];
	x[jj1] += x[jj2];
	x[jj2] = t * w;
      }
    }
  }
  /* unscramble */
  {
    int i, j;
    for (/*k =*/ i = j = 0;  i < n-1;  ++i) {
      if (i < j) {
	swap(x[i],x[j]);
      }
      int k;
      for (k = n/2;  k <= j;  k /= 2) {
	j -= k;
      }
      j += k;
    }
  }
  /* fix level */
  if (!inv) {
    for (int i = 0;  i < n;  ++i) {
      x[i] /= n;
    }
  }else{
    untested();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
