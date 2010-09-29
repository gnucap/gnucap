/*$Id: s_dc_swp.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * dc analysis sweep
 */
#include "l_denoise.h"
#include "l_compar.h"
#include "e_card.h"
#include "constant.h"
#include "s_dc.h"
/*--------------------------------------------------------------------------*/
//	void	DCOP::sweep(void);
//	void	DCOP::first(int);
//	bool	DCOP::next(int);
/*--------------------------------------------------------------------------*/
void DCOP::sweep(void)
{
  int converged;
  int itl = OPT::itl[OPT::DCBIAS];
  
  head(start[0], stop[0], linswp[0], " ");
  bypass_ok = false;
  inc_mode = tsBAD;
  if (cont) {
    untested();
    restore();
  }
  
  first(0);
  clear_limit();
  CARD_LIST::card_list.dc_begin();
  do {
    IO::suppresserrors = trace < tVERBOSE;
    converged = solve(itl,trace);
    if (!converged) {
      error(bWARNING, "did not converge\n");
    }
    STATUS::accept.start();
    set_limit();
    CARD_LIST::card_list.tr_accept();
    STATUS::accept.stop();
    keep();
    outdata(*sweepval[0]);
    itl = OPT::itl[OPT::DCXFER];
  } while (next(0));
}
/*--------------------------------------------------------------------------*/
void DCOP::first(int ii)
{
  *sweepval[ii] = start[ii];
  if (reverse) {
    untested();
    reverse = false;
    while (next(ii))
      /* nothing */;
    reverse = true;
    next(ii);
  }
  phase = pINIT_DC;
}
/*--------------------------------------------------------------------------*/
bool DCOP::next(int ii)
{
  bool ok = false;
  {if (linswp[ii]) {
    double fudge = step[ii] / 10.;
    {if (step[ii] == 0.) {
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
    }}
  }else{
    double fudge = pow(step[ii], .1);
    {if (step[ii] == 1.) {
      untested();
      ok = false;
    }else{
      {if (!reverse) {
	*(sweepval[ii]) *= step[ii];
	ok=in_order(start[ii]/fudge,*(sweepval[ii]),stop[ii]*fudge);
	if (!ok  &&  loop) {
	  untested();
	  reverse = true;
	}     
      }else{
	untested();
      }}
      if (reverse) {
	untested();
	*(sweepval[ii]) /= step[ii];
	ok=in_order(start[ii]/fudge,*(sweepval[ii]),stop[ii]*fudge);
      }
    }}
  }}
  phase = pDC_SWEEP;
  return ok;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
