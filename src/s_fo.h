/*$Id: s_fo.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Fourier analysis
 */
//testing=script,complete 2006.07.14
#include "s_tr.h"
#ifndef S_FO_H
#define S_FO_H
/*--------------------------------------------------------------------------*/
class PROBE;
/*--------------------------------------------------------------------------*/
class FOURIER : private TRANSIENT {
public:
  void	command(CS&);
  explicit FOURIER():
    TRANSIENT(),
    fstart(0.),
    fstop(0.),
    fstep(0.),
    timesteps(0),
    fdata(NULL)
  {
  }
  ~FOURIER() {}
private:
  explicit FOURIER(const FOURIER&): TRANSIENT() {unreachable(); incomplete();}
  void	setup(CS&);	/* s_fo_set.cc */
  void	fftallocate();
  void	fftunallocate();
  void	foout();	/* s_fo_out.cc */
  void	fohead(const PROBE&);
  void	foprint(COMPLEX*);
  void	store();
private:
  double fstart;	/* user start frequency */
  double fstop;		/* user stop frequency */
  double fstep;		/* fft frequecncy step */
  int    timesteps;	/* number of time steps in tran analysis, incl 0 */
  COMPLEX** fdata;	/* storage to allow postprocessing */
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
