/*$Id: s_dc_set.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * dc analysis setup
 */
#include "u_prblst.h"
#include "ap.h"
#include "u_cardst.h"
#include "s_dc.h"
/*--------------------------------------------------------------------------*/
//	void	DCOP::finish(void);
//	void	OP::setup(CS&);
//	void	DC::setup(CS&);
//	void	DCOP::options(CS&);
//	void	DCOP::by(CS&);
//	void	DCOP::decade(CS&);
//	void	DCOP::times(CS&);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DCOP::finish(void)
{
  for (int ii = 0;  ii < DCNEST && exists(zap[ii]);  ++ii) {
    stash[ii].restore();
    zap[ii]->dec_probes();
    zap[ii]->expand();
    zap[ii]->precalc();
  }
}
/*--------------------------------------------------------------------------*/
void OP::setup(CS& cmd)
{
  zap[0] = NULL;
  sweepval[0] = &temp;
  start[0] = (cmd.is_float()) ? cmd.ctof()-ABS_ZERO : OPT::tempamb;
  stop[0]  = (cmd.is_float()) ? cmd.ctof()-ABS_ZERO : start[0];
  step[0] = 0.;
  genout = 0.;
  options(cmd);
  if (step[0] == 0.) {
    step[0] = stop[0] - start[0];
    linswp[0] = true;
  }else{
    untested();
  }
}
/*--------------------------------------------------------------------------*/
void DC::setup(CS& cmd)
{
  CARD_LIST::fat_iterator ci = findbranch(cmd, &CARD_LIST::card_list);
  {if (!ci.isend()) {
    zap[0] = *ci;
  }else if (cmd.is_float()) {		/* sweep the generator */
    zap[0] = NULL;
  }else{
    // leave as it was .. repeat cmd with no args
  }}
  
  {if (cmd.is_float()) {			/* set up parameters */
    start[0] = cmd.ctof();
    stop[0] = (cmd.is_float()) ? cmd.ctof() : start[0];
    step[0] = 0.;
  }else{
    // leave it as it was .. repeat cmd with no args
  }}
  
  {if (exists(zap[0])) { // component
    stash[0] = zap[0];			/* stash the std value */
    zap[0]->inc_probes();		/* we need to keep track of it */
    zap[0]->set_value(zap[0]->value(),0);/* zap out the extensions */
    zap[0]->set_constant(false);
    sweepval[0] = zap[0]->set_value(); 	/* point to value to patch */
  }else{ // generator
    sweepval[0] = &genout;		/* point to value to patch */
  }}
  
  genout = 0.;
  temp = OPT::tempamb;
  options(cmd);
  if (step[0] == 0.) {	// prohibit log sweep from 0
    step[0] = stop[0] - start[0];
    linswp[0] = true;
  }
}
/*--------------------------------------------------------------------------*/
void DCOP::options(CS& cmd)
{
  out = IO::mstdout;
  out.reset(); /* BUG: don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  uic = loop = reverse = cont = false;
  trace = tNONE;
  int here = cmd.cursor();
  do{
    if      (cmd.is_float())       by(cmd);
    else if (cmd.pmatch("*$$"))    times(cmd);
    else if (cmd.pmatch("By"))     by(cmd);
    else if (cmd.pmatch("Decade")) decade(cmd);
    else if (cmd.pmatch("TImes"))  times(cmd);
    get(cmd, "Ambient",	  &temp,   mOFFSET, OPT::tempamb);
    get(cmd, "Continue",   &cont);
    get(cmd, "LOop", 	  &loop);
    get(cmd, "PLot",	  &ploton);
    get(cmd, "Reftemp",	  &temp,   mOFFSET, OPT::tnom);
    get(cmd, "REverse",	  &reverse);
    get(cmd, "Temperature",&temp,   mOFFSET, -ABS_ZERO);
    cmd.pmatch("TRace") &&
      (   set(cmd, "None",	&trace, tNONE)
       || set(cmd, "Off",	&trace, tNONE)
       || set(cmd, "Warnings",	&trace, tUNDER)
       || set(cmd, "Alltime",	&trace, tALLTIME)
       || set(cmd, "Rejected",	&trace, tREJECTED)
       || set(cmd, "Iterations",&trace, tITERATION)
       || set(cmd, "Verbose",	&trace, tVERBOSE)
       || cmd.warn(bWARNING, 
	  "need none, off, warnings, alltime, rejected, iterations, verbose"));
    outset(cmd,&out);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(out);
}
/*--------------------------------------------------------------------------*/
void DCOP::by(CS& cmd)
{
  step[0] = cmd.ctof();
  linswp[0] = true;
  if (step[0] == 0.) {
    untested();
    step[0] = stop[0] - start[0];
  }
}
/*--------------------------------------------------------------------------*/
void DCOP::decade(CS& cmd)
{
  double junk = cmd.ctopf();
  if (junk == 0.) {
    untested();
    junk = 1.;
  }
  junk = pow(10., 1./junk);
  step[0] = junk;
  linswp[0] = false;
}
/*--------------------------------------------------------------------------*/
void DCOP::times(CS& cmd)
{
  untested();
  step[0] = cmd.ctopf();
  linswp[0] = false;
  if (step[0] == 0.  &&  start[0] != 0.)
    step[0] = stop[0] / start[0];
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
