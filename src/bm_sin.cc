/*$Id: bm_sin.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * SPICE compatible SIN
 */
//testing=script,complete 2005.10.07
#include "l_denoise.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_frequency (NOT_INPUT);
const double _default_delay	(0);
const double _default_damping	(0);
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN::EVAL_BM_SIN(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _frequency(_default_frequency),
   _delay(_default_delay),
   _damping(_default_damping),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN::EVAL_BM_SIN(const EVAL_BM_SIN& p)
  :EVAL_BM_ACTION_BASE(p),
   _offset(p._offset),
   _amplitude(p._amplitude),
   _frequency(p._frequency),
   _delay(p._delay),
   _damping(p._damping),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SIN::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_SIN* p = dynamic_cast<const EVAL_BM_SIN*>(&x);
  bool rv = p
    && _offset == p->_offset
    && _amplitude == p->_amplitude
    && _frequency == p->_frequency
    && _delay == p->_delay
    && _damping == p->_damping
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::print(OMSTREAM& o)const
{
  o << ' ' << name()
    << " offset="    << _offset
    << " amplitude=" << _amplitude
    << " frequency=" << _frequency
    << " delay="     << _delay
    << " damping="   << _damping;
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _offset.e_val(_default_offset, par_scope);
  _amplitude.e_val(_default_amplitude, par_scope);
  _frequency.e_val(_default_frequency, par_scope);
  _delay.e_val(_default_delay, par_scope);
  _damping.e_val(_default_damping, par_scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::tr_eval(ELEMENT* d)const
{
  double reltime = ioffset(SIM::time0);
  double ev = _offset;
  if (reltime > _delay) {
    double x=_amplitude*fixzero(sin(M_TWO_PI*_frequency*(reltime-_delay)),1.);
    if (_damping != 0.) {
      x *= exp(-(reltime-_delay)*_damping);
    }
    ev += x;
  }
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SIN::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (PARAMETER<double>* i = &_offset;  i < &_end;  ++i) {
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
bool EVAL_BM_SIN::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Offset",	&_offset)
    || get(cmd, "Amplitude",	&_amplitude)
    || get(cmd, "Frequency",	&_frequency)
    || get(cmd, "DElay",	&_delay)
    || get(cmd, "DAmping",	&_damping)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
