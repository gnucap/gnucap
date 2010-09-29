/*$Id: s_dc_swp.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * dc analysis sweep
 */
//testing=script 2006.07.14
#include "l_denoise.h"
#include "s_dc.h"
/*--------------------------------------------------------------------------*/
//	void	DCOP::sweep(void);
//	void	DCOP::sweep_recursive(int);
//	void	DCOP::first(int);
//	bool	DCOP::next(int);
/*--------------------------------------------------------------------------*/
void DCOP::sweep(void)
{
  head(start[0], stop[0], " ");
  bypass_ok = false;
  inc_mode = tsBAD;
  if (cont) {untested();
    restore();
   }
  
  clear_limit();
  CARD_LIST::card_list.dc_begin();
  sweep_recursive(n_sweeps);
}
/*--------------------------------------------------------------------------*/
void DCOP::sweep_recursive(int ii)
{
  --ii;
  assert(ii >= 0);
  assert(ii < DCNEST);

  OPT::ITL itl = OPT::DCBIAS;
  
  first(ii);
  do {
    if (ii == 0) {
      IO::suppresserrors = trace < tVERBOSE;
      int converged = solve(itl,trace);
      if (!converged) {itested();
	error(bWARNING, "did not converge\n");
      }
      ::status.accept.start();
      set_limit();
      CARD_LIST::card_list.tr_accept();
      ::status.accept.stop();
      keep();
      outdata(*sweepval[ii]);
      itl = OPT::DCXFER;
    }else{
      sweep_recursive(ii);
    }
  } while (next(ii));
}
/*--------------------------------------------------------------------------*/
void DCOP::first(int ii)
{
  assert(ii >= 0);
  assert(ii < DCNEST);
  assert(start);
  assert(sweepval);
  assert(sweepval[ii]);

  *sweepval[ii] = start[ii];
  if (reverse) {untested();
    reverse = false;
    while (next(ii)) {untested();
      /* nothing */;
    }
    reverse = true;
    next(ii);
  }
  phase = pINIT_DC;
}
/*--------------------------------------------------------------------------*/
bool DCOP::next(int ii)
{
  bool ok = false;
  if (linswp[ii]) {
    double fudge = step[ii] / 10.;
    if (step[ii] == 0.) {
      ok = false;
    }else{
      if (!reverse) {
	*(sweepval[ii]) += step[ii];
	fixzero(sweepval[ii], step[ii]);
	ok=in_order(start[ii]-fudge,*(sweepval[ii]),stop[ii]+fudge);
	if (!ok  &&  loop) {
	  reverse = true;
	}
      }
      if (reverse) {
	*(sweepval[ii]) -= step[ii];
	fixzero(sweepval[ii], step[ii]);
	ok=in_order(start[ii]-fudge,*(sweepval[ii]),stop[ii]+fudge);
      }
    }
  }else{
    double fudge = pow(step[ii], .1);
    if (step[ii] == 1.) {untested();
      ok = false;
    }else{
      if (!reverse) {
	*(sweepval[ii]) *= step[ii];
	ok=in_order(start[ii]/fudge,*(sweepval[ii]),stop[ii]*fudge);
	if (!ok  &&  loop) {untested();
	  reverse = true;
	}     
      }
      if (reverse) {untested();
	*(sweepval[ii]) /= step[ii];
	ok=in_order(start[ii]/fudge,*(sweepval[ii]),stop[ii]*fudge);
      }
    }
  }
  phase = pDC_SWEEP;
  return ok;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
