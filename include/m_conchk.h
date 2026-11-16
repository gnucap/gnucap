/*$Id: e_compon.h  $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2025 Felix Salfelder
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
 * convergence check
 */
#ifndef M_CONCHK_H
#define M_CONCHK_H
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
inline bool conchk(double o, double n,
		   double a=OPT::abstol, double r=OPT::reltol)
{
  double d = r * std::abs(n) + a;
  // spice double d = r * (std::max(std::abs(n), std::abs(o))) + a;
  return std::abs(n-o) <= d;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
