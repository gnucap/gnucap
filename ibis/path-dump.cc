/*$Id: path-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "path.h"
/*--------------------------------------------------------------------------*/
void Path::Pin::dump(std::ostream& out)const
{
  out << "Pin " << name() << '\n';
}
/*--------------------------------------------------------------------------*/
void Path::Seg::dump(std::ostream& out)const
{
  out << "Len = " << len();
  {if (lumped()) {
    if (r() != 0) {
      out << " R = " << r();
    }
    if (l() != 0) {
      out << " L = " << l();
    }
    if (c() != 0) {
      out << " C = " << c();
    }
  }else{
    out << " R = " << r()
	<< " L = " << l()
	<< " C = " << c();
  }}
  out << "/\n";
}
/*--------------------------------------------------------------------------*/
void Path::Fork::dump(std::ostream& out)const
{
  out << "Fork\n";
  List_Base<Path_Item_Base>::dump(out);
  out << "Endfork\n";
}
/*--------------------------------------------------------------------------*/
void Path::Node::dump(std::ostream& out)const
{
  out << "Node " << name() << '\n';
}
/*--------------------------------------------------------------------------*/
void Path::dump(std::ostream& out)const
{
  out << name() << '\n';
  List_Base<Path_Item_Base>::dump(out);
  out << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
