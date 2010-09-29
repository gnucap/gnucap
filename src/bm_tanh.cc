/*$Id: bm_tanh.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling hyperbolic tangent
 */
//testing=script 2005.10.07
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const double _default_gain (NOT_INPUT);
const double _default_limit (NOT_INPUT);
/*--------------------------------------------------------------------------*/
EVAL_BM_TANH::EVAL_BM_TANH(int c)
  :EVAL_BM_ACTION_BASE(c),
   _gain(NOT_INPUT),
   _limit(NOT_INPUT)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_TANH::EVAL_BM_TANH(const EVAL_BM_TANH& p)
  :EVAL_BM_ACTION_BASE(p),
   _gain(p._gain),
   _limit(p._limit)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_TANH::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_TANH* p = dynamic_cast<const EVAL_BM_TANH*>(&x);
  bool rv = p
    && _gain == p->_gain
    && _limit == p->_limit
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TANH::print(OMSTREAM& o)const
{
  o << ' ' << name()
    << " gain=" << _gain
    << " limit="<< _limit;
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TANH::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _gain.e_val(_default_gain, par_scope);
  _limit.e_val(_default_limit, par_scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TANH::tr_eval(ELEMENT* d)const
{
  double x = ioffset(d->_y0.x);
  double aa = x * _gain/_limit;
  double f1, f0;
  if (aa > LOGBIGBIG) {
    f1 = 0;
    f0 = _limit;
  }else if (aa < -LOGBIGBIG) {
    f1 = 0;
    f0 = -_limit;
  }else{
    double cosine = cosh(aa);
    f1 = _gain / (cosine*cosine);
    f0 = _limit * tanh(aa);
  }
  d->_y0 = FPOLY1(x, f0, f1);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_TANH::parse_numlist(CS& cmd)
{
  int here = cmd.cursor();
  double gain=NOT_VALID;
  double limit=NOT_VALID;
  cmd >> gain >> limit;
  if (cmd.gotit(here)) {
    _gain  = gain;
    _limit = limit;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_TANH::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Gain",	&_gain)
    || get(cmd, "Limit",&_limit)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
