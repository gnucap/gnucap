/*$Id: declare.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * global functions
 * A remnant of the old C code
 */
#ifndef DECLARE_H
#define DECLARE_H
#include "md.h"
/*--------------------------------------------------------------------------*/
class CS;
class CARD;
class OMSTREAM;
/*--------------------------------------------------------------------------*/
		void	new_event(double);
/* compiler */	void	  initialize_io();
		void	  setup_traps();
/* crtset   */	int	  testcrt();
		struct graph *initcrt();
/* fft      */	void	  fft(COMPLEX*,int,int);
/* c_file   */	char	  *getcmd(const char*,char*,int);
/* generat  */	double	  gen();
/* line     */	void	  initgraph(struct graph*);
 		void	  stext(int,int,const char*,int);
		void	  setpixel(int,int,int);
		void	  box(int,int,int,int,int);
		void	  line(int,int,int,int,int);
/* main     */	int	  main(int,const char*[]);
/* plot     */	void	  plottr(double);
		int	  plopen(int,double,double,bool);
		void	  plclose();
		void	  plclear();
		void	  pllocate();
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
