/*$Id: md.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Non-portable functions.  Do all portability patches here.
 */
#include "md.h"
#include <errno.h>
#include <signal.h>
#if defined(WIN32)
#include <dos.h>
#include <process.h>
#include <time.h>
#endif
/*--------------------------------------------------------------------------*/
	void    setup_traps(void);
static	void	new_ex_handler();
extern "C" {
  static void	sig_int(SIGNALARGS);
  static void	sig_fpe(SIGNALARGS);
}
	int	getrusage(int,struct rusage*);
/*--------------------------------------------------------------------------*/
void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
#if !defined(NEW_IS_BROKEN)
  std::set_new_handler(new_ex_handler);
#endif
}
/*--------------------------------------------------------------------------*/
#if !defined(NEW_IS_BROKEN)
static void new_ex_handler()
{
  untested();
  error(bERROR, "out of memory\n");
}
#endif
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {
    signal(SIGINT,sig_int);
    error(bERROR, "\n");
  }
}
/*--------------------------------------------------------------------------*/
extern "C" {
  static void sig_fpe(SIGNALARGS)
  {
    untested();
    signal(SIGFPE,sig_fpe);
    error(bDANGER, "floating point error\n");
  }
}
/*--------------------------------------------------------------------------*/
#if defined(NEEDS_GETRUSAGE)
int getrusage(int /*who*/, struct rusage *rusage)
{
  double ticks = (double)clock();
  rusage->ru_stime.tv_sec = rusage->ru_stime.tv_usec = 0;
  rusage->ru_utime.tv_usec =
    (long)fmod(ticks, (double)CLK_TCK) * (1000000./(double)CLK_TCK);
  rusage->ru_utime.tv_sec = (long)(ticks/(double)CLK_TCK);
  return 0;
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
