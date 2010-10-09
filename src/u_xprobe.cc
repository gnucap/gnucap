/*$Id: u_xprobe.cc,v 26.110 2009/05/28 15:32:04 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * extended probe data
 * used for AC analysis
 */
//testing=script 2006.07.14
#include "m_phase.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
double XPROBE::operator()(mod_t m, bool db)const
{
  if (exists()) {
    if (m == mtNONE) {
      m = _modifier;
    }
    double rv = NOT_VALID;
    switch (m) {
    case mtNONE:
      unreachable();
      break;
    case mtMAG:
      rv = std::abs(_value);
      break;
    case mtPHASE:
      rv = phase(_value);
      break;
    case mtREAL:
      rv = real(_value);
      break;
    case mtIMAG:
      rv = imag(_value);
      break;
    }
    return (db) ? _dbscale * log10(std::max(rv,VOLTMIN)) : rv;
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
XPROBE& XPROBE::operator=(const XPROBE& p)
{
  _value = p._value;
  _modifier = p._modifier;
  _dbscale = p._dbscale;
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
