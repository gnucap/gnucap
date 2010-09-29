/*$Id: mg_main.cc,v 21.14 2002/03/26 09:20:13 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 */
#include "mg_out.h"
/*global*/ int errorcount = 0;
/*--------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
  {if (argc > 2 && strcmp(argv[1],"-h")==0) {
    {for (int i=2; i<argc; ++i) {
      File f(argv[i]);
      make_h_file(f);
    }}
  }else if (argc > 2 && strcmp(argv[1],"-cc")==0) {
    {for (int i=2; i<argc; ++i) {
      File f(argv[i]);
      make_cc_file(f);
    }}
  }else if (argc > 1) {
    {for (int i=1; i<argc; ++i) {
      File f(argv[i]);
      make_h_file(f);
      make_cc_file(f);
    }}
  }else{
    error("no input files");
  }}
  return errorcount;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
