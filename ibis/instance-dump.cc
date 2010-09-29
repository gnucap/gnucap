/*$Id: instance-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
void Foreach_Item::dump(std::ostream& out)const
{
  {for (const_iterator i = begin(); i != end(); ++i) {
    out << *i << '\t';
  }}
  out << '\n';
}
/*--------------------------------------------------------------------------*/
void Foreach_Data::dump(std::ostream& out)const
{
  out << short_name() << '\n';
  List_Base<Foreach_Item>::dump(out);
}
/*--------------------------------------------------------------------------*/
void String_Var::dump(std::ostream& out)const
{
  out << short_name() << ' ' << value() << '\n';
}
/*--------------------------------------------------------------------------*/
void Scalar::dump(std::ostream& out)const
{
  out << short_name() << ' ' << value() << '\n';
}
/*--------------------------------------------------------------------------*/
void Table::dump(std::ostream& out)const
{
  out << short_name() << '\n';
  Vector::dump(out);
}
/*--------------------------------------------------------------------------*/
void Ramp_Data::dump(std::ostream& out)const
{
  out << short_name() << "\n"
    "dV/dt_r " << dv_dt_r() << "\n"
    "dV/dt_f " << dv_dt_f() << "\n"
    "R_load  " << r_load() << "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Instance::dump(std::ostream& out)const
{
  out << full_name() << '\n'
      << base_type() << "_type " << sub_type() << '\n';
  Scope::dump(out);
  out << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
