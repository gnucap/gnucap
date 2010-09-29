/*$Id: s_ac.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * AC analysis
 */
#ifndef S_AC_H
#define S_AC_H
#include "s__.h"
/*--------------------------------------------------------------------------*/
class AC : private SIM {
public:
	void	command(CS&);
private:
	void	sweep();
	void	first();
	bool	next();
	void	solve();
	void	clear();
	void	setup(CS&);
	void	by(CS&);
	void	decade(CS&);
	void	lin(CS&);
	void	octave(CS&);
	void	times(CS&);
private:
  double start;		// sweep start time
  double stop;		// sweep stop time
  double step;		// printed step size
  bool linswp;		// flag: use linear sweep (vs log sweep)
  bool echo;		// flag: echo the input when using input data file
  bool cold;		// flag: power off.  all DC voltages are 0
  bool cont;		// flag: continue.  don't do op first
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
