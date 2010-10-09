/*$Id: m_divdiff.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * divided differences
 * in:
 *   c = numerator (data points) (lost)
 *   t = denominator (time)
 *   n = size of array
 *   # of divided differences == n-1
 * out:
 *   c = "divided differences" (mult by n! to get derivative)
 */
//testing=script,complete 2006.07.13
/*--------------------------------------------------------------------------*/
#if 0
template<class T1, class T2>
inline void divided_differences(T1 c[], int n, const T2 t[])
{
  untested();
  for (int d=1; d<n; ++d) {
    untested();
    for (int i=n-1; i>=d; --i) {
      untested();
      c[i] = (c[i-1] - c[i]) / (t[i-d] - t[i]);
    }
  }
}
#endif
/*--------------------------------------------------------------------------*/
template<class T1, class T2>
inline void derivatives(T1 c[], int n, const T2 t[])
{
  for (int d=1; d<n; ++d) {
    for (int i=n-1; i>=d; --i) {
      c[i] = d * (c[i-1] - c[i]) / (t[i-d] - t[i]);
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
