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
static const int sysparams_count = 14;
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, std::string const& key, method_t* method); // u_opt2.cc
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
  PARAMETER<vString> _method{};
  PARAMETER<double> _temperature; // absolute temperature (K)
  PARAMETER<double> _dtemp;       // difference to ambient
  PARAMETER<double> _tnom;        // nominal (K)

private: // hmm. use a flag instead?
  PARAMETER<double> _temp_c;
  PARAMETER<double> _tnom_c;

private:
  explicit HS_PARAM(HS_PARAM const& p) : COMMON_PARAMLIST(p),
    _mfactor(p._mfactor),
    _xposition(p._xposition),
    _yposition(p._yposition),
    _zposition(p._zposition),
    _hflip(p._hflip),
    _vflip(p._vflip),
    _zflip(p._zflip),
    _angle(p._angle),
    _method(p._method),
    _temperature(p._temperature),
    _dtemp(p._dtemp),
    _tnom(p._tnom),
    _temp_c(p._temp_c),
    _tnom_c(p._tnom_c)
  {
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
    _method.set_default(vString(std::string("")));
    _temperature.set_default(OPT::temp_k);
    _dtemp.set_default(0);
    _tnom.set_default(OPT::temp_k);
    _temp_c.set_default(OPT::temp_k - P_CELSIUS0);
    _tnom_c.set_default(OPT::temp_k - P_CELSIUS0);
  }
  explicit HS_PARAM(int i) : COMMON_PARAMLIST(i){
    _mfactor.set_default(1.);
    _xposition.set_default(0.);
    _yposition.set_default(0.);
    _zposition.set_default(0.);
    _hflip.set_default(1);
    _vflip.set_default(1);
    _zflip.set_default(1);
    _angle.set_default(0.);
    _method.set_default(vString(std::string("")));
    _temperature.set_default(OPT::temp_k);
    _dtemp.set_default(0);
    _tnom.set_default(OPT::temp_k);
    _temp_c.set_default(OPT::temp_k - P_CELSIUS0);
    _tnom_c.set_default(OPT::temp_k - P_CELSIUS0);
  }
  ~HS_PARAM() {}
  HS_PARAM* clone()const override {return new HS_PARAM(*this);}

  bool operator==(const COMMON_COMPONENT& x)const override;
  bool operator<(const COMMON_COMPONENT& x)const override {
    return HS_PARAM::compare(x) < 0;
  }
  int compare(const COMMON_COMPONENT& x)const override;
  bool has_less()const override { untested(); return true;}

  bool param_is_printable(int i)const override;
  void set_param_by_index(int i, std::string& v, int)override;
  int set_param_by_name(std::string Name, std::string Value) override;
  void set_mfactor(double x)override { itested(); _mfactor = x; }
  int param_count()const override {
    return sysparams_count + COMMON_PARAMLIST::param_count();
  }
  std::string param_name(int i, int j)const override { untested();
    assert(i < HS_PARAM::param_count());
    if (j == 0) {
      return param_name(i);
    }else if (j == 1) { untested();
      if(i==11){ untested();
	return "temp"; // legacy, celsius. handle in lang_spice?
      }else if(i==12){ untested();
	return "tnom"; // legacy, celsius. handle in lang_spice?
      }else{ untested();
      }
    }else{
    }
    return "";
  }

  std::string param_name(int i)const override;
  std::string param_value(int i)const override;

public: // hsp access
  double mfactor(PARAM_LIST const* Scope)const;
  double temp_k(PARAM_LIST const* Scope)const;
  double temp_diff(PARAM_LIST const* Scope)const;
  method_t method(PARAM_LIST const* Scope)const;

  void precalc_first(PARAM_LIST const*)override {}
  void expand(COMPONENT const* c) override;
  void precalc_last(PARAM_LIST const*)override;
  void precalc_hierarchy(PARAM_LIST const*, PARAM_LIST*)const;
private:
  double precalc_mfactor(PARAM_LIST const*)const;
  double precalc_temperature(PARAM_LIST const*)const;
  method_t precalc_method(PARAM_LIST const*)const;

  HS_PARAM* hsparam()override { return this; }

private:
  method_t method_propagate(method_t env, method_t here) const;
public:
 // static Base const* new_default(std::string const& name);
  void print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const override;
public:
  static HS_PARAM hs_param;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
