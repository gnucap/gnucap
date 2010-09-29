/*$Id: e_aux.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * helper functions, etc., that sort of belong to circuit elements
 */
#ifndef E_AUX_H
#define E_AUX_H
#include "e_node.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
class node_t;
/*--------------------------------------------------------------------------*/
#if defined(COMPILE_TEMPLATES)
  #error untested
  #include "e_aux.cc"
#else
  template <class T>
  T port_impedance(const node_t& n1, const node_t& n2,
		   BSMATRIX<T>& mat, const T& parallel);
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
