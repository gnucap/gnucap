/*$Id: bm_value.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
EVAL_BM_VALUE::EVAL_BM_VALUE(int c)
  :EVAL_BM_ACTION_BASE(c),
   _value(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_VALUE::EVAL_BM_VALUE(const EVAL_BM_VALUE& p)
  :EVAL_BM_ACTION_BASE(p),
   _value(p._value)
{
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    int paren = cmd.skiplparen();
    double value=NOT_VALID;
    cmd >> value;
    if (cmd.gotit(here)){
      _value = value;
    }
    paren -= cmd.skiprparen();
    if (paren != 0){
      untested();
      cmd.warn(bWARNING, "need )");
    }
    parse_base(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  parse_base_finish();
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::print(OMSTREAM& where)const
{
  where << "  " << _value;
  print_base(where);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_VALUE::tr_eval(ELEMENT* d)const
{
  tr_finish_tdv(d, _value);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
