/*$Id: s_ac_set.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * ac analysis setup
 */
#include "u_prblst.h"
#include "u_opt.h"
#include "constant.h"
#include "ap.h"
#include "s_ac.h"
/*--------------------------------------------------------------------------*/
//	void	AC::setup(CS&);
//	void	AC::by(CS&);
//	void	AC::decade(CS&);
//	void	AC::lin(CS&);
//	void	AC::octave(CS&);
//	void	AC::times(CS&);
/*--------------------------------------------------------------------------*/
static int needslinfix;	// flag: lin option needs patch later (spice compat)
/*--------------------------------------------------------------------------*/
void AC::setup(CS& cmd)
{
  out = IO::mstdout;
  out.reset(); /* BUG: don't know why this is needed */
  temp = OPT::tempamb;
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  cold = cont = echo = false;

       if (cmd.pmatch("*$$"))    times(cmd);
  else if (cmd.pmatch("By"))     by(cmd);
  else if (cmd.pmatch("Decade")) decade(cmd);
  else if (cmd.pmatch("TImes"))  times(cmd);
  else if (cmd.pmatch("LIn"))	 lin(cmd);
  else if (cmd.pmatch("Octave")) octave(cmd);
  
  if (cmd.is_float()){
    start = cmd.ctof();
    stop  = cmd.ctof();
    if (stop==0.){
      stop = start;
    }
    if (cmd.is_float()){
      by(cmd);
    }
  }

  int here = cmd.cursor();
  do{
         if (cmd.pmatch("*$$"))    times(cmd);
    else if (cmd.pmatch("+$$"))    by(cmd);
    else if (cmd.pmatch("By"))     by(cmd);
    else if (cmd.pmatch("Decade")) decade(cmd);
    else if (cmd.pmatch("TImes"))  times(cmd);
    else if (cmd.pmatch("LIn"))	   lin(cmd);
    else if (cmd.pmatch("Octave")) octave(cmd);
    get(cmd, "Ambient",	  &temp,   mOFFSET, OPT::tempamb);
    get(cmd, "Cold",	  &cold);
    get(cmd, "CONTinue",  &cont);
    get(cmd, "Echo",	  &echo);
    get(cmd, "PLot",	  &ploton);
    get(cmd, "Reftemp",	  &temp,   mOFFSET, OPT::tnom);
    get(cmd, "Temperature",&temp,   mOFFSET, -ABS_ZERO);
    outset(cmd,&out);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(out);
  if (needslinfix){			    // LIN option is # of points.
    untested();
    step=(stop-start)/(step-1.);	    // Must compute step after 
    needslinfix = false;		    // reading start and stop,
  }					    // but step must be read first
  if (step==0.){			    // for Spice compatibility
    step = stop - start;
    linswp = true;
  }
}
/*--------------------------------------------------------------------------*/
void AC::by(CS& cmd)
{
  step = cmd.ctof();
  needslinfix = false;
  linswp = true;
}
/*--------------------------------------------------------------------------*/
void AC::decade(CS& cmd)
{
  step = cmd.ctopf();
  if (step == 0.){
    step = 1.;
  }
  step = pow(10., 1./step);
  needslinfix = false;
  linswp = false;
}
/*--------------------------------------------------------------------------*/
void AC::lin(CS& cmd)
{
  step = cmd.ctopf();		// need to fix step, later
  if (step == 0.){		// do it at the end of setup
    step = 1.;			// a kluge, but this is a patch
    untested();
  }else{
    untested();
  }
  needslinfix = true;		// and I am too lazy to do it
  linswp = true;		// right.
}
/*--------------------------------------------------------------------------*/
void AC::octave(CS& cmd)
{
  step = cmd.ctopf();
  if (step == 0.){
    step = 1.;
    untested();
  }
  step = pow(2.00000001, 1./step);
  needslinfix = false;
  linswp = false;
}
/*--------------------------------------------------------------------------*/
void AC::times(CS& cmd)
{
  step = cmd.ctopf();
  if (step == 0.   &&   start != 0.){
    step = stop / start;
    untested();
  }else{
    untested();
  }
  needslinfix = false;
  linswp = false;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
