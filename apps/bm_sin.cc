/*$Id: bm_sin.cc 2016/03/23 al $ -*- C++ -*-
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
 * SPICE compatible SIN
 */
//testing=script,complete 2005.10.07
#include "globals.h"
#include "e_elemnt.h"
#include "u_lang.h"
#include "l_denoise.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_frequency (NOT_INPUT);
const double _default_delay	(0);
const double _default_damping	(0);
const double _default_samples	(4);
const bool   _default_zero	(false);
const bool   _default_peak	(false);
/*--------------------------------------------------------------------------*/
class EVAL_BM_SIN : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _offset;
  PARAMETER<double> _amplitude;
  PARAMETER<double> _frequency;
  PARAMETER<double> _delay;
  PARAMETER<double> _damping;
  PARAMETER<double> _end;
  PARAMETER<double> _samples;
  PARAMETER<bool>   _zero;
  PARAMETER<bool>   _peak;
  mutable double _actual_frequency;
  explicit	EVAL_BM_SIN(const EVAL_BM_SIN& p);
public:
  explicit      EVAL_BM_SIN(int c=0);
		~EVAL_BM_SIN()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const override;
  COMMON_COMPONENT* clone()const override	{return new EVAL_BM_SIN(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const override;

  void		precalc_last(const CARD_LIST*) override;
  void		tr_eval(ELEMENT*)const override;
  TIME_PAIR	tr_review(COMPONENT*)const override;
  std::string	name()const override		{return "sin";}
  bool		ac_too()const override		{return false;}
  bool		parse_numlist(CS&) override;
  bool		parse_params_obsolete_callback(CS&) override;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN::EVAL_BM_SIN(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _frequency(_default_frequency),
   _delay(_default_delay),
   _damping(_default_damping),
   _end(NOT_VALID),
   _samples(_default_samples),
   _zero(_default_zero),
   _peak(_default_peak),
   _actual_frequency(0)
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
   _end(NOT_VALID),
   _samples(p._samples),
   _zero(p._zero),
   _peak(p._peak),
   _actual_frequency(p._actual_frequency)
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
    && _samples == p->_samples
    && _zero == p->_zero
    && _peak == p->_peak
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  print_pair(o, lang, "offset",	   _offset);
  print_pair(o, lang, "amplitude", _amplitude);
  print_pair(o, lang, "frequency", _frequency);
  print_pair(o, lang, "delay",     _delay);
  print_pair(o, lang, "damping",   _damping);
  print_pair(o, lang, "samples",   _samples, _samples.has_hard_value());
  print_pair(o, lang, "zero",      _zero,    _zero.has_hard_value());
  print_pair(o, lang, "peak",      _peak,    _peak.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  _offset.e_val(_default_offset, Scope);
  _amplitude.e_val(_default_amplitude, Scope);
  _frequency.e_val(_default_frequency, Scope);
  _delay.e_val(_default_delay, Scope);
  _damping.e_val(_default_damping, Scope);
  _samples.e_val(_default_samples, Scope);
  _zero.e_val(_default_zero, Scope);
  _peak.e_val(_default_peak, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SIN::tr_eval(ELEMENT* d)const
{
  if (!_frequency.has_hard_value()) {
    _actual_frequency = d->_sim->_freq;
  }else{
    _actual_frequency = _frequency;
  }

  double reltime = ioffset(d->_sim->_time0);
  double ev = _offset;
  if (reltime > _delay) {
    double x=_amplitude*fixzero(sin(M_TWO_PI*_actual_frequency*(reltime-_delay)),1.);
    if (_damping != 0.) {
      x *= exp(-(reltime-_delay)*_damping);
    }else{
    }
    ev += x;
  }else{
  }
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_SIN::tr_review(COMPONENT* d)const
{
  double reltime = ioffset(d->_sim->_time0) + 1e-6/_actual_frequency;

  if (reltime > _delay) {
    if (_peak && _zero) {
      d->_time_by.min_event(floor(reltime * 4 * _actual_frequency + 1) / (4 * _actual_frequency));
    }else if (_peak) {
      d->_time_by.min_event((floor(reltime*2*_actual_frequency+.5) + .5) / (2*_actual_frequency));
    }else if (_zero) {
      d->_time_by.min_event(floor(reltime * 2 * _actual_frequency + 1) / (2 * _actual_frequency));
    }else{
    }
    d->_time_by.min_error_estimate(d->_sim->_time0 + 1. / (_samples * _actual_frequency));
  }else{
    d->_time_by.min_event(_delay);
  }

  trace3("bm_sin", d->_sim->_time0, d->_time_by._event, d->_time_by._error_estimate);
  assert(d->_time_by._event >  d->_sim->_time0);
  assert(d->_time_by._error_estimate > d->_sim->_time0);
  assert(d->_time_by._event >  d->_sim->_time0 + d->_sim->_dtmin);
  assert(d->_time_by._error_estimate > d->_sim->_time0 + d->_sim->_dtmin);

  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SIN::parse_numlist(CS& cmd)
{
  size_t start = cmd.cursor();
  size_t here = cmd.cursor();
  for (PARAMETER<double>* i = &_offset;  i < &_end;  ++i) {
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
bool EVAL_BM_SIN::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "o{ffset}",	&_offset)
    || Get(cmd, "a{mplitude}",	&_amplitude)
    || Get(cmd, "f{requency}",	&_frequency)
    || Get(cmd, "de{lay}",	&_delay)
    || Get(cmd, "da{mping}",	&_damping)
    || Get(cmd, "sa{mples}",	&_samples)
    || Get(cmd, "ze{ro}", 	&_zero)
    || Get(cmd, "pe{ak}", 	&_peak)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_SIN p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "sin|sine", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
