/*$Id: l_compar.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 */
#ifndef L_COMPAR_H
#define L_COMPAR_H
#include "md.h"
/*--------------------------------------------------------------------------*/
/* uporder: returns true if a,b,c are in non-decreasing order */
template <class T>
inline bool up_order(T a, T b, T c)
   {return (a<=b) && (b<=c);}

/* inorder: returns true if b is between a and c */
template <class T>
inline bool in_order(T a, T b, T c)
   {return up_order(a,b,c) || up_order(c,b,a);}

/* torange: returns b, clipped to range a..c */
template <class T>
inline T to_range(T a, T b, T c)
   {return std::min(std::max(a,b),c);}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
