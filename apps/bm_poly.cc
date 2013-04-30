/*$Id: bm_poly.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * HSPICE compatible POLY
 */
//testing=script,complete 2005.10.06
#include "u_lang.h"
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const double _default_max(BIGBIG);
const double _default_min(-BIGBIG);
const bool   _default_abs(false);
/*--------------------------------------------------------------------------*/
class EVAL_BM_POLY : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _min;
  PARAMETER<double> _max;
  PARAMETER<bool>   _abs;
  std::vector<PARAMETER<double> > _c;
  explicit	EVAL_BM_POLY(const EVAL_BM_POLY& p);
public:
  explicit      EVAL_BM_POLY(int c=0);
		~EVAL_BM_POLY()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_POLY(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		tr_eval(ELEMENT*)const;
  std::string	name()const		{return "poly";}
  bool		ac_too()const		{untested();return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
  void		skip_type_tail(CS& cmd)const {cmd.umatch("(1)");}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(int c)
  :EVAL_BM_ACTION_BASE(c),
   _min(_default_min),
   _max(_default_max),
   _abs(_default_abs)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY::EVAL_BM_POLY(const EVAL_BM_POLY& p)
  :EVAL_BM_ACTION_BASE(p),
   _min(p._min),
   _max(p._max),
   _abs(p._abs),
   _c(p._c)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POLY::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_POLY* p = dynamic_cast<const EVAL_BM_POLY*>(&x);
  bool rv = p
    && _min == p->_min
    && _max == p->_max
    && _abs == p->_abs
    && _c == p->_c
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    incomplete();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name() << '(';
  for (std::vector<PARAMETER<double> >::const_iterator
	 p = _c.begin();  p != _c.end();  ++p) {
    o << *p << ' ';
  }
  o << ')';
  print_pair(o, lang, "min", _min, _min.has_hard_value());
  print_pair(o, lang, "max", _max, _max.has_hard_value());
  print_pair(o, lang, "abs", _abs, _abs.has_hard_value());
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  for (std::vector<PARAMETER<double> >::const_iterator
	 p = _c.begin();  p != _c.end();  ++p) {
    (*p).e_val(0, Scope);
  }
  _min.e_val(_default_min, Scope);
  _max.e_val(_default_max, Scope);
  _abs.e_val(_default_abs, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_POLY::tr_eval(ELEMENT* d)const
{
  double x = ioffset(d->_y[0].x);
  double f0 = 0.;
  double f1 = 0.;
  for (size_t i=_c.size()-1; i>0; --i) {
    f0 += _c[i];
    f0 *= x;
    f1 *= x;
    f1 += _c[i]*int(i);
  }
  f0 += _c[0];

  if (_abs && f0 < 0) {
    f0 = -f0;
    f1 = -f1;
  }

  if (f0 > _max) {
    f0 = _max;
    f1 = 0;
  }else if (f0 < _min) {
    f0 = _min;
    f1 = 0;
  }

  d->_y[0] = FPOLY1(x, f0, f1);
  tr_final_adjust(&(d->_y[0]), d->f_is_value());
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_POLY::parse_numlist(CS& cmd)
{
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
  for (;;) {
    unsigned old_here = here;
    PARAMETER<double> val;
    cmd >> val;
    if (cmd.stuck(&here)) {
      // no more, graceful finish
      break;
    }else{
      if (cmd.match1('=')) {
	// got one that doesn't belong, back up
	cmd.reset(old_here);
	break;
      }else{
	_c.push_back(val);
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
bool EVAL_BM_POLY::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "min", &_min)
    || Get(cmd, "max", &_max)
    || Get(cmd, "abs", &_abs)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_POLY p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "poly", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
