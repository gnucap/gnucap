/*$Id: s_fo_set.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * set up  fourier analysis
 */
//testing=script,sparse 2006.07.16
#include "u_prblst.h"
#include "ap.h"
#include "s_fo.h"
/*--------------------------------------------------------------------------*/
//	void	FOURIER::setup(CS&);
//	void	FOURIER::fftallocate();
//	void	FOURIER::fftunallocate();
static	int	to_pow_of_2(double);
/*--------------------------------------------------------------------------*/
/* fo_setup: fourier analysis: parse command string and set options
 * 	(options set by call to TRANSIENT::options)
 */
void FOURIER::setup(CS& cmd)
{
  cont = true;
  if (cmd.is_pfloat()) {
    double arg1 = cmd.ctopf();
    double arg2 = cmd.ctopf();
    double arg3 = cmd.ctopf();
    if (arg3 != 0.) {			    /* 3 args: all */
      fstart = arg1;
      fstop  = arg2;
      fstep  = arg3;
    }else if (arg2 != 0.) {		    /* 2 args: start = 0 */
      untested();
      if (arg1 >= arg2) {		    /* 2 args: stop, step */
	untested();
	fstart = 0.;		    	    /* 	(stop > step) */
	fstop  = arg1;
	fstep  = arg2;
      }else{ /* arg1 < arg2 */		    /* 2 args: step, stop */
	untested();
	fstart = 0.;
	fstop  = arg2;
	fstep  = arg1;
      }
    }else if (arg1 == 0.) {		    /* 1 arg: start */
      untested();
      fstart = 0.;
      /* fstop unchanged */
      /* fstep unchanged */
    }else{				    /* 1 arg: step */
      untested();
      fstart = 0.;
      fstop  = OPT::harmonics * arg1;
      fstep  = arg1;
    }
  }else{
    untested();
  }
  /* else (no args) : no change */
  
  if (fstep == 0.) {
    untested();
    error(bERROR, "frequency step = 0\n");
  }
  if (fstop == 0.) {
    untested();
    fstop = OPT::harmonics * fstep;
  }
  options(cmd);
  
  timesteps = to_pow_of_2(fstop*2 / fstep) + 1;
  if (cold) {
    untested();
    cont = false;
    tstart = 0.;
  }else{
    cont = true;
    tstart = last_time;
  }
  tstop = tstart + 1. / fstep;
  tstep = 1. / fstep / (timesteps-1);
  time1 = time0 = tstart;
  dtmax = tstep / static_cast<double>(skip);
  dtmin = std::max(dtmin,dtmax/dtratio);
}
/*--------------------------------------------------------------------------*/
/* allocate:  allocate space for fft
 */
void FOURIER::fftallocate()
{
  int probes = printlist().size();
  fdata = new COMPLEX*[probes];
  for (int ii = 0;  ii < probes;  ++ii) {
    fdata[ii] = new COMPLEX[timesteps+100];
  }
}
/*--------------------------------------------------------------------------*/
/* unallocate:  unallocate space for fft
 */
void FOURIER::fftunallocate()
{
  for (int ii = 0;  ii < printlist().size();  ++ii) {
    delete [] fdata[ii];
  }
  delete [] fdata;
  fdata = NULL;
}
/*--------------------------------------------------------------------------*/
/* to_pow_of_2: round up to nearest power of 2
 * example: z=92 returns 128
 */
static int to_pow_of_2(double z)
{
  long x = static_cast<long>(floor(z));
  long y;
  for (y = 1; x > 0; x >>= 1) {
    y <<= 1;
  }
  return y;
}   
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
