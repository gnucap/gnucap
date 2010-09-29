/*$Id: u_sdp.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * A class for Size Dependent Parameters, like those used in Spice BSIM models
 */
//testing=script 2006.07.14
#include "u_sdp.h"
/*--------------------------------------------------------------------------*/
bool get(CS& cmd, const std::string& key, SDP* value)
{
  return ONE_OF
    || get(cmd, key,       &value->_nom)
    || get(cmd, "W" + key, &value->_wd)
    || get(cmd, "L" + key, &value->_ld)
    || get(cmd, "P" + key, &value->_pd)
    ;
}
/*--------------------------------------------------------------------------*/
void SDP::print(OMSTREAM& o, const std::string& name)const
{
  o << ("  " + name + '=') << _nom;
  if (_wd != 0) {untested();
    o << (" w" + name + '=') << _wd;
  }
  if (_ld != 0) {untested();
    o << (" l" + name + '=') << _ld;
  }
  if (_pd != 0) {untested();
    o << (" p" + name + '=') << _pd;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
