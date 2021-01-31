/*$Id: c_prbcmd.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
 * probe and plot commands
 * set up print and plot (select points, maintain probe lists)
 * command line operations
 */
//testing=script,sparse 2006.07.17
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#ifdef TRACE_UNTESTED
#include "e_cardlist.h" // not actually used.
#endif
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
void do_probe(CS& cmd, PROBELIST *probes, CARD_LIST *scope)
{
  assert(probes);
  assert(scope);
#ifdef TRACE_UNTESTED
  if(scope==&CARD_LIST::card_list){
  }else{ untested();
  }
#endif

  CKT_BASE::_sim->set_command_none();
  enum {aADD, aDELETE, aNEW} action;
  SIM_MODE simtype = s_NONE;

  if (cmd.match1('-')) {untested();	/* handle .probe - ac ...... */
    action = aDELETE;		/* etc. 		     */
    cmd.skip();
  }else if (cmd.match1('+')) {untested();
    action = aADD;
    cmd.skip();
  }else{			/* no -/+ means clear, but wait for */
    action = aNEW;		/* .probe ac + ..... 		    */
  }				/* which will not clear first	    */

  ONE_OF
    || Set(cmd, "tr{ansient}", &simtype, s_TRAN)
    || Set(cmd, "ac",	       &simtype, s_AC)
    || Set(cmd, "dc",	       &simtype, s_DC)
    || Set(cmd, "op",	       &simtype, s_OP)
    || Set(cmd, "fo{urier}",   &simtype, s_FOURIER)
    ;
  
  if (!simtype) {			/* must be all simtypes */
    if (cmd.is_end()) {			/* list all */
      probes[s_TRAN].listing("tran");
      probes[s_AC].listing("ac");
      probes[s_DC].listing("dc");
      probes[s_OP].listing("op");
      probes[s_FOURIER].listing("fourier");
    }else if (cmd.umatch("clear ")) {		/* clear all */
      for (int ii = sSTART;  ii < sCOUNT;  ++ii) {
	probes[ii].clear();
      }
    }else{itested();				/* error */
      throw Exception_CS("what's this?", cmd);
    }
  }else{
    if (cmd.is_end()) {untested();		/* list */
      probes[simtype].listing("");
    }else if (cmd.umatch("clear ")) {untested();/* clear */
      probes[simtype].clear();
    }else{					/* add/remove */
      CKT_BASE::_sim->init(scope);
      if (cmd.match1('-')) {itested();		/* setup cases like: */
	action = aDELETE;			/* .probe ac + ....  */
	cmd.skip();
      }else if (cmd.match1('+')) {
	action = aADD;
	cmd.skip();
      }else{
      }
      if (action == aNEW) {			/* no +/- here or at beg. */
	probes[simtype].clear();		/* means clear first	  */
	action = aADD;
      }else{
      }
      while (cmd.more()) {			/* do-it */
	if (cmd.match1('-')) {			/* handle cases like:	    */
	  action = aDELETE;			/* .pr ac +v(7) -e(6) +r(8) */
	  cmd.skip();
	}else if (cmd.match1('+')) {itested();
	  action = aADD;
	  cmd.skip();
	}else{
	}
	if (action == aDELETE) {
	  probes[simtype].remove_list(cmd);
	}else{
	  probes[simtype].add_list(cmd);
	}
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
class CMD_STORE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if(Scope==&CARD_LIST::card_list){
    }else{ untested();
    }
#endif
    assert(_probe_lists);
    assert(_probe_lists->store);
    do_probe(cmd, _probe_lists->store, Scope);
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store", &p0);
/*--------------------------------------------------------------------------*/
class CMD_ALARM : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if(Scope==&CARD_LIST::card_list){
    }else{ untested();
    }
#endif
    assert(_probe_lists);
    assert(_probe_lists->alarm);
    do_probe(cmd, _probe_lists->alarm, Scope);
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "alarm", &p1);
/*--------------------------------------------------------------------------*/
class CMD_PLOT : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if(Scope==&CARD_LIST::card_list){
    }else{ untested();
    }
#endif
    IO::plotset = true;
    assert(_probe_lists);
    assert(_probe_lists->plot);
    do_probe(cmd, _probe_lists->plot, Scope);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "iplot|plot", &p2);
/*--------------------------------------------------------------------------*/
class CMD_PRINT : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if(Scope==&CARD_LIST::card_list){
    }else{ untested();
    }
#endif
    IO::plotset = false;
    assert(_probe_lists);
    assert(_probe_lists->print);
    do_probe(cmd, _probe_lists->print, Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "iprint|print|probe", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
