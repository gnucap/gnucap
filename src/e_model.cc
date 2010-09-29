/*$Id: e_model.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * base class for all models
 */
#include "ap.h"
#include "e_model.h"
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD()
  :CARD(),
   _tnom(NOT_INPUT)
{
  root_model_list.push_back(this);
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::~MODEL_CARD()
{
  root_model_list.erase(
	remove(root_model_list.begin(), root_model_list.end(), this),
	root_model_list.end());
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::parse(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();	/* skip known ".model" */
  parse_label(cmd);
  parse_front(cmd);
  cmd.skiplparen();
  int here = cmd.cursor();
  do{
    parse_params(cmd);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skiprparen();
  cmd.check(bWARNING, "what's this?");
  parse_finish();
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::print(OMSTREAM& where, int)const
{
  where.setfloatwidth(7);
  where << ".model  " << short_label();
  print_front(where);
  where << "  (";
  print_params(where);
  where << ")\n*+(";
  print_calculated(where);
  where << ")\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
