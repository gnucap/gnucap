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
 * hierarchical system parameters
 */
/*--------------------------------------------------------------------------*/
#ifndef HS_PARAM_H
#define HS_PARAM_H
/*--------------------------------------------------------------------------*/
#include "u_parameter.h"
#include "io_trace.h"
#include "e_paramlist.h"
/*--------------------------------------------------------------------------*/
static const int sysparams_count = 8;
/*--------------------------------------------------------------------------*/
class HS_PARAM : public COMMON_PARAMLIST {
  PARAMETER<double> _mfactor;
  PARAMETER<double> _xposition;
  PARAMETER<double> _yposition;
  PARAMETER<double> _zposition;
  PARAMETER<double> _hflip;
  PARAMETER<double> _vflip;
  PARAMETER<double> _zflip;
  PARAMETER<double> _angle;

private: // fixed values. combining hierarchical and specified.
  double _mfactor_fixed{1.};

  explicit HS_PARAM(HS_PARAM const& p) : COMMON_PARAMLIST(p),
    _mfactor(p._mfactor),
    _xposition(p._xposition),
    _yposition(p._yposition),
    _zposition(p._zposition),
    _hflip(p._hflip),
    _vflip(p._vflip),
    _zflip(p._zflip),
    _angle(p._angle),
    _mfactor_fixed(p._mfactor_fixed){
      params()->set("$mfactor_hier", "1.");
    }
public:
  explicit HS_PARAM() : COMMON_PARAMLIST() {
    _mfactor.set_default(1.);
    _xposition.set_default(0.);
    _yposition.set_default(0.);
    _zposition.set_default(0.);
    _hflip.set_default(1);
    _vflip.set_default(1);
    _zflip.set_default(1);
    _angle.set_default(0.);
    params()->set("$mfactor_hier", "1.");
  }
  ~HS_PARAM() {}
  bool operator==(const COMMON_COMPONENT& x)const override;
  HS_PARAM* clone()const override { return new HS_PARAM(*this); }

  bool param_is_printable(int i)const override;
  void set_param_by_index(int i, std::string& v, int)override;
  int set_param_by_name(std::string Name, std::string Value) override;
  void set_mfactor(double x)override { itested(); _mfactor = x; }
  int param_count()const override {
    return sysparams_count;
  }
  std::string param_name(int i, int j)const override { untested();
    assert(i < HS_PARAM::param_count());
    if (j == 0) { untested();
      return param_name(i);
    }else{ untested();
      return "";
    }
  }
  std::string param_name(int i)const override;
  std::string param_value(int i)const override;

public: // hsp access
  double mfactor()const {
    assert(_mfactor_fixed);
    return _mfactor_fixed;
  }

  void precalc_first(PARAM_LIST const*)override {}
  void expand(COMPONENT const* c) override;
  void precalc_last(PARAM_LIST const*)override;

  HS_PARAM* hsparam()override { return this; }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
