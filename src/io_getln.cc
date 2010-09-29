/*$Id: io_getln.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * get a bunch of lines, from a file
 * interface is just line fgets.
 * hooks together extension lines
 * not recommended for getting from stdin.  use for files only.
 * is always a line ahead
 * start with + is extension line, spice compatibility.
 */
#include "l_lib.h"
// testing=nonstrict
/*--------------------------------------------------------------------------*/
std::string getlines(FILE *fileptr)
{
  assert(fileptr);
  const int buffer_size = BIGBUFLEN;
  std::string s;

  bool more = true;  // get another line (extend)
  while (more){
    char buffer[buffer_size+1];
    char* got = fgets(buffer, buffer_size, fileptr);
    {if (!got){ // probably end of file
      more = false;
    }else{
      trim(buffer);
      int count = strlen(buffer);
      {if (buffer[count-1] == '\\'){
	untested();
	buffer[count-1] = '\0';
      }else{
	int c = fgetc(fileptr);
	{if (c == '+'){
	  more = true;
	}else{
	  more = false;
	  ungetc(c,fileptr);
	}}
      }}
      s += buffer;
      s += ' ';
    }}
  }
  return s;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
