/*$Id: c_status.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Displays the status of the system.  Makes all the calculations associated
 * with allocating memory but does not actually allocate it, unless necessary
 * to make the rest of the calculations.
 *
 *   If "allocate" is changed, this must also be changed.
 */
//testing=script 2006.07.17
#include "d_subckt.h"
#include "d_bjt.h"
#include "d_logic.h"
#include "d_mos_base.h"
#include "s_tr.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
void CMD::status(CS&cmd)
{ 
  IO::mstdout << "Gnucap   System status\n";
  
  if (!cmd.pmatch("Notime")) {
    ::status.compute_overhead();
    IO::mstdout
      << "command ------ last -- total\n"
      << ::status.get
      << ::status.op
      << ::status.dc
      << ::status.tran
      << ::status.four
      << ::status.ac
      << "function ----- last -- total\n"
      << ::status.set_up
      << ::status.order
      << "function ----- last -- total\n"
      << ::status.advance
      << ::status.queue
      << ::status.evaluate
      << ::status.load
      << ::status.lud
      << ::status.back
      << ::status.review
      << ::status.accept
      << ::status.output
      << ::status.overhead;
    if (OPT::showall) {untested();
      IO::mstdout 
	<< ::status.aux1
	<< ::status.aux2
	<< ::status.aux3;
    }
    IO::mstdout << ::status.total;
  }

  IO::mstdout
    << "iterations: op=" << ::status.iter[sOP]
    << ", dc=" << ::status.iter[sDC]
    << ", tran=" << ::status.iter[sTRAN]
    << ", fourier=" << ::status.iter[sFOURIER]
    << ", total=" << ::status.iter[iTOTAL]
    << "\n";
  IO::mstdout
    << "transient timesteps: accepted=" << TRANSIENT::steps_accepted()
    << ", rejected=" << TRANSIENT::steps_rejected()
    << ", total=" << TRANSIENT::steps_total()
    << "\n";  
  IO::mstdout
    << "nodes: user=" << ::status.user_nodes
    << ", subckt=" << ::status.subckt_nodes
    << ", model=" << ::status.model_nodes
    << ", total=" << ::status.total_nodes
    << "\n";  
  IO::mstdout
    << "devices: diodes=" << DEV_DIODE::count()
    << ", bjts=" << DEV_BJT::count()
    << ", mosfets=" << DEV_MOS::count()
    << ", gates=" << DEV_LOGIC::count()
    << ", subckts=" << DEV_SUBCKT::count()
    << "\n";
  IO::mstdout
    << "commons: diodes=" << COMMON_DIODE::count()
    << ", bjts=" << COMMON_BJT::count()
    << ", mosfets=" << COMMON_MOS::count()
    << ", gates=" << COMMON_LOGIC::count()
    << ", subckts=" << COMMON_SUBCKT::count()
    << "\n";
  IO::mstdout
    << "models:  diodes=" << MODEL_DIODE::count()
    << ", bjts=" << MODEL_BJT::count()
    << ", mosfets=" << MODEL_MOS_BASE::count()
    << ", gates=" << MODEL_LOGIC::count()
    << ", subckts=" << MODEL_SUBCKT::count()
    << "\n";
  IO::mstdout.form("dctran density=%.1f%%, ac density=%.1f%%\n",
  		   CKT_BASE::aa.density()*100., CKT_BASE::acx.density()*100.);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
