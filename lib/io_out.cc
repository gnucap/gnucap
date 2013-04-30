/*$Id: io_out.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * output text to files, devices, or whatever
 * m???? = multiple output to a bunch of io devices.
 *	    with character count (so tab will work)
 * Will start a new line first if the entire output will not fit.
 * so wrap will not break a word or number.
 * Where is a bit mask of places to send the output.
 * A possible portability problem exists with the handle numbers.
 * It assumes they start at 0, and count up, and that there are no more than
 * the number of bits in an integer (MAXHANDLE).
 * but I have yet to find a system that did not meet this form.
 */
//testing=script,sparse 2006.07.17
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
	const char* octal(int x);
//	OMSTREAM & OMSTREAM::tab(int count)
//	OMSTREAM & OMSTREAM::form(const char*,...);
//	OMSTREAM & OMSTREAM::operator<<(const char *str)
//	OMSTREAM & OMSTREAM::operator<<(char chr)
/*--------------------------------------------------------------------------*/
unsigned OMSTREAM::_cpos[MAXHANDLE+1];         /* character counter    */
/*--------------------------------------------------------------------------*/
/* octal: make octal string for an int
 */
const char* octal(int x)
{
  static char s[sizeof(int)*3+1];
  sprintf(s, "%o", x);
  return s;
}
/*--------------------------------------------------------------------------*/
/* mtab: tab to column "count" on output devices "where"
 * by outputting spaces.
 * If already beyond, start new line, then tab to column.
 */
OMSTREAM & OMSTREAM::tab(unsigned count)
{
  for (int ii=0, mm=1;   ii<=MAXHANDLE;   ++ii, mm<<=1) {
    if (_mask & mm) {
      OMSTREAM this_file(_mask & mm);
      if (_cpos[ii] > count) {
	this_file << '\n';
      }else{
      }
      while (_cpos[ii]<count) {
	this_file << ' ';
      }
    }else{
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* mprintf: multiple printf
 * printf to "m" style files.
 */
OMSTREAM & OMSTREAM::form(const char *fmt, ...)
{
  char buffer[BIGBUFLEN];
  va_list arg_ptr;
  
  va_start(arg_ptr,fmt);
  vsprintf(buffer,fmt,arg_ptr);
  va_end(arg_ptr);
  
  *this << buffer;
  return *this;
}
/*--------------------------------------------------------------------------*/
/* mputs: multiple puts.
 * puts to "m" style files.
 * also....
 * starts new line, prefixes it with + if it would exceed width
 * width is handled separately for each file to support different widths
 * (which are not currently handled by .options)
 * and it is possible that current contents of lines may be different
 */
OMSTREAM & OMSTREAM::operator<<(const char *str)
{
  assert(str);

  if (_mask & 1) {
    unreachable(); 
    _mask &= ~1;
    error(bDANGER, "internal error: out to stdin\n");
  }else{
  }

  bool newline = false;	/* true if any destination is at beginning of line */
  size_t sl = strlen(str);

  /* kluge:  If a string ends with certain characters,
   * require some extra space following it, possibly forcing to next line.
   * Why? to avoid a line break between '=' and its argument.
   */
  if (sl == 0) {
  }else if (strchr("@", str[sl-1])) {
    sl += 16;
  }else if (strchr("=", str[sl-1])) {
    sl += 15;
  }else if (strchr("(", str[sl-1])) {
    sl += 12;
  }else{
  }

  /* auto line break, with a '+' to continue. */
  for (int ii=0, mm=1;   ii<=MAXHANDLE;   ++ii, mm<<=1) {
    if ((_mask & mm)
	&& (sl+_cpos[ii]) >= OPT::outwidth
	&& _cpos[ii] != 0) {
      OMSTREAM this_file(_mask & mm);
      this_file << '\n' << '+';
    }else{
    }				/* see if it fits .... */
    if (_cpos[ii]==0) {		/* if not, next line   */
      newline = true;
    }else{
    }
  }

  if (cipher() && newline) {untested(); 
    *this << '\t';
  }else{
  }

  while (*str && (str[1] || *str != '@')) {
    *this << *str++;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* mputc: multiple putc
 * multiple putc
 * also....
 * crunch spaces, if selected
 * encripts, if selected
 * keeps track of character count
 */
OMSTREAM & OMSTREAM::operator<<(char chr)
{
  if (_mask & 1) {
    unreachable(); 
    _mask &= ~1;
    error(bDANGER, "internal error: out to stdin\n");
  }else{
  }

  static int old = '\0';
  static int cchr = 'w';		/* starting encryption seed	    */
					/* arbitrary printable character    */
  bool count;
  if (chr=='\t') {itested();
    chr = ' ';
    count = false;
  }else{
    count = true;
  }
  
  bool suppress = (pack() && old==' ' && chr==' ');
  old = chr;
  if (cipher() && !suppress && isprint(chr)) {untested(); 
    cchr += static_cast<int>(chr);
    while (!isascii(cchr)  ||  !isprint(cchr)) {untested();
      cchr -= (0x7f-0x20);
    }
    chr = static_cast<char>(cchr);
  }else{
  }
  
  for (int ii=0, mm=1;   ii<=MAXHANDLE;   ++ii, mm<<=1) {
    if (_mask & mm) {
      assert(IO::stream[ii]);
      if (chr=='\b') {untested();
	--_cpos[ii];
	fflush(IO::stream[ii]);
      }else if (count) {
	++_cpos[ii];
      }else{itested();
      }
      
      if (chr=='\n') {
	_cpos[ii] = 0;
	fflush(IO::stream[ii]);
      }else if (chr=='\r') {itested();
	if (_cpos[ii] == 0) {untested();
	  suppress = true;
	}else{itested();
	  _cpos[ii] = 0;
	  fflush(IO::stream[ii]);
	}
      }else{
      }
      if (!suppress) {
	fputc(chr,IO::stream[ii]);
      }else{itested();
      }
    }else{
    }
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
