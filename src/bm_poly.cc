/*$Id: bm_poly.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * HSPICE compatible POLY
 */
#include "ap.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_max(BIGBIG);
const double _default_min(-BIGBIG);
const bool   _default_abs(false);
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(int c)
  :EVAL_BM_ACTION_BASE(c),
   _max(_default_max),
   _min(_default_min),
   _abs(_default_abs)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(const EVAL_BM_POLY& p)
  :EVAL_BM_ACTION_BASE(p),
   _max(p._max),
   _min(p._min),
   _abs(p._abs),
   _c(p._c)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    for (;;){
      double value=NOT_VALID;
      cmd >> value;
      if (cmd.stuck(&here)){
	break;
      }
      _c.push_back(value);
    }
    paren -= cmd.skiprparen();
    if (paren != 0){
      cmd.warn(bWARNING, "need )");
    }
    get(cmd, "MIn", &_min);
    get(cmd, "MAx", &_max);
    get(cmd, "Abs", &_abs);
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::print(OMSTREAM& where)const
{
  where << "  " << name() << '(';
  for (std::vector<double>::const_iterator
	 p = _c.begin();  p != _c.end();  ++p){
    where << "  " << *p;
  }
  where << ')';
  print_base(where);
  if (_min != _default_min) {where << "  min=" << _min;}
  if (_max != _default_max) {where << "  max=" << _max;}
  if (_abs != _default_abs) {where << "  abs=" << _abs;}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::tr_eval(ELEMENT* d)const
{
  double x = ioffset(d->_y0.x);
  double f0 = 0.;
  double f1 = 0.;
  for (int i=_c.size()-1; i>0; --i){
    f0 += _c[i];
    f0 *= x;
    f1 *= x;
    f1 += _c[i]*i;
  }
  f0 += _c[0];

  if (_abs && f0 < 0){
    f0 = -f0;
    f1 = -f1;
  }

  {if (f0 > _max){
    f0 = _max;
    f1 = 0;
  }else if (f0 < _min){
    f0 = _min;
    f1 = 0;
  }}

  d->_y0 = FPOLY1(x, f0, f1);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
