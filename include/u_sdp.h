/*$Id: u_sdp.h,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
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
 * A class for Size Dependent Parameters, like those used in Spice BSIM models
 * A single read gets the nominal, and length and width dependencies.
 * A single print prints them all.
 * operator() returns the value to use, adjusted for L and W.
 */
//testing=script 2006.07.14
#ifndef U_SDP_H
#define U_SDP_H
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
class OMSTREAM;
class CARD_LIST;
/*--------------------------------------------------------------------------*/
class SDP {
  //friend bool get(CS& cmd, const std::string& key, SDP* value);
private:
  PARAMETER<double> _nom;	// nominal value
  PARAMETER<double> _ld;	// length dependency
  PARAMETER<double> _wd;	// width dependency
  PARAMETER<double> _pd;	// cross-term dependency (p is for product)

  explicit SDP() {unreachable();}
public:
  explicit SDP(const SDP& p) :_nom(p._nom),_ld(p._ld),_wd(p._wd),_pd(p._pd) {}
  ~SDP() {}
  explicit SDP(double Nom) :_nom(Nom), _ld(0.), _wd(0.), _pd(0.) {}
  //void print(OMSTREAM& o, LANGUAGE*, const std::string& name)const;
  double operator()(double L,double W,double def,const CARD_LIST* scope)const {
    return _nom.e_val(def,scope) + _ld.e_val(0.,scope)/L
      + _wd.e_val(0.,scope)/W + _pd.e_val(0.,scope)/(W*L);
  }
  double nom()const {return _nom;}
  void set_nom(double n) {untested();_nom = n;}
  void set_w(double n) {untested();_wd = n;}
  void set_l(double n) {untested();_ld = n;}
  void set_p(double n) {untested();_pd = n;}
  void set_nom(const std::string& n) {_nom = n;}
  void set_w(const std::string& n) {_wd = n;}
  void set_l(const std::string& n) {_ld = n;}
  void set_p(const std::string& n) {itested();_pd = n;}

  bool has_hard_value()const {return _nom.has_hard_value();}
  bool has_good_value()const {return _nom.has_good_value();}

  bool has_value()const   {return _nom.has_hard_value();}
  bool w_has_value()const {return _wd.has_hard_value();}
  bool l_has_value()const {return _ld.has_hard_value();}
  bool p_has_value()const {return _pd.has_hard_value();}

  std::string string()const {return _nom.string();}
  std::string w_string()const {return _wd.string();}
  std::string l_string()const {return _ld.string();}
  std::string p_string()const {return _pd.string();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
