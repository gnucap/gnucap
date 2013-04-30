/*$Id: d_cs.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * functions for fixed current source
 * x = 0, y.f0 = nothing, ev = y.f1 = amps.
 */
//testing=script 2006.07.17
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CS : public ELEMENT {
private:
  explicit DEV_CS(const DEV_CS& p) :ELEMENT(p) {}
public:
  explicit DEV_CS()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{return 'I';}
  std::string value_name()const {itested(); return "dc";}
  std::string dev_type()const	{return "isource";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   is_source()const	{return true;}
  bool	   f_is_value()const	{return true;}
  bool	   has_iv_probe()const  {return true;}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_CS(*this);}
  void     precalc_last();
  void	   tr_iwant_matrix()	{/* nothing */}
  void	   tr_begin();
  bool	   do_tr();
  void	   tr_load()		{tr_load_source();}
  void	   tr_unload()		{untested();tr_unload_source();}
  double   tr_involts()const	{return 0.;}
  double   tr_involts_limited()const {unreachable(); return 0.;}
  void	   ac_iwant_matrix()	{/* nothing */}
  void	   ac_begin()		{_acg = _ev = 0.;}
  void	   do_ac();
  void	   ac_load()		{ac_load_source();}
  COMPLEX  ac_involts()const	{untested();return 0.;}
  COMPLEX  ac_amps()const	{return _acg;}

  std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_CS::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(!has_tr_eval());
  set_converged(!has_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_CS::tr_begin()
{
  ELEMENT::tr_begin();
  _y1.f0 = _y[0].f0 = 0.; // override
  _m0.x  = 0.;
  _m0.c0 = _y[0].f1;
  _m0.c1 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
}
/*--------------------------------------------------------------------------*/
bool DEV_CS::do_tr()
{
  assert(_m0.x == 0.);
  if (using_tr_eval()) {
    _y[0].x = _sim->_time0;
    tr_eval();
    store_values();
    q_load();
    _m0.c0 = _y[0].f1;
    assert(_m0.c1 == 0.);
  }else{untested();
    assert(_y[0].x  == 0.);
    assert(_y[0].f0 == 0.);
    assert(_y[0].f1 == value());
    assert(_m0.x  == 0.);
    assert(_m0.c0 == _y[0].f1);
    assert(_m0.c1 == 0.);
    assert(_y1 == _y[0]);
    assert(converged());
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CS::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
    _acg = _ev;
  }else{untested();
    assert(_acg == 0.);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "I|csource|isource", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
