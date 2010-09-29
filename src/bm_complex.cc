/*$Id: bm_complex.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * behavioral modeling complex value
 * used with tc, etc, and conditionals
 */
//testing=script 2006.07.13
#include "e_elemnt.h"
#include "bm.h"
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
void EVAL_BM_COMPLEX::print(OMSTREAM& o)const
{
  o << ' ' << name() << '(' 
    << _value.real() << ',' << _value.imag() << ')';
  EVAL_BM_ACTION_BASE::print(o);
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
  int here = cmd.cursor();
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
