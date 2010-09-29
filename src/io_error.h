/*$Id: io_error.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
 * data for error and exception handling
 *
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
 */
#ifndef bERROR
/*--------------------------------------------------------------------------*/
/* arg to error() (badness) to decide severity of exception */
#define	bNOERROR	0
#define bTRACE		1
#define bLOG		2
#define bDEBUG		3
#define bPICKY		4
#define	bWARNING	5
#define bDANGER		6
#define bERROR		7
#define bEXIT		8
#define bDISASTER	9
	void	  error(int,const char*,...);
	void	  error(int,const std::string&);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
