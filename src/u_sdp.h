/*$Id: u_sdp.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * A class for Size Dependent Parameters, like those used in Spice BSIM models
 * A single read gets the nominal, and length and width dependencies.
 * A single print prints them all.
 * operator() returns the value to use, adjusted for L and W.
 */
#ifndef U_SDP_H
#define U_SDP_H
#include "ap.h"
/*--------------------------------------------------------------------------*/
class OMSTREAM;
/*--------------------------------------------------------------------------*/
class SDP {
  friend CS& get(CS& cmd, const std::string& key, SDP* value,
		 AP_MOD mod=mNONE, double scale=0);
private:
  double _nom;	// nominal value
  double _ld;	// length dependency
  double _wd;	// width dependency
  double _pd;	// cross-term dependency (p is for product)
public:
  explicit SDP(double nom) :_nom(nom), _ld(0.), _wd(0.), _pd(0.) {}
  void print(OMSTREAM& o, const std::string& name)const;
  double operator()(double L, double W)const {
    return _nom + _ld/L + _wd/W + _pd/(W*L);}
  double nom()const {return _nom;}
  void set_nom(double n) {_nom = n;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
