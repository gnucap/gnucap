/*$Id: d_vcg.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
class DEV_VCG : public ELEMENT {
private:
  explicit DEV_VCG(const DEV_VCG& p) :ELEMENT(p) {}
public:
  explicit DEV_VCG()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const override	{untested();return '\0';}
  std::string value_name()const override {untested(); return "g";}
  std::string dev_type()const override	{return "vcg";}
  int	   max_nodes()const override	{return 4;}
  int	   min_nodes()const override	{return 4;}
  int	   matrix_nodes()const override	{return 4;}
  int	   net_nodes()const override	{return 4;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override		{return new DEV_VCG(*this);}
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_extended();}
  void     tr_begin()override;
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_shunt(); tr_load_active();}
  void	   tr_unload() override		{untested(); tr_unload_shunt(); tr_unload_active();}
  double   tr_involts()const override	{return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const override {return volts_limited(_n[IN1],_n[IN2]);}
  void	    ac_iwant_matrix()override	{ac_iwant_matrix_extended();}
  void	    ac_begin()override		{_ev = _y[0].f0;  _acg = _m0.c1;}
  void	    do_ac()override;
  void	    ac_load()override		{ac_load_shunt(); ac_load_active();}
  COMPLEX   ac_involts()const override	{return _n[IN1]->vac() - _n[IN2]->vac();}

  std::string port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VCG::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f0 = _y[0].f0 = 0;  // override  
  _loss1 = _loss0 = 0;
  _m0.x  = 0.;
  _m0.c1 = 0.;
  _m0.c0 = 0.;
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
bool DEV_VCG::do_tr()
{
  _y[0].x = tr_input_limited();
  tr_eval();
  assert(_y[0].f0 != LINEAR);

  store_values();
  q_load();

  _loss0 = _y[0].f0;
  _m0.x = tr_outvolts(); // fake
  _m0.c1 = _y[0].f1 * tr_outvolts();
  _m0.c0 = -_y[0].x * _m0.c1;
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCG::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = _ev * _m0.x;
    _ev *= _y[0].x;
  }else{
    assert(_ev == _y[0].f0);
    assert(_acg == _m0.c1);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VCG p3;
DISPATCHER<CARD>::INSTALL d3(&device_dispatcher, "vcg", &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
