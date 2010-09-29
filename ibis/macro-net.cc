/*$Id: macro-net.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 */
#include "macro.h"
/*--------------------------------------------------------------------------*/
namespace Netlist {
/*--------------------------------------------------------------------------*/
static void make_macro_body(std::ostream& out, const Macro_Body& m)
{
  {for (Macro_Body::const_iterator i = m.begin(); i != m.end(); ++i) {
    // each **i is an element in the macro
    assert(*i);
    assert(!dynamic_cast<const Inherit*>(*i));
    // we are working with a "reduced" copy, so the following stmt
    // prints it in a reduced form, ready to simulate. (supposedly)
    out << '.' << **i;
  }}
}
/*--------------------------------------------------------------------------*/
void make_macro(std::ostream& out, const Macro* m,
		const std::string& subckt_name, const std::string&)
{
  assert(m);
  out << ".subckt " << subckt_name << " ( ";
  {for (Port_List::const_iterator
	 i = m->ports().begin();  i != m->ports().end();  ++i) {
    out << **i << ' ';
  }}
  out << ")\n";
  make_macro_body(out, m->body());
  out << ".ends " << subckt_name <<"\n\n";
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
