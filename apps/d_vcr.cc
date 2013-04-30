/*$Id: d_vcr.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * voltage controlled resistor.
 * y.x  = volts(control), ev = y.f0 = ohms,       y.f1 = ohms/volt
 * m.x  = volts(control),      m.c0 = 0,    acg = m.c1 = mhos
 * _loss0 == 1/R. (mhos)
 */
//testing=script,complete 2006.07.17
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_VCR : public ELEMENT {
private:
  explicit DEV_VCR(const DEV_VCR& p) :ELEMENT(p) {}
public:
  explicit DEV_VCR()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{itested();return '\0';}
  std::string value_name()const {itested(); return "r";}
  std::string dev_type()const	{return "vcr";}
  int	   max_nodes()const	{return 4;}
  int	   min_nodes()const	{return 4;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 4;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_VCR(*this);}
  void     precalc_last();
  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  void     tr_begin();
  bool	   do_tr();
  void	   tr_load()		{tr_load_shunt(); tr_load_active();}
  void	   tr_unload()		{untested(); tr_unload_shunt(); tr_unload_active();}
  double   tr_involts()const	{untested(); return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  double   tr_amps()const	{untested(); return ELEMENT::tr_amps();}
  void	   ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	   ac_begin();
  void	   do_ac();
  void	   ac_load()		{ac_load_shunt(); ac_load_active();}
  COMPLEX  ac_involts()const	{untested();return _n[IN1]->vac()-_n[IN2]->vac();}
  COMPLEX  ac_amps()const	{untested(); return ELEMENT::ac_amps();}

  std::string port_name(int i)const {untested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VCR::precalc_last()
{
  ELEMENT::precalc_last();
  assert(!is_constant());
  set_not_converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCR::tr_begin()
{
  ELEMENT::tr_begin();
  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = 0.;
  _m0.c1 = 0.;
  _m0.c0 = 0.;
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
bool DEV_VCR::do_tr()
{
  _y[0].x = tr_input_limited();
  tr_eval();
  trace3("vcr", _y[0].x, _y[0].f0, _y[0].f1);
  assert(_y[0].f0 != LINEAR);
  if (_y[0].f0 == 0.) {
    error(bDEBUG, long_label() + ": short circuit\n");
    _y[0].f0 = OPT::shortckt;
    set_converged(conv_check());
  }else{
  }
  store_values();
  q_load();

  _loss0 = 1./_y[0].f0;
  _m0.x = tr_outvolts(); // fake
  _m0.c1 = -_y[0].f1 * _loss0 * _loss0 * tr_outvolts();
  _m0.c0 = -_y[0].x * _m0.c1;
  trace3("vcr", _loss0, _m0.c0, _m0.c1);
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCR::ac_begin()
{
  _ev  = _y[0].f0;
  _acg = _m0.c1;
  trace4("vcr-ac_begin", _y[0].f0, _y[0].f1, _m0.c0, _m0.c1);
  trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
  trace1("", _loss0);
}
/*--------------------------------------------------------------------------*/
void DEV_VCR::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_ev * _loss0 * _loss0 * _m0.x;
    trace4("vcr-do_ac(eval)", _y[0].f0, _y[0].f1, _m0.c0, _m0.c1);
    trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
    _ev *= _y[0].x;
    trace4("", _ev.real(), _ev.imag(), _loss0, _m0.x);
  }else{
    trace4("vcr-do_ac", _y[0].f0, _y[0].f1, _m0.c0, _m0.c1);
    trace4("", _ev.real(), _ev.imag(), _acg.real(), _acg.imag());
    trace1("", _loss0);
    assert(_ev == _y[0].f0);
    assert(_acg == _m0.c1);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VCR p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "vcr", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
