/*$Id: s_ac.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * AC analysis
 */
//testing=script,complete 2006.07.14
#ifndef S_AC_H
#define S_AC_H
#include "s__.h"
/*--------------------------------------------------------------------------*/
class AC : private SIM {
public:
  void	command(CS&);

  explicit AC():
    SIM(),
    start(0.),
    stop(0.),
    step(0.),
    linswp(false)
  {}

  ~AC() {}
private:
  explicit AC(const AC&):SIM() {unreachable(); incomplete();}
  void	sweep();
  void	first();
  bool	next();
  void	solve();
  void	clear();
  void	setup(CS&);
  bool	by(CS&);
  bool	decade(CS&);
  bool	lin(CS&);
  bool	octave(CS&);
  bool	times(CS&);
private:
  double start;		// sweep start time
  double stop;		// sweep stop time
  double step;		// printed step size
  bool linswp;		// flag: use linear sweep (vs log sweep)
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
