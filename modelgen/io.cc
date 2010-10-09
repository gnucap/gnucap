/*$Id: io.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * shared data for all io functions, initialization, default values
 */
//testing=trivial 2006.07.17
#include "io_.h"

OMSTREAM IO::mstdout(stdout);
OMSTREAM IO::error(stdout);
OMSTREAM IO::plotout;
bool	IO::plotset(false);
int	IO::formaat(0);
bool	IO::incipher(false);
FILE*	IO::stream[MAXHANDLE+1] = {0, stdout, stderr};
