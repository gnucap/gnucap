/*$Id: d_ccvs.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * functions for ccvs
 * It is really voltage controlled, taking into account the sense element.
 * Then adjust the gain to account for the sense element.
 */
//testing=script 2006.07.17
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CCVS : public CCSRC_BASE {
private:
  explicit DEV_CCVS(const DEV_CCVS& p) :CCSRC_BASE(p) {}
public:
  explicit DEV_CCVS()		:CCSRC_BASE() {}
private: // override virtual
  char	   id_letter()const	{return 'H';}
  std::string value_name()const {untested(); return "gain";}
  std::string dev_type()const	{return "ccvs";}
  bool	   use_obsolete_callback_parse()const {return true;}
  CARD*	   clone()const		{return new DEV_CCVS(*this);}
  void     precalc_last();
  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  void     tr_begin();
  bool     do_tr()		{_sim->_late_evalq.push_back(this); return true;}
  bool	   do_tr_last();
  void	   tr_load()		{tr_load_shunt(); tr_load_active();}
  void	   ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	   ac_begin()		{_loss1=_loss0=1./OPT::shortckt; _ev = _y[0].f1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_active();}
  COMPLEX  ac_amps()const	{untested(); return ELEMENT::ac_amps();}

  std::string port_name(int i)const {untested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }
  std::string current_port_name(int i)const {untested();
    assert(i >= 0);
    assert(i < 1);
    static std::string names[] = {"in"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_CCVS::precalc_last()
{
  CCSRC_BASE::precalc_last();
  set_converged();
  assert(!is_constant()); /* because of incomplete analysis */
}
/*--------------------------------------------------------------------------*/
void DEV_CCVS::tr_begin()
{
  CCSRC_BASE::tr_begin();
  _loss1 = _loss0 = 1./OPT::shortckt;
  _m0.x  = _y[0].x;
  _m0.c1 = -_loss0 * value();
  _m0.c0 = 0.;
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
bool DEV_CCVS::do_tr_last()
{
  assert(_input);
  if (using_tr_eval()) {
    _m0.x = tr_involts_limited();
    _y[0].x = tr_input_limited();
    tr_eval();
    assert(_y[0].f0 != LINEAR);
    _m0 = CPOLY1(_y[0]);
  }else{
    assert(_y[0].f0 == LINEAR);
    assert(_y[0].f1 == value());
    _m0.c0 = 0.;
    assert(converged());
  }
  if (_input->has_inode()) {untested();
    // nothing
  }else if (_input->has_iv_probe()) {
    _m0.c0 += _y[0].f1 * _input->_m0.c0;
    _m0.c1  = _y[0].f1 * (_input->_loss0 + _input->_m0.c1);
  }else{unreachable();
  }
  _m0 *= -_loss0;
  store_values();
  q_load();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CCVS::do_ac()
{
  assert(_input);
  if (!_input->evaluated()) {untested();
    ELEMENT* input = const_cast<ELEMENT*>(_input);
    input->do_ac();	    //BUG// premature load of sense element
  }else{
  }
  ac_load_shunt();
  if (using_ac_eval()) {untested();
    ac_eval();
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  if (_input->is_source()) {
    _acg = -_loss0 * _ev * _input->_acg;
    ac_load_source();
    _acg = -_loss0 * _ev * _input->_loss0;
  }else if (_input->has_inode()) {untested();
    _acg = -_loss0 * _ev;
  }else if (_input->has_iv_probe()) {
    _acg = -_loss0 * _ev * _input->_acg;
  }else{unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CCVS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "H|ccvs", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
