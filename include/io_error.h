/*$Id: io_error.h,v 26.130 2009/11/15 21:51:59 al Exp $ -*- C++ -*-
 * data for error and exception handling
 *
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
//testing=trivial 2006.07.17
#ifndef bDANGER
//#include "md.h"  included by
//#include "io_trace.h"
/*--------------------------------------------------------------------------*/
/* arg to error() (badness) to decide severity of exception */
#define	bNOERROR	0
#define bTRACE		1
#define bLOG		2
#define bDEBUG		3
#define bPICKY		4
#define	bWARNING	5
#define bDANGER		6
INTERFACE void error(int,const char*,...);
INTERFACE void error(int,const std::string&);
INTERFACE std::string to_string(int);
/*--------------------------------------------------------------------------*/
struct Exception {
  std::string _message;
  virtual const std::string message()const {return _message;}
  Exception(const std::string& Message)
    :_message(Message) {
  }
  virtual ~Exception() {}
};
class CS;
struct Exception_CS :public Exception {
  std::string _cmd;
  unsigned _cursor;
  const std::string message()const;
  //Exception_CS(const std::string& Message, const CS& cmd, unsigned cursor);
  Exception_CS(const std::string& Message, const CS& cmd);
};
struct Exception_No_Match :public Exception{
  std::string _key;
  Exception_No_Match(const std::string& key)
    :Exception("no match: " + key),
     _key(key) {
  }
};
struct Exception_Cant_Find :public Exception{
  std::string _device, _key, _scope;
  Exception_Cant_Find(const std::string& dev, const std::string& key, const std::string& scope)
    :Exception(dev + ": can't find: " + key + " in " + scope),
     _device(dev), _key(key), _scope(scope) {
  }
  Exception_Cant_Find(const std::string& dev, const std::string& key)
    :Exception(dev + ": can't find: " + key),
     _device(dev), _key(key), _scope("") {
  }
};
struct Exception_Too_Many :public Exception{
  int _requested, _max, _offset;
  Exception_Too_Many(int requested, int max, int offset)
    :Exception("too many: requested=" + to_string(requested+offset)
	       + " max=" + to_string(max+offset)),
     _requested(requested), _max(max), _offset(offset) {untested();
  }
};
struct Exception_Type_Mismatch :public Exception{
  std::string _device, _name, _need_type;
  Exception_Type_Mismatch(const std::string& dev, const std::string& name,
			  const std::string& type)
    :Exception(dev + ": " + name + " is not a " + type),
     _device(dev), _name(name), _need_type(type) {
  }
};
struct Exception_Model_Type_Mismatch :public Exception{
  std::string _device, _modelname, _need_type;
  Exception_Model_Type_Mismatch(const std::string& d, const std::string& m,
				const std::string& n)
    :Exception(d + ": model " + m + " is not a " + n),
     _device(d), _modelname(m), _need_type(n) {
  }
};
struct Exception_End_Of_Input :public Exception{
  Exception_End_Of_Input(const std::string& Message) 
    :Exception(Message) {
  }
};
struct Exception_File_Open :public Exception{
  Exception_File_Open(const std::string& Message) 
    :Exception(Message) {
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
