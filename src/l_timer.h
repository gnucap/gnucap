/*$Id: l_timer.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * CPU time accounting
 */
//testing=script 2006.07.13
#ifndef U_TIMER_H
#define U_TIMER_H
#include "io_.h"
/*--------------------------------------------------------------------------*/
class TIMER {
private:
  enum {_T_NAME_LEN = 8};
  double _ref;		// time the clock was started
  double _last;		// time of timed operation
  double _total;	// time since program start
  bool	 _running;
  char	 _name[_T_NAME_LEN+1];
public:
  explicit	TIMER();
  explicit	TIMER(const char*);
		~TIMER() {}
  TIMER&	fullreset();
  TIMER&	reset();
  TIMER&	start();
  TIMER&	stop();
  TIMER&	check();
  double	elapsed()const		{untested();return _last;}
  bool		is_running()const	{return _running;}

  TIMER&	print(OMSTREAM& s = IO::mstdout);
  TIMER& 	operator=(const TIMER&);
  friend TIMER	operator-(const TIMER&,const TIMER&);
};
template <class T>
T& operator<<(T & s, TIMER & t) {t.print(s); return s;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
