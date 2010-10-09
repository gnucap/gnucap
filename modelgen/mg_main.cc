/*$Id: mg_main.cc,v 26.81 2008/05/27 05:33:43 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
//testing=script 2006.11.01
#include "u_opt.h"
#include "mg_out.h"
#include "patchlev.h"
/*global*/ int errorcount = 0;
RUN_MODE ENV::run_mode = rPRE_MAIN;
/*--------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
  try {
    if (argc > 2 && strcmp(argv[1],"-h")==0) {
      for (int i=2; i<argc; ++i) {
	File f(argv[i]);
	make_h_file(f);
      }
    }else if (argc > 2 && strcmp(argv[1],"-cc")==0) {
      for (int i=2; i<argc; ++i) {
	File f(argv[i]);
	make_cc_file(f);
      }
    }else if (argc > 2 && strcmp(argv[1],"-dump")==0) {
      for (int i=2; i<argc; ++i) {
	File f(argv[i]);
	make_dump_file(f);
      }
    }else if (argc > 2 && strcmp(argv[1],"-v")==0) {untested();
      std::cerr <<
	"Gnucap model compiler "  PATCHLEVEL  "\n"
	"Part of the Gnu Circuit Analysis Package\n"
	"Never trust any version less than 1.0\n"
	" or any version with a number that looks like a date.\n"
	"Copyright 2001-2003, Albert Davis\n"
	"Gnucap comes with ABSOLUTELY NO WARRANTY\n"
	"This is free software, and you are welcome\n"
	"to redistribute it under certain conditions\n"
	"according to the GNU General Public License.\n"
	"See the file \"COPYING\" for details.\n";
    }else if (argc > 1) {itested();
      for (int i=1; i<argc; ++i) {itested();
	File f(argv[i]);
	make_h_file(f);
	make_cc_file(f);
      }
    }else{untested();
      throw Exception("no input files");
    }
  }catch (Exception e) {
    std::cout << e.message() << '\n';
  }
  return errorcount;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
