/*$Id: u_sdp.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * A class for Size Dependent Parameters, like those used in Spice BSIM models
 * A single read gets the nominal, and length and width dependencies.
 * A single print prints them all.
 * operator() returns the value to use, adjusted for L and W.
 */
//testing=script 2006.07.14
#ifndef U_SDP_H
#define U_SDP_H
#include "ap.h"
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
class OMSTREAM;
class CARD_LIST;
/*--------------------------------------------------------------------------*/
class SDP {
  friend bool get(CS& cmd, const std::string& key, SDP* value);
private:
  PARAMETER<double> _nom;	// nominal value
  PARAMETER<double> _ld;	// length dependency
  PARAMETER<double> _wd;	// width dependency
  PARAMETER<double> _pd;	// cross-term dependency (p is for product)

  explicit SDP() {unreachable();}
  explicit SDP(const SDP&) {unreachable();}
public:
  ~SDP() {}
  explicit SDP(double nom) :_nom(nom), _ld(0.), _wd(0.), _pd(0.) {}
  void print(OMSTREAM& o, const std::string& name)const;
  double operator()(double L,double W,double def,const CARD_LIST* scope)const {
    return _nom.e_val(def,scope) + _ld.e_val(0.,scope)/L
      + _wd.e_val(0.,scope)/W + _pd.e_val(0.,scope)/(W*L);
  }
  double nom()const {return _nom;}
  void set_nom(double n) {untested();_nom = n;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
