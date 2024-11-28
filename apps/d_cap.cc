/*$Id: d_cap.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
 * capacitance devices:
 *	self-capacitance (C device)
 *	trans-capacitance (non-spice charge transfer device)
 *------------------------------------------------------------------
 * capacitor models
 * y.x = volts, y.f0 = coulombs, ev = y.f1 = farads
 * q = y history in time
 * i.x = volts, i.f0 = amps,	      i.f1 = mhos
 * m.x = volts, m.c0 = amps,    acg = m.c1 = mhos
 */
//testing=script 2006.07.17
#include "globals.h"
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CAPACITANCE : public STORAGE {
protected:
  explicit DEV_CAPACITANCE(const DEV_CAPACITANCE& p) :STORAGE(p) {}
public:
  explicit DEV_CAPACITANCE()	:STORAGE() {}
protected: // override virtual
  char	   id_letter()const override	{return 'C';}
  std::string value_name()const override{return "c";}
  std::string dev_type()const override	{return "capacitor";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 2;}
  bool	   has_iv_probe()const override {return true;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override		{return new DEV_CAPACITANCE(*this);}
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_passive();}
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_passive();}
  void	   tr_unload()override		{tr_unload_passive();}
  double   tr_involts()const override	{return tr_outvolts();}
  double   tr_involts_limited()const override {return tr_outvolts_limited();}
  double   tr_probe_num(const std::string&)const override;
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_passive();}
  void	   ac_begin()override		{STORAGE::ac_begin(); _ev = _y[0].f1;}
  void	   do_ac()override;
  void	   ac_load()override		{ac_load_passive();}
  COMPLEX  ac_involts()const override	{itested();return ac_outvolts();}

  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
class DEV_TRANSCAP : public DEV_CAPACITANCE {
private:
  explicit DEV_TRANSCAP(const DEV_TRANSCAP& p) :DEV_CAPACITANCE(p){}
public:
  explicit DEV_TRANSCAP()	:DEV_CAPACITANCE() {}
private: // override virtual
  char     id_letter()const override	{untested();return '\0';}
  std::string value_name()const override{untested(); return "c";}
  std::string dev_type()const override	{return "tcap";}
  int	   max_nodes()const override	{return 4;}
  int	   min_nodes()const override	{return 4;}
  int	   matrix_nodes()const override	{return 4;}
  int	   net_nodes()const override	{return 4;}
  bool	   has_iv_probe()const override {untested(); return false;}
  bool	   f_is_value()const override	{untested();return true;}
  CARD*	   clone()const override	{return new DEV_TRANSCAP(*this);}
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_active();}
  void	   tr_load()override		{tr_load_active();}
  double   tr_involts()const override	{return dn_diff(n_(IN1).v0(),n_(IN2).v0());}
  double   tr_involts_limited()const override {return volts_limited(n_(IN1),n_(IN2));}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_active();}
  void	   ac_load()override		{untested(); ac_load_active();}

  std::string port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
//BUG// doesn't model dynamic effects of control.
class DEV_VCCAP : public DEV_CAPACITANCE {
private:
  explicit DEV_VCCAP(const DEV_VCCAP& p) :DEV_CAPACITANCE(p) {}
public:
  explicit DEV_VCCAP()		:DEV_CAPACITANCE() {}
private: // override virtual
  char     id_letter()const override	{untested();return '\0';}
  std::string value_name()const override{untested(); return "c";}
  std::string dev_type()const override	{return "vccap";}
  int	   max_nodes()const override	{return 4;}
  int	   min_nodes()const override	{return 4;}
  int	   matrix_nodes()const override	{return 4;}
  int	   net_nodes()const override	{return 4;}
  bool	   has_iv_probe()const override {untested(); return false;}
  bool	   f_is_value()const override	{untested();return true;}
  CARD*	   clone()const override	{return new DEV_VCCAP(*this);}
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_extended();}
  bool     do_tr()override;
  double   tr_involts()const override	{return dn_diff(n_(IN1).v0(),n_(IN2).v0());}
  double   tr_involts_limited()const override {return volts_limited(n_(IN1),n_(IN2));}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_extended();}

  std::string port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool DEV_CAPACITANCE::do_tr()
{
  if (using_tr_eval()) {
    _y[0].x = tr_input_limited();
    tr_eval();
  }else{
    _y[0].x = tr_input(); // tr_involts();
    assert(_y[0].f1 == value());
    _y[0].f0 = _y[0].x * _y[0].f1;
    assert(converged());
  }
  store_values();
  q_load();

  trace3("q", _y[0].x, _y[0].f0, _y[0].f1);
  _i[0] = differentiate(_y, _i, _time, _method_a);
  trace3("i", _i[0].x, _i[0].f0, _i[0].f1);
  _m0 = CPOLY1(_i[0]);
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CAPACITANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  _acg = _ev * _sim->_jomega;
}
/*--------------------------------------------------------------------------*/
double DEV_CAPACITANCE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "q{cap} |ch{arge} ")) {
    return _y[0].f0;
  }else if (Umatch(x, "c{apacitance} ")) {
    return _y[0].f1;
  }else if (Umatch(x, "dcdt ")) {untested();
    return (_y[0].f1 - _y[1].f1) / _dt;
  }else if (Umatch(x, "dc ")) {untested();
    return (_y[0].f1 - _y[1].f1);
  }else if (Umatch(x, "dqdt ")) {
    return (_y[0].f0 - _y[1].f0) / _dt;
  }else if (Umatch(x, "dq ")) {
    return (_y[0].f0 - _y[1].f0);
  }else{
    return STORAGE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_VCCAP::do_tr()
{
  _y[0].x = tr_input_limited();
  tr_eval();

  store_values();
  q_load();

  _y[0].x = tr_outvolts();
  _y[0].f1 = _y[0].f0;		 // self capacitance
  _y[0].f0 = _y[0].x * _y[0].f1; // charge

  _i[0] = differentiate(_y, _i, _time, _method_a);
  _m0.x  = _i[0].x;
  _m0.c1 = _i[0].f1;
  _m0.c0 = _i[0].f0 - _i[0].x * _i[0].f1;
  return converged();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CAPACITANCE p1;
DEV_TRANSCAP    p2;
DEV_VCCAP       p3;
DISPATCHER<CARD>::INSTALL
  d1(&device_dispatcher, "C|capacitor",	    &p1),
  d2(&device_dispatcher, "tcap|tcapacitor", &p2),
  d3(&device_dispatcher, "vccap",	    &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
