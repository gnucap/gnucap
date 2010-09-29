/*$Id: file-net.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "net.h"
/*--------------------------------------------------------------------------*/
namespace Netlist {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void make_header(std::ostream& out, const Overhead& o)
{
  out << "*[File Name] " << o.file_name()  << '\n'
      << "*[File Rev]  " << o.file_rev()   << '\n'
      << "*[Date]      " << o.date()       << '\n';
    //<< "*[Source]    " << o.source()     << '\n'
    //<< "*[Notes]     " << o.notes()      << '\n'
    //<< "*[Disclaimer]" << o.disclaimer() << '\n'
    //<< "*[Copyright] " << o.copyright()  << '\n';
}
/*--------------------------------------------------------------------------*/
void make_body(std::ostream& out, const Ibis_File& file,
	       const std::string& state, const std::string& prefix)
{
  out << "*[IBIS Ver]  " << file.ibis_ver()   << '\n'
      << ".option namednodes\n";
  make_header(	  out, file.overhead());
  make_components(out, file.components(), prefix, state);
  make_ebd(	  out, file.boards(),	  prefix, state);
  make_models(	  out, file.models(),	  prefix, state);
  make_instances( out, file.instances(),  prefix, state);
  out << ".option nonamednodes\n";
}
/*--------------------------------------------------------------------------*/
void make_file(const Ibis_File& file, const std::string& filename,
	       const std::string& state, const std::string& prefix)
{
  {if (filename == "-") {
    make_body(std::cout, file, state, prefix);
  }else{
    std::ofstream out(filename.c_str());
    if (!out) {
      untested();
      os_error(file.name());
    }
    make_body(out, file, state, prefix);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
