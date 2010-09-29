/*$Id: u_xprobe.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * extended probe data
 * used for AC analysis
 */
#ifndef U_XPROBE_H
#define U_XPROBE_H
#include "constant.h"
#include "l_compar.h"
/*--------------------------------------------------------------------------*/
typedef enum {mtNONE, mtMAG, mtPHASE, mtREAL, mtIMAG} mod_t;
/*--------------------------------------------------------------------------*/
class XPROBE{
private:
  COMPLEX _value;
  mod_t   _modifier; // default
  double  _dbscale;  // 20 for voltage, 10 for power, etc.
public:  
  //explicit XPROBE():
  //  _value(COMPLEX(NOT_VALID, NOT_VALID)),
  //  _modifier(mtNONE),
  //  _dbscale(20.) {}
  explicit XPROBE(COMPLEX v,mod_t m=mtMAG,double d=20.):
    _value(v),
    _modifier(m),
    _dbscale(d) {}
  explicit XPROBE(double v,mod_t m=mtREAL,double d=20.):
    _value(v),
    _modifier(m),
    _dbscale(d) {}
  bool OK()const {return _modifier != mtNONE;}
  double operator()(mod_t m=mtNONE, bool db = false)const{
    if (OK()){
      if (m == mtNONE) {
	m = _modifier;
      }
      double rv = NOT_VALID;
      switch (m){
      case mtNONE:  unreachable();		break;
      case mtMAG:   rv = abs(_value);	break;
      case mtPHASE: rv = arg(_value)*RTOD;	break;
      case mtREAL:  rv = real(_value);	break;
      case mtIMAG:  rv = imag(_value);	break;
      }
      return (db) ? _dbscale * log10(std::max(rv,VOLTMIN)) : rv;
    }else{
      return NOT_VALID;
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
