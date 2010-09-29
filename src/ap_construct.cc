/*$Id: ap_construct.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * construction, copy, etc.
 */
#include <fcntl.h>
#include "ap.h"
/*--------------------------------------------------------------------------*/
CS::CS(CS_FILE, const std::string& name, int i)
  :_name(0),
   _cmd(0),
   _cnt(i),
   _ok(true),
   _length(0),
   _begin_match(0),
   _end_match(0)
{
  _name = new char[name.length()+1];
  strcpy(_name, name.c_str());
  int f = open(_name, O_RDONLY);
  if (f == EOF) {
    error(bERROR, name + ':' + strerror(errno));
  }
  _length = lseek(f, off_t(0), SEEK_END);
  lseek(f, off_t(0), SEEK_SET);
  _cmd = new char[_length+2];
  read(f, _cmd, _length);
  _cmd[_length++] = '\0';
  close(f);
  assert(_cmd);
}
/*--------------------------------------------------------------------------*/
CS::CS(const std::string& s, int i)
  :_name(0),
   _cmd(0),
   _cnt(i),
   _ok(true),
   _length(s.length()),
   _begin_match(0),
   _end_match(0)
{
  _cmd = new char[_length+1];
  strcpy(_cmd, s.c_str());
}
/*--------------------------------------------------------------------------*/
CS::CS(const CS& p)
  :_name(0),
   _cmd(0),
   _cnt(p._cnt),
   _ok(p._ok),
   _length(p._length),
   _begin_match(0),
   _end_match(0)
{
  if (p._name) {
    _name = new char[strlen(p._name)+1];
    strcpy(_name, p._name);
  }
  _cmd = new char[_length+1];
  strcpy(_cmd, p._cmd);
}
/*--------------------------------------------------------------------------*/
CS& CS::operator=(const std::string& s)
{
  if (_cmd){
    delete[] _cmd;
  }
  _cnt = 0;
  _ok = true;
  _length = s.length();
  _cmd = new char[_length+1];
  strcpy(_cmd, s.c_str());
  return *this;
}    
/*--------------------------------------------------------------------------*/
CS& CS::operator=(const CS& p)
{
  assert(&p != this);
  if (p._name) {
    if (_name) {
      delete[] _name;
    }
    _name = new char[strlen(p._name)+1];
    strcpy(_name, p._name);
  }
  if (_cmd){
    delete[] _cmd;
  }
  _cnt = p._cnt;
  _ok = p._ok;
  _length = p._length;
  _cmd = new char[_length+1];
  strcpy(_cmd, p._cmd);
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
