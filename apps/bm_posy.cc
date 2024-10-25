/*$Id: bm_posy.cc 2016/03/23 al $ -*- C++ -*-
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
 * behavioral modeling - posynomial (non-integer powers)
 * pair ...  first is key, second is value
 */
//testing=script 2005.10.06
#include "globals.h"
#include "u_lang.h"
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_max(BIGBIG);
const double _default_min(-BIGBIG);
const bool   _default_abs(false);
const bool   _default_odd(false);
const bool   _default_even(false);
/*--------------------------------------------------------------------------*/
class EVAL_BM_POSY : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _min;
  PARAMETER<double> _max;
  PARAMETER<bool>   _abs;
  PARAMETER<bool>   _odd;
  PARAMETER<bool>   _even;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  explicit	EVAL_BM_POSY(const EVAL_BM_POSY& p);
public:
  explicit      EVAL_BM_POSY(int c=0);
		~EVAL_BM_POSY()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const override;
  COMMON_COMPONENT* clone()const override	{return new EVAL_BM_POSY(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const override;

  void		precalc_last(const CARD_LIST*)override;
  void		tr_eval(ELEMENT*)const override;
  std::string	name()const override		{return "posy";}
  bool		ac_too()const override		{untested();return false;}
  bool		parse_numlist(CS&)override;
  bool		parse_params_obsolete_callback(CS&)override;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY::EVAL_BM_POSY(int c)
  :EVAL_BM_ACTION_BASE(c),
   _min(_default_min),
   _max(_default_max),
   _abs(_default_abs),
   _odd(_default_odd),
   _even(_default_even),
   _table()
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY::EVAL_BM_POSY(const EVAL_BM_POSY& p)
  :EVAL_BM_ACTION_BASE(p),
   _min(p._min),
   _max(p._max),
   _abs(p._abs),
   _odd(p._odd),
   _even(p._even),
   _table(p._table)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POSY::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_POSY* p = dynamic_cast<const EVAL_BM_POSY*>(&x);
  bool rv = p
    && _min == p->_min
    && _max == p->_max
    && _abs == p->_abs
    && _odd == p->_odd
    && _even == p->_even
    && _table == p->_table
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) { untested();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name() << '(';
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 const_iterator p = _table.begin();  p != _table.end();  ++p) {
    o << p->second << ',' << p->first << ' ';
  }
  o << ')';
  print_pair(o, lang, "min", _min, _min.has_hard_value());
  print_pair(o, lang, "max", _max, _max.has_hard_value());
  print_pair(o, lang, "abs", _abs, _abs.has_hard_value());
  print_pair(o, lang, "odd", _odd, _odd.has_hard_value());
  print_pair(o, lang, "even",_even,_even.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_last(Scope);
  _min.e_val(_default_min, Scope);
  _max.e_val(_default_max, Scope);
  _abs.e_val(_default_abs, Scope);
  _odd.e_val(_default_odd, Scope);
  _even.e_val(_default_even, Scope);
  for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	 iterator p = _table.begin();  p != _table.end();  ++p) {
    p->first.e_val(0, Scope);
    p->second.e_val(0, Scope);
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POSY::tr_eval(ELEMENT* d)const
{
  double x_raw = ioffset(d->_y[0].x);
  trace1("before", x_raw);
  double x = (x_raw < 0)
    ? ((_odd || _even) ? -x_raw : 0.)
    : x_raw;
  assert(x >= 0);
  trace1("rev", x);

  double f0 = 0.;
  double f1 = 0.;
  if (x > 0) {
    for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	   const_iterator p = _table.begin();  p != _table.end();  ++p) {
      double coeff = p->second * pow(x, p->first-1);
      f1 += coeff * p->first;
      f0 += coeff * x;
    }
  }else{
    assert(x == 0);
    for (std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > >::
	   const_iterator p = _table.begin();  p != _table.end();  ++p) {
      if (p->first == 0.) { untested();
	f0 += 1;
	untested();
      }
    }
  }

  trace3("", x, f0, f1);

  if (_odd && x_raw < 0) {
    f0 = -f0;
  }
  if (_even && x_raw < 0) { untested();
    f1 = -f1;
    untested();
  }
  trace3("after", x, f0, f1);

  if (_abs && f0 < 0) { untested();
    f0 = -f0;
    f1 = -f1;
    untested();
  }

  if (f0 > _max) { untested();
    f0 = _max;
    f1 = 0;
    untested();
  }else if (f0 < _min) { untested();
    f0 = _min;
    f1 = 0;
    untested();
  }

  d->_y[0] = FPOLY1(x_raw, f0, f1);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
  trace3("fa", d->_y[0].x, d->_y[0].f0, d->_y[0].f1);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POSY::parse_numlist(CS& cmd)
{
  size_t start = cmd.cursor();
  size_t here = cmd.cursor();
  for (;;) {
    size_t start_of_pair = here;
    std::pair<PARAMETER<double>, PARAMETER<double> > p;
    p.second.obsolete_parse(cmd);
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      p.first.obsolete_parse(cmd);
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
  }else{ untested();
    untested();
  }
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POSY::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "min",  &_min)
    || Get(cmd, "max",  &_max)
    || Get(cmd, "abs",  &_abs)
    || Get(cmd, "odd",  &_odd)
    || Get(cmd, "even", &_even)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_POSY p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "posy", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
