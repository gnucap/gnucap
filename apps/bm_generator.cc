/*$Id: bm_generator.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
//testing=script,complete 2005.10.06
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class EVAL_BM_GENERATOR : public EVAL_BM_ACTION_BASE {
private:
  explicit	EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p);
public:
  explicit      EVAL_BM_GENERATOR(int c=0);
		~EVAL_BM_GENERATOR()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_GENERATOR(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  void		tr_eval(ELEMENT*)const;
  void		ac_eval(ELEMENT*)const;
  std::string	name()const		{return "generator";}
  bool		ac_too()const		{return true;}
  bool		parse_numlist(CS&);
};
/*--------------------------------------------------------------------------*/
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
void EVAL_BM_GENERATOR::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, d->_sim->_genout);
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
  unsigned here = cmd.cursor();
  PARAMETER<double> new_value(NOT_VALID);
  cmd >> new_value;
  if (cmd.gotit(here)) {
    _scale = new_value;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "gen|generator", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
