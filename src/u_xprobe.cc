/*$Id: u_xprobe.cc,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * extended probe data
 * used for AC analysis
 */
#include "u_opt.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
double XPROBE::operator()(mod_t m, bool db)const
{
  {if (OK()){
    if (m == mtNONE) {
      m = _modifier;
    }
    double rv = NOT_VALID;
    switch (m){
    case mtNONE:  unreachable();	break;
    case mtMAG:   rv = abs(_value);	break;
    case mtPHASE:
      rv = (OPT::phase == pDEGREES) ? arg(_value)*RTOD : arg(_value);	break;
    case mtREAL:  rv = real(_value);	break;
    case mtIMAG:  rv = imag(_value);	break;
    }
    return (db) ? _dbscale * log10(std::max(rv,VOLTMIN)) : rv;
  }else{
    return NOT_VALID;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
