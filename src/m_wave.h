/*$Id: m_wave.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * "wave" class, for transmission lines and delays
 */
//testing=script,sparse 2006.07.13
#include "l_denoise.h"
#include "m_interp.h"
/*--------------------------------------------------------------------------*/
class WAVE {
private:
  std::deque<DPAIR> _w;
  double _delay;
  explicit WAVE(const WAVE&) {unreachable();}
public:
  explicit WAVE(double d=0);
	  ~WAVE() {}
  void	   set_delay(double d);
  void	   initialize();
  void	   push(double t, double v);
  double   v_out(double t)const;
  double   v_reflect(double t, double v_total)const;
  WAVE&	   operator+(const WAVE& x);
  WAVE&	   operator+(double x);
  WAVE&	   operator*(const WAVE& x);
  WAVE&	   operator*(double x);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// push: insert a signal on the "input" end.
// args: t = the time now
//       v = the value to push
//
inline void WAVE::push(double t, double v)
{
  _w.push_back(DPAIR(t+_delay, v));
}
/*--------------------------------------------------------------------------*/
// initialize: remove all info, fill it with all 0.
//
inline void WAVE::initialize()
{
  _w.clear();
  push(0-_delay, 0.);
  push(0., 0.);
}
/*--------------------------------------------------------------------------*/
// constructor -- argument is the delay
//
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
// v_out: return the value at the "output" end
// args: t = the time now
//
inline double WAVE::v_out(double t)const
{
  FPOLY1 rv=interpolate(_w.begin(), _w.end(), t, 0., 0.);
  return rv.f0;
}
/*--------------------------------------------------------------------------*/
// reflect: calculate a reflection
// args: t = the time now
//       v_total = actual voltage across the termination
// returns: the value (voltage) to send back as the reflection
//
inline double WAVE::v_reflect(double t, double v_total)const
{
  // return (v_total*2 - v_out(t)); // de-noised
  return dn_diff(v_total*2, v_out(t));
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator+(const WAVE& x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second += x.v_out((*i).first);
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator+(double x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second += x;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator*(const WAVE& x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second *= x.v_out((*i).first);
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator*(double x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second *= x;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
