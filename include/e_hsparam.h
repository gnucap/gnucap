/*                  $ -*- C++ -*-
 * Copyright (C) 2024, 2025 Felix Salfelder
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
 */
#include "e_compon.h" // BUG
#include "e_cardlist.h" // BUG
class HS_PARAM {
  std::array<PARAMETER<double>, sysparams_count> _p;
  explicit HS_PARAM(HS_PARAM const& p) : _p(p._p) {}
public:
  explicit HS_PARAM(){}
  HS_PARAM* clone() const{ return new HS_PARAM(*this); }

  void set_by_index(int i, std::string const& v){
    _p[i] = v;
  }
  bool is_printable(int i)const {
    return _p[i].has_hard_value();
  }
  std::string const param_value(int i) const {
    return _p[i].string();
  }
  double mfactor() const{
    return _p[0];
  }
  void set_mfactor(double x) {itested();
    _p[0] = x;
  }
  void precalc(CARD_LIST const* scope){
    _p[0].e_val(1., scope->params());
    for(int i=1; i< sysparams_count; ++i){
      _p[i].e_val(0, scope->params());
    }
  }

};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
