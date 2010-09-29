/*$Id: m_interp.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
 * interpolation on a sorted array
 *
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
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
 */
#ifndef M_INTERP_H
#define M_INTERP_H
#include "m_cpoly.h"
/*--------------------------------------------------------------------------*/
template <class Iterator>
FPOLY1 interpolate(Iterator begin, Iterator end, double x, 
		   double below, double above);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
