/*$Id: c_status.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * Displays the status of the system.  Makes all the calculations associated
 * with allocating memory but does not actually allocate it, unless necessary
 * to make the rest of the calculations.
 *
 *   If "allocate" is changed, this must also be changed.
 */
//testing=script 2006.07.17
#include "u_sim_data.h"
#include "u_status.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_STATUS : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    IO::mstdout << "Gnucap   System status\n";
    
    if (!cmd.umatch("n{otime} ")) {
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
      if (OPT::showall) {itested();
	IO::mstdout 
	  << ::status.aux1
	  << ::status.aux2
	  << ::status.aux3;
      }
      IO::mstdout << ::status.total;
    }
    
    IO::mstdout
      << "iterations: op=" << _sim->_iter[s_OP]
      << ", dc=" << _sim->_iter[s_DC]
      << ", tran=" << _sim->_iter[s_TRAN]
      << ", fourier=" << _sim->_iter[s_FOURIER]
      << ", total=" << _sim->_iter[iTOTAL]
      << "\n";    
    for(DISPATCHER<CKT_BASE>::const_iterator
	  i = status_dispatcher.begin();
	i != status_dispatcher.end();
	++i) {
      CKT_BASE* c = i->second;
      if (c) {
	IO::mstdout << c->status();
      }else{
      }
    }
    IO::mstdout
      << "nodes: user=" << _sim->_user_nodes
      << ", subckt=" << _sim->_subckt_nodes
      << ", model=" << _sim->_model_nodes
      << ", total=" << _sim->_total_nodes
      << "\n";  
    IO::mstdout.form("dctran density=%.1f%%, ac density=%.1f%%\n",
		 _sim->_aa.density()*100., _sim->_acx.density()*100.);
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "status", &p);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
