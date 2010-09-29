/*$Id: bm_generator.cc,v 24.16 2004/01/11 02:47:28 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * behavioral modeling simple value
 * used with tc, etc, and conditionals
 */
#include "ap.h"
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
void EVAL_BM_GENERATOR::parse_numlist(CS& cmd)
{
  int here = cmd.cursor();
  double value=NOT_VALID;
  cmd >> value;
  if (cmd.gotit(here)){
    _scale = value;
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::print(OMSTREAM& where)const
{
  where << "  " << name();
  print_base(where);
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
/*--------------------------------------------------------------------------*/
