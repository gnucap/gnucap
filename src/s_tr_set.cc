/*$Id: s_tr_set.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * set up transient and fourier analysis
 */
//testing=script 2006.07.14
#include "u_prblst.h"
#include "ap.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::setup(CS&);
//	void	TRANSIENT::options(CS&);
/*--------------------------------------------------------------------------*/
/* tr_setup: transient analysis: parse command string and set options
 * 	(options set by call to tr_options)
 */
void TRANSIENT::setup(CS& cmd)
{
  double oldrange = tstop - tstart;
  cont = true;
  if (cmd.is_pfloat()) {
    double arg1 = cmd.ctopf();
    double arg2 = cmd.ctopf();
    double arg3 = cmd.ctopf();
    if (arg3 != 0.) {			    /* 3 args: all */
      if (arg1 == 0.  ||  arg1 > arg3) {   /* eca (logical) order: */
	tstart = arg1;			    /* tstart tstop step */
	tstop  = arg2;				
	tstep  = arg3;
      }else{ 				    /* spice (illogical) order */
	itested();
	tstart = arg3;		    	    /* tstep tstop tstart */
	tstop  = arg2;
	tstep  = arg1;
      }
    }else if (arg2 != 0.) {		    /* 2 args */
      if (arg1 == 0.) { 		    /* 2 args: tstart, tstop */
	untested();
	tstart = arg1;
	tstop  = arg2;
	/* tstep unchanged */
      }else if (arg1 >= arg2) {		    /* 2 args: tstop, tstep */
	tstart = last_time;
	tstop  = arg1;
	tstep  = arg2;
      }else{ /* arg1 < arg2 */		    /* 2 args: tstep, tstop */
	tstart = arg3; /* 0 */	   	    /* spice order */
	tstop  = arg2;
	tstep  = arg1;
      }
    }else if (arg1 > last_time) {	    /* 1 arg: tstop */
      untested();
      tstart = last_time;
      tstop  = arg1;
      /* tstep unchanged */
    }else if (arg1 == 0.) {		    /* 1 arg: tstart */
      itested();
      tstart = 0.;
      tstop  = oldrange;
      /* tstep unchanged */
    }else{ /* arg1 < last_time, but not 0 */  /* 1 arg: tstep */
      untested();
      tstart = last_time;
      tstop  = last_time + oldrange;
      tstep  = arg1;
    }
  }else{ /* no args */
    tstart = last_time;
    tstop  = last_time + oldrange;
    /* tstep unchanged */
  }
  if (cmd.is_pfloat()) {
    dtmax = std::abs(cmd.ctof());
    skip = static_cast<int>(tstep / dtmax + .5);
  }
  options(cmd);
  
  dtmax = tstep / static_cast<double>(skip);
  dtmin = std::max(dtmin,dtmax/dtratio);
  if  (tstart < last_time  ||  last_time <= 0.) {
    cont = false;
    time1 = time0 = 0.;
  }else{
    cont = true;
    time1 = time0 = last_time;
  }
  if (tstep==0.) {
    untested();
    error(bERROR, "time step = 0\n");
  }
}
/*--------------------------------------------------------------------------*/
/* tr_options: set options common to transient and fourier analysis
 */
void TRANSIENT::options(CS& cmd)
{
  out = IO::mstdout;
  out.reset(); /* BUG: don't know why this is needed */
  temp_c = OPT::temp_c;
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  uic = echo = cold = false;
  trace = tNONE;
  dtmin = OPT::dtmin;
  dtratio = OPT::dtratio;
  int here = cmd.cursor();
  do{
    ONE_OF
      || get(cmd, "Cold",	&cold)
      || get(cmd, "DTEmp",	&temp_c,  mOFFSET, OPT::temp_c)
      || get(cmd, "DTMIn",	&dtmin,   mPOSITIVE)
      || get(cmd, "DTRatio",	&dtratio, mPOSITIVE)
      || get(cmd, "Echo",	&echo)
      || get(cmd, "PLot",	&ploton)
      || get(cmd, "SKip",	&skip)
      || get(cmd, "TEmperature",&temp_c)
      || get(cmd, "UIC",	&uic)
      || (cmd.pmatch("TRace") &&
	  (ONE_OF
	   || set(cmd, "None",	    &trace, tNONE)
	   || set(cmd, "Off",	    &trace, tNONE)
	   || set(cmd, "Warnings",  &trace, tUNDER)
	   || set(cmd, "Alltime",   &trace, tALLTIME)
	   || set(cmd, "Rejected",  &trace, tREJECTED)
	   || set(cmd, "Iterations",&trace, tITERATION)
	   || set(cmd, "Verbose",   &trace, tVERBOSE)
	   || cmd.warn(bWARNING, "need none, off, warnings, alltime, "
		       "rejected, iterations, verbose")
	   
	   )
	  )
      || outset(cmd,&out)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");

  if (skip < 1) {
    skip = 1;
  }
  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(out);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
