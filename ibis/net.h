/*$Id: net.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
/*--------------------------------------------------------------------------*/
#include <string>
#include <iostream>
#include "file.h"
class Ibis_File;
class Instance_List;
class Macro;
/*--------------------------------------------------------------------------*/
namespace Netlist {
  void make_components(std::ostream& out, const Component_List& list,
		       const std::string& prefix, const std::string& state);
  void make_ebd(std::ostream& out, const Board_List& list,
		const std::string& prefix, const std::string& state);
  void make_models(std::ostream& out, const Model_List& list,
		   const std::string& prefix, const std::string& state);
  void make_instances(std::ostream& out, const Instance_List& list,
		      const std::string& prefix, const std::string& state);
  void make_macro(std::ostream& out, const Macro* m,
		  const std::string& subckt_name, const std::string& state);
  void make_file(const Ibis_File&, const std::string& file_name,
		 const std::string& state, const std::string& prefix);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
