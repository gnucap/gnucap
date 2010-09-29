/*$Id: comp-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include <fstream>
#include "ebd.h"
#include "comp.h"
/*--------------------------------------------------------------------------*/
void Component::Pin_Die::dump(std::ostream& out)const
{
  switch (_pd) {
  case pdPIN: out << "Pin"; return;
  case pdDIE: out << "Die"; return;
  }
  unreachable();
  out << " ?error? " << int(_pd);
}
/*--------------------------------------------------------------------------*/
void Component::Package::dump(std::ostream& out)const
{
  out << "\n"
    "| variable typ min max\n"
    "R_pkg " << r_pkg() << "\n"
    "L_pkg " << c_pkg() << "\n"
    "C_pkg " << c_pkg() << "\n";
}
/*--------------------------------------------------------------------------*/
void Component::Pin::dump(std::ostream& out)const
{
  out << name() << ' ' << signal() << ' ' << model();
  if (r_pin() != NA || l_pin() != NA || c_pin() != NA) {
    out << ' ' << r_pin() << ' ' << l_pin() << ' ' << c_pin();
  }
  out << '\n';
}
/*--------------------------------------------------------------------------*/
void Component::Pin_Map::dump(std::ostream& out)const
{
  out << name() << ' ' << pulldown_ref() << ' ' << pullup_ref();
  if (gnd_clamp_ref() != "" || pwr_clamp_ref() != "") {
    out << ' ' << gnd_clamp_ref() << ' ' << pwr_clamp_ref();
  }
  out << '\n';
}
/*--------------------------------------------------------------------------*/
void Component::Diff_Pin::dump(std::ostream& out)const
{
  out << pin() << ' ' << diff_pin() << ' ' << vdiff() << ' ' 
      << tdelay() << '\n';
}
/*--------------------------------------------------------------------------*/
void Component::Series_Pin::dump(std::ostream& out)const
{
  out << pin1() << ' ' << pin2() << ' ' << model() << ' ' << group() << '\n';
}
/*--------------------------------------------------------------------------*/
void Component::Series_Switch_List::dump(std::ostream& out)const
{
  out << '\n';
  {for (const_iterator i = begin(); i != end(); ++i) {
    out << ((i->second) ? "On  " : "Off ") << i->first << " /\n";
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Component::dump(std::ostream& out)const
{
  out <<
    "[Component] "	    << name()		<< "\n"
    "Si_location "	    << si_location()	<< "\n"
    "Timing_location "	    << timing_location()<< "\n"
    "[Manufacturer] "	    << manufacturer()	<< "\n\n"
    "[Package] "	    << package()	<< "\n"
    "[Pin] "		    << pins()		<< "\n"
    "[Pin Mapping]\n"	    << pin_mapping()	<< "\n"
    "[Diff Pin]\n"	    << diff_pins()	<< "\n"
    "[Series Pin Mapping]\n"<< series_pins()	<< "\n"
    "[Series Switch Groups]"<< series_switches()<< "\n"
    "|[End Component]\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
