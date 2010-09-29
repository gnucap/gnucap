/*$Id: bm_sffm.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * SPICE compatible SFFM
 */
//testing=script 2005.10.07
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_offset	(0);
const double _default_amplitude	(1);
const double _default_carrier	(NOT_INPUT);
const double _default_modindex	(NOT_INPUT);
const double _default_signal	(NOT_INPUT);
/*--------------------------------------------------------------------------*/
EVAL_BM_SFFM::EVAL_BM_SFFM(int c)
  :EVAL_BM_ACTION_BASE(c),
   _offset(_default_offset),
   _amplitude(_default_amplitude),
   _carrier(_default_carrier),
   _modindex(_default_modindex),
   _signal(_default_signal),
   _end(NOT_VALID)
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
   _end(NOT_VALID)
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
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::print(OMSTREAM& o)const
{
  o << ' ' << name()
    << " offset="    << _offset
    << " amplitude=" << _amplitude
    << " carrier="   << _carrier
    << " modindex="  << _modindex
    << " signal="    << _signal;
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _offset.e_val(_default_offset, par_scope);
  _amplitude.e_val(_default_amplitude, par_scope);
  _carrier.e_val(_default_carrier, par_scope);
  _modindex.e_val(_default_modindex, par_scope);
  _signal.e_val(_default_signal, par_scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_SFFM::tr_eval(ELEMENT* d)const
{
  double time = SIM::time0;
  double mod = (_modindex * sin(M_TWO_PI * _signal * time));
  double ev = _offset + _amplitude * sin(M_TWO_PI * _carrier * time + mod);
  tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (PARAMETER<double>* i = &_offset;  i < &_end;  ++i) {
    double value=NOT_VALID;
    cmd >> value;
    if (cmd.stuck(&here)) {
      break;
    }else{
      untested();
      *i = value;
    }
  }
  if (cmd.gotit(start)) {
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_SFFM::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Offset",	&_offset)
    || get(cmd, "Amplitude",	&_amplitude)
    || get(cmd, "Carrier",	&_carrier)
    || get(cmd, "Modindex",	&_modindex)
    || get(cmd, "Signal",	&_signal)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
