/*$Id: md.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * Non-portable functions.  Do all portability patches here.
 */
//testing=script,sparse 2006.07.16
#include "md.h"
/*--------------------------------------------------------------------------*/
	void    setup_traps(void);
static	void	new_ex_handler();
extern "C" {
  static void	sig_abrt(SIGNALARGS);
  static void	sig_int(SIGNALARGS);
  static void	sig_fpe(SIGNALARGS);
}
/*--------------------------------------------------------------------------*/
void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
  signal(SIGABRT,sig_abrt);
  std::set_new_handler(new_ex_handler);
}
/*--------------------------------------------------------------------------*/
static void new_ex_handler()
{
  untested();
  error(bERROR, "out of memory\n");
}
/*--------------------------------------------------------------------------*/
/* sig_abrt: trap asserts
 */
extern "C" {
  static void sig_abrt(SIGNALARGS)
  {
    itested();
    signal(SIGINT,sig_abrt);
    error(bERROR, "\n");
  }
}
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {
    itested();
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
/*--------------------------------------------------------------------------*/
