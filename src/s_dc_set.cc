/*$Id: s_dc_set.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * dc analysis setup
 */
//testing=script 2006.07.14
#include "u_prblst.h"
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
  for (int ii = 0;  ii < n_sweeps;  ++ii) {
    if (exists(zap[ii])) { // component
      stash[ii].restore();
      zap[ii]->dec_probes();
      zap[ii]->elabo1();
      zap[ii]->precalc();
    }
  }
}
/*--------------------------------------------------------------------------*/
void OP::setup(CS& cmd)
{
  zap[0] = NULL;
  sweepval[0] = &temp_c;
  start[0] = (cmd.is_float()) ? cmd.ctof() : OPT::temp_c;
  stop[0]  = (cmd.is_float()) ? cmd.ctof() : start[0];
  step[0] = 0.;
  genout = 0.;
  options(cmd,0);
  if (step[0] == 0.) {
    step[0] = stop[0] - start[0];
    linswp[0] = true;
  }else{
    untested();
  }
  n_sweeps = 1;
  cmd.check(bWARNING, "what's this?");
}
/*--------------------------------------------------------------------------*/
void DC::setup(CS& cmd)
{
  if (cmd.more()) {
    for (n_sweeps = 0; cmd.more() && n_sweeps < DCNEST; ++n_sweeps) {
      CARD_LIST::fat_iterator ci = findbranch(cmd, &CARD_LIST::card_list);
      if (!ci.is_end()) {			// sweep a component
	zap[n_sweeps] = *ci;
      }else if (cmd.is_float()) {		// sweep the generator
	zap[n_sweeps] = NULL;
      }else{
	// leave as it was .. repeat cmd with no args
      }
      
      if (cmd.is_float()) {			// set up parameters
	start[n_sweeps] = cmd.ctof();
	stop[n_sweeps] = (cmd.is_float()) ? cmd.ctof() : start[n_sweeps];
	step[n_sweeps] = 0.;
      }else{
	// leave it as it was .. repeat cmd with no args
      }
      
      genout = 0.;
      temp_c = OPT::temp_c;
      options(cmd,n_sweeps);
      if (step[n_sweeps] == 0.) {	// prohibit log sweep from 0
	step[n_sweeps] = stop[n_sweeps] - start[n_sweeps];
	linswp[n_sweeps] = true;
      }
    }
  }else{
    options(cmd,n_sweeps);	// there is important initialization here
  }
  cmd.check(bWARNING, "what's this?");

  assert(n_sweeps > 0);
  for (int ii = 0;  ii < n_sweeps;  ++ii) {
    if (exists(zap[ii])) { // component
      stash[ii] = zap[ii];			// stash the std value
      zap[ii]->inc_probes();			// we need to keep track of it
      zap[ii]->set_value(zap[ii]->value(),0);	// zap out extensions
      zap[ii]->set_constant(false);		// so it will be updated
      sweepval[ii] = zap[ii]->set__value();	// point to value to patch
    }else{ // generator
      sweepval[ii] = &genout;			// point to value to patch
    }
  }
}
/*--------------------------------------------------------------------------*/
void DCOP::options(CS& cmd, int i)
{
  out = IO::mstdout;
  out.reset(); /* BUG: don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  uic = loop = reverse = cont = false;
  trace = tNONE;
  int here = cmd.cursor();
  do{
    ONE_OF
      || (cmd.is_float()	&& by(cmd,i))
      || (cmd.pmatch("*$$")	&& times(cmd,i))
      || (cmd.pmatch("By")	&& by(cmd,i))
      || (cmd.pmatch("DEcade")	&& decade(cmd,i))
      || (cmd.pmatch("TImes")	&& times(cmd,i))
      || get(cmd, "Continue",   &cont)
      || get(cmd, "DTemp",	&temp_c,   mOFFSET, OPT::temp_c)
      || get(cmd, "LOop", 	&loop)
      || get(cmd, "PLot",	&ploton)
      || get(cmd, "REverse",	&reverse)
      || get(cmd, "TEmperature",&temp_c)
      || (cmd.pmatch("TRace") &&
	  (ONE_OF
	   || set(cmd, "None",	    &trace, tNONE)
	   || set(cmd, "Off",	    &trace, tNONE)
	   || set(cmd, "Warnings",  &trace, tUNDER)
	   || set(cmd, "Iterations",&trace, tITERATION)
	   || set(cmd, "Verbose",   &trace, tVERBOSE)
	   || cmd.warn(bWARNING, 
		       "need none, off, warnings, iterations, verbose")
	   )
	  )
      || outset(cmd,&out)
      ;
  }while (cmd.more() && !cmd.stuck(&here));

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(out);
}
/*--------------------------------------------------------------------------*/
bool DCOP::by(CS& cmd, int i)
{
  step[i] = cmd.ctof();
  linswp[i] = true;
  if (step[i] == 0.) {
    untested();
    step[i] = stop[i] - start[i];
  }
  return true;
}
/*--------------------------------------------------------------------------*/
bool DCOP::decade(CS& cmd, int i)
{
  double junk = cmd.ctopf();
  if (junk == 0.) {
    untested();
    junk = 1.;
  }
  junk = pow(10., 1./junk);
  step[i] = junk;
  linswp[i] = false;
  return true;
}
/*--------------------------------------------------------------------------*/
bool DCOP::times(CS& cmd, int i)
{
  untested();
  step[i] = cmd.ctopf();
  linswp[i] = false;
  if (step[i] == 0.  &&  start[i] != 0.) {
    untested();
    step[i] = stop[i] / start[i];
  }else{
    untested();
  }
  return true;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
