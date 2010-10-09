/*$Id: bm_sffm.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * SPICE compatible SFFM
 */
//testing=script 2005.10.07
#include "e_elemnt.h"
#include "l_denoise.h"
#include "u_lang.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_carrier	(NOT_INPUT);
const double _default_modindex	(NOT_INPUT);
const double _default_signal	(NOT_INPUT);
const double _default_samples	(4);
const bool   _default_zero	(true);
const bool   _default_peak	(true);
/*--------------------------------------------------------------------------*/
class EVAL_BM_SFFM : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _offset;
  PARAMETER<double> _amplitude;
  PARAMETER<double> _carrier;
  PARAMETER<double> _modindex;
  PARAMETER<double> _signal;
  PARAMETER<double> _end;
  PARAMETER<double> _samples;
  PARAMETER<bool>   _zero;
  PARAMETER<bool>   _peak;
  explicit	EVAL_BM_SFFM(const EVAL_BM_SFFM& p);
public:
  explicit      EVAL_BM_SFFM(int c=0);
		~EVAL_BM_SFFM()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_SFFM(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		tr_eval(ELEMENT*)const;
  TIME_PAIR	tr_review(COMPONENT*);
  std::string	name()const		{return "sffm";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM::EVAL_BM_SFFM(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _carrier(_default_carrier),
   _modindex(_default_modindex),
   _signal(_default_signal),
   _end(NOT_VALID),
   _samples(_default_samples),
   _zero(_default_zero),
   _peak(_default_peak)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM::EVAL_BM_SFFM(const EVAL_BM_SFFM& p)
  :EVAL_BM_ACTION_BASE(p),
   _offset(p._offset),
   _amplitude(p._amplitude),
   _carrier(p._carrier),
   _modindex(p._modindex),
   _signal(p._signal),
   _end(NOT_VALID),
   _samples(p._samples),
   _zero(p._zero),
   _peak(p._peak)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_SFFM* p = dynamic_cast<const EVAL_BM_SFFM*>(&x);
  bool rv = p
    && _offset == p->_offset
    && _amplitude == p->_amplitude
    && _carrier == p->_carrier
    && _modindex == p->_modindex
    && _signal == p->_signal
    && _samples == p->_samples
    && _zero == p->_zero
    && _peak == p->_peak
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  print_pair(o, lang, "offset",    _offset);
  print_pair(o, lang, "amplitude", _amplitude);
  print_pair(o, lang, "carrier",   _carrier);
  print_pair(o, lang, "modindex",  _modindex);
  print_pair(o, lang, "signal",    _signal);
  print_pair(o, lang, "samples",   _samples, _samples.has_hard_value());
  print_pair(o, lang, "zero",      _zero,    _zero.has_hard_value());
  print_pair(o, lang, "peak",      _peak,    _peak.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _offset.e_val(_default_offset, Scope);
  _amplitude.e_val(_default_amplitude, Scope);
  _carrier.e_val(_default_carrier, Scope);
  _modindex.e_val(_default_modindex, Scope);
  _signal.e_val(_default_signal, Scope);
  _samples.e_val(_default_samples, Scope);
  _zero.e_val(_default_zero, Scope);
  _peak.e_val(_default_peak, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::tr_eval(ELEMENT* d)const
{
  double time = d->_sim->_time0;
  double mod = (_modindex * sin(M_TWO_PI * _signal * time));
  double ev = _offset + _amplitude * sin(M_TWO_PI * _carrier * time + mod);
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_SFFM::tr_review(COMPONENT* d)
{
  double time = d->_sim->_time0 + d->_sim->_dtmin * .01;
  double old_time;
  double N = 0;
  double old_N;
  trace2("", time, N);
  do {
    // simple fixed point iteration to find peaks and zero crossings
    old_time = time;
    old_N = N;
    double mod = (_modindex * sin(M_TWO_PI * _signal * time));
    double inst_freq = _carrier * (1 + mod / (M_TWO_PI * _carrier * time));
    if (N == 0) {
      if (_peak && _zero) {
	N = floor(time * 4 * inst_freq + 1);
      }else if (_peak) {untested();
	N = floor(time * 2 * inst_freq + .5) + .5;
      }else if (_zero) {untested();
	N = floor(time * 2 * inst_freq + 1);
      }else{
	N = 0;
      }
    }else{
    }
    if (_peak && _zero) {
      time = N / (4 * inst_freq);
    }else if (_peak) {untested();
      time = N / (2 * inst_freq);
    }else if (_zero) {untested();
      time = N / (2 * inst_freq);
    }else{
      time = NEVER;
    }
    trace2("", time, N);
  } while (std::abs(dn_diff(time, old_time)) > 0);

  d->_time_by.min_error_estimate(d->_sim->_time0 + 1. / (_samples * _carrier));
  d->_time_by.min_event(old_time);
  
  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  for (PARAMETER<double>* i = &_offset;  i < &_end;  ++i) {
    PARAMETER<double> val(NOT_VALID);
    cmd >> val;
    if (cmd.stuck(&here)) {
      break;
    }else{
      untested();
      *i = val;
    }
  }
  if (cmd.gotit(start)) {
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "offset",	&_offset)
    || Get(cmd, "amplitude",	&_amplitude)
    || Get(cmd, "carrier",	&_carrier)
    || Get(cmd, "modindex",	&_modindex)
    || Get(cmd, "signal",	&_signal)
    || Get(cmd, "samples",	&_samples)
    || Get(cmd, "zero", 	&_zero)
    || Get(cmd, "peak", 	&_peak)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "sffm", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
