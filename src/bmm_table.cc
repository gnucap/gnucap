/*$Id: bmm_table.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::print(OMSTREAM& o)const
{
  untested();
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
void EVAL_BM_TABLE::expand()
{
  const MODEL_TABLE* m = dynamic_cast<const MODEL_TABLE*>(attach_model());
  if (!m) {
    untested();
    error(bERROR, "model " + modelname() + " is not a table\n");
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
void MODEL_TABLE::parse_params(CS& cmd)
{
  get(cmd, "Order", &_order);
  get(cmd, "Below", &_below);
  get(cmd, "Above", &_above);
  int paren = cmd.skiplparen();
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
  paren -= cmd.skiprparen();
  if (paren != 0){
    untested();
    cmd.warn(bWARNING, "need )");
  }
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
	 p = _table.begin();  p != _table.end();  ++p){
    o << "  " << p->first << ',' << p->second;
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::parse_finish()
{
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
