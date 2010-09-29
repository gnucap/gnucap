/*$Id: d_coment.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * comment statement
 */
//testing=script 2006.07.17
#ifndef D_COMENT_H
#define D_COMENT_H
#include "e_card.h"
/*--------------------------------------------------------------------------*/
class DEV_COMMENT : public CARD {
private:
  std::string	_s;
  explicit	DEV_COMMENT(const DEV_COMMENT& p)
				:CARD(p) {untested(); set_constant(true);}
public:
  explicit	DEV_COMMENT()	:CARD()  {set_constant(true);}
private: // override virtual
  char		id_letter()const	{untested();return '\0';}
  const char*	dev_type()const	{untested(); return "comment";}
  CARD*		clone()const	{untested(); return new DEV_COMMENT(*this);}
  void		parse_spice(CS& cmd);
  void		print_spice(OMSTREAM& where, int detail)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
