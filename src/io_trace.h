/*$Id: io_trace.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * trace macros for model debugging
 */
/* allow multiple inclusions with different DO_TRACE */
#include <stdio.h>
#undef trace_line
#undef trace0
#undef trace1
#undef trace2
#undef trace3
#undef trace4
#undef untested
#undef unreachable
#undef incomplete
/*--------------------------------------------------------------------------*/
#ifdef DO_TRACE
#define trace_line() (printf("@@#\n@#@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __FUNCTION__))
#define trace0(s) (printf("@#@%s\n", s))
#define trace1(s,x) (printf("@#@%s  %s=%g\n", s, #x, (double)(x)))
#define trace2(s,x,y) (printf("@#@%s  %s=%g  %s=%g\n",\
	s, #x, (double)(x), #y, (double)(y)))
#define trace3(s,x,y,z) (printf("@#@%s  %s=%g  %s=%g  %s=%g\n",\
	s, #x, (double)(x), #y, (double)(y), #z, (double)(z)))
#define trace4(s,w,x,y,z)(printf("@#@%s  %s=%g  %s=%g  %s=%g  %s=%g\n",\
	s, #w, (double)(w), #x, (double)(x), #y, (double)(y), #z, (double)(z)))
#else
#define trace_line()
#define trace0(s)
#define trace1(s,x)
#define trace2(s,x,y)
#define trace3(s,x,y,z)
#define trace4(s,w,x,y,z)
#endif

#define unreachable() (printf("@@#\n@@@unreachable:%s:%u:%s\n", \
			   __FILE__, __LINE__, __FUNCTION__))

#define incomplete() (printf("@@#\n@@@incomplete:%s:%u:%s\n", \
			   __FILE__, __LINE__, __FUNCTION__))

#ifdef TRACE_UNTESTED
#define untested() (printf("@@#\n@@@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __FUNCTION__))
#else
#define untested()
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
