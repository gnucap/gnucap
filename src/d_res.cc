/*$Id: d_res.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
  char	   id_letter()const	{return 'R';}
  std::string value_name()const {return "r";}
  std::string dev_type()const	{return "resistor";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   has_iv_probe()const  {return true;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_RESISTANCE(*this);}
  void     precalc_last();
  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  void     tr_begin();
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  void	   tr_unload()		{untested();tr_unload_passive();}
  double   tr_involts()const	{return tr_outvolts();}
  double   tr_input()const	{untested(); return _m0.c0 + _m0.c1 * tr_involts();}
  double   tr_involts_limited()const {return tr_outvolts_limited();}
  double   tr_input_limited()const {return _m0.c0+_m0.c1*tr_involts_limited();}
  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void     ac_begin()           {_ev = _y[0].f1; _acg = 1. / _ev;} 
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{return ac_outvolts();}

  std::string port_name(int i)const {itested();
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
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_RESISTANCE::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f1 = _y[0].f1 = (value() != 0.) ? value() : OPT::shortckt;
  _m0.x  = _y[0].x;  
  _m0.c1 = 1./_y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  if (value() == 0. && !has_common()) {
    error(bPICKY, long_label() + ": short circuit\n");
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
    if (_y[0].f1 == 0.) {
      error(bPICKY, long_label() + ": short circuit\n");
      _y[0].f1 = OPT::shortckt;
      set_converged(conv_check());
    }else{
    }
    store_values();
    q_load();
    _m0.c1 = 1./_y[0].f1;
    _m0.c0 = _y[0].x - _y[0].f0 / _y[0].f1;
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
void DEV_RESISTANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    if (_ev == 0.) {
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
