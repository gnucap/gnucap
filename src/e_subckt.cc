/*$Id: e_subckt.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * base class for elements made of subcircuits
 */
//testing=script,complete 2006.07.12
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::parse_spice(CS& cmd)
{
  assert(has_common());
  COMMON_COMPONENT* c = common()->clone();
  assert(c);

  parse_Label(cmd);
  _net_nodes = parse_nodes(cmd,max_nodes(),min_nodes(),1/*tail*/,0/*start*/);
  _net_nodes = std::max(_net_nodes, min_nodes());
  c->parse(cmd);
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::print_spice(OMSTREAM& o, int)const
{
  const COMMON_COMPONENT* c = common();
  assert(c);

  o << short_label();
  printnodes(o);
  c->print(o);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
