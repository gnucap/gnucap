/*$Id: bm_exp.cc,v 22.19 2002/09/26 04:54:38 al Exp $ -*- C++ -*-
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
 * SPICE compatible EXP
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_iv    (NOT_INPUT);
const double _default_pv    (NOT_INPUT);
const double _default_td1   (0);
const double _default_tau1  (0);
const double _default_td2   (0);
const double _default_tau2  (0);
const double _default_period(BIGBIG);
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
void EVAL_BM_EXP::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    double* i;
    for (i = &_iv;  i < &_end;  ++i){
      double value=NOT_VALID;
      cmd >> value;
      if (cmd.stuck(&here)){
	break;
      }
      *i = value;
    }
    assert(i <= &_end);
    paren -= cmd.skiprparen();
    if (paren != 0){
      untested();
      cmd.warn(bWARNING, "need )");
    }
    get(cmd, "IV",	&_iv);
    get(cmd, "PV",	&_pv);
    get(cmd, "TD1",	&_td1);
    get(cmd, "TAU1",	&_tau1);
    get(cmd, "TD2",	&_td2);
    get(cmd, "TAU2",	&_tau2);
    get(cmd, "PEriod",	&_period);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  if (_period == 0.) {
    _period = _default_period;
  }
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXP::print(OMSTREAM& o)const
{
  o << "  " << name()
    << "  iv="	 << _iv
    << "  pv="	 << _pv
    << "  td1="	 << _td1
    << "  tau1=" << _tau1
    << "  td2="	 << _td2
    << "  tau2=" << _tau2;
  if (_period != _default_period) {
    o << "  period=" << _period;
  }
  print_base(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_EXP::tr_eval(ELEMENT* d)const
{
  double ev = _iv;
  for (double time = SIM::time0;  time >= 0;  time -= _period){
    if (time > _td1){
      ev += (_pv - _iv) * (1. - exp(-(time-_td1)/_tau1));
    }
    if (time > _td2){
      ev += (_iv - _pv) * (1. - exp(-(time-_td2)/_tau2));
    }
  }
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
