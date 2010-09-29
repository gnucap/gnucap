/*$Id: s_tr_set.cc,v 24.5 2003/04/27 01:05:05 al Exp $ -*- C++ -*-
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
 * set up transient and fourier analysis
 */
#include "u_prblst.h"
#include "l_denoise.h"
#include "constant.h"
#include "ap.h"
#include "s_tr.h"
// testing=implicit
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
  {if (cmd.is_pfloat()) {
    double arg1 = cmd.ctopf();
    double arg2 = cmd.ctopf();
    double arg3 = cmd.ctopf();
    {if (arg3 != 0.) {			    /* 3 args: all */
      {if (arg1 == 0.  ||  arg1 > arg3) {   /* eca (logical) order: */
	tstart = arg1;			    /* tstart tstop step */
	tstop  = arg2;				
	tstep  = arg3;
      }else{ 				    /* spice (illogical) order */
	untested();
	tstart = arg3;		    	    /* tstep tstop tstart */
	tstop  = arg2;
	tstep  = arg1;
      }}
    }else if (arg2 != 0.) {		    /* 2 args */
      {if (arg1 == 0.) {		    /* 2 args: tstart, tstop */
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
      }}
    }else if (arg1 > last_time) {	    /* 1 arg: tstop */
      untested();
      tstart = last_time;
      tstop  = arg1;
      /* tstep unchanged */
    }else if (arg1 == 0.) {		    /* 1 arg: tstart */
      //untested();
      tstart = 0.;
      tstop  = oldrange;
      /* tstep unchanged */
    }else{ /* arg1 < last_time, but not 0 */  /* 1 arg: tstep */
      untested();
      tstart = last_time;
      tstop  = last_time + oldrange;
      tstep  = arg1;
    }}
  }else{ /* no args */
    tstart = last_time;
    tstop  = last_time + oldrange;
    /* tstep unchanged */
  }}
  if (cmd.is_pfloat()) {
    dtmax = std::abs(cmd.ctof());
    skip = static_cast<int>(tstep / dtmax + .5);
  }
  options(cmd);
  
  dtmax = tstep / static_cast<double>(skip);
  dtmin = std::max(dtmin,dtmax/dtratio);
  {if  (tstart < last_time  ||  last_time <= 0.) {
    cont = false;
    time1 = time0 = 0.;
  }else{
    cont = true;
    time1 = time0 = last_time;
  }}
  if (tstep==0.) {
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
  temp = OPT::tempamb;
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  uic = echo = cold = false;
  trace = tNONE;
  dtmin = OPT::dtmin;
  dtratio = OPT::dtratio;
  int here = cmd.cursor();
  do{
    0
      || get(cmd, "Ambient",	&temp,   mOFFSET, OPT::tempamb)
      || get(cmd, "Cold",	&cold)
      || get(cmd, "DTMIn",	&dtmin,  mPOSITIVE)
      || get(cmd, "DTRatio",	&dtratio,mPOSITIVE)
      || get(cmd, "Echo",	&echo)
      || get(cmd, "PLot",	&ploton)
      || get(cmd, "Reftemp",	&temp,   mOFFSET, OPT::tnom)
      || get(cmd, "SKip",	&skip)
      || get(cmd, "Temperature",&temp,   mOFFSET, -ABS_ZERO)
      || get(cmd, "UIC",	&uic)
      || (cmd.pmatch("TRace") &&
	  (0
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
