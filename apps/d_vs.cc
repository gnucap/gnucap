/*$Id: d_vs.cc 2016/09/11 al $ -*- C++ -*-
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
 * functions for fixed voltage sources
 * temporary kluge: it has resistance
 */
//testing=script 2006.07.17
#include "globals.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_VS : public ELEMENT {
private:
  explicit DEV_VS(const DEV_VS& p) :ELEMENT(p) {}
public:
  explicit DEV_VS()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const override	{return 'V';}
  std::string value_name()const override{return "dc";}
  std::string dev_type()const override	{return "vsource";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 2;}
  bool	   is_source()const override	{return true;}
  bool	   f_is_value()const override	{return true;}
  bool	   has_iv_probe()const override	{return true;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override	{return new DEV_VS(*this);}
  void     precalc_last()override;
  void     dc_advance()override;
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_passive();}
  void	   tr_begin()override;
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_shunt(); tr_load_source();}
  void	   tr_unload()override		{untested();tr_unload_source();}
  double   tr_involts()const override	{return 0.;}
  double   tr_involts_limited()const override{unreachable(); return 0.;}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_passive();}
  void	   ac_begin()override		{_loss1 = _loss0 = 1./OPT::shortckt; _acg = _ev = 0.;}
  void	   do_ac()override;
  void	   ac_load()override		{ac_load_shunt(); ac_load_source();}
  COMPLEX  ac_involts()const override	{return 0.;}
  COMPLEX  ac_amps()const override	{return (_acg + ac_outvolts()*_loss0);}

  std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_VS::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!using_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_VS::dc_advance()
{
  ELEMENT::dc_advance();

  if(using_tr_eval()){
  }else{
    _y[0].f1 = value();
    if(_y[0].f1 != _y1.f1){ untested();
      store_values();
      q_load();
      _m0.c0 = -_loss0 * _y[0].f1;
      // set_constant(false); not needed. nothing to do in do_tr.
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_VS::tr_begin()
{
  ELEMENT::tr_begin();
  _y[0].x  = 0.;
  _y[0].f1 = value();
  _y1.f0 = _y[0].f0 = 0.;	//BUG// override
  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = 0.;
  _m0.c0 = -_loss0 * _y[0].f1;
  _m0.c1 = 0.;
  _m1 = _m0;
  if (!using_tr_eval()) {
    if (_n[OUT2].m_() == 0) {
      _sim->set_limit(value());
    }else if (_n[OUT1].m_() == 0) {
      _sim->set_limit(-value());
    }else{
      //BUG// don't set limit
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_VS::do_tr()
{
  assert(_m0.x == 0.);
  if (using_tr_eval()) {
    _y[0].x = _sim->_time0;
    tr_eval();
    if (_n[OUT2].m_() == 0) {
      _sim->set_limit(_y[0].f1);
    }else if (_n[OUT1].m_() == 0) {
      _sim->set_limit(-_y[0].f1);
    }else{
      //BUG// don't set limit
    }
    store_values();
    q_load();
    _m0.c0 = -_loss0 * _y[0].f1;
    assert(_m0.c1 == 0.);
  }else{
    assert(conchk(_loss0, 1./OPT::shortckt));
    assert(_y[0].x == 0.);
    assert(_y[0].f0 == 0.);
    assert(_y[0].f1 == value());
    assert(_m0.x == 0.);
    assert(conchk(_m0.c0, -_loss0 * _y[0].f1));
    assert(_m0.c1 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_VS::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = -_loss0 * _ev;
  }else{itested();
    assert(_acg == 0.);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_VS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "V|vsource", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
