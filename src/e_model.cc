/*$Id: e_model.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2006.07.12
#include "e_model.h"
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD()
  :CARD(),
   _tnom_c(NOT_INPUT)
{
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::~MODEL_CARD()
{
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::parse_spice(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();	/* skip known ".model" */
  parse_label(cmd);
  parse_front(cmd);
  cmd.skip1b('(');
  int here = cmd.cursor();
  do{
    parse_params(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skip1b(')');
  cmd.check(bWARNING, "what's this?");
  parse_finish();
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::print_spice(OMSTREAM& o, int)const
{
  o.setfloatwidth(7);
  o << ".model  " << short_label();
  print_front(o);
  o << "  (";
  print_params(o);
  o << ")\n*+(";
  print_calculated(o);
  o << ")\n";
}
/*--------------------------------------------------------------------------*/
bool MODEL_CARD::parse_params(CS& cmd)
{
  untested();
  return ONE_OF
    || get(cmd, "TNOM", &_tnom_c);
    ;
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::print_params(OMSTREAM& o)const
{
  o << " tnom="  << _tnom_c;
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::elabo1()
{
  if (1 || !evaluated()) {
    CARD::elabo1();
    _tnom_c.e_val(OPT::tnom_c, scope());
  }else{
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
