/*$Id: bm.cc,v 22.21 2002/10/06 07:21:50 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * behavioral modeling action base
 */
#include "l_stlextra.h"
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_bandwidth	(NOT_INPUT);
const double _default_delay	(0);
const double _default_phase	(0);
const double _default_ooffset	(0);
const double _default_ioffset	(0);
const double _default_scale	(1.);
const double _default_tc1	(0);
const double _default_tc2	(0);
const double _default_ic	(NOT_INPUT);
/*--------------------------------------------------------------------------*/
class EVAL_DISPATCHER {
private:
  std::map<std::string, COMMON_COMPONENT*> _map;
public:
  explicit EVAL_DISPATCHER();
  COMMON_COMPONENT* operator()(CS& cmd);
};
/*--------------------------------------------------------------------------*/
static EVAL_DISPATCHER bm_dispatcher;
/*--------------------------------------------------------------------------*/
EVAL_DISPATCHER::EVAL_DISPATCHER()
{
  _map["complex"]  = new EVAL_BM_COMPLEX;
  _map["exp"]	   = new EVAL_BM_EXP;
  _map["fit"]	   = new EVAL_BM_FIT;
  _map["generator"] =
    _map["gen"]    = new EVAL_BM_GENERATOR;
  _map["poly"]     = new EVAL_BM_POLY;
  _map["posy"]     = new EVAL_BM_POSY;
  _map["pulse"]    = new EVAL_BM_PULSE;
  _map["pwl"]	   = new EVAL_BM_PWL;
  _map["sffm"]     = new EVAL_BM_SFFM;
  _map["sin"]	   = new EVAL_BM_SIN;
  _map["tanh"]     = new EVAL_BM_TANH;
  _map["value"]    = new EVAL_BM_VALUE;
}
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* EVAL_DISPATCHER::operator()(CS& cmd)
{
  {if (cmd.is_float( )) {
    return new EVAL_BM_VALUE;
  }else{
    int here = cmd.cursor();
    std::string s;
    cmd >> s;
    notstd::to_lower(&s);
    const COMMON_COMPONENT* p = _map[s];
    {if (p) {
      if (dynamic_cast<const EVAL_BM_POLY*>(p)
	  || dynamic_cast<const EVAL_BM_PWL*>(p)) {
	cmd.ematch("(1)"); // HSpice compatibility kluge
      }
      return p->clone();
    }else{
      cmd.reset(here);
      return 0;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* EVAL_BM_ACTION_BASE::parse_func_type(CS& cmd)
{
  return bm_dispatcher(cmd);
}
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
   _ic(_default_ic),
   _needs_ac_eval(false),
   _has_ext_args(false)
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
   _ic(p._ic),
   _needs_ac_eval(p._needs_ac_eval),
   _has_ext_args(p._has_ext_args)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::parse_base(CS& cmd)
{
  get(cmd, "Bandwidth",&_bandwidth);
  get(cmd, "Delay",    &_delay);
  get(cmd, "Phase",    &_phase);
  get(cmd, "IOffset",  &_ioffset);
  get(cmd, "OOffset",  &_ooffset);
  get(cmd, "Scale",    &_scale);
  get(cmd, "TNOM",     &_tnom, mOFFSET, -ABS_ZERO);
  get(cmd, "TC1",      &_tc1);
  get(cmd, "TC2",      &_tc2);
  get(cmd, "IC",       &_ic);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::parse_base_finish()
{
  {if (_tnom == NOT_INPUT){
    _tnom = OPT::tnom;
  }else{
    untested();
  }}
  _needs_ac_eval = (_bandwidth != _default_bandwidth 
		    || _delay != _default_delay 
		    || _phase != _default_phase);
  _has_ext_args = (_needs_ac_eval
		   || _ooffset != _default_ooffset
		   || _ioffset != _default_ioffset
		   || _scale != _default_scale
		   || _tc1 != _default_tc1
		   || _tc2 != _default_tc2
		   || _ic != _default_ic);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::print_base(OMSTREAM& where)const
{
  if(_bandwidth!= _default_bandwidth){where<<"  bandwidth="<<_bandwidth;}
  if(_delay    != _default_delay)    {where<<"  delay="	   <<_delay;}
  if(_phase    != _default_phase)    {where<<"  phase="	   <<_phase;}
  if(_ioffset  != _default_ioffset)  {where<<"  ioffset="  <<_ioffset;}
  if(_ooffset  != _default_ooffset)  {where<<"  ooffset="  <<_ooffset;}
  if(_scale    != _default_scale)    {where<<"  scale="	   <<_scale;}
  if(_tnom     != OPT::tnom)	     {where<<"  tnom="	   <<_tnom+ABS_ZERO;}
  if(_tc1      != _default_tc1)	     {where<<"  tc1="	   <<_tc1;}
  if(_tc2      != _default_tc2)	     {where<<"  tc2="	   <<_tc2;}
  if(_ic       != _default_ic)	     {where<<"  ic="	   <<_ic;}
}
/*--------------------------------------------------------------------------*/
double EVAL_BM_ACTION_BASE::temp_adjust()const
{
  double tempdiff = SIM::temp - _tnom;
  return (_scale * (1 + _tc1*tempdiff + _tc2*tempdiff*tempdiff));
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::tr_final_adjust(FPOLY1* y, bool f_is_value)const
{
  if (f_is_value){
    y->f1 = y->f0;
    y->f0 = 0.;
  }
  *y *= temp_adjust();
  y->f0 += _ooffset;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::tr_finish_tdv(ELEMENT* d, double val)const
{
  d->_y0 = FPOLY1(CPOLY1(ioffset(d->_y0.x), 0., val));
  tr_final_adjust(&(d->_y0), false);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_final_adjust(COMPLEX* y)const
{
  if (_bandwidth != NOT_INPUT && _bandwidth != 0.){
    untested();
    assert(y->imag() == 0);
    double ratio = SIM::freq / _bandwidth;
    double coeff = y->real() / (1.+(ratio*ratio));
    *y = COMPLEX(coeff, -coeff * ratio);
  }
  
  if (_phase != 0.){
    untested();
    *y *= std::polar(1., _phase*DTOR);
  }

  if (_delay != 0.){
    untested();
    double ratio = SIM::freq * _delay;
    if (ratio > 100000.){
      error(bPICKY, "delay too long\n");
      ratio = 0.;
    }
    *y *= std::polar(1., -360.0 * DTOR * ratio);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_final_adjust_with_temp(COMPLEX* y)const
{
  *y *= temp_adjust();
  ac_final_adjust(y);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_ACTION_BASE::ac_eval(ELEMENT* d)const 
{
  tr_eval(d);
  d->_ev = d->_y0.f1;
  ac_final_adjust(&(d->_ev));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
