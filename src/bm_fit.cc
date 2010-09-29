/*$Id: bm_fit.cc,v 24.16 2004/01/11 02:47:28 al Exp $ -*- C++ -*-
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
 * cubic spline bm function
 */
#include "ap.h"
#include "m_interp.h"
#include "m_spline.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
const int    _default_order (3);
const double _default_below (NOT_INPUT);
const double _default_above (NOT_INPUT);
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
EVAL_BM_FIT::EVAL_BM_FIT(int c)
  :EVAL_BM_ACTION_BASE(c),
   _order(_default_order),
   _below(_default_below),
   _above(_default_above),
   _delta(_default_delta),
   _smooth(_default_smooth),
   _table(),
   _spline(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_FIT::EVAL_BM_FIT(const EVAL_BM_FIT& p)
  :EVAL_BM_ACTION_BASE(p),
   _order(p._order),
   _below(p._below),
   _above(p._above),
   _delta(_default_delta),
   _smooth(_default_smooth),
   _table(p._table),
   _spline(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_FIT::~EVAL_BM_FIT()
{
  delete _spline;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::parse_front()
{
  delete _spline;
  _spline = 0;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::parse_numlist(CS& cmd)
{
  int here = cmd.cursor();
  for (;;){
    double key  =NOT_VALID;
    double value=NOT_VALID;
    cmd >> key >> value;
    if (cmd.stuck(&here)){
      break;
    }
    std::pair<double,double> p(key,value);
    _table.push_back(p);
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FIT::parse_params(CS& cmd)
{
  return ONE_OF
    || get(cmd, "Order", &_order)
    || get(cmd, "Below", &_below)
    || get(cmd, "Above", &_above)
    || get(cmd, "Delta", &_delta)
    || get(cmd, "SMooth",&_smooth)
    || EVAL_BM_ACTION_BASE::parse_params(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::parse_finish()
{
  EVAL_BM_ACTION_BASE::parse_finish();
  _spline = new SPLINE(_table, _below, _above, _order);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::print(OMSTREAM& where)const
{
  where << "  " << name() << '(';
  for (std::vector<std::pair<double,double> >::const_iterator
	 p = _table.begin();  p != _table.end();  ++p){
    where << "  " << p->first << ',' << p->second;
  }
  where << ')';
  print_base(where);
  where << "  order=" << _order;
  if (_below != NOT_INPUT)	  {where << "  below=" << _below;}
  if (_above != NOT_INPUT)	  {where << "  above=" << _above;}
  if (_delta  != _default_delta)  {where << "  delta=" << _delta;  untested();}
  if (_smooth != _default_smooth) {where << "  smooth="<< _smooth; untested();}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::tr_eval(ELEMENT* d)const
{
  d->_y0 = _spline->at(d->_y0.x);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
