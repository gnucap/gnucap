/*$Id: bm_value.cc,v 26.130 2009/11/15 21:51:59 al Exp $ -*- C++ -*-
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
 * behavioral modeling simple value
 * used with tc, etc, and conditionals
 */
//testing=script 2005.10.07
#include "bm.h"
/*--------------------------------------------------------------------------*/
static EVAL_BM_VALUE p1(CC_STATIC);
static DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "value|eval_bm_value", &p1);
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_VALUE* p = dynamic_cast<const EVAL_BM_VALUE*>(&x);
  return  p && EVAL_BM_ACTION_BASE::operator==(x);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  o << _value;
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::is_trivial()const
{
  return  !(_bandwidth.has_hard_value()
	    || _delay.has_hard_value()
	    || _phase.has_hard_value()
	    || _ooffset.has_hard_value()
	    || _ioffset.has_hard_value()
	    || _scale.has_hard_value()
	    || _tc1.has_hard_value()
	    || _tc2.has_hard_value()
	    || _ic.has_hard_value()
	    || _tnom_c.has_hard_value()
	    || _dtemp.has_hard_value()
	    || _temp_c.has_hard_value());
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::precalc_first(const CARD_LIST* Scope)
{
  if (modelname() != "") {
    _value = modelname();
  }else{
  }
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::parse_numlist(CS& cmd)
{
  unsigned here = cmd.cursor();
  PARAMETER<double> new_value(NOT_VALID);
  cmd >> new_value;
  if (cmd.gotit(here)) {
    _value = new_value;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_VALUE::parse_params_obsolete_callback(CS& cmd)
{
  return ONE_OF
    || Get(cmd, "=", &_value)
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
