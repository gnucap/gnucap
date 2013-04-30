/*$Id: l_lib.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
//testing=script 2006.07.13
#ifndef L_LIB_H
#define L_LIB_H
#include "md.h"
/*--------------------------------------------------------------------------*/
	  char*		trim(char*);
INTERFACE bool		Umatch(const std::string&, const std::string&);
INTERFACE bool		wmatch(const std::string& s1,const std::string& s2);
INTERFACE std::string	to_string(unsigned);
INTERFACE std::string	to_string(int);
INTERFACE std::string	to_string(double);
INTERFACE char*		ftos(double,int,int,int);
/*--------------------------------------------------------------------------*/
//ftos stuff
enum {			/* formatting bit-fields */
  ftos_DEFAULT = 0,	/* default formatting, with letters */
  ftos_EXP = 1,		/* use 'e' notation, almost like printf */
  ftos_SIGN = 2,	/* always include sign */
  ftos_FILL = 4		/* fill in trailing zeros */
};
/*--------------------------------------------------------------------------*/
// wrappers for old standard C lpbrary
namespace OS {
  inline void system(const std::string& s) {itested();
    ::system(s.c_str());
  }

  inline void chdir(const std::string& s) {itested();
    ::chdir(s.c_str());
  }

  inline void remove(const std::string& s) {itested();
    ::remove(s.c_str());
  }

  inline bool access_ok(const std::string& file, int mode) {
    return (::access(file.c_str(), mode) == 0/*file_ok*/);
  }

  inline std::string getcwd() {itested();
    char buf[BUFLEN+1];
    char* cwd = ::getcwd(buf,BUFLEN);
    if (cwd) {itested();
      return cwd;
    }else{untested();
      return "";
    }
  }

  inline std::string getenv(const std::string& s) {
    char* ev = ::getenv(s.c_str());
    if (ev) {
      return ev;
    }else{itested();
      return "";
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
