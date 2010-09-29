/*$Id: bm_fit.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * cubic spline bm function
 */
//testing=script 2006.04.18
#include "e_elemnt.h"
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
   _delta(p._delta),
   _smooth(p._smooth),
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
bool EVAL_BM_FIT::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_FIT* p = dynamic_cast<const EVAL_BM_FIT*>(&x);
  bool rv = p
    && _order == p->_order
    && _below == p->_below
    && _above == p->_above
    && _delta == p->_delta
    && _smooth == p->_smooth
    && _table == p->_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::print(OMSTREAM& o)const
{
  o << ' ' << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
  o << " order=" << _order;
  if (_below.has_value())  {o << " below=" << _below;}
  if (_above.has_value())  {o << " above=" << _above;}
  if (_delta.has_value())  {o << " delta=" << _delta;  untested();}
  if (_smooth.has_value()) {o << " smooth="<< _smooth; untested();}
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  EVAL_BM_ACTION_BASE::elabo3(c);
  _order.e_val(_default_order, par_scope);
  _below.e_val(_default_below, par_scope);
  _above.e_val(_default_above, par_scope);
  _delta.e_val(_default_delta, par_scope);
  _smooth.e_val(_default_smooth, par_scope);
  {
    double last = -BIGBIG;
    for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	   iterator p = _table.begin();  p != _table.end();  ++p) {
      p->first.e_val(0, par_scope);
      p->second.e_val(0, par_scope);
      if (last > p->first) {
	untested();
	error(bWARNING, "%s: FIT table is out of order: (%g, %g)\n",
	      c->long_label().c_str(), last, double(p->first));
      }else{
	//std::pair<double,double> x(p->first, p->second);
	//_num_table.push_back(x);
      }
      last = p->first;
    }
  }
  delete _spline;
  int order = _order;
  _spline = new SPLINE(_table, _below, _above, order);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::tr_eval(ELEMENT* d)const
{
  d->_y0 = _spline->at(d->_y0.x);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FIT::parse_numlist(CS& cmd)
{
  int start = cmd.cursor();
  int here = cmd.cursor();
  for (;;) {
    int start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    cmd >> p.first; // key
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      cmd >> p.second; // value
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
/*--------------------------------------------------------------------------*/
