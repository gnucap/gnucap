/*$Id: d_dot.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
 */
#ifndef D_DOT_H
#define D_DOT_H
#include "e_card.h"
/*--------------------------------------------------------------------------*/
class DEV_DOT : public CARD {
private:
  std::string	_s;
  explicit DEV_DOT(const DEV_DOT& p) :CARD(p) {set_constant(true);}
public:
  explicit	DEV_DOT()		:CARD() {set_constant(true);}
private: // override virtual
  std::string   value_name()const	{return "";}
  char		id_letter()const	{untested();return '\0';}
  std::string	dev_type()const		{untested();return "dotcard";}
  CARD*		clone()const		{return new DEV_DOT(*this);}
public:
  void set(const std::string& S) {_s = S;}
  const std::string& s()const {return _s;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
