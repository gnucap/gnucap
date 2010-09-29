/*$Id: declare.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * global functions
 * A remnant of the old C code
 */
//testing=trivial 2006.07.17
#ifndef DECLARE_H
#define DECLARE_H
#include "md.h"
/*--------------------------------------------------------------------------*/
/* md	    */	void	  setup_traps();
/* fft      */	void	  fft(COMPLEX*,int,int);
/* c_file   */	char	  *getcmd(const char*,char*,int);
/* generat  */	double	  gen();
/* plot     */	void	  plottr(double);
		int	  plopen(double,double);
		void	  plclose();
		void	  plclear();
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
