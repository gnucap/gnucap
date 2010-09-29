/*$Id: bm_poly.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * HSPICE compatible POLY
 */
//testing=script,complete 2005.10.06
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_max(BIGBIG);
const double _default_min(-BIGBIG);
const bool   _default_abs(false);
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(int c)
  :EVAL_BM_ACTION_BASE(c),
   _min(_default_min),
   _max(_default_max),
   _abs(_default_abs)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(const EVAL_BM_POLY& p)
  :EVAL_BM_ACTION_BASE(p),
   _min(p._min),
   _max(p._max),
   _abs(p._abs),
   _c(p._c)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POLY::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_POLY* p = dynamic_cast<const EVAL_BM_POLY*>(&x);
  bool rv = p
    && _min == p->_min
    && _max == p->_max
    && _abs == p->_abs
    && _c == p->_c
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    incomplete();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::print(OMSTREAM& o)const
{
  o << ' ' << name() << '(';
  for (std::vector<PARAMETER<double> >::const_iterator
	 p = _c.begin();  p != _c.end();  ++p) {
    o << *p << ' ';
  }
  o << ')';
  if (_min.has_value()) {o << " min=" << _min;}
  if (_max.has_value()) {o << " max=" << _max;}
  if (_abs.has_value()) {o << " abs=" << _abs;}
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  for (std::vector<PARAMETER<double> >::const_iterator
	 p = _c.begin();  p != _c.end();  ++p) {
    (*p).e_val(0, par_scope);
  }
  _min.e_val(_default_min, par_scope);
  _max.e_val(_default_max, par_scope);
  _abs.e_val(_default_abs, par_scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::tr_eval(ELEMENT* d)const
{
  double x = ioffset(d->_y0.x);
  double f0 = 0.;
  double f1 = 0.;
  for (int i=_c.size()-1; i>0; --i) {
    f0 += _c[i];
    f0 *= x;
    f1 *= x;
    f1 += _c[i]*i;
  }
  f0 += _c[0];

  if (_abs && f0 < 0) {
    f0 = -f0;
    f1 = -f1;
  }

  if (f0 > _max) {
    f0 = _max;
    f1 = 0;
  }else if (f0 < _min) {
    f0 = _min;
    f1 = 0;
  }

  d->_y0 = FPOLY1(x, f0, f1);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POLY::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (;;) {
    int old_here = here;
    PARAMETER<double> value;
    cmd >> value;
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      if (cmd.match1('=')) {
	// got one that doesn't belong, back up
	cmd.reset(old_here);
	break;
      }else{
	_c.push_back(value);
      }
    }
  }
  if (cmd.gotit(start)) {
  }else{
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POLY::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "MIn", &_min)
    || get(cmd, "MAx", &_max)
    || get(cmd, "Abs", &_abs)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
