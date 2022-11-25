/*$Id: d_cccs.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
 * functions for cccs
 * It is really voltage controlled, taking into account the sense element.
 * Then adjust the gain to account for the sense element.
 */
//testing=script,complete 2008.10.09
#include "globals.h"
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CCCS : public CCSRC_BASE {
private:
  explicit DEV_CCCS(const DEV_CCCS& p) :CCSRC_BASE(p) {}
public:
  explicit DEV_CCCS()		:CCSRC_BASE() {}
private: // override virtual
  char	   id_letter()const override	{return 'F';}
  std::string value_name()const override{return "gain";}
  std::string dev_type()const override	{return "cccs";}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override	{return new DEV_CCCS(*this);}
  void     precalc_last()override;
  void     dc_advance()override;
  void	   tr_iwant_matrix()override	{tr_iwant_matrix_active();}
  void     tr_begin()override;
  bool     do_tr()override	{_sim->_late_evalq.push_back(this); return true;}
  bool	   do_tr_last()override;
  void	   tr_load()override	{tr_load_active();}
  void	   ac_iwant_matrix()override {ac_iwant_matrix_active();}
  void	   ac_begin()override	{CCSRC_BASE::ac_begin(); _ev = _y[0].f1;}
  void	   do_ac()override;
  void	   ac_load()override	{ac_load_active();}

  std::string port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"sink", "src"};
    return names[i];
  }
  std::string current_port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 1);
    static std::string names[] = {"in"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV_CCCS::dc_advance()
{
  CCSRC_BASE::dc_advance();
  if(using_tr_eval()){
  }else{
    _y[0].f1 = value();

    if(_y1.f1 != _y[0].f1){ untested();
      store_values();
      // q_load(); // called unconditionally in do_tr_last.
      _m0.c0 = _y[0].f1;
      // set_constant(false); not needed. nothing to do in do_tr.
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_CCCS::precalc_last()
{
  CCSRC_BASE::precalc_last();
  set_converged(!has_tr_eval());
  set_constant(!using_tr_eval());
}
/*--------------------------------------------------------------------------*/
void DEV_CCCS::tr_begin()
{
  CCSRC_BASE::tr_begin();
  _m0.x  = _y[0].x;
  _m0.c1 = _y[0].f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
}
/*--------------------------------------------------------------------------*/
bool DEV_CCCS::do_tr_last()
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
  store_values();
  q_load();
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_CCCS::do_ac()
{
  assert(_input);
  if (!_input->evaluated()) {	/* patch for forward reference */
    ELEMENT* input = const_cast<ELEMENT*>(_input);
    input->do_ac();		/* make sure sense elt is evaluated first */
  }else{
  }
  if (using_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y[0].f1);
    assert(has_tr_eval() || _ev == double(value()));
  }
  if (_input->is_source()) {	/* if the sense elt is a fixed source.. */
    _acg = _ev * _input->_acg;	/* then part of this one can be modeled */
    ac_load_source();		/* as a fixed source. ...		*/
    _acg = _ev * _input->_loss0;/* so load it in 2 pieces		*/
  }else if (_input->has_inode()) {untested();
    _acg = _ev;
  }else if (_input->has_iv_probe()) {
    _acg = _ev * _input->_acg;
  }else{unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CCCS p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "F|cccs", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
