/*$Id: pkg-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "pkg.h"
/*--------------------------------------------------------------------------*/
void Package_Model::Matrix::Type::dump(std::ostream& out)const
{
  switch (_type) {
  case FULL:	out << "Full_Matrix";   return;
  case BANDED:	out << "Banded_Matrix"; return;
  case SPARSE:	out << "Sparse_Matrix"; return;
  case NA:	untested();out << "uninitialized"; /* drop out */
  }
  unreachable();
  out << " ?error? " << int(_type);
}
/*--------------------------------------------------------------------------*/
void Package_Model::Matrix::Row::dump(std::ostream& out)const
{
  {if (_size > 0) {
    int i;
    {for (i=0; i<_size; ++i) {
      out << _data[i] << ' ';
      if ((i+1)%5 == 0) {
	out << '\n';
      }
    }}
    if (i%5 != 0) {
      out << '\n';
    }
  }else{
    {for (std::map<int,double>::const_iterator
	   i = _sparse.begin(); i != _sparse.end(); ++i) {
      out << i->first << ' ' << i->second << '\n';
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Package_Model::Matrix::dump(std::ostream& out)const
{
  out << _type << '\n';
  switch (_type.tag()) {
  case Type::NA:     unreachable();	break;
  case Type::FULL:   break;
  case Type::SPARSE: break;
  case Type::BANDED: out << "[Bandwidth ] " << _bandwidth << '\n'; break;
  }
  {for (int i=0; i<_n_rows; ++i) {
    out << "[Row ]" << i+1 << '\n';
    {if (_data[i]) {
      out << *(_data[i]) << '\n';
    }else{
      out << "??error - row missing\n";
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Package_Model::dump(std::ostream& out)const
{
  out <<
    "[Define Package Model] "	<< name()	 << "\n"
    "[Manufacturer] "		<< manufacturer()<< "\n"
    "[OEM] "			<< oem()	 << "\n"
    "[Description] "		<< description() << "\n";
  if (n_sections() > 0) {
    out << "[Number Of Sections] " << n_sections() << "\n";
  }
  out <<
    "[Number Of Pins] " 	<< n_pins()	 << "\n"
    "[Pin Numbers]\n"		<< pins()	 << "\n";
  if (n_sections() == 0) {
    out << "[Model Data]\n"
      "[Resistance Matrix] "	<< resistance_matrix()	<< "\n"
      "[Inductance Matrix] "	<< inductance_matrix()	<< "\n"
      "[Capacitance Matrix] "	<< capacitance_matrix()	<< "\n"
      "[End Model Data]\n";
  }
  out <<
    "[End Package Model]\n\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
