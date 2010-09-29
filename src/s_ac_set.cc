/*$Id: s_ac_set.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * ac analysis setup
 */
//testing=script 2006.07.14
#include "u_prblst.h"
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
  temp_c = OPT::temp_c;
  bool ploton = IO::plotset  &&  plotlist().size() > 0;

       if (cmd.pmatch("*$$"))    {untested();	times(cmd);}
  else if (cmd.pmatch("By"))     {untested();	by(cmd);}
  else if (cmd.pmatch("Decade")) {		decade(cmd);}
  else if (cmd.pmatch("TImes"))  {untested();	times(cmd);}
  else if (cmd.pmatch("LIn"))    {untested();	lin(cmd);}
  else if (cmd.pmatch("Octave")) {		octave(cmd);}
  
  if (cmd.is_float()) {
    start = cmd.ctof();
    stop  = cmd.ctof();
    if (stop==0.) {
      stop = start;
    }
    if (cmd.is_float()) {
      by(cmd);
    }
  }

  int here = cmd.cursor();
  do{
    ONE_OF
      || (cmd.pmatch("*$$")	&& times(cmd))
      || (cmd.pmatch("+$$")	&& by(cmd))
      || (cmd.pmatch("By")	&& by(cmd))
      || (cmd.pmatch("Decade")	&& decade(cmd))
      || (cmd.pmatch("TImes")	&& times(cmd))
      || (cmd.pmatch("LIn")	&& lin(cmd))
      || (cmd.pmatch("Octave")	&& octave(cmd))
      || get(cmd, "DTemp",	&temp_c,  mOFFSET, OPT::temp_c)
      || get(cmd, "PLot",	&ploton)
      || get(cmd, "TEmperature",&temp_c)
      || outset(cmd,&out)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(out);
  if (needslinfix) {untested();		// LIN option is # of points.
    assert(step >= 2);			// Must compute step after 
    step=(stop-start)/(step-1.);	// reading start and stop,
    needslinfix = false;		// but step must be read first
  }					// for Spice compatibility
  if (step==0.) {
    step = stop - start;
    linswp = true;
  }
}
/*--------------------------------------------------------------------------*/
bool AC::by(CS& cmd)
{
  step = cmd.ctof();
  needslinfix = false;
  linswp = true;
  return true;
}
/*--------------------------------------------------------------------------*/
bool AC::decade(CS& cmd)
{
  step = cmd.ctopf();
  if (step == 0.) {
    step = 1.;
  }
  step = pow(10., 1./step);
  needslinfix = false;
  linswp = false;
  return true;
}
/*--------------------------------------------------------------------------*/
bool AC::lin(CS& cmd)
{untested();
  step = cmd.ctopf();		// need to fix step, later
  if (step <= 2.) {untested();		// do it at the end of setup
    step = 2.;			// a kluge, but this is a patch
  }else{untested();
  }
  needslinfix = true;		// and I am too lazy to do it
  linswp = true;		// right.
  return true;
}
/*--------------------------------------------------------------------------*/
bool AC::octave(CS& cmd)
{
  step = cmd.ctopf();
  if (step == 0.) {untested();
    step = 1.;
  }
  step = pow(2.00000001, 1./step);
  needslinfix = false;
  linswp = false;
  return true;
}
/*--------------------------------------------------------------------------*/
bool AC::times(CS& cmd)
{untested();
  step = cmd.ctopf();
  if (step == 0.  &&  start != 0.) {untested();
    step = stop / start;
    untested();
  }else{untested();
  }
  needslinfix = false;
  linswp = false;
  return true;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
