/*$Id: bm_pulse.cc 2016/09/15 al $ -*- C++ -*-
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
 * SPICE compatible PULSE
 */
//testing=script 2016.09.15
#include "globals.h"
#include "e_elemnt.h"
#include "u_lang.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_iv    (NOT_INPUT);
const double _default_pv    (NOT_INPUT);
const double _default_delay (0);
const double _default_rise  (0);
const double _default_fall  (0);
const double _default_width (BIGBIG);
const double _default_period(BIGBIG);
/*--------------------------------------------------------------------------*/
class EVAL_BM_PULSE : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _iv;
  PARAMETER<double> _pv;
  PARAMETER<double> _delay;
  PARAMETER<double> _rise;
  PARAMETER<double> _fall;
  PARAMETER<double> _width;
  PARAMETER<double> _period;
  PARAMETER<double> _end;
  explicit	EVAL_BM_PULSE(const EVAL_BM_PULSE& p);
public:
  explicit      EVAL_BM_PULSE(int c=0);
		~EVAL_BM_PULSE()	{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const override;
  COMMON_COMPONENT* clone()const override	{return new EVAL_BM_PULSE(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const override;

  void		precalc_last(const CARD_LIST*)override;
  void		tr_eval(ELEMENT*)const override;
  TIME_PAIR	tr_review(COMPONENT*)const override;
  std::string	name()const override		{return "pulse";}
  bool		ac_too()const override		{return false;}
  bool		parse_numlist(CS&)override;
  bool		parse_params_obsolete_callback(CS&)override;
};
/*--------------------------------------------------------------------------*/
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
  if (rv) {untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  print_pair(o, lang, "iv", _iv);
  print_pair(o, lang, "pv", _pv);
  print_pair(o, lang, "delay", _delay);
  print_pair(o, lang, "rise", _rise);
  print_pair(o, lang, "fall", _fall);
  print_pair(o, lang, "width", _width);
  print_pair(o, lang, "period", _period);
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  _iv.e_val(_default_iv, Scope);
  _pv.e_val(_default_pv, Scope);
  _delay.e_val(_default_delay, Scope);
  _rise.e_val(_default_rise, Scope);
  _fall.e_val(_default_fall, Scope);
  _width.e_val(_default_width, Scope);
  _period.e_val(_default_period, Scope);

  if (_width == 0.) {
    _width = _default_width;
  }else{
  }
  if (_period == 0.) {
    _period = _default_period;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_PULSE::tr_eval(ELEMENT* d)const
{
  double ev = 0; // effective value

  double raw_time = d->_sim->_time0;

  if (raw_time <= _delay) {			/* init val	*/
    ev = _iv;
  }else{
    double reltime;
    if (0 < _period  &&  _period < BIGBIG) {
      reltime = fmod(raw_time - _delay, _period);
    }else{
      reltime = raw_time - _delay;
    }

    if (reltime < _rise) {				/* rising 	*/
      double interp = reltime / _rise;
      ev = _iv + interp * (_pv - _iv);
    }else if (reltime <= _rise + _width) {		/* pulse val 	*/
      ev = _pv;
    }else if (reltime <  _rise + _width + _fall) {	/* falling 	*/
      double interp = (reltime - (_rise+_width)) / _fall;
      ev = _pv + interp * (_iv - _pv);
    }else{						/* past pulse	*/
      ev = _iv;
    }
  }

  //d->q_accept();
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_PULSE::tr_review(COMPONENT* d)const
{
  double raw_time = d->_sim->_time0 + d->_sim->_dtmin * .01;
				    // hack to avoid duplicate events from numerical noise

  if (raw_time <= _delay) {
    d->_time_by.min_event(_delay);
  }else{
    double reltime;
    if (0 < _period && _period < BIGBIG) {
      reltime = fmod(raw_time - _delay, _period);
    }else{
      reltime = raw_time - _delay;
    }
    double time_offset = raw_time - reltime;
    
    if (reltime < _rise) {				/* rising 	*/
      d->_time_by.min_event(_rise + time_offset);
    }else if (reltime < _rise + _width) {		/* pulse val 	*/
      d->_time_by.min_event(_rise + _width + time_offset);
    }else if (reltime < _rise + _width + _fall) {	/* falling 	*/
      d->_time_by.min_event(_rise + _width + _fall + time_offset);
    }else{						/* past pulse	*/
      d->_time_by.min_event(_period + time_offset);
    }
  }

  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PULSE::parse_numlist(CS& cmd)
{
  size_t start = cmd.cursor();
  size_t here = cmd.cursor();
  for (PARAMETER<double>* i = &_iv;  i < &_end;  ++i) {
    PARAMETER<double> val(NOT_VALID);
    cmd >> val;
    if (cmd.stuck(&here)) {
      break;
    }else{
      *i = val;
    }
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_PULSE::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "iv", 	  &_iv)
    || Get(cmd, "pv", 	  &_pv)
    || Get(cmd, "delay",  &_delay)
    || Get(cmd, "rise",   &_rise)
    || Get(cmd, "fall",   &_fall)
    || Get(cmd, "width",  &_width)
    || Get(cmd, "period", &_period)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_PULSE p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "pulse", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
