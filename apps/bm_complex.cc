/*$Id: bm_complex.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * behavioral modeling complex value
 * used with tc, etc, and conditionals
 */
//testing=script 2006.07.13
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class EVAL_BM_COMPLEX : public EVAL_BM_ACTION_BASE {
private:
  COMPLEX _value;
  explicit	EVAL_BM_COMPLEX(const EVAL_BM_COMPLEX& p);
public:
  explicit      EVAL_BM_COMPLEX(int c=0);
		~EVAL_BM_COMPLEX()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COMPLEX(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  void		tr_eval(ELEMENT*)const;
  void		ac_eval(ELEMENT*)const;
  std::string	name()const		{return "complex";}
  bool		ac_too()const		{untested();return true;}
  bool		parse_numlist(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_COMPLEX::EVAL_BM_COMPLEX(int c)
  :EVAL_BM_ACTION_BASE(c),
   _value(NOT_INPUT)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_COMPLEX::EVAL_BM_COMPLEX(const EVAL_BM_COMPLEX& p)
  :EVAL_BM_ACTION_BASE(p),
   _value(p._value)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_COMPLEX::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_COMPLEX* p = dynamic_cast<const EVAL_BM_COMPLEX*>(&x);
  bool rv = p
    && _value == p->_value
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COMPLEX::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name() << '(' 
    << _value.real() << ',' << _value.imag() << ')';
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COMPLEX::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value.real());
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_COMPLEX::ac_eval(ELEMENT* d)const
{
  d->_ev = _value;
  ac_final_adjust_with_temp(&(d->_ev));
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_COMPLEX::parse_numlist(CS& cmd)
{
  unsigned here = cmd.cursor();
  double real = NOT_VALID;
  double imag = 0.;
  cmd >> real >> imag;
  if (cmd.gotit(here)) {
    _value = COMPLEX(real,imag);
    return true;
  }else{
    untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_COMPLEX p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "complex", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
