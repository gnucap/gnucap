/*$Id: bmm_semi.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
 * behavioral modeling
 * Spice3 compatible "semiconductor resistor and capacitor"
 */
//testing=script 2006.07.13
#include "u_parameter.h"
#include "bmm_semi.h"
/*--------------------------------------------------------------------------*/
double const EVAL_BM_SEMI_BASE::_default_length = NOT_INPUT;
double const EVAL_BM_SEMI_BASE::_default_width = NOT_INPUT;
double const EVAL_BM_SEMI_BASE::_default_value = NOT_INPUT;
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _length(_default_length),
   _width(_default_width),
   _value(_default_value)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SEMI_BASE::EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p)
  :EVAL_BM_ACTION_BASE(p),
   _length(p._length),
   _width(p._width),
   _value(p._value)
{
  untested();
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_BASE::operator==(const COMMON_COMPONENT& x)const
{
  untested();
  const EVAL_BM_SEMI_BASE* p = dynamic_cast<const EVAL_BM_SEMI_BASE*>(&x);
  bool rv = p
    && _length == p->_length
    && _width == p->_width
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::print(OMSTREAM& o)const
{
  o << ' '   << modelname()
    << " l=" << _length;
  if (_width.has_value()) {o << " w=" << _width;}
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* s = c->scope();
  assert(s);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _length.e_val(_default_length, s);
  _width.e_val(_default_width, s);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_BASE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_BASE::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "L",	&_length)
    || get(cmd, "W",	&_width)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_CAPACITOR::operator==(const COMMON_COMPONENT& x)const
{
  untested();
  const EVAL_BM_SEMI_CAPACITOR*
    p = dynamic_cast<const EVAL_BM_SEMI_CAPACITOR*>(&x);
  bool rv = p
    && EVAL_BM_SEMI_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_CAPACITOR::elabo3(const COMPONENT* d)
{
  EVAL_BM_SEMI_BASE::elabo3(d);

  const MODEL_SEMI_CAPACITOR* m 
    = dynamic_cast<const MODEL_SEMI_CAPACITOR*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + ": model " + modelname()
	  + " is not a semi-capacitor (C)\n");
  }
  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;
  if (eff_width <= 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective width is negative or zero\n");
  }
  if (eff_length <= 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective length is negative or zero\n");
  }

  _value = m->_cj * eff_length * eff_width
    + 2. * m->_cjsw * (eff_length + eff_width);

  double tempdiff = (_temp_c - m->_tnom_c);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SEMI_RESISTOR::operator==(const COMMON_COMPONENT& x)const
{
  untested();
  const EVAL_BM_SEMI_RESISTOR*
    p = dynamic_cast<const EVAL_BM_SEMI_RESISTOR*>(&x);
  bool rv = p
    && EVAL_BM_SEMI_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SEMI_RESISTOR::elabo3(const COMPONENT* d)
{
  EVAL_BM_SEMI_BASE::elabo3(d);

  const MODEL_SEMI_RESISTOR* m 
    = dynamic_cast<const MODEL_SEMI_RESISTOR*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + ": " + "model " + modelname()
	  + " is not a semi-resistor (R)\n");
  }
  double width = (_width == NOT_INPUT) ? m->_defw : _width;
  double eff_width = width - m->_narrow;
  double eff_length = _length - m->_narrow;
  if (eff_width <= 0.) {
    untested();
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective width is negative or zero\n");
  }
  if (eff_length <= 0.) {
    error(bWARNING, d->long_label() + ": " + modelname()
	  + ": effective length is negative or zero\n");
  }

  if (eff_width != 0.) {
    _value = m->_rsh * eff_length / eff_width;
  }else{
    untested();
    _value = BIGBIG;
  }

  double tempdiff = (_temp_c - m->_tnom_c);
  _value *= 1 + m->_tc1*tempdiff + m->_tc2*tempdiff*tempdiff;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_BASE::_default_narrow = 0.;
double const MODEL_SEMI_BASE::_default_defw = 1e-6;
double const MODEL_SEMI_BASE::_default_tc1 = 0.;
double const MODEL_SEMI_BASE::_default_tc2 = 0.;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_BASE::MODEL_SEMI_BASE()
  :MODEL_CARD(),
   _narrow(_default_narrow),
   _defw(_default_defw),
   _tc1(_default_tc1),
   _tc2(_default_tc2)
{
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_BASE::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "NARROW", &_narrow)
    || get(cmd, "DEFW", &_defw)
    || get(cmd, "TC1", &_tc1)
    || get(cmd, "TC2", &_tc2)
    || MODEL_CARD::parse_params(cmd);
    ;
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_BASE::print_params(OMSTREAM& o)const
{
  o << " narrow="<< _narrow;
  o << " defw="  << _defw;
  o << " tc1="	 << _tc1;
  o << " tc2="   << _tc2;
  MODEL_CARD::print_params(o);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_BASE::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* s = scope();
    assert(s);
    MODEL_CARD::elabo1();
    _narrow.e_val(_default_narrow, s);
    _defw.e_val(_default_defw, s);
    _tc1.e_val(_default_tc1, s);
    _tc2.e_val(_default_tc2, s);
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_CAPACITOR::_default_cj = 0.;
double const MODEL_SEMI_CAPACITOR::_default_cjsw = 0.;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_CAPACITOR::MODEL_SEMI_CAPACITOR()
  :MODEL_SEMI_BASE(),
   _cj(_default_cj),
   _cjsw(_default_cjsw)
{
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_CAPACITOR::parse_front(CS& cmd)
{
  bool dummy;
  return set(cmd, "Cap", &dummy, true);
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_CAPACITOR::parse_params(CS& cmd)
{  return ONE_OF
    || get(cmd, "CJ", &_cj)
    || get(cmd, "CJSW", &_cjsw)
    || MODEL_SEMI_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::print_front(OMSTREAM& o)const
{
  o << " c";
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::print_params(OMSTREAM& o)const
{
  o << " cj=" << _cj;
  o << " cjsw=" << _cjsw;
  MODEL_SEMI_BASE::print_params(o);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_CAPACITOR::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* s = scope();
    assert(s);
    MODEL_SEMI_BASE::elabo1();
    _cj.e_val(_default_cj, s);
    _cjsw.e_val(_default_cjsw, s);
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double const MODEL_SEMI_RESISTOR::_default_rsh = NOT_INPUT;
/*--------------------------------------------------------------------------*/
MODEL_SEMI_RESISTOR::MODEL_SEMI_RESISTOR()
  :MODEL_SEMI_BASE(),
   _rsh(_default_rsh)
{
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_RESISTOR::parse_front(CS& cmd)
{
  bool dummy;
  return set(cmd, "Res", &dummy, true);
}
/*--------------------------------------------------------------------------*/
bool MODEL_SEMI_RESISTOR::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "RSH", &_rsh)
    || MODEL_SEMI_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::print_front(OMSTREAM& o)const
{
  o << " r";
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::print_params(OMSTREAM& o)const
{
  o << " rsh=" << _rsh;
  MODEL_SEMI_BASE::print_params(o);
}
/*--------------------------------------------------------------------------*/
void MODEL_SEMI_RESISTOR::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_SEMI_BASE::elabo1();
    _rsh.e_val(_default_rsh, par_scope);
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
