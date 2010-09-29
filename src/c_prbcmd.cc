/*$Id: c_prbcmd.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * probe and plot commands
 * set up print and plot (select points, maintain probe lists)
 * command line operations
 */
//testing=script,sparse 2006.07.17
#include "ap.h"
#include "s__.h"
#include "c_comand.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::alarm(CS&);
//	void	CMD::plot(CS&);
//	void	CMD::print(CS&);
static	void	do_probe(CS&,PROBELIST*);
/*--------------------------------------------------------------------------*/
PROBELIST PROBE_LISTS::alarm[sCOUNT]; /* list of alarm points */
PROBELIST PROBE_LISTS::plot[sCOUNT];  /* list of plot points */
PROBELIST PROBE_LISTS::print[sCOUNT]; /* list of print points */
PROBELIST PROBE_LISTS::store[sCOUNT]; /* list of pts to store for postproc */
/*--------------------------------------------------------------------------*/
void CMD::alarm(CS& cmd)
{
  do_probe(cmd,PROBE_LISTS::alarm);
}
/*--------------------------------------------------------------------------*/
void CMD::plot(CS& cmd)
{
  IO::plotset = true;
  do_probe(cmd,PROBE_LISTS::plot);
}
/*--------------------------------------------------------------------------*/
void CMD::print(CS& cmd)
{
  IO::plotset = false;
  do_probe(cmd,PROBE_LISTS::print);
}
/*--------------------------------------------------------------------------*/
static void do_probe(CS& cmd, PROBELIST *probes)
{
  enum {aADD, aDELETE, aNEW} action;
  SIM_MODE simtype = sNONE;

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
    || set(cmd, "TRansient", &simtype,	sTRAN)
    || set(cmd, "AC",	     &simtype,	sAC)
    || set(cmd, "DC",	     &simtype,	sDC)
    || set(cmd, "OP",	     &simtype,	sOP)
    || set(cmd, "FOurier",   &simtype,	sFOURIER)
    ;
  
  if (!simtype) {			/* must be all simtypes */
    if (cmd.is_end()) {			/* list all */
      probes[sTRAN].listing("tran");
      probes[sAC].listing("ac");
      probes[sDC].listing("dc");
      probes[sOP].listing("op");
      probes[sFOURIER].listing("fourier");
    }else if (cmd.pmatch("CLEAR")) {		/* clear all */
      for (int ii = sSTART;  ii < sCOUNT;  ++ii) {
	probes[ii].clear();
      }
    }else{untested();					/* error */
      cmd.warn(bERROR, "what's this?");
    }
  }else{
    if (cmd.is_end()) {untested();		/* list */
      probes[simtype].listing("");
    }else if (cmd.pmatch("CLEAR")) {untested();	/* clear */
      probes[simtype].clear();
    }else{				/* add/remove */
      SIM::init();
      if (cmd.match1('-')) {untested();			/* setup cases like: */
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
	if (cmd.match1('-')) {untested();			/* handle cases like:	    */
	  action = aDELETE;			/* .pr ac +v(7) -e(6) +r(8) */
	  cmd.skip();
	}else if (cmd.match1('+')) {untested();
	  action = aADD;
	  cmd.skip();
	}else{
	}
	if (action == aDELETE) {untested();
	  probes[simtype].remove_list(cmd);
	}else{
	  probes[simtype].add_list(cmd);
	}
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
