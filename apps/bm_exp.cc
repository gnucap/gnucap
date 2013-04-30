/*$Id: bm_exp.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * SPICE compatible EXP
 */
//testing=script 2005.10.06
#include "e_elemnt.h"
#include "u_lang.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_iv    (NOT_INPUT);
const double _default_pv    (NOT_INPUT);
const double _default_td1   (0);
const double _default_tau1  (0);
const double _default_td2   (0);
const double _default_tau2  (0);
const double _default_period(BIGBIG);
/*--------------------------------------------------------------------------*/
class EVAL_BM_EXP : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _iv;	// initial value
  PARAMETER<double> _pv;	// pulsed value
  PARAMETER<double> _td1;	// rise delay
  PARAMETER<double> _tau1;	// rise time constant
  PARAMETER<double> _td2;	// fall delay
  PARAMETER<double> _tau2;	// fall time constant
  PARAMETER<double> _period;	// repeat period
  PARAMETER<double> _end;	// marks the end of the list
  explicit	EVAL_BM_EXP(const EVAL_BM_EXP& p);
public:
  explicit      EVAL_BM_EXP(int c=0);
		~EVAL_BM_EXP()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_EXP(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		tr_eval(ELEMENT*)const;
  TIME_PAIR	tr_review(COMPONENT*);
  std::string	name()const		{return "exp";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_EXP::EVAL_BM_EXP(int c)
  :EVAL_BM_ACTION_BASE(c),
   _iv(_default_iv),
   _pv(_default_pv),
   _td1(_default_td1),
   _tau1(_default_tau1),
   _td2(_default_td2),
   _tau2(_default_tau2),
   _period(_default_period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_EXP::EVAL_BM_EXP(const EVAL_BM_EXP& p)
  :EVAL_BM_ACTION_BASE(p),
   _iv(p._iv),
   _pv(p._pv),
   _td1(p._td1),
   _tau1(p._tau1),
   _td2(p._td2),
   _tau2(p._tau2),
   _period(p._period),
   _end(NOT_VALID)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXP::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_EXP* p = dynamic_cast<const EVAL_BM_EXP*>(&x);
  bool rv = p
    && _iv== p->_iv
    && _pv== p->_pv
    && _td1== p->_td1
    && _tau1== p->_tau1
    && _td2== p->_td2
    && _tau2== p->_tau2
    && _period== p->_period
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXP::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  print_pair(o, lang, "iv",    _iv);
  print_pair(o, lang, "pv",    _pv);
  print_pair(o, lang, "td1",   _td1);
  print_pair(o, lang, "tau1",  _tau1);
  print_pair(o, lang, "td2",   _td2);
  print_pair(o, lang, "tau2",  _tau2);
  print_pair(o, lang, "period",_period, _period.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXP::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _iv.e_val(_default_iv, Scope);
  _pv.e_val(_default_pv, Scope);
  _td1.e_val(_default_td1, Scope);
  _tau1.e_val(_default_tau1, Scope);
  _td2.e_val(_default_td2, Scope);
  _tau2.e_val(_default_tau2, Scope);
  _period.e_val(_default_period, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXP::tr_eval(ELEMENT* d)const
{
  double ev = _iv;
  for (double time = d->_sim->_time0;  time >= 0;  time -= _period) {
    if (time > _td1) {
      ev += (_pv - _iv) * (1. - exp(-(time-_td1)/_tau1));
    }else{
    }
    if (time > _td2) {
      ev += (_iv - _pv) * (1. - exp(-(time-_td2)/_tau2));
    }else{
    }
  }
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_EXP::tr_review(COMPONENT* d)
{
  double time = d->_sim->_time0;
  time += d->_sim->_dtmin * .01;  // hack to avoid duplicate events from numerical noise
  double raw_time = time;

  if (0 < _period && _period < BIGBIG) {
    time = fmod(time,_period);
  }else{
  }
  double time_offset = raw_time - time;

  double dt = NEVER;
  if (time > _td2) {
    d->_time_by.min_event(_period + time_offset);
    dt = (_tau2 > 0) ? _tau2 : NEVER;
  }else if (time > _td1) {
    d->_time_by.min_event(_td2 + time_offset);
    dt = (_tau1 > 0) ? _tau1 : NEVER;
  }else if (d->_sim->_time0 < _period) {
    d->_time_by.min_event(_td1 + time_offset);
    dt = NEVER;
  }else{
    d->_time_by.min_event(_td1 + time_offset);
    dt = (_tau2 > 0) ? _tau2 : NEVER;
  }
  d->_time_by.min_error_estimate(d->_sim->_time0 + dt);

  return d->_time_by;
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXP::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  for (PARAMETER<double>* i = &_iv;  i < &_end;  ++i) {
    PARAMETER<double> val(NOT_VALID);
    cmd >> val;
    if (cmd.stuck(&here)) {
      break;
    }else{
      *i = val;
      untested();
    }
  }
  if (cmd.gotit(start)) {
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_EXP::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "iv",	  &_iv)
    || Get(cmd, "pv",	  &_pv)
    || Get(cmd, "td1",	  &_td1)
    || Get(cmd, "tau1",   &_tau1)
    || Get(cmd, "td2",	  &_td2)
    || Get(cmd, "tau2",   &_tau2)
    || Get(cmd, "period", &_period)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_EXP p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "exp", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
