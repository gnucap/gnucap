/*$Id: ebd-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
/*--------------------------------------------------------------------------*/
void Board::Pin::dump(std::ostream& out)const
{
  out << name() << ' ' << signal() << '\n';
}
/*--------------------------------------------------------------------------*/
void Board::Path_List::dump(std::ostream& out)const
{
  {for (const_iterator i = begin(); i != end(); ++i) {
    out << "[Path Description] " << **i;
  }}
}
/*--------------------------------------------------------------------------*/
void Board::Ref_Des_Map::dump(std::ostream& out)const
{
  out << name() << ' ' << file() << ' ' << component() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Board::dump(std::ostream& out)const
{
  out <<
    "[Begin Board Description] " << name()	  << "\n"
    "[Manufacturer] "		 << manufacturer()<< "\n\n"
    "[Number of Pins] "		 << n_pins()	  << "\n"
    "[Pin List]\n"		 << pins()	  << "\n"
      << paths() << "\n"
    "[Reference Designator Map]\n" << ref_des_map() << "\n"
    "[End Board Description]\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
