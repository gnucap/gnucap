/*$Id: s_dc.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * DC and OP analysis
 */
#include "u_cardst.h"
#include "s__.h"
#ifndef S_DC_H
#define S_DC_H
/*--------------------------------------------------------------------------*/
class CARD;
/*--------------------------------------------------------------------------*/
class DCOP : public SIM {
public:				/* s_dc_set.cc */
	void	finish();
protected:
	void	options(CS&);
private:
	void	by(CS&);
	void	decade(CS&);
	void	times(CS&);
private:			/* s_dc_swp.cc */
	void	sweep();
	void	first(int);
	bool	next(int);

protected:
  enum {DCNEST = 1};
  double start[DCNEST];
  double stop[DCNEST];
  double step[DCNEST];
  bool linswp[DCNEST];
  double* (sweepval[DCNEST]);	/* pointer to thing to sweep, dc command */
  CARD* (zap[DCNEST]);		/* to branch to zap, for re-expand */
  CARDSTASH stash[DCNEST];	/* store std values of elements being swept */
private:
  bool loop;			/* flag: do it again backwards */
  bool reverse;			/* flag: sweep backwards */
  bool cont;			/* flag: continue from previous run */
  TRACE trace;			/* enum: show extended diagnostics */
};
/*--------------------------------------------------------------------------*/
class DC : public DCOP {
public:
  	void	command(CS&);
private:
	void	setup(CS&);
};
/*--------------------------------------------------------------------------*/
class OP : public DCOP {
public:
  	void	command(CS&);
private:
	void	setup(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
