/*$Id: s__aux.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * aux functions associated with the SIM class
 */
#include "u_status.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
void SIM::set_limit()
{
  for (int ii = 1;  ii <= STATUS::total_nodes;  ++ii) {
    CKT_BASE::set_limit(v0[ii]);
  }
}
/*--------------------------------------------------------------------------*/
void SIM::keep()
{
  if (!freezetime) {
    for (int ii = 1;  ii <= STATUS::total_nodes;  ++ii) {
      vdc[ii] = v0[ii];
    }
    last_time = (time0 > 0.) ? time0 : 0.;
  }
}
/*--------------------------------------------------------------------------*/
void SIM::restore()
{
  for (int ii = 1;  ii <= STATUS::total_nodes;  ++ii) {
    vt1[ii] = v0[ii] = vdc[ii];
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
