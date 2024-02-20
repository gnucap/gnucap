/*$Id: d_admit.cc,v 26.138 2013/04/24 02:44:30 al Exp $ -*- C++ -*-
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
 * admittance devices:
 *	self-admittance (old Y device)
 *		y.x = volts, y.f0 = amps, ev  = y.f1 = mhos.
 *		m.x = volts, m.c0 = amps, acg = m.c1 = mhos.
 *	trans-admittance (VCCS, G device)
 *	voltage controlled admittance
 *		y.x  = volts(control), y.f0 = mhos, ev = y.f1 = mhos/volt
 *		m.x  = volts(control), m.c0 = 0,    acg = m.c1 = mhos
 *		_loss0 == 1/R. (mhos)
 */
//testing=script 2006.07.17
#include "globals.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_ADMITTANCE : public ELEMENT {
protected:
  explicit DEV_ADMITTANCE(const DEV_ADMITTANCE& p) :ELEMENT(p) {}
public:
  explicit DEV_ADMITTANCE()	:ELEMENT() {}
protected: // override virtual
  char	   id_letter()const override	{return 'Y';}
  std::string value_name()const override{return "g";}
  std::string dev_type()const override	{return "admittance";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 2;}
  bool	   has_iv_probe()const override {return true;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override	{return new DEV_ADMITTANCE(*this);}
  void     precalc_last()override;
  void	   dc_advance()override;
  void	   tr_iwant_matrix() override	{tr_iwant_matrix_passive();}
  void     tr_begin()override;
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_passive();}
  void	   tr_unload()override		{tr_unload_passive();}
  double   tr_involts()const override	{return tr_outvolts();}
  double   tr_involts_limited()const override {return tr_outvolts_limited();}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_passive();}
  void	   ac_begin()override		{ELEMENT::ac_begin(); _acg = _ev = _y[0].f1;}
  void	   do_ac()override;
  void	   ac_load()override		{ac_load_passive();}
  COMPLEX  ac_involts()const override	{untested();return ac_outvolts();}

  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
class DEV_VCCS : public DEV_ADMITTANCE {
protected:
  explicit DEV_VCCS(const DEV_VCCS& p) :DEV_ADMITTANCE(p) {}
public:
  explicit DEV_VCCS()		:DEV_ADMITTANCE() {}
protected: // override virtual
  char	   id_letter()const override	{return 'G';}
  std::string value_name()const override {return "gm";}
  std::string dev_type()const override	{return "vccs";}
  int	   max_nodes()const override	{return 4;}
  int	   min_nodes()const override	{return 4;}
  int	   matrix_nodes()const override	{return 4;}
  int	   net_nodes()const override	{return 4;}
  bool	   has_iv_probe()const override  {return false;}
  CARD*	   clone()const override		{return new DEV_VCCS(*this);}
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_active();}
  void	   tr_load()override		{tr_load_active();}
  void	   tr_unload()override		{untested();tr_unload_active();}
  double   tr_involts()const override	{return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const override {return volts_limited(_n[IN1],_n[IN2]);}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_active();}
  void	   ac_load()override		{ac_load_active();}
  COMPLEX  ac_involts()const override	{untested();return _n[IN1]->vac() - _n[IN2]->vac();}

  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"sink", "src", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!using_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::dc_advance()
{
  ELEMENT::dc_advance();
  if(using_tr_eval()){
  }else{
    _y[0].f1 = value();
    _y[0].f0 = LINEAR;

    if(_y[0].f1 != _y1.f1){
      store_values();
      _m0.c1 = _y[0].f1;
      _m0.c0 = 0.;
      q_load();
      // set_constant(false); not needed. nothing to do in do_tr.
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::tr_begin()
{
  ELEMENT::tr_begin();
  _m0.x  = _y[0].x;
  _m0.c1 = _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
}
/*--------------------------------------------------------------------------*/
bool DEV_ADMITTANCE::do_tr()
{
  if (using_tr_eval()) {
    _y[0].x = _m0.x = tr_involts_limited();
    _y[0].f0 = _m0.c1 * _m0.x + _m0.c0;	//BUG//  patch for diode
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    store_values();
    q_load();
    _m0 = CPOLY1(_y[0]);
  }else{
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value());
    assert(_m0.c1 == _y[0].f1);
    assert(_m0.c0 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_ADMITTANCE::do_ac()
{
  if (using_ac_eval()) {itested();
    ac_eval();
    _acg = _ev;
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  assert(_acg == _ev);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_ADMITTANCE p1;
DEV_VCCS       p2;
DISPATCHER<CARD>::INSTALL
  d1(&device_dispatcher, "Y|admittance", &p1),
  d2(&device_dispatcher, "G|vccs",       &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
