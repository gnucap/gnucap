/*$Id: main.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
void make_dump_file(const Ibis_File& file, const std::string& name)
{
  {if (name == "-") {
    std::cout << file;
  }else{
    std::ofstream out(name.c_str());
    if (!out) {
      untested();
      os_error(file.name());
    }
    out << file;
  }}
}
/*--------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
  CS::set_exact();
  {if (argc < 2) {
    std::cerr << "IBIS-X preliminary translator version 2003.04.15\n"
      "Copyright 2003, Albert Davis\n"
      "This is preliminary, incomplete software,\n"
      "Redistribution is governed by the GNU General Public License.\n"
      "See the file \"COPYING\" for details.\n"
      "Please send comments to aldavis@ieee.org\n";
  }else{
    std::string prefix = "";
    std::string state = "";
    std::string mode = "typ";
    Ibis_File *f = NULL;
    {for (char** ap = argv+1; *ap; ++ap) {
      std::string arg(*ap);
      {try {
	std::cerr << arg << ' ';
	{if (arg == "--typ") {
	  set_typ();
	  mode = "typ";
	}else if (arg == "--min") {
	  set_min();
	  mode = "min";
	}else if (arg == "--max") {
	  set_max();
	  mode = "max";
	}else if (arg == "--power-on") {
	  state = "power_on";	  
	}else if (arg == "--on") {
	  state = "on";	  
	}else if (arg == "--off") {
	  state = "off";	  
	}else if (arg == "--prefix") {
	  char** next = ap+1;
	  {if (*next) {
	    prefix = *next;
	    ap = next;
	  }}
	}else if (arg == "--dump") {
	  {if (f) {
	    std::string name;
	    char** next = ap+1;
	    {if (!*next || ((*next)[0] == '-' && (*next)[1] == '-')) {
	      name = f->name() + ".dump";
	    }else{
	      name = *next;
	      ap = next;
	    }}
	    std::cerr << name;
	    make_dump_file(*f, name);
	  }else{
	    throw "no in file";
	  }}
	}else if (arg == "--spice") {
	  {if (f) {
	    std::string name;
	    char** next = ap+1;
	    {if (!*next || ((*next)[0] == '-' && (*next)[1] == '-')) {
	      name = f->name() + "." + mode + ".ckt";
	    }else{
	      name = *next;
	      ap = next;
	    }}
	    std::cerr << name;
	    Netlist::make_file(*f, name, state, prefix);
	  }else{
	    throw "no in file";
	  }}
	}else{
	  delete f;
	  f = new Ibis_File(arg);
	}}
	std::cerr << '\n';
      }catch(...) {
	std::cerr << arg << ": error - giving up\n";
      }}
    }}
  }}
  return 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
