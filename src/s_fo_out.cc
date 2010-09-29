/*$Id: s_fo_out.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * functions needed in addition to the transient analysis
 * to perform the fourier command.
 */
#include "u_prblst.h"
#include "constant.h"
#include "declare.h"	/* plclose, plclear, fft */
#include "s_fo.h"
/*--------------------------------------------------------------------------*/
//	void	FOURIER::store();
//	void	FOURIER::foout();
//	void	FOURIER::fohead(const PROBE&);
//	void	FOURIER::foprint(COMPLEX*);
static  int	stepnum(double,double,double);
static	COMPLEX	find_max(COMPLEX*,int,int);
static	double  phaze(COMPLEX);
static	double  db(COMPLEX);
/*--------------------------------------------------------------------------*/
/* store: stash time domain data in preparation for Fourier Transform
 */
void FOURIER::store()
{
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=printlist().begin();  p!=printlist().end();  ++p){
    fdata[ii][stepno] = p->value();
    ++ii;
  }
}
/*--------------------------------------------------------------------------*/
/* foout:  print out the results of the transform
 */
void FOURIER::foout()
{
  plclose();
  plclear();
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=printlist().begin();  p!=printlist().end();  ++p){
    fohead(*p);
    fft(fdata[ii], timesteps-1,  0);
    foprint(fdata[ii]);
    ++ii;
  }
}
/*--------------------------------------------------------------------------*/
/* fo_head: print output header
 * arg is index into probe array, to select probe name
 */
void FOURIER::fohead(const PROBE& prob)
{
  out.form("# %-10s", prob.label().c_str())
    << "--------- actual ---------  -------- relative --------\n"
    << "#freq       "
    << "value        dB      phase  value        dB      phase\n";
}
/*--------------------------------------------------------------------------*/
/* fo_print: print results of fourier analysis
 * for all points at single probe
 */
void FOURIER::foprint(COMPLEX *data)
{
  int startstep = stepnum(0., fstep, fstart);
  int stopstep  = stepnum(0., fstep, fstop );
  COMPLEX maxvalue = find_max(data,std::max(1,startstep),stopstep);
  if (maxvalue == 0.){
    untested();
    maxvalue = 1.;
  }
  data[0] /= 2;
  for (int ii = startstep;  ii <= stopstep;  ++ii){
    double frequency = fstep * ii;
    COMPLEX unscaled = data[ii];
    COMPLEX scaled = unscaled / maxvalue;
    unscaled *= 2;
    out.form("%s%s%7.2f %8.3f %s%7.2f %8.3f\n",
	     ftos(frequency,    11,5,out.format()),
        ftos(abs(unscaled),11,5,out.format()),
	     db(unscaled),
	     phaze(unscaled*COMPLEX(0.,1)),
        ftos(abs(scaled),  11,5,out.format()),
	     db(scaled),
	     phaze(scaled) ) ;
  }
}
/*--------------------------------------------------------------------------*/
/* stepnum: return step number given its frequency or time
 */
static int stepnum(double start, double step, double here)
{
  return int((here-start)/step + .5);
}
/*--------------------------------------------------------------------------*/
/* find_max: find the max magnitude in a COMPLEX array
 */
static COMPLEX find_max(COMPLEX *data, int start, int stop)
{
  COMPLEX maxvalue = 0.;
  for (int ii = start;  ii <= stop;  ++ii){
    if (abs(data[ii]) > abs(maxvalue)){
      maxvalue = data[ii];
    }
  }
  return maxvalue;
}
/*--------------------------------------------------------------------------*/
/* phaze: extract phase (degrees) from COMPLEX number
 * rotates 90 degrees!  (ref to sine instead of cosine)
 */
static double phaze(COMPLEX x)
{
  return arg(x)* RTOD;
}
/*--------------------------------------------------------------------------*/
static double db(COMPLEX value)
{
  return  20. * log10(std::max(std::abs(value),VOLTMIN));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
