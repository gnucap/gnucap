/*$Id: s_fo.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * tran and fourier commands -- top
 * performs transient analysis, silently, then fft.
 * outputs results of fft
 */
//testing=script 2006.07.17
#include "u_status.h"
#include "s_fo.h"
/*--------------------------------------------------------------------------*/
//	void	FOURIER::command(CS&);
/*--------------------------------------------------------------------------*/
void FOURIER::command(CS& cmd)
{
  mode = sFOURIER;
  reset_timers();
  ::status.four.reset().start();

  init();
  alloc_vectors();
      
  aa.reallocate();
  aa.dezero(OPT::gmin);
  aa.set_min_pivot(OPT::pivtol);
  
  lu.reallocate();
  lu.dezero(OPT::gmin);
  lu.set_min_pivot(OPT::pivtol);

  setup(cmd);
  fftallocate();

  ::status.set_up.stop();
  switch (ENV::run_mode) {
  case rINTERACTIVE:
    untested();
  case rBATCH:
    untested();
  case rSCRIPT:
    sweep();
    foout();
    break;
  case rIGNORE:
    untested();
  case rPRESET:
    untested();
    /*nothing*/
    break;
  }
  fftunallocate();
  unalloc_vectors();
  lu.unallocate();
  aa.unallocate();
  
  ::status.four.stop();
  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
