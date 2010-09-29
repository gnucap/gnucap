/*$Id: u_cardst.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * a structure to stash a "card" for fault. sweep, etc.
 */
#ifndef U_CARDST_H
#define U_CARDST_H
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class CARDSTASH {
private:
  COMPONENT*		_brh;
  double		_value;
  COMMON_COMPONENT*	_c;
public:
  explicit CARDSTASH()		:_brh(0), _value(0.), _c(0) {}
  explicit CARDSTASH(CARD* b);
	   CARDSTASH(const CARDSTASH& p);
	   ~CARDSTASH()		{detach_common(&_c);}
  void	   operator=(CARD* b);
  void	   restore()		{assert(_brh); _brh->set_value(_value, _c);}
  bool operator==(const CARDSTASH&)const
	{incomplete();unreachable();return false;}
  bool operator!=(const CARDSTASH&)const
	{incomplete();unreachable();return false;}
  bool operator<(const CARDSTASH&)const
	{incomplete();unreachable();return false;}
  bool operator>(const CARDSTASH&)const
	{incomplete();unreachable();return false;}
  // comparisons here because MS list requires them, even though not used here.
};
/*--------------------------------------------------------------------------*/
inline void CARDSTASH::operator=(CARD* b)
{
  assert(b);
  _brh = prechecked_cast<COMPONENT*>(b);
  assert(_brh);
  _value = _brh->value();
  detach_common(&_c);
  attach_common(_brh->mutable_common(), &_c);
}
/*--------------------------------------------------------------------------*/
inline CARDSTASH::CARDSTASH(CARD* b)
  :_brh(0),
   _value(NOT_VALID),
   _c(0)  
{
  *this = b;
}
/*--------------------------------------------------------------------------*/
inline CARDSTASH::CARDSTASH(const CARDSTASH& p)
  :_brh(p._brh),
   _value(p._value),
   _c(0)
{
  attach_common(p._c, &_c);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
