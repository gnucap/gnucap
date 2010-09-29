/*$Id: m_wave.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * "wave" class, for transmission lines and delays
 */
#include "l_denoise.h"
#include "m_interp.h"
/*--------------------------------------------------------------------------*/
class WAVE {
private:
  std::deque<std::pair<double,double> > _w;
  double _delay;
  explicit WAVE(const WAVE&);
public:
  explicit WAVE(double d=0);
  void	   set_delay(double d);
  void	   initialize();
  void	   push(double t, double v);
  double   v_out(double t)const;
  double   v_reflect(double t, double v_total)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline void WAVE::push(double t, double v)
{
  _w.push_back(std::pair<double,double>(t+_delay, v));
}
/*--------------------------------------------------------------------------*/
inline void WAVE::initialize()
{
  _w.clear();
  push(0-_delay, 0.);
  push(0., 0.);
}
/*--------------------------------------------------------------------------*/
inline WAVE::WAVE(double d)
  :_w(),
   _delay(d)
{
  initialize();
}
/*--------------------------------------------------------------------------*/
inline void WAVE::set_delay(double d) 
{
  _delay = d; 
  initialize();
}
/*--------------------------------------------------------------------------*/
inline double WAVE::v_out(double t)const
{
  FPOLY1 rv=interpolate(_w.begin(), _w.end(), t, 0., 0.);
  return rv.f0;
}
/*--------------------------------------------------------------------------*/
inline double WAVE::v_reflect(double t, double v_total)const
{
  return dn_diff(v_total*2, v_out(t));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
