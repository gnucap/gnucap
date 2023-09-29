/*$Id: d_res.cc  2016/09/11  $ -*- C++ -*-
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
 * functions for resistor.
 * y.x = amps,  y.f0 = volts, ev = y.f1 = ohms
 * m.x = volts, m.c0 = amps, acg = m.c1 = mhos.
 */
//testing=script,complete 2006.07.17
#include "globals.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_RESISTANCE : public ELEMENT {
private:
  explicit DEV_RESISTANCE(const DEV_RESISTANCE& p) :ELEMENT(p) {}
public:
  explicit DEV_RESISTANCE()	:ELEMENT() {}
private: // override virtual
  char	   id_letter()const override	{return 'R';}
  std::string value_name()const override{return "r";}
  std::string dev_type()const override	{return "resistor";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 2;}
  bool	   has_iv_probe()const override {return true;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override	{return new DEV_RESISTANCE(*this);}
  void     precalc_last()override;
  void     dc_advance()override;
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_passive();}
  void     tr_begin()override;
  void     tr_restore()override;
  bool	   do_tr()override;
  void	   tr_load()override		{tr_load_passive();}
  void	   tr_unload()override		{untested();tr_unload_passive();}
  double   tr_involts()const override	{return tr_outvolts();}
  double   tr_input()const override	{untested(); return _m0.c0 + _m0.c1 * tr_involts();}
  double   tr_involts_limited()const override {return tr_outvolts_limited();}
  double   tr_input_limited()const override {return _m0.c0+_m0.c1*tr_involts_limited();}
  void	   ac_iwant_matrix()override	{ac_iwant_matrix_passive();}
  void     ac_begin()override;
  void	   do_ac()override;
  void	   ac_load()override		{ac_load_passive();}
  COMPLEX  ac_involts()const override	{return ac_outvolts();}

  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!using_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::dc_advance()
{
  ELEMENT::dc_advance();

  if(using_tr_eval()){
  }else{
    assert(_m0.c0 == 0.);
    if(value() == 0.) {
      _y[0].f1 = OPT::shortckt;
    }else if(std::abs(value())<OPT::shortckt){
      _y[0].f1 = OPT::shortckt;
    }else {
      _y[0].f1 = value();
    }
    if(_y[0].f1 != _y1.f1) {
      store_values();
      q_load();
      _m0.c1 = 1./_y[0].f1;
      assert(_m0.c0 == 0.);
      // set_constant(false); not needed. nothing to do in do_tr.
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tr_begin()
{
  ELEMENT::tr_begin();
  if(value() == 0.){
    _y[0].f1 = OPT::shortckt;
  }else if(std::abs(value()) < OPT::shortckt) {
    _y[0].f1 = OPT::shortckt;
  }else {
    _y[0].f1 = value();
  }
  _y1.f1 = _y[0].f1;
  _m0.x  = _y[0].x;
  _m0.c1 = 1./_y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  if(has_common()) {
  }else if (std::abs(value()) <= OPT::shortckt) {
    error(bPICKY, long_label() + ": short circuit\n");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tr_restore()
{
  ELEMENT::tr_restore();

  // is this needed in ELEMENT?
  if (_time[0] > _sim->_time0) {untested();
  }else if (_time[0] != _sim->_time0) {untested();
  }else if (_y[0].f0 != LINEAR){
  }else if (using_tr_eval()) {
    trace3("tr_restore invent history", long_label(), _y[0].f0, _y[1].f1);
    _m0.x = tr_involts_limited();
    _y[0].x = tr_input_limited();
    tr_eval();
    store_values();
    for (int i=1  ; i<OPT::_keep_time_steps; ++i) {
      _time[i] = 0;
      _y[i] = _y[0];
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_RESISTANCE::do_tr()
{
  if (using_tr_eval()) {
    _m0.x = tr_involts_limited();
    _y[0].x = tr_input_limited();;
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    if (std::abs(_y[0].f1) <= OPT::shortckt) {
      error(bPICKY, long_label() + ": short circuit\n");
      _y[0].f1 = OPT::shortckt;
      set_converged();
    }else{
    }
    store_values();
    q_load();
    _m0.c1 = 1./_y[0].f1;
    _m0.c0 = _y[0].x - _y[0].f0 / _y[0].f1;
    trace4("DEV_RESISTANCE::dtr", long_label(), _sim->_time0, _y[0].f0, _y[0].f1);
  }else{
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value() || _y[0].f1 == OPT::shortckt);
    assert(conchk(_m0.c1, 1./_y[0].f1));
    assert(_m0.c0 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::ac_begin()
{
  ELEMENT::ac_begin();
  _ev = _y[0].f1;
  _acg = 1. / _ev;
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    if (std::abs(_ev) <= OPT::shortckt) {
      error(bPICKY, long_label() + ": short circuit\n");
      _ev = OPT::shortckt;
    }else{
    }
    _acg = 1. / _ev;
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()) || _ev == OPT::shortckt);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_RESISTANCE p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "R|resistor", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
