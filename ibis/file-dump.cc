/*$Id: file-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "file.h"
/*--------------------------------------------------------------------------*/
void Overhead::dump(std::ostream& out)const
{
  out <<
    "[File Name] " << file_name()  << "\n"
    "[File Rev]  " << file_rev()   << "\n"
    "[Date]      " << date()       << "\n"
    "[Source]    " << source()     << "\n"
    "[Notes]     " << notes()      << "\n"
    "[Disclaimer] "<< disclaimer() << "\n"
    "[Copyright] " << copyright()  << "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Choice::dump(std::ostream& out)const
{
  out << name() << ' ' << description() << '\n';
}
/*--------------------------------------------------------------------------*/
void Model_Selector::dump(std::ostream& out)const
{
  out << "[Model Selector] " << name() << '\n';
  List<Model_Choice>::dump(out);
  out << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Ibis_File::dump(std::ostream& out)const
{
  out << "[IBIS Ver]  " << ibis_ver()   << "\n"
      << overhead()		<< '\n'
      << components()		<< '\n'
      << model_selectors()	<< '\n'
      << package_models()	<< '\n'
      << boards()		<< '\n'
      << models()		<< '\n'
      << macros()		<< '\n'
      << instances()		<< '\n'
      << "[End]\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
