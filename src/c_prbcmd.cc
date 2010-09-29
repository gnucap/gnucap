/*$Id: c_prbcmd.cc,v 24.5 2003/04/27 01:05:05 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * probe and plot commands
 * set up print and plot (select points, maintain probe lists)
 * command line operations
 */
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

  {if (cmd.match1('-')){	/* handle .probe - ac ...... */
    action = aDELETE;		/* etc. 		     */
    cmd.skip();
  }else if (cmd.match1('+')){
    action = aADD;
    cmd.skip();
  }else{			/* no -/+ means clear, but wait for */
    action = aNEW;		/* .probe ac + ..... 		    */
  }}				/* which will not clear first	    */

  0
    || set(cmd, "TRan",	   &simtype,	sTRAN)
    || set(cmd, "AC",	   &simtype,	sAC)
    || set(cmd, "DC",	   &simtype,	sDC)
    || set(cmd, "OP",	   &simtype,	sOP)
    || set(cmd, "FOurier", &simtype,	sFOURIER)
    ;
  
  if (!simtype){			/* must be all simtypes */
    {if (cmd.end()){				/* list all */
      probes[sTRAN].listing("tran");
      probes[sAC].listing("ac");
      probes[sDC].listing("dc");
      probes[sOP].listing("op");
      probes[sFOURIER].listing("fourier");
    }else if (cmd.pmatch("CLEAR")){		/* clear all */
      for (int ii = sSTART;  ii < sCOUNT;  ++ii)
	probes[ii].clear();
    }else{					/* error */
      cmd.warn(bERROR, "what's this?");
    }}
  }else{
    {if (cmd.end()){			/* list */
      probes[simtype].listing("");
    }else if (cmd.pmatch("CLEAR")){	/* clear */
      probes[simtype].clear();
    }else{				/* add/remove */
      SIM::init();
      {if (cmd.match1('-')){			/* setup cases like: */
	action = aDELETE;			/* .probe ac + ....  */
	cmd.skip();
      }else if (cmd.match1('+')){
	action = aADD;
	cmd.skip();
      }}
      if (action == aNEW){			/* no +/- here or at beg. */
	probes[simtype].clear();		/* means clear first	  */
	action = aADD;
      }
      while (cmd.more()){			/* do-it */
	{if (cmd.match1('-')){			/* handle cases like:	    */
	  action = aDELETE;			/* .pr ac +v(7) -e(6) +r(8) */
	  cmd.skip();
	}else if (cmd.match1('+')){
	  action = aADD;
	  cmd.skip();
	}}
	{if (action == aDELETE){
	  probes[simtype] -= cmd;
	}else{
	  probes[simtype] += cmd;
	}}
      }
    }}
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
