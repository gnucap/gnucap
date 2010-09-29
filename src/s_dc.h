/*$Id: s_dc.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * DC and OP analysis
 */
//testing=script,complete 2006.07.14
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
  void	options(CS&, int);
private:
  bool	by(CS&, int);
  bool	decade(CS&, int);
  bool	times(CS&, int);
private:			/* s_dc_swp.cc */
  void	sweep();
  void	sweep_recursive(int);
  void	first(int);
  bool	next(int);
  explicit DCOP(const DCOP&): SIM() {unreachable(); incomplete();}
protected:
  explicit DCOP():
    SIM(),
    n_sweeps(1),
    loop(false),
    reverse(false),
    cont(false),
    trace(tNONE)
  {
    for (int i = 0; i < DCNEST; ++i) {
      start[i]=stop[i]=step[i]=0.;
      linswp[i]=true;
      sweepval[i]=&genout;
      zap[i]=NULL; 
    }

    // BUG:: in SIM.  should be initialized there.
    genout=0.;
    temp_c=OPT::temp_c;
    out=IO::mstdout;
    uic=false;
  }
  ~DCOP() {}
  
protected:
  enum {DCNEST = 4};
  int n_sweeps;
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
  explicit DC(): DCOP() {}
  ~DC() {}
  void	command(CS&);
private:
  void	setup(CS&);
  explicit DC(const DC&): DCOP() {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
class OP : public DCOP {
public:
  explicit OP(): DCOP() {}
  ~OP() {}
  void	command(CS&);
private:
  void	setup(CS&);
  explicit OP(const OP&): DCOP() {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
