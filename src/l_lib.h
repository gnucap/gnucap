/*$Id: l_lib.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
#ifndef L_LIB_H
#define L_LIB_H
#include "md.h"
/*--------------------------------------------------------------------------*/
	char*	trim(char*);
	int	pmatch(const char*,const char*);
	bool	wmatch(const char*,const char*);
/*--------------------------------------------------------------------------*/
//ftos stuff
	char*	utos(unsigned,char*,int);
	char*	itos(int,char*,int,int);
	char*	ftos(double,int,int,int);
enum {			/* formatting bit-fields */
  ftos_DEFAULT = 0,	/* default formatting, with letters */
  ftos_EXP = 1,		/* use 'e' notation, almost like printf */
  ftos_SIGN = 2,	/* always include sign */
  ftos_FILL = 4		/* fill in trailing zeros */
};
std::string to_string(unsigned);
/*--------------------------------------------------------------------------*/
// wrappers for type conversion
inline bool wmatch(const std::string& s1,const char* s2){
  return wmatch(s1.c_str(), s2);
}
inline bool wmatch(const std::string& s1,const std::string& s2){
  return wmatch(s1.c_str(), s2.c_str());
}
/*--------------------------------------------------------------------------*/
// updated forms of old stuff
namespace OS {
  inline void system(const std::string& s){
    ::system(s.c_str());
  }
  inline void chdir(const std::string& s){
    ::chdir(s.c_str());
  }
  inline void remove(const std::string& s){
    ::remove(s.c_str());
  }
  inline bool access_ok(const std::string& file, int mode){
    return (::access(file.c_str(), mode) == 0/*file_ok*/);
  }
  inline std::string getcwd(){
    char buf[BUFLEN+1];
    char* cwd = ::getcwd(buf,BUFLEN);
    {if (cwd){
      return cwd;
    }else{
      untested();
      return "";
    }}
  }
  inline std::string getenv(const std::string& s){
    char* ev = ::getenv(s.c_str());
    {if (ev){
      return ev;
    }else{
      return "";
    }}
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
