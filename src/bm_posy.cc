/*$Id: bm_posy.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling - posynomial (non-integer powers)
 * pair ...  first is key, second is value
 */
//testing=script 2005.10.06
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_max(BIGBIG);
const double _default_min(-BIGBIG);
const bool   _default_abs(false);
const bool   _default_odd(false);
const bool   _default_even(false);
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY::EVAL_BM_POSY(int c)
  :EVAL_BM_ACTION_BASE(c),
   _min(_default_min),
   _max(_default_max),
   _abs(_default_abs),
   _odd(_default_odd),
   _even(_default_even),
   _table()
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY::EVAL_BM_POSY(const EVAL_BM_POSY& p)
  :EVAL_BM_ACTION_BASE(p),
   _min(p._min),
   _max(p._max),
   _abs(p._abs),
   _odd(p._odd),
   _even(p._even),
   _table(p._table)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POSY::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_POSY* p = dynamic_cast<const EVAL_BM_POSY*>(&x);
  bool rv = p
    && _min == p->_min
    && _max == p->_max
    && _abs == p->_abs
    && _odd == p->_odd
    && _even == p->_even
    && _table == p->_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::print(OMSTREAM& o)const
{
  o << ' ' << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->second << ',' << p->first << ' ';
  }
  o << ')';
  if (_min.has_value())  {o << " min="  << _min;  untested();}
  if (_max.has_value())  {o << " max="  << _max;  untested();}
  if (_abs.has_value())  {o << " abs="  << _abs;  untested();}
  if (_odd.has_value())  {o << " odd="  << _odd;}
  if (_even.has_value()) {o << " even=" << _even;  untested();}
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _min.e_val(_default_min, par_scope);
  _max.e_val(_default_max, par_scope);
  _abs.e_val(_default_abs, par_scope);
  _odd.e_val(_default_odd, par_scope);
  _even.e_val(_default_even, par_scope);
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 iterator p = _table.begin();  p != _table.end();  ++p) {
    p->first.e_val(0, par_scope);
    p->second.e_val(0, par_scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::tr_eval(ELEMENT* d)const
{
  double x_raw = ioffset(d->_y0.x);
  trace1("before", x_raw);
  double x = (x_raw < 0)
    ? ((_odd || _even) ? -x_raw : 0.)
    : x_raw;
  assert(x >= 0);
  trace1("rev", x);

  double f0 = 0.;
  double f1 = 0.;
  if (x > 0) {
    for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	   const_iterator p = _table.begin();  p != _table.end();  ++p) {
      double coeff = p->second * pow(x, p->first-1);
      f1 += coeff * p->first;
      f0 += coeff * x;
    }
  }else{
    assert(x == 0);
    for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	   const_iterator p = _table.begin();  p != _table.end();  ++p) {
      if (p->first == 0) {
	f0 += 1;
	untested();
      }
    }
  }

  trace3("", x, f0, f1);

  if (_odd && x_raw < 0) {
    f0 = -f0;
  }
  if (_even && x_raw < 0) {
    f1 = -f1;
    untested();
  }
  trace3("after", x, f0, f1);

  if (_abs && f0 < 0) {
    f0 = -f0;
    f1 = -f1;
    untested();
  }

  if (f0 > _max) {
    f0 = _max;
    f1 = 0;
    untested();
  }else if (f0 < _min) {
    f0 = _min;
    f1 = 0;
    untested();
  }

  d->_y0 = FPOLY1(x_raw, f0, f1);
  tr_final_adjust(&(d->_y0), d->f_is_value());
  trace3("fa", d->_y0.x, d->_y0.f0, d->_y0.f1);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POSY::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (;;) {
    int start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    cmd >> p.second; // value
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      cmd >> p.first; // key
      if (cmd.stuck(&here)) {
	// ran out, but already have half of the pair
	// back up one, hoping somebody else knows what to do with it
	cmd.reset(start_of_pair);
	break;
      }else{
	_table.push_back(p);
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
bool EVAL_BM_POSY::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "MIn",  &_min)
    || get(cmd, "MAx",  &_max)
    || get(cmd, "Abs",  &_abs)
    || get(cmd, "Odd",  &_odd)
    || get(cmd, "Even", &_even)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
