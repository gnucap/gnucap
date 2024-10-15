/*$Id: u_function.h 2015/01/21 al $ -*- C++ -*-
 * Copyright (C) 2008 Albert Davis
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
//testing=none
#ifndef U_FUNCTION_H
#define U_FUNCTION_H
#include "e_base.h"
/*--------------------------------------------------------------------------*/
class CARD_LIST;
class Expression;
/*--------------------------------------------------------------------------*/
class FUNCTION : public CKT_BASE {
#ifndef NDEBUG
  mutable int _which{0}; // catch infinite loop
#endif

  //BUG//needs constructors
  //BUG//need label support
public:
  virtual std::string eval(CS&, const CARD_LIST*)const;
  virtual void stack_op(Expression*)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
