/*$Id: e_aux.cc,v 22.7 2002/07/23 04:23:34 al Exp $ -*- C++ -*-
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
#ifndef E_AUX_CC
#define E_AUX_CC
#include "e_aux.h"
/*--------------------------------------------------------------------------*/
template <class T>
T port_impedance(const node_t& n1, const node_t& n2,
		 BSMATRIX<T>& mat, const T& parallel)
{
  T* zapit = new T[mat.size()+2];

  for (int ii = 0;  ii < mat.size()+2;  ++ii){
    zapit[ii] = 0.;
  }
  if (n1.m_() != 0)
    zapit[n1.m_()] =  1.;
  if (n2.m_() != 0)
    zapit[n2.m_()] = -1.;

  mat.fbsub(zapit);
  T raw_z = zapit[n1.m_()] - zapit[n2.m_()];
  delete [] zapit;
  return (parallel != 0.) 
    ? 1. / ((1./raw_z)-parallel)
    : raw_z;
}
/*--------------------------------------------------------------------------*/
#if defined(MANUAL_TEMPLATES)
  template double
  port_impedance(const node_t& a, const node_t& b,
		 BSMATRIX<double> &c, const double &d);
  template COMPLEX
  port_impedance(const node_t& a, const node_t& b,
		 BSMATRIX<COMPLEX> &c,const COMPLEX &d);
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
