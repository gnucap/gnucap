/*$Id: bm.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
 * behavioral modeling action base
 */
//testing=script 2006.07.13
#include "u_lang.h"
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_bandwidth	(NOT_INPUT);
const double _default_delay	(0.);
const double _default_phase	(0.);
const double _default_ooffset	(0.);
const double _default_ioffset	(0.);
const double _default_scale	(1.);
const double _default_tc1	(0.);
const double _default_tc2	(0.);
const double _default_ic	(0.);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_ACTION_BASE::EVAL_BM_ACTION_BASE(int c)
  :EVAL_BM_BASE(c),
   _bandwidth(_default_bandwidth),
   _delay(_default_delay),
   _phase(_default_phase),
   _ooffset(_default_ooffset),
   _ioffset(_default_ioffset),
   _scale(_default_scale),
   _tc1(_default_tc1),
   _tc2(_default_tc2),
   _ic(_default_ic)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_ACTION_BASE::EVAL_BM_ACTION_BASE(const EVAL_BM_ACTION_BASE& p)
  :EVAL_BM_BASE(p),
   _bandwidth(p._bandwidth),
   _delay(p._delay),
   _phase(p._phase),
   _ooffset(p._ooffset),
   _ioffset(p._ioffset),
   _scale(p._scale),
   _tc1(p._tc1),
   _tc2(p._tc2),
   _ic(p._ic)
{
}
/*--------------------------------------------------------------------------*/
double EVAL_BM_ACTION_BASE::temp_adjust()const
{
  double tempdiff = _temp_c - _tnom_c;
  return (_scale * (1 + _tc1*tempdiff + _tc2*tempdiff*tempdiff));
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::tr_final_adjust(FPOLY1* y, bool f_is_value)const
{
  if (f_is_value) {
    y->f1 = y->f0;
    y->f0 = 0.;
  }else{
  }
  *y *= temp_adjust();
  y->f0 += _ooffset;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::tr_finish_tdv(ELEMENT* d, double val)const
{
  d->_y[0] = FPOLY1(CPOLY1(ioffset(d->_y[0].x), 0., val));
  tr_final_adjust(&(d->_y[0]), false);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_final_adjust(COMPLEX* y)const
{
  if (_bandwidth != NOT_INPUT && _bandwidth != 0.) {untested();
    assert(y->imag() == 0);
    double ratio = CKT_BASE::_sim->_freq / _bandwidth;
    double coeff = y->real() / (1.+(ratio*ratio));
    *y = COMPLEX(coeff, -coeff * ratio);
  }else{
  }
  
  if (_phase != 0.) {itested();
    *y *= std::polar(1., _phase*DTOR);
  }else{
  }

  if (_delay != 0.) {untested();
    double ratio = CKT_BASE::_sim->_freq * _delay;
    if (ratio > 100000.) {untested();
      error(bPICKY, "delay too long\n");
      ratio = 0.;
    }else{untested();
    }
    *y *= std::polar(1., -360.0 * DTOR * ratio);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_final_adjust_with_temp(COMPLEX* y)const
{
  *y *= temp_adjust();
  ac_final_adjust(y);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_ACTION_BASE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_ACTION_BASE* p = dynamic_cast<const EVAL_BM_ACTION_BASE*>(&x);
  bool rv = p
    && _bandwidth == p->_bandwidth
    && _delay == p->_delay
    && _phase == p->_phase
    && _ooffset == p->_ooffset
    && _ioffset == p->_ioffset
    && _scale == p->_scale
    && _tc1 == p->_tc1
    && _tc2 == p->_tc2
    && _ic == p->_ic
    && EVAL_BM_BASE::operator==(x);
  if (rv) {untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  print_pair(o, lang, "bandwidth",_bandwidth,_bandwidth.has_hard_value());
  print_pair(o, lang, "delay",	  _delay,    _delay.has_hard_value());
  print_pair(o, lang, "phase",	  _phase,    _phase.has_hard_value());
  print_pair(o, lang, "ioffset",  _ioffset,  _ioffset.has_hard_value());
  print_pair(o, lang, "ooffset",  _ooffset,  _ooffset.has_hard_value());
  print_pair(o, lang, "scale",	  _scale,    _scale.has_hard_value());
  print_pair(o, lang, "tc1",	  _tc1,	     _tc1.has_hard_value());
  print_pair(o, lang, "tc2",	  _tc2,	     _tc2.has_hard_value());
  print_pair(o, lang, "ic",	  _ic,	     _ic.has_hard_value());
  COMMON_COMPONENT::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_first(Scope);
  _bandwidth.e_val(_default_bandwidth, Scope);
  _delay.e_val(_default_delay, Scope);
  _phase.e_val(_default_phase, Scope);
  _ooffset.e_val(_default_ooffset, Scope);
  _ioffset.e_val(_default_ioffset, Scope);
  _scale.e_val(_default_scale, Scope);
  _tc1.e_val(_default_tc1, Scope);
  _tc2.e_val(_default_tc2, Scope);
  _ic.e_val(_default_ic, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_eval(ELEMENT* d)const 
{
  tr_eval(d);
  d->_ev = d->_y[0].f1;
  ac_final_adjust(&(d->_ev));
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "bandwidth",&_bandwidth)
    || Get(cmd, "delay",    &_delay)
    || Get(cmd, "phase",    &_phase)
    || Get(cmd, "ioffset",  &_ioffset)
    || Get(cmd, "ooffset",  &_ooffset)
    || Get(cmd, "scale",    &_scale)
    || Get(cmd, "tc1",      &_tc1)
    || Get(cmd, "tc2",      &_tc2)
    || Get(cmd, "ic",       &_ic)
    || COMMON_COMPONENT::parse_params_obsolete_callback(cmd);
    ;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_ACTION_BASE::has_ext_args()const
{
  return  (_bandwidth.has_hard_value()
	   || _delay.has_hard_value()
	   || _phase.has_hard_value()
	   || _ooffset.has_hard_value()
	   || _ioffset.has_hard_value()
	   || _scale.has_hard_value()
	   || _tc1.has_hard_value()
	   || _tc2.has_hard_value()
	   || _ic.has_hard_value());
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* EVAL_BM_ACTION_BASE::parse_func_type(CS& cmd)
{
  const COMMON_COMPONENT* p = 
    (cmd.is_float() || cmd.match1('_') || cmd.skip1b('='))
    ? bm_dispatcher["eval_bm_value"]
    : bm_dispatcher[cmd];

  if (p) {
    p->skip_type_tail(cmd);
    return p->clone();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
