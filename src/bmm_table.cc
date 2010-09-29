/*$Id: bmm_table.cc,v 24.16 2004/01/11 02:47:28 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 * Spice3 compatible "semiconductor resistor and capacitor"
 */
#include "ap.h"
#include "m_spline.h"
#include "bmm_table.h"
/*--------------------------------------------------------------------------*/
const int    _default_order (3);
const double _default_below (NOT_INPUT);
const double _default_above (NOT_INPUT);
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
void EVAL_BM_TABLE::parse(CS& cmd)
{
  int here = cmd.cursor();
  bool paren = cmd.skiplparen();
  do{
    if (paren) {
      {if (cmd.skiprparen()) {
	untested();
        break;
      }else{
	untested();
      }}
    }
    parse_params(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::print(OMSTREAM& o)const
{
  o << "  " << modelname();
  print_base(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::tr_eval(ELEMENT* d)const
{
  model()->tr_eval(d);
  tr_final_adjust(&(d->_y0), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::expand(const COMPONENT* d)
{
  const MODEL_TABLE* m = dynamic_cast<const MODEL_TABLE*>(attach_model(d));
  if (!m) {
    untested();
    error(bERROR, d->long_label() + "model " + modelname()
	  + " is not a table\n");
  }
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
    bool got_opening_paren = cmd.skiplparen();
    int here = cmd.cursor();
    double max_key = -BIGBIG;
    double min_key = BIGBIG;
    std::deque<std::pair<double,double> > temp_table;
    for (;;) {
      double key  =NOT_VALID;
      double value=NOT_VALID;
      int old_here = here;
      cmd >> key >> value;
      if (cmd.stuck(&here)) {
	break;
      }
      std::pair<double,double> p(key,value);
      {if (key > max_key) {
	// table is in ascending order
	max_key = key;
	temp_table.push_back(p);
	if (key < min_key) {
	  // first time
	  min_key = key;
	}
      }else if (key < min_key) {
	// table is in decending order
	min_key = key;
	temp_table.push_front(p);
      }else{
	untested();
	cmd.warn(bDANGER, old_here, "table is out of order, dropping");
      }}
    }
    {if (got_opening_paren && !cmd.skiprparen()) {
      untested();
      cmd.warn(bWARNING, "need )");
    }else if (!got_opening_paren && cmd.skiprparen()) {
      untested();
      cmd.warn(bWARNING, here, "need (");
    }}
    for (std::deque<std::pair<double,double> >::const_iterator
	   p = temp_table.begin();  p != temp_table.end();  ++p) {
      _table.push_back(*p);
    }
  }
  return !(cmd.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::print_front(OMSTREAM& o)const
{
  o << "  table";
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::print_params(OMSTREAM& o)const
{
  o << "  order=" << _order;
  if (_below != NOT_INPUT) {o << "  below=" << _below;}
  if (_above != NOT_INPUT) {o << "  above=" << _above;}
  o << "  (";
  for (std::vector<std::pair<double,double> >::const_iterator
	 p = _table.begin();  p != _table.end();  ++p) {
    o << "  " << p->first << ',' << p->second;
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::parse_finish()
{
  assert(!_spline);
  _spline = new SPLINE(_table, _below, _above, _order);
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
