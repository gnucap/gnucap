/*$Id: bm_fit.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_spline.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const int    _default_order (3);
const double _default_below (NOT_INPUT);
const double _default_above (NOT_INPUT);
const double _default_delta (NOT_INPUT);
const int    _default_smooth(0);
/*--------------------------------------------------------------------------*/
class EVAL_BM_FIT : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<int>    _order;
  PARAMETER<double> _below;
  PARAMETER<double> _above;
  PARAMETER<double> _delta;
  PARAMETER<int>    _smooth;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  SPLINE* _spline;
  explicit	EVAL_BM_FIT(const EVAL_BM_FIT& p);
public:
  explicit      EVAL_BM_FIT(int c=0);
		~EVAL_BM_FIT();
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_FIT(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		precalc_last(const CARD_LIST*);

  void		tr_eval(ELEMENT*)const;
  std::string	name()const		{return "fit";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
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
void EVAL_BM_FIT::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
  print_pair(o, lang, "order", _order);
  print_pair(o, lang, "below", _below, _below.has_hard_value());
  print_pair(o, lang, "above", _above, _above.has_hard_value());
  print_pair(o, lang, "delta", _delta, _delta.has_hard_value());
  print_pair(o, lang, "smooth",_smooth,_smooth.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _order.e_val(_default_order, Scope);
  _below.e_val(_default_below, Scope);
  _above.e_val(_default_above, Scope);
  _delta.e_val(_default_delta, Scope);
  _smooth.e_val(_default_smooth, Scope);
  
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 iterator p = _table.begin();  p != _table.end();  ++p) {
    p->first.e_val(0, Scope);
    p->second.e_val(0, Scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  
  double last = -BIGBIG;
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 iterator p = _table.begin();  p != _table.end();  ++p) {
    if (last > p->first) {untested();
      throw Exception_Precalc("FIT table is out of order: (" + to_string(last)
      			      + ", " + to_string(p->first) + ")\n");
    }else{
    }
    last = p->first;
  }
  delete _spline;
  double below = _below.has_hard_value() ? _below : NOT_INPUT;
  double above = _above.has_hard_value() ? _above : NOT_INPUT;
  _spline = new SPLINE(_table, below, above, _order);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FIT::tr_eval(ELEMENT* d)const
{
  d->_y[0] = _spline->at(d->_y[0].x);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FIT::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  for (;;) {
    unsigned start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    //cmd >> key >> value;
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
bool EVAL_BM_FIT::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "order", &_order)
    || Get(cmd, "below", &_below)
    || Get(cmd, "above", &_above)
    || Get(cmd, "delta", &_delta)
    || Get(cmd, "smooth",&_smooth)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_FIT p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "fit", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
