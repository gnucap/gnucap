/*$Id: l_lib.h 2014/11/23$ -*- C++ -*-
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
inline std::string	to_string(std::string const& s) { return s;}
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
// wrappers for old standard C library
namespace OS {
  inline void system(const std::string& s) {itested();
    ::system(s.c_str());
  }

  inline void chdir(const std::string& s) {
    ::chdir(s.c_str());
  }

  inline void remove(const std::string& s) {itested();
    ::remove(s.c_str());
  }

  inline bool access_ok(const std::string& file, int mode) {
    return (::access(file.c_str(), mode) == 0/*file_ok*/);
  }

  inline std::string getcwd() {
    char buf[BUFLEN+1];
    char* cwd = ::getcwd(buf,BUFLEN);
    if (cwd) {
      return cwd;
    }else{untested();
      return "";
    }
  }

  inline std::string dirname(std::string const& fn) {
    char* buf = strdup(fn.c_str());
    char* dir = ::dirname(buf);
    std::string ret(dir);
    free(buf);
    return ret;
  }

  inline std::string basename(std::string const& fn) {
    char* buf = strdup(fn.c_str());
    char* base = ::basename(buf);
    std::string ret(base);
    free(buf);
    return ret;
  }

  inline std::string getenv(const std::string& s) {
    char* ev = ::getenv(s.c_str());
    if (ev) {
      return ev;
    }else{
      return "";
    }
  }

  inline void setenv(const std::string& Name, const std::string& Value, bool Overwrite=true) {
    if (Name == "") {untested();
      throw Exception("setenv: bad name " + Name);
    }else if (Value == "") {untested();
      throw Exception("setenv: bad value " + Value);
    }else if (!Overwrite && getenv(Name) != "") {
      error(bDEBUG, "setenv: " + Name + " overwrite prohibited");
    }else{
      std::string ev = Name + "=" + Value;
      char *es = ::strdup(ev.c_str());	//BUG// memory leak
      assert(es);
      if (::putenv(es) != 0) {untested();
	throw Exception("");
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
