/*$Id: bmm_table.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 * table as a .model card
 */
//testing=script 2006.04.18
#include "u_lang.h"
#include "e_elemnt.h"
#include "m_spline.h"
#include "e_model.h" 
#include "bm.h"
/*--------------------------------------------------------------------------*/
class SPLINE;
/*--------------------------------------------------------------------------*/
class EVAL_BM_TABLE : public EVAL_BM_ACTION_BASE {
protected:
  explicit EVAL_BM_TABLE(const EVAL_BM_TABLE& p);
public:
  explicit EVAL_BM_TABLE(int c=0);
  ~EVAL_BM_TABLE() {}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new EVAL_BM_TABLE(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  void  	expand(const COMPONENT*);
  void		tr_eval(ELEMENT*)const;
  std::string	name()const	{untested();return modelname().c_str();}
  bool		ac_too()const		{untested();return false;}
};
/*--------------------------------------------------------------------------*/
class MODEL_TABLE : public MODEL_CARD {
public:
  PARAMETER<int>    _order;
  PARAMETER<double> _below;
  PARAMETER<double> _above;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  SPLINE* _spline;
private:
  static int    const _default_order;
  static double const _default_below;
  static double const _default_above;
private:
  explicit MODEL_TABLE(const MODEL_TABLE& p);
public:
  explicit MODEL_TABLE();
  ~MODEL_TABLE();
private: // override virtual
  std::string dev_type()const		{return "table";}
  void  precalc_first();
  COMMON_COMPONENT* new_common()const	{return new EVAL_BM_TABLE;}
  CARD* clone()const			{return new MODEL_TABLE(*this);}

  bool use_obsolete_callback_print()const {return true;}
  bool use_obsolete_callback_parse()const {return true;}
  void print_args_obsolete_callback(OMSTREAM&,LANGUAGE*)const;
  bool parse_params_obsolete_callback(CS&);

  void tr_eval(COMPONENT*)const;
};
/*--------------------------------------------------------------------------*/
int    const MODEL_TABLE::_default_order = 3;
double const MODEL_TABLE::_default_below = NOT_INPUT;
double const MODEL_TABLE::_default_above = NOT_INPUT;
/*--------------------------------------------------------------------------*/
static MODEL_TABLE p1;
static DISPATCHER<MODEL_CARD>::INSTALL
d1(&model_dispatcher, "table", &p1);
/*--------------------------------------------------------------------------*/
EVAL_BM_TABLE::EVAL_BM_TABLE(int c)
  :EVAL_BM_ACTION_BASE(c)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_TABLE::EVAL_BM_TABLE(const EVAL_BM_TABLE& p)
  :EVAL_BM_ACTION_BASE(p)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_TABLE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_TABLE* p = dynamic_cast<const EVAL_BM_TABLE*>(&x);
  bool rv = p && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << modelname();
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::expand(const COMPONENT* d)
{
  EVAL_BM_ACTION_BASE::expand(d);
  attach_model(d);

  const MODEL_TABLE* m = dynamic_cast<const MODEL_TABLE*>(model());
  if (!m) {untested();
    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), "table");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_TABLE::tr_eval(ELEMENT* d)const
{
  model()->tr_eval(d);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MODEL_TABLE::MODEL_TABLE()
  :MODEL_CARD(NULL),
   _order(_default_order),
   _below(_default_below),
   _above(_default_above),
   _table(),
   _spline(0)
{
}
/*--------------------------------------------------------------------------*/
MODEL_TABLE::MODEL_TABLE(const MODEL_TABLE& p)
  :MODEL_CARD(p),
   _order(p._order),
   _below(p._below),
   _above(p._above),
   _table(p._table),
   _spline(p._spline)
{
}
/*--------------------------------------------------------------------------*/
MODEL_TABLE::~MODEL_TABLE()
{
  delete _spline;
}
/*--------------------------------------------------------------------------*/
bool MODEL_TABLE::parse_params_obsolete_callback(CS& cmd)
{
  unsigned here1 = cmd.cursor();
  {
    Get(cmd, "order", &_order);
    Get(cmd, "below", &_below);
    Get(cmd, "above", &_above);
    bool got_opening_paren = cmd.skip1b('(');
    unsigned here = cmd.cursor();
    for (;;) {
      unsigned start_of_pair = here;
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
  return !(cmd.stuck(&here1));
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::print_args_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  print_pair(o, lang, "order", _order);
  print_pair(o, lang, "below", _below, _below.has_hard_value());
  print_pair(o, lang, "above", _above, _above.has_hard_value());
  o << " (";
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->first << ',' << p->second << ' ';
  }
  o << ')';
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::precalc_first()
{
  MODEL_CARD::precalc_first();

  const CARD_LIST* par_scope = scope();
  assert(par_scope);

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
  double below = _below.has_hard_value() ? _below : NOT_INPUT;
  double above = _above.has_hard_value() ? _above : NOT_INPUT;
  _spline = new SPLINE(_table, below, above, _order);
}
/*--------------------------------------------------------------------------*/
void MODEL_TABLE::tr_eval(COMPONENT* brh)const
{
  ELEMENT* d = prechecked_cast<ELEMENT*>(brh);
  assert(d);
  d->_y[0] = _spline->at(d->_y[0].x);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
