/*$Id: s_tr_swp.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * sweep time and simulate.  output results.
 * manage event queue
 */
#include "e_card.h"
#include "u_opt.h"
#include "declare.h"	/* gen */
#include "u_status.h"
#include "constant.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::sweep(void);
	void	new_event(double);
//	void	TRANSIENT::first(void);
//	bool	TRANSIENT::next(void);
//	void	TRANSIENT::accept(void);
//	void	TRANSIENT::reject(void);
/*--------------------------------------------------------------------------*/
std::priority_queue<double, std::vector<double> > eq;
			/* the event queue */
static double nexttick;	/* next pre-scheduled user "event" */
/*--------------------------------------------------------------------------*/
namespace TR {
static const char* step_cause[] = {
  "impossible",
    "user requested",
    "event queue",
    "command line \"skip\"",
    "convergence failure (itl4)",
    "slow convergence (itl3)",
    "truncation error",
    "limit growth",
    "limit growth",
    "half step"
    };
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep(void)
{
  head(tstart, tstop, true/*linear*/, "Time");
  bypass_ok = false;
  inc_mode = tsBAD;
  {if (cont) {			/* use the data from last time */
    restore();
    CARD_LIST::card_list.tr_restore();
  }else{
    clear_limit();
    CARD_LIST::card_list.tr_begin();
  }}
  
  first();
  phase = pINIT_DC;
  genout = gen();
  IO::suppresserrors = trace < tVERBOSE;
  bool converged = solve(OPT::itl[OPT::DCBIAS],trace);
  if (!converged) {
    untested();
    error(bWARNING, "did not converge\n");
  }
  review(); 
  accept();

  {if (printnow) {
    keep();
    outdata(time0);
  }else{
    untested();
  }}
  
  while (next()) {
    bypass_ok = false;
    phase = pTRAN;
    genout = gen();
    IO::suppresserrors = trace < tVERBOSE;
    converged = solve(OPT::itl[OPT::TRHIGH],trace);
    review();
    
    {if (approxtime > time0) {
      accept();
    }else{
      reject();
    }}
    
    {if (trace >= tREJECTED) {
      printnow = true;
    }else if (!converged  ||  approxtime <= time0) {
      printnow = false;
    }} // else (usual case) use the value set in next
    
    if (printnow) {
      keep();
      outdata(time0);
    }
  }
}
/*--------------------------------------------------------------------------*/
void new_event(double etime)
{
  eq.push(etime);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE c)
{
  switch (c) {
  case scUSER:
  case scEVENTQ:
  case scSKIP:
  case scITER_R:
  case scITER_A:
  case scTE:
  case scRDT:
  case scADT:
  case scHALF:
    STATUS::control[cSTEPCAUSE] = c;
    break;
  case scREJECT:
  case scZERO:
  case scSMALL:
  case scNO_ADVANCE:
    STATUS::control[cSTEPCAUSE] += c;
    break;
  }
}
/*--------------------------------------------------------------------------*/
int TRANSIENT::step_cause()const
{
  return STATUS::control[cSTEPCAUSE];
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first(void)
{
  /* usually, time0, time1 == 0, from setup */
  STATUS::review.start();
  nexttick = time0 + tstep;		/* set next user step */
  //eq.Clear();				/* empty the queue */
  while (!eq.empty()) {
    eq.pop();
  }
  printnow = true;
  stepno = 0;
  set_step_cause(scUSER);
  ++STATUS::control[cSTEPS];
  STATUS::review.stop();
}
/*--------------------------------------------------------------------------*/
bool TRANSIENT::next(void)
{
  STATUS::review.start();
  /* review(tr); */			/* trunc error, etc.*/
 					/* was already done before print */
  double newtime = nexttick;				/* user time steps */
  set_step_cause(scUSER);
  
  if (!eq.empty() && eq.top() <= newtime) {
    newtime = eq.top();
    set_step_cause(scEVENTQ);
  }
 
  if (approxtime < newtime - dtmin) {
    {if (approxtime < (newtime + time0) / 2.) {
      newtime = approxtime;
      set_step_cause(control);
    }else{
      newtime = (newtime + time0) / 2.;
      set_step_cause(scHALF);
    }}
  }
  
  {if (nexttick < newtime + dtmin) {	/* advance user time */
    printnow = true;			/* print if user step */
    stepno++;
    nexttick += tstep;			/* BUG??? :  does the print get */
  }else{				/* messed up if a user step is */
    printnow = trace >= tALLTIME;	/* rejected?? */
  }}
  
  while (!eq.empty() && eq.top() <= newtime) {  /* advance event queue */
    eq.pop();
  }

  if (newtime < time1 + dtmin) {
    untested();
    assert(step_cause() < scREJECT);
    error(bDANGER,"very backward time step\n%s\n",
	  TR::step_cause[step_cause()]);
    error(bTRACE, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime, time0, time1, time1 + dtmin);
    newtime = time1 + dtmin;
    set_step_cause(scNO_ADVANCE);
  }
  {if (newtime <= time0 - dtmin) {
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    set_step_cause(scREJECT);
    if (newtime < (nexttick - tstep)) {
      error(bLOG, "user step rejected\n");
      nexttick -= tstep;
    }
  }else if (newtime < time0 + dtmin) {
    untested();
    error(bWARNING, "zero time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    time1 = time0;
    newtime = time0 + dtmin;
    set_step_cause(scZERO);
  }else{
    time1 = time0;
  }}
  
  time0 = newtime;
  ++STATUS::control[cSTEPS];
  STATUS::review.stop();
  return (time0 <= tstop + dtmin);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept(void)
{
  STATUS::accept.start();
  set_limit();
  {if (OPT::traceload) {
    while (!acceptq.empty()) {
      acceptq.back()->tr_accept();
      acceptq.pop_back();
    }
  }else{
    acceptq.clear();
    CARD_LIST::card_list.tr_accept();
  }}
  STATUS::accept.stop();
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::reject(void)
{
  STATUS::accept.start();
  acceptq.clear();
  STATUS::accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
