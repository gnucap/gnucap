/*$Id: d_vcvs.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
 * functions for vcvs
 * temporary kluge: it has resistance
 */
//testing=script,complete 2006.07.17
#include "globals.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_VCVS : public ELEMENT {
private:
  explicit DEV_VCVS(const DEV_VCVS& p) :ELEMENT(p) {}
public:
  explicit DEV_VCVS()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const override	{return 'E';}
  std::string value_name()const override{return "gain";}
  std::string dev_type()const override	{return "vcvs";}
  int	   max_nodes()const override	{return 4;}
  int	   min_nodes()const override	{return 4;}
  int	   matrix_nodes()const override	{return 4;}
  int	   net_nodes()const override	{return 4;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override	{return new DEV_VCVS(*this);}
  void     precalc_last()override;
  void     dc_advance()override;
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_extended();}
  void     tr_begin()override;
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_shunt(); tr_load_active();}
  void	   tr_unload()override		{untested();tr_unload_active();}
  double   tr_involts()const override	{return dn_diff(n_(IN1).v0(), n_(IN2).v0());}
  double   tr_involts_limited()const override {return volts_limited(n_(IN1),n_(IN2));}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_extended();}
  void	   ac_begin()override;
  void	   do_ac()override;
  void	   ac_load()override		{ac_load_shunt(); ac_load_active();}
  COMPLEX  ac_involts()const override	{return n_(IN1)->vac() - n_(IN2)->vac();}

  std::string port_name(int i)const override { untested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"p", "n", "ps", "ns"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VCVS::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!using_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::dc_advance()
{
  ELEMENT::dc_advance();
  if(using_tr_eval()){
  }else{
    _y[0].f1 = value();

    if(_y1.f1 != _y[0].f1){
      store_values();
      q_load();
      _m0.c1 = -_loss0 * _y[0].f1;
      _m0.c0 = 0.;
      trace1("DEV_VCVS::dc_advance", _m0.c0);
      // set_constant(false); not needed. nothing to do in do_tr.
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::tr_begin()
{
  ELEMENT::tr_begin();
  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = 0.;
  _m0.c1 = -_loss0 * _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
bool DEV_VCVS::do_tr()
{
  if (using_tr_eval()) {
    _y[0].x = _m0.x = tr_involts_limited();
    //_y[0].x = tr_input_limited();
    //assert(_y[0].x == _m0.x);
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    store_values();
    q_load();
    _m0 = CPOLY1(_y[0]);
    _m0 *= -_loss0;
  }else{
    assert(conchk(_loss0, 1./OPT::shortckt));
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value());
    assert(conchk(_m0.c1, -_loss0 * _y[0].f1));
    assert(_m0.c0 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::ac_begin()
{
  ELEMENT::ac_begin();
  _loss1 = _loss0 = 1./OPT::shortckt;
  _ev = _y[0].f1;
  _acg = -_loss0 * _ev;
}
/*--------------------------------------------------------------------------*/
void DEV_VCVS::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VCVS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "E|vcvs", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
