/*$Id: c_genrat.cc 2015.01.08 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
 * set up generator for transient analysis
 */
//testing=script,complete 2015.01.08
//BUG// no step control
//BUG// encapsulation violation: gen() is global
#include "u_sim_data.h"
#include "globals.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
static double freq = 0;
static double ampl = 1;
static double phaz = 0.;
static double maxv = 1.;
static double minv = 0.;
static double offset = 0.;
static double init_ = 0.;
static double rise = 1e-12;
static double fall = 1e-12;
static double delay = 0.;
static double width = 0.;
static double period = 0.;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_ : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    OMSTREAM where = (cmd.more())  ?  OMSTREAM()  :  IO::mstdout;
    unsigned here = cmd.cursor();
    do{
      ONE_OF
	|| ::Get(cmd, "f{requency}",&freq,   mPOSITIVE)
	|| ::Get(cmd, "a{mplitude}",&ampl)
	|| ::Get(cmd, "p{hase}",    &phaz)
	|| ::Get(cmd, "ma{x}",	    &maxv)
	|| ::Get(cmd, "mi{n}",	    &minv)
	|| ::Get(cmd, "o{ffset}",   &offset)
	|| ::Get(cmd, "i{nitial}",  &init_)
	|| ::Get(cmd, "r{ise}",	    &rise,   mPOSITIVE)
	|| ::Get(cmd, "f{all}",	    &fall,   mPOSITIVE)
	|| ::Get(cmd, "d{elay}",    &delay,  mPOSITIVE)
	|| ::Get(cmd, "w{idth}",    &width,  mPOSITIVE)
	|| ::Get(cmd, "pe{riod}",   &period, mPOSITIVE)
	;
    }while (cmd.more() && !cmd.stuck(&here));
    cmd.check(bWARNING, "what's this");
    
    where.setfloatwidth(7);
    where <<   "freq="	<< freq;
    where << "  ampl="	<< ampl;
    where << "  phase="	<< phaz;
    where << "  max="	<< maxv;
    where << "  min="	<< minv;
    where << "  offset="<< offset;
    where << "  init="	<< init_;
    where << "  rise="	<< rise;
    where << "  fall="	<< fall;
    where << "  delay="	<< delay;
    where << "  width="	<< width;
    where << "  period="<< period;
    where << "\n";
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "generator", &p);
}
/*--------------------------------------------------------------------------*/
double gen()
{
  if (CKT_BASE::_sim->_time0 <= delay) {
    return init_;
  }else{
  }
  double loctime = CKT_BASE::_sim->_time0 - delay;
  if (period > 0.) {
    
    loctime = fmod(loctime, period);
  }else{
    // not periodic
  }

  double level;
  if (CKT_BASE::_sim->_time0 <= delay + rise) {			/* initial rise */
    level = (maxv - 0) * (loctime/rise) + 0;
  }else if (loctime <= rise) {				/* rising       */
    
    level = (maxv - minv) * (loctime/rise) + minv;
  }else if (width==0.  ||  (loctime-=rise) <= width) {	/* pulse on     */
    level = maxv;
  }else if ((loctime-=width) <= fall) {			/* falling      */
    
    level = (minv - maxv) * (loctime/fall) + maxv;
  }else{						/* pulse off    */
    
    level = minv;
  }
  level *= (freq == 0.) 
    ? ampl
    : ampl * sin(M_TWO_PI * freq*(CKT_BASE::_sim->_time0-delay) + phaz * DTOR);
  return (CKT_BASE::_sim->_time0 <= delay + rise)
    ? level + (offset - init_) * (loctime/rise) + init_
    : level + offset;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
