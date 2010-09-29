/*$Id: bm_pulse.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * SPICE compatible PULSE
 */
//testing=script 2005.10.06
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_iv    (NOT_INPUT);
const double _default_pv    (NOT_INPUT);
const double _default_delay (0);
const double _default_rise  (0);
const double _default_fall  (0);
const double _default_width (BIGBIG);
const double _default_period(BIGBIG);
/*--------------------------------------------------------------------------*/
EVAL_BM_PULSE::EVAL_BM_PULSE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _iv(_default_iv),
   _pv(_default_pv),
   _delay(_default_delay),
   _rise(_default_rise),
   _fall(_default_fall),
   _width(_default_width),
   _period(_default_period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_PULSE::EVAL_BM_PULSE(const EVAL_BM_PULSE& p)
  :EVAL_BM_ACTION_BASE(p),
   _iv(p._iv),
   _pv(p._pv),
   _delay(p._delay),
   _rise(p._rise),
   _fall(p._fall),
   _width(p._width),
   _period(p._period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PULSE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_PULSE* p = dynamic_cast<const EVAL_BM_PULSE*>(&x);
  bool rv = p
    && _iv == p->_iv
    && _pv == p->_pv
    && _delay == p->_delay
    && _rise == p->_rise
    && _fall == p->_fall
    && _width == p->_width
    && _period == p->_period
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::print(OMSTREAM& o)const
{
  o << ' ' << name()
    << " iv="	  << _iv
    << " pv="	  << _pv
    << " delay="  << _delay
    << " rise="	  << _rise
    << " fall="	  << _fall
    << " width="  << _width
    << " period=" << _period;
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _iv.e_val(_default_iv, par_scope);
  _pv.e_val(_default_pv, par_scope);
  _delay.e_val(_default_delay, par_scope);
  _rise.e_val(_default_rise, par_scope);
  _fall.e_val(_default_fall, par_scope);
  _width.e_val(_default_width, par_scope);
  _period.e_val(_default_period, par_scope);

  if (_width == 0.) {
    _width = _default_width;
    untested();
  }
  if (_period == 0.) {
    _period = _default_period;
    untested();
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::tr_eval(ELEMENT* d)const
{
  double time = SIM::time0;
  if (0 < _period && _period < BIGBIG) {
    time = fmod(time,_period);
  }
  
  double ev = 0; // effective value
  if (time > _delay+_rise+_width+_fall) {	/* past pulse	*/
    ev = _iv;
  }else if (time > _delay+_rise+_width) {	/* falling 	*/
    double interp = (time - (_delay+_rise+_width)) / _fall;
    ev = _pv + interp * (_iv - _pv);
  }else if (time > _delay+_rise) {		/* pulse val 	*/
    ev = _pv;
  }else if (time > _delay) {			/* rising 	*/
    double interp = (time - _delay) / _rise;
    ev = _iv + interp * (_pv - _iv);
  }else{					/* init val	*/
    ev = _iv;
  }

  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PULSE::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (PARAMETER<double>* i = &_iv;  i < &_end;  ++i) {
    double value=NOT_VALID;
    cmd >> value;
    if (cmd.stuck(&here)) {
      break;
    }else{
      *i = value;
    }
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PULSE::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "IV", 	  &_iv)
    || get(cmd, "PV", 	  &_pv)
    || get(cmd, "Delay",  &_delay)
    || get(cmd, "Rise",	  &_rise)
    || get(cmd, "Fall",	  &_fall)
    || get(cmd, "Width",  &_width)
    || get(cmd, "PEriod", &_period)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
