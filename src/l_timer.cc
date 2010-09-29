/*$Id: l_timer.cc,v 20.5 2001/09/17 15:43:17 al Exp $	-*- C++ -*-
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
 * Time a command, or whatever
 */
/*--------------------------------------------------------------------------*/
#include <assert.h>
#include "l_timer.h"
/*--------------------------------------------------------------------------*/
//		TIMER::TIMER();
//		TIMER::TIMER(const char*);
//	TIMER&	TIMER::fullreset();
//	TIMER&	TIMER::reset();
//	TIMER&	TIMER::zstart();
//	TIMER&	TIMER::start();
//	TIMER&	TIMER::stop();
//	TIMER&	TIMER::check();
//	TIMER&	TIMER::print();
//	TIMER&	TIMER::operator=(const TIMER&);
	TIMER	operator-(const TIMER&,const TIMER&);
/*--------------------------------------------------------------------------*/
TIMER::TIMER()
{
  _name[0] = '\0';
  fullreset();
}
/*--------------------------------------------------------------------------*/
TIMER::TIMER(const char* label)
{
  strncpy(_name, label, _T_NAME_LEN);
  _name[_T_NAME_LEN] = '\0';
  fullreset();
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::fullreset()
{
  _total_user   = 0.;
  _total_system = 0.;
  return reset();
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::reset()
{
  _last_user   = 0.;
  _last_system = 0.;
  _ref_user    = 0.;
  _ref_system  = 0.;
  _running     = false;
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::zstart()
{
  untested();
  return reset().start();
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::start()
{
  assert(!_running);
  if (_running) {
    untested();
    stop();
  }
  struct rusage x;
  getrusage(RUSAGE_SELF,&x);
  _ref_user =
    static_cast<double>(x.ru_utime.tv_sec)
    + static_cast<double>(x.ru_utime.tv_usec)*1e-6; 
  _ref_system =
    static_cast<double>(x.ru_stime.tv_sec)
    + static_cast<double>(x.ru_stime.tv_usec)*1e-6; 
  _running = true;

  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::stop()
{
  if (_running) {
    struct rusage x;
    getrusage(RUSAGE_SELF,&x);
    {
      double utime =
	static_cast<double>(x.ru_utime.tv_sec)
	+ static_cast<double>(x.ru_utime.tv_usec)*1e-6; 
      double runtime = utime - _ref_user;
      _ref_user	  =  0.;
      _last_user  += runtime;
      _total_user += runtime;
    }
    {
      double stime =
	static_cast<double>(x.ru_stime.tv_sec)
	+ static_cast<double>(x.ru_stime.tv_usec)*1e-6; 
      double runtime = stime - _ref_system;
      _ref_system   =  0.;
      _last_system  += runtime;
      _total_system += runtime;
    }
    _running = false;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::check()
{
  if (_running) {
    stop();
    start();
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::print(OMSTREAM& s)
{
  s.form("%10s %8.2f %8.2f %8.2f   %8.2f %8.2f %8.2f\n", _name,
	  _last_user,  _last_system,  _last_user  + _last_system,
	  _total_user, _total_system, _total_user + _total_system);
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER& TIMER::operator=(const TIMER& x)
{
  _last_user    = x._last_user;
  _last_system  = x._last_system;
  _ref_user     = x._ref_user;
  _ref_system   = x._ref_system;
  _total_user   = x._total_user;
  _total_system = x._total_system;
  _running      = x._running;
  // but don't copy the name
  return *this;
}
/*--------------------------------------------------------------------------*/
TIMER operator-(const TIMER& x, const TIMER& y)
{
  TIMER z("temp");
  z._last_user    = x._last_user    - y._last_user;
  z._last_system  = x._last_system  - y._last_system;
  z._ref_user     = 0.;	// when did the difference start running?
  z._ref_system   = 0.;
  z._total_user   = x._total_user   - y._total_user;
  z._total_system = x._total_system - y._total_system;
  z._running      = false;
  // but don't copy the name
  return z;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
