/*$Id: u_xprobe.h,v 26.106 2008/12/13 00:48:28 al Exp $ -*- C++ -*-
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
#ifndef U_XPROBE_H
#define U_XPROBE_H
#include "constant.h"
/*--------------------------------------------------------------------------*/
enum mod_t {mtNONE, mtMAG, mtPHASE, mtREAL, mtIMAG};
/*--------------------------------------------------------------------------*/
class XPROBE{
private:
  COMPLEX _value;
  mod_t   _modifier; // default
  double  _dbscale;  // 20 for voltage, 10 for power, etc.

  explicit XPROBE():
    _value(COMPLEX(NOT_VALID, NOT_VALID)),
    _modifier(mtNONE),
    _dbscale(20.) {untested();}
public:
  XPROBE(const XPROBE& p):
    _value(p._value),
    _modifier(p._modifier),
    _dbscale(p._dbscale) {untested();}
  explicit XPROBE(COMPLEX v):
    _value(v),
    _modifier(mtMAG),
    _dbscale(20.) {}
  explicit XPROBE(COMPLEX v, mod_t m):
    _value(v),
    _modifier(m),
    _dbscale(20.) {}
  explicit XPROBE(COMPLEX v, mod_t m, double d):
    _value(v),
    _modifier(m),
    _dbscale(d) {}
  explicit XPROBE(double v):
    _value(v),
    _modifier(mtREAL),
    _dbscale(20.) {}
  //explicit XPROBE(double v, mod_t m):
  //  _value(v),
  //  _modifier(m),
  //  _dbscale(20.) {}
  //explicit XPROBE(double v, mod_t m, double d):
  //  _value(v),
  //  _modifier(m),
  //  _dbscale(d) {}
  ~XPROBE() {}

  bool exists()const {return _modifier != mtNONE;}
  double operator()(mod_t m=mtNONE, bool db = false)const;
  XPROBE& operator=(const XPROBE& p);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
