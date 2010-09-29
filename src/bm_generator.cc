/*$Id: bm_generator.cc,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * behavioral modeling simple value
 * used with tc, etc, and conditionals
 */
//testing=script,complete 2005.10.06
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR::EVAL_BM_GENERATOR(int c)
  :EVAL_BM_ACTION_BASE(c)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR::EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p)
  :EVAL_BM_ACTION_BASE(p)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_GENERATOR::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_GENERATOR* p = dynamic_cast<const EVAL_BM_GENERATOR*>(&x);
  bool rv = p
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    incomplete();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::print(OMSTREAM& o)const
{
  o << ' ' << name();
  EVAL_BM_ACTION_BASE::print(o);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, SIM::genout);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::ac_eval(ELEMENT* d)const
{
  d->_ev = 1;
  ac_final_adjust_with_temp(&(d->_ev));
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_GENERATOR::parse_numlist(CS& cmd)
{
  int here = cmd.cursor();
  double value=NOT_VALID;
  cmd >> value;
  if (cmd.gotit(here)) {
    _scale = value;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
