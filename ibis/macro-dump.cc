/*$Id: macro-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
const char* c_str(Symbol_Type t)
{
  switch (t) {
  case tNODE:		return "node";
  case tKEYWORD:	return "keyword";
  case tSCALAR:		return "scalar";
  case tVECTOR:		return "table";
  case tSCOPE:		return "scope";
  case tRAMP:		return "ramp";
  case tSTRINGVAR:	return "string";
  case tFOREACH:	return "foreach";
  case tNUMBER_CONST:	untested(); return "number_constant";
  case tSTRING_CONST:	untested(); return "string_constant";
  };
  unreachable();
  return "???error???";
}
/*--------------------------------------------------------------------------*/
std::ostream& operator<<(std::ostream& out, Symbol_Type t)
{
  out << c_str(t);
  return out;
}
/*--------------------------------------------------------------------------*/
void Symbol::dump(std::ostream& out)const
{
  out << "| " << type() << ' ' << name() << '\n';
}
/*--------------------------------------------------------------------------*/
void Symbol_Table::dump(std::ostream& out)const
{
  out << "|symbols\n";
  {for (const_iterator i = begin(); i != end(); ++i) {
    if (i->second) { // fix because failed lookup produces a null entry
      out << *(i->second);
    }
  }}
  out << "|end symbols\n";
}
/*--------------------------------------------------------------------------*/
void Ramp_Name::dump(std::ostream& out)const
{
  out << _name;
}
/*--------------------------------------------------------------------------*/
void Test::dump(std::ostream& out)const
{
  out << _expr;
}
/*--------------------------------------------------------------------------*/
void Port_List::dump(std::ostream& out)const
{
  if (begin() != end()) {
    out << "( ";
    {for (const_iterator i = begin(); i != end(); ++i) {
      assert(*i);
      out << **i << ' ';
    }}
    out << ")";
  }
}
/*--------------------------------------------------------------------------*/
void Ckt_Block::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << ' ' << ports() << ' ' 
      << lvalue() << " = " << value() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Driver::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << ' ' << ports() << " ("
    " S0 = " << s0() << ","
    " S1 = " << s1() << ","
    " W01 = " << w01() << ","
    " W10 = " << w10() << ","
    " Ramp = " << ramp()
      << " )\n";
}
/*--------------------------------------------------------------------------*/
void Reshape::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << ' ' << ports() << ' ' 
      << rise_on() << ' ' << rise_off() << ' '
      << fall_on() << ' ' << fall_off() << '\n';
}
/*--------------------------------------------------------------------------*/
void T_Line::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << ' ' << ports()
      << " Z0 = " << z0() << ", Delay = " << delay() << '\n';
}
/*--------------------------------------------------------------------------*/
void Subckt::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << ' ' << ports() << ' ' 
      << subckt() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Assert::dump(std::ostream& out)const
{
  out << type_name() << ' ' << test() << '\n';
}
/*--------------------------------------------------------------------------*/
void Define::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << " = " << value() << '\n';
}
/*--------------------------------------------------------------------------*/
void Export::dump(std::ostream& out)const
{
  out << type_name() << ' ' << _string << '\n';
}
/*--------------------------------------------------------------------------*/
void Foreach::dump(std::ostream& out)const
{
  out << type_name() << ' ' << iterator() << " in " << name() << '\n'
      << body()
      << "else\n"
      << false_part()
      << "end foreach\n";
}
/*--------------------------------------------------------------------------*/
void If::dump(std::ostream& out)const
{
  out << type_name() << ' ' << test() << '\n'
      << true_part()
      << "else\n"
      << false_part()
      << "end if\n";
}
/*--------------------------------------------------------------------------*/
void Inherit::dump(std::ostream& out)const
{
  out << type_name() << ' ' << _string << '\n';
}
/*--------------------------------------------------------------------------*/
void Declare::dump(std::ostream& out)const
{
  out << _type << ' ' << _string << '\n';
}
/*--------------------------------------------------------------------------*/
void Case::dump(std::ostream& out)const
{
  out << type_name() << ' ' << name() << '\n'
      << body()
      << "end case\n";
}
/*--------------------------------------------------------------------------*/
void Select::dump(std::ostream& out)const
{
  out << type_name() << " (" << control() << ")\n";
  List_Base<Case>::dump(out);
  out << "end select\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Macro::dump(std::ostream& out)const
{
  out <<
    "[Define " << type() << "] " << name() << ' ' << ports() << '\n'
      << symbols()
      << body()
      << "[End Define " << type() << "]\n\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
