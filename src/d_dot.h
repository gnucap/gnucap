/*$Id: d_dot.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * a "dot card"
 */
#ifndef D_DOT_H
#define D_DOT_H
#include "e_card.h"
/*--------------------------------------------------------------------------*/
class DEV_DOT : public CARD {
private:
  std::string	_s;
  explicit DEV_DOT(const DEV_DOT& p) :CARD(p) {untested(); set_constant(true);}
public:
  explicit	DEV_DOT()		:CARD() {set_constant(true);}
private: // override virtual
  char		id_letter()const	{return '\0';}
  const char*	dev_type()const		{untested(); return "dotcard";}
  CARD*		clone()const		{untested();return new DEV_DOT(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&,int)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
