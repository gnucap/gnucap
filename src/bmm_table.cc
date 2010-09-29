/*$Id: bmm_table.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 * table as a .model card
 */
//testing=script 2006.04.18
#include "e_elemnt.h"
#include "e_model.h"
#include "ap.h"
#include "m_spline.h"
#include "bmm_table.h"
/*--------------------------------------------------------------------------*/
int    const MODEL_TABLE::_default_order = 3;
double const MODEL_TABLE::_default_below = NOT_INPUT;
double const MODEL_TABLE::_default_above = NOT_INPUT;
/*--------------------------------------------------------------------------*/
EVAL_BM_TABLE::EVAL_BM_TABLE(int c)
  :EVAL_BM_ACTION_BASE(c)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_TABLE::EVAL_BM_TABLE(const EVAL_BM_TABLE& p)
  :EVAL_BM_ACTION_BASE(p)
{
  untested();
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_TABLE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_TABLE* p = dynamic_cast<const EVAL_BM_TABLE*>(&x);
  bool rv = p
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::print(OMSTREAM& o)const
{
  untested();
  o << ' ' << modelname();
  EVAL_BM_ACTION_BASE::print(o);
  untested();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::elabo3(const COMPONENT* d)
{
  EVAL_BM_ACTION_BASE::elabo3(d);
  const MODEL_TABLE* m = dynamic_cast<const MODEL_TABLE*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + "model " + modelname()
	  + " is not a table\n");
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::tr_eval(ELEMENT* d)const
{
  model()->tr_eval(d);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_TABLE::MODEL_TABLE()
  :MODEL_CARD(),
   _order(_default_order),
   _below(_default_below),
   _above(_default_above),
   _table(),
   _spline(0)
{
}
/*--------------------------------------------------------------------------*/
MODEL_TABLE::~MODEL_TABLE()
{
  delete _spline;
}
/*--------------------------------------------------------------------------*/
bool MODEL_TABLE::parse_front(CS& cmd)
{
  bool dummy;
  return set(cmd, "TABLE", &dummy, true);
}
/*--------------------------------------------------------------------------*/
bool MODEL_TABLE::parse_params(CS& cmd)
{
  int here = cmd.cursor();
  {
    get(cmd, "Order", &_order);
    get(cmd, "Below", &_below);
    get(cmd, "Above", &_above);
    bool got_opening_paren = cmd.skip1b('(');
    int here = cmd.cursor();
    for (;;) {
      int start_of_pair = here;
      std::pair<PARAMETER<double>, PARAMETER<double> > p;
      cmd >> p.first; // key
      if (cmd.stuck(&here)) {
	break;
      }else{
	cmd >> p.second; // value
	if (cmd.stuck(&here)) {
	  cmd.reset(start_of_pair);
	  break;
	}else{
	  _table.push_back(p);
	}
      }
    }
    if (got_opening_paren && !cmd.skip1b(')')) {
      untested();
      cmd.warn(bWARNING, "need )");
    }else if (!got_opening_paren && cmd.skip1b(')')) {
      untested();
      cmd.warn(bWARNING, here, "need (");
    }
  }
  return !(cmd.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::print_front(OMSTREAM& o)const
{
  o << " table";
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::print_params(OMSTREAM& o)const
{
  o << " order=" << _order;
  if (_below != NOT_INPUT) {o << " below=" << _below;}
  if (_above != NOT_INPUT) {o << " above=" << _above;}
  o << " (";
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_CARD::elabo1();
    _order.e_val(_default_order, par_scope);
    _below.e_val(_default_below, par_scope);
    _above.e_val(_default_above, par_scope);
    {
      double last = -BIGBIG;
      for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	     iterator p = _table.begin();  p != _table.end();  ++p) {
	p->first.e_val(0, par_scope);
	p->second.e_val(0, par_scope);
	if (last > p->first) {
	  untested();
	  error(bWARNING, "%s: table is out of order: (%g, %g)\n",
		long_label().c_str(), last, double(p->first));
	}else{
	  //std::pair<double,double> x(p->first, p->second);
	  //_num_table.push_back(x);
	}
	last = p->first;
      }
    }
    delete _spline;
    _spline = new SPLINE(_table, _below, _above, _order);
  }else{
    untested();
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::tr_eval(COMPONENT* brh)const
{
  ELEMENT* d = prechecked_cast<ELEMENT*>(brh);
  assert(d);
  d->_y0 = _spline->at(d->_y0.x);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
