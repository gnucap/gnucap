/*$Id: bm_posy.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * behavioral modeling - posynomial (non-integer powers)
 * pair ...  first is key, second is value
 */
#include "ap.h"
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
   _max(_default_max),
   _min(_default_min),
   _abs(_default_abs),
   _odd(_default_odd),
   _even(_default_even)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY::EVAL_BM_POSY(const EVAL_BM_POSY& p)
  :EVAL_BM_ACTION_BASE(p),
   _max(p._max),
   _min(p._min),
   _abs(p._abs),
   _odd(p._odd),
   _even(p._even),
   _table(p._table)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    for (;;){
      double key  =NOT_VALID;
      double value=NOT_VALID;
      cmd >> value >> key;
      if (cmd.stuck(&here)){
	break;
      }
      std::pair<double,double> p(key,value);
      _table.push_back(p);
    }
    paren -= cmd.skiprparen();
    if (paren != 0){
      untested();
      cmd.warn(bWARNING, "need )");
    }
    get(cmd, "MIn",  &_min);
    get(cmd, "MAx",  &_max);
    get(cmd, "Abs",  &_abs);
    get(cmd, "Odd",  &_odd);
    get(cmd, "Even", &_even);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::print(OMSTREAM& where)const
{
  where << "  " << name() << '(';
  for (std::vector<std::pair<double,double> >::const_iterator
	 p = _table.begin();  p != _table.end();  ++p){
    where << "  " << p->second << ',' << p->first;
  }
  where << ')';
  print_base(where);
  if (_min  != _default_min)  {where << "  min="  << _min;}
  if (_max  != _default_max)  {where << "  max="  << _max;}
  if (_abs  != _default_abs)  {where << "  abs="  << _abs;}
  if (_odd  != _default_odd)  {where << "  odd="  << _odd;}
  if (_even != _default_even) {where << "  even=" << _even;}
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
  {if (x > 0) {
    for (std::vector<std::pair<double,double> >::const_iterator
	   p = _table.begin();  p != _table.end();  ++p){
      double coeff = p->second * pow(x, p->first-1);
      f1 += coeff * p->first;
      f0 += coeff * x;
    }
  }else{
    assert(x == 0);
    for (std::vector<std::pair<double,double> >::const_iterator
	   p = _table.begin();  p != _table.end();  ++p){
      if (p->first == 0) {
	untested();
	f0 += 1;
      }
    }
  }}

  trace3("", x, f0, f1);

  if (_odd && x_raw < 0) {
    f0 = -f0;
  }
  if (_even && x_raw < 0) {
    f1 = -f1;
  }
  trace3("after", x, f0, f1);

  if (_abs && f0 < 0){
    untested();
    f0 = -f0;
    f1 = -f1;
  }

  {if (f0 > _max){
    untested();
    f0 = _max;
    f1 = 0;
  }else if (f0 < _min){
    untested();
    f0 = _min;
    f1 = 0;
  }}

  d->_y0 = FPOLY1(x_raw, f0, f1);
  tr_final_adjust(&(d->_y0), d->f_is_value());
  trace3("fa", d->_y0.x, d->_y0.f0, d->_y0.f1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
