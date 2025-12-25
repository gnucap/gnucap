/*$Id: bm_value.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
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
#include "globals.h"
#include "e_elemnt.h"
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
 // o << _value;
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::precalc_first(const PARAM_LIST* Scope)
{
  if (modelname() != "") {
    // incomplete();?
    (_value = "") = modelname();
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
  size_t here = cmd.cursor();
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
namespace {
class EVAL_BM_SIMPLE : public EVAL_BM_BASE {
public:
  explicit EVAL_BM_SIMPLE (EVAL_BM_VALUE const& b)
    : EVAL_BM_BASE(b) {
  }
  bool operator==(const COMMON_COMPONENT& x)const override {
    const EVAL_BM_SIMPLE* p = dynamic_cast<const EVAL_BM_SIMPLE*>(&x);
    return p && EVAL_BM_BASE::operator==(x);
  }
private:
  EVAL_BM_SIMPLE* clone()const override {
    return new EVAL_BM_SIMPLE(*this);
  }
  void tr_eval(ELEMENT* d)const override {
    d->_y[0] = FPOLY1(CPOLY1(d->_y[0].x, 0., _value));
  }
  void ac_eval(ELEMENT* d)const override {
    tr_eval(d);
    d->_ev = d->_y[0].f1;
  }
};
} // namespace
/*--------------------------------------------------------------------------*/
COMMON_COMPONENT* EVAL_BM_VALUE::deflate()
{
  if(has_ext_args()){
    return this; // no deflate.
  }else if(modelname() != ""){
    return this; // not sure what this is.
		 // but need to get rid of model anyway
  }else{
    // return this; // itested: more numerical noise.
    // use a simpler version
    auto n = new EVAL_BM_SIMPLE(*this);
    assert(n->next_common() == next_common());
    return n;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
