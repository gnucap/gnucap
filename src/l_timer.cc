/*$Id: l_timer.cc,v 26.81 2008/05/27 05:34:00 al Exp $	-*- C++ -*-
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
 * Time a command, or whatever
 */
//testing=script,sparse 2006.07.13
#include "l_timer.h"
/*--------------------------------------------------------------------------*/
//		TIMER::TIMER();
//		TIMER::TIMER(const char*);
//	TIMER&	TIMER::fullreset();
//	TIMER&	TIMER::reset();
//	TIMER&	TIMER::start();
//	TIMER&	TIMER::stop();
//	TIMER&	TIMER::check();
//	TIMER&	TIMER::print();
//	TIMER&	TIMER::operator=(const TIMER&);
	TIMER	operator-(const TIMER&,const TIMER&);
/*--------------------------------------------------------------------------*/
inline double run_time()
{
  return static_cast<double>(clock()) / static_cast<double>(CLOCKS_PER_SEC);
}
/*--------------------------------------------------------------------------*/
TIMER::TIMER()
{
  fullreset();
}
/*--------------------------------------------------------------------------*/
TIMER::TIMER(const std::string& label)
  :_name(label)
{
  fullreset();
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::fullreset()
{
  _total = 0.;
  return reset();
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::reset()
{
  _last = 0.;
  _ref  = 0.;
  _running = false;
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::start()
{
  assert(!_running);
  if (_running) {
    untested();
    stop();
  }
  _ref = run_time();
  _running = true;
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::stop()
{
  if (_running) {
    double runtime = run_time() - _ref;
    _ref = 0.;
    _last  += runtime;
    _total += runtime;
    _running = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::check()
{
  itested();
  if (_running) {
    itested();
    stop();
    start();
  }else{
    untested();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::print(OMSTREAM& s)
{
  s.form("%10s %8.2f %8.2f\n", _name.c_str(), _last, _total);
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::operator=(const TIMER& x)
{
  _last    = x._last;
  _ref     = x._ref;
  _total   = x._total;
  _running = x._running;
  // but don't copy the name
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER operator-(const TIMER& x, const TIMER& y)
{
  TIMER z("temp");
  z._last  = x._last - y._last;
  z._ref   = 0.;	// when did the difference start running?
  z._total = x._total - y._total;
  z._running = false;
  // but don't copy the name
  return z;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
