/*$Id: u_statu2.cc,v 20.15 2001/10/30 03:58:55 al Exp $ -*- C++ -*-
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
 * Displays the status of the system.  Makes all the calculations associated
 * with allocating memory but does not actually allocate it, unless necessary
 * to make the rest of the calculations.
 *
 *   If "allocate" is changed, this must also be changed.
 */
#include "d_subckt.h"
#include "d_logic.h"
#include "d_mos_base.h"
#include "ap.h"
#include "u_status.h"
/*--------------------------------------------------------------------------*/
//	void	STATUS::command(CS& cmd);
/*--------------------------------------------------------------------------*/
void STATUS::command(CS& cmd)
{ 
  IO::mstdout << "GnuCap   System status\n";
  init();
  
  if (!cmd.pmatch("Notime")){  
    overhead = total - advance - evaluate - load - lud - back - output 
      - review - accept;
    IO::mstdout
      << "command      --------  last  --------    --------  total  --------\n"
      << "               user      sys    total       user      sys    total\n"
      << get
      << op
      << dc
      << tran
      << four
      << ac
      << "function     --------  last  --------    --------  total  --------\n"
      << "               user      sys    total       user      sys    total\n"
      << set_up
      << order
      << "function     --------  last  --------    --------  total  --------\n"
      << "               user      sys    total       user      sys    total\n"
      << advance
      << evaluate
      << load
      << lud
      << back
      << review
      << accept
      << output
      << overhead;
    if (OPT::showall){
      IO::mstdout << aux1 << aux2 << aux3;
    }
    IO::mstdout << total;
  }

  IO::mstdout.form(
	  "iterations: op=%d, dc=%d, tran=%d, fourier=%d, total=%d\n",
	  iter[sOP], iter[sDC], iter[sTRAN], iter[sFOURIER], iter[iTOTAL]);
  IO::mstdout.form(
	  "nodes: user=%d, subckt=%d, model=%d, total=%d\n",
	  user_nodes, subckt_nodes, model_nodes, total_nodes);
  IO::mstdout.form(
	  "devices: diodes=%d, mosfets=%d, gates=%d, subckts=%d\n",
	  DEV_DIODE::count(), DEV_MOS::count(),
	  DEV_LOGIC::count(), DEV_SUBCKT::count());
  IO::mstdout.form(
	  "commons: diodes=%d, mosfets=%d, gates=%d, subckts=%d\n",
	  COMMON_DIODE::count(), COMMON_MOS::count(),
	  COMMON_LOGIC::count(), COMMON_SUBCKT::count());
  IO::mstdout.form(
	  "models:  diodes=%d, mosfets=%d, gates=%d, subckts=%d\n",
	  MODEL_DIODE::count(), MODEL_MOS_BASE::count(),
	  MODEL_LOGIC::count(), MODEL_SUBCKT::count());
  IO::mstdout.form("density=%.1f%%\n", aa.density()*100.);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
