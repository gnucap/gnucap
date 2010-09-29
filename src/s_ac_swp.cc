/*$Id: s_ac_swp.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * ac sweep control
 */
//testing=script,complete 2006.07.14
#include "e_cardlist.h"
#include "s_ac.h"
/*--------------------------------------------------------------------------*/
//	void	AC::sweep();
//	void	AC::first();
//	bool	AC::next();
/*--------------------------------------------------------------------------*/
void AC::sweep()
{
  head(start, stop, "Freq");
  first();
  CARD_LIST::card_list.ac_begin();
  do {
    jomega = COMPLEX(0., freq * M_TWO_PI);
    solve();
    outdata(freq);
  } while (next());
}
/*--------------------------------------------------------------------------*/
void AC::first()
{
  freq = start;
}
/*--------------------------------------------------------------------------*/
bool AC::next()
{
  double realstop = (linswp)
    ? stop - step/100.
    : stop / pow(step,.01);
  if (!in_order(start, freq, realstop)) {
    return false;
  }

  freq = (linswp)
    ? freq + step
    : freq * step;
  if (in_order(freq, start, stop)) {
    return false;
  }else{
    return true;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
