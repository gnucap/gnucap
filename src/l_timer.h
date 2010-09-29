/*$Id: l_timer.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * CPU time accounting
 */
#ifndef U_TIMER_H
#define U_TIMER_H
#include "io_.h"
/*--------------------------------------------------------------------------*/
class TIMER {
private:
  enum {_T_NAME_LEN = 8};
  double _ref_user;	// time the clock was started
  double _ref_system;
  double _last_user;	// time of timed operation
  double _last_system;
  double _total_user;	// time since program start
  double _total_system;
  bool	 _running;
  char	 _name[_T_NAME_LEN+1];
public:
  explicit	TIMER();
  explicit	TIMER(const char*);
  TIMER&	fullreset();
  TIMER&	reset();
  TIMER&	zstart();
  TIMER&	start();
  TIMER&	stop();
  TIMER&	check();
  double	elapsed()const	{return _last_user+_last_system;}
  TIMER&	print(OMSTREAM& s = IO::mstdout);
  TIMER& 	operator=(const TIMER&);
  friend TIMER	operator-(const TIMER&,const TIMER&);
};
template <class T>
T& operator<<(T & s, TIMER & t) {t.print(s); return s;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
