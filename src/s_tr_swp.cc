/*$Id: s_tr_swp.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * sweep time and simulate.  output results.
 * manage event queue
 */
//testing=script 2006.06.14
#include "e_card.h"
#include "declare.h"	/* gen */
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::sweep(void);
//	void	TRANSIENT::first(void);
//	bool	TRANSIENT::next(void);
//	void	TRANSIENT::accept(void);
//	void	TRANSIENT::reject(void);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace TR {
  static const char* step_cause[] = {
    "impossible",
    "user requested",
    "event queue",
    "command line \"skip\"",
    "convergence failure, reducing (itl4)",
    "slow convergence, holding (itl3)",
    "truncation error",
    "ambiguous event",
    "limit growth",
    "initial guess"
  };
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep(void)
{
  head(tstart, tstop, "Time");
  bypass_ok = false;
  inc_mode = tsBAD;
  
  if (cont) {			/* use the data from last time */
    restore();
    CARD_LIST::card_list.tr_restore();
  }else{
    clear_limit();
    CARD_LIST::card_list.tr_begin();
  }
  
  first();
  phase = pINIT_DC;
  genout = gen();
  IO::suppresserrors = trace < tVERBOSE;
  
  if (uic_now()) {
    zero_voltages();
    CARD_LIST::card_list.do_tr();    //evaluate_models
    converged = true;
  }else{
    converged = solve(OPT::DCBIAS,trace);
    if (!converged) {
      untested();
      error(bWARNING, "did not converge\n");
    }
  }
  review(); 
  accepted = true;
  accept();
  
  {
    bool printnow = (time0 == tstart || trace >= tALLTIME);
    if (printnow) {
      keep();
      outdata(time0);
    }else{
      itested();
    }
  }
  
  while (next()) {
    bypass_ok = false;
    phase = pTRAN;
    genout = gen();
    IO::suppresserrors = trace < tVERBOSE;
    converged = solve(OPT::TRHIGH,trace);

    accepted = converged && review();

    if (accepted) {
      assert(converged);
      assert(time0 <= time_by_user_request);
      accept();
      if (step_cause() == scUSER) {
	assert(time0 == time_by_user_request);
	stepno++;
	time_by_user_request += tstep;	/* advance user time */
      }
      assert(time0 < time_by_user_request);
    }else{
      reject();
      assert(time1 < time_by_user_request);
    }
    {
      bool printnow =
	(trace >= tREJECTED)
	|| accepted && ((trace >= tALLTIME)
			|| (step_cause() == scUSER && time0+dtmin > tstart));
      if (printnow) {
	keep();
	outdata(time0);
      }
    }
    
    if (!converged && OPT::quitconvfail) {
      untested();
      outdata(time0);
      error(bERROR, "convergence failure, giving up\n");
    }
  }
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE c)
{
  switch (c) {

  case scITER_R:
    itested();
  case scINITIAL:
  case scSKIP:
  case scITER_A:
  case scTE:
  case scADT:
  case scAMBEVENT:
  case scEVENTQ:
  case scUSER:
    ::status.control[cSTEPCAUSE] = c;
    break;
    
  case scZERO:
    untested();
  case scSMALL:
    itested();
  case scREJECT:
  case scNO_ADVANCE:
    ::status.control[cSTEPCAUSE] += c;
    break;
  }
}
/*--------------------------------------------------------------------------*/
int TRANSIENT::step_cause()const
{
  return ::status.control[cSTEPCAUSE];
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first(void)
{
  /* usually, time0, time1 == 0, from setup */
  assert(time0 == time1);
  assert(time0 <= tstart);
  ::status.review.start();
  time_by_user_request = time0 + tstep;		/* set next user step */
  //eq.Clear();				/* empty the queue */
  while (!eq.empty()) {
    untested();
    eq.pop();
  }
  stepno = 0;
  set_step_cause(scUSER);
  ++::status.control[cSTEPS];
  ::status.review.stop();
}
/*--------------------------------------------------------------------------*/
#define check_consistency() {\
    trace4("", __LINE__, newtime, almost_fixed_time, fixed_time); \
    assert(almost_fixed_time <= fixed_time);				\
    assert(newtime <= fixed_time);					\
    assert(newtime <= almost_fixed_time);				\
    assert(newtime > time1);						\
    assert(newtime > reftime);						\
    assert(new_dt > 0.);						\
    assert(new_dt >= dtmin);						\
  }
/*--------------------------------------------------------------------------*/
/* next: go to next time step
 * Set time0 to the next time step, store the old one in time1.
 * Try several methods.  Take the one that gives the shortest step.
 */
bool TRANSIENT::next(void)
{
  ::status.review.start();

  double old_dt = time0 - time1;
  assert(old_dt >= 0);
  
  double newtime = NEVER;
  double new_dt = NEVER;
  STEP_CAUSE new_control = scNO_ADVANCE;

  if (phase == pINIT_DC) {
    // for the first time, just guess
    // make it 100x smaller than expected
    new_dt = std::max(dtmax/100., dtmin);
    newtime = time0 + new_dt;
    new_control = scINITIAL;
  }else if (!converged) {
    new_dt = old_dt / OPT::trstepshrink;
    newtime = time_by_iteration_count = time1 + new_dt;
    new_control = scITER_R;
  }else{
    double reftime;
    if (accepted) {
      reftime = time0;
      trace0("accepted");
    }else{
      reftime = time1;
      trace0("rejected");
    }
    trace2("", step_cause(), old_dt);
    trace3("", time1, time0, reftime);

    newtime = time_by_user_request;
    new_dt = newtime - reftime;
    new_control = scUSER;
    double fixed_time = newtime;
    double almost_fixed_time = newtime;
    check_consistency();
    
    // event queue, events that absolutely will happen
    // exact time.  NOT ok to move or omit, even by dtmin
    // some action is associated with it.
    if (!eq.empty() && eq.top() < newtime) {
      newtime = eq.top();
      new_dt = newtime - reftime;
      new_control = scEVENTQ;
      fixed_time = newtime;
      almost_fixed_time = newtime;
      check_consistency();
    }
    
    // device events that may not happen
    // not sure of exact time.  will be rescheduled if wrong.
    // ok to move by dtmin.  time is not that accurate anyway.
    if (time_by_ambiguous_event < newtime - dtmin) {
      newtime = time_by_ambiguous_event;
      new_dt = newtime - reftime;
      new_control = scAMBEVENT;
      almost_fixed_time = newtime;
      check_consistency();
    }
    
    // device error estimates
    if (time_by_error_estimate < newtime - dtmin) {
      newtime = time_by_error_estimate;
      new_dt = newtime - reftime;
      new_control = scTE;
      check_consistency();
    }
    
    // skip parameter
    if (new_dt > dtmax) {
      if (new_dt > dtmax + dtmin) {
	new_control = scSKIP;
      }
      new_dt = dtmax;
      newtime = reftime + new_dt;
      check_consistency();
    }

    // converged but with more iterations than we like
    if ((new_dt > (old_dt + dtmin) * OPT::trstephold)
	&& exceeds_iteration_limit(OPT::TRLOW)) {
      assert(accepted);
      new_dt = old_dt * OPT::trstephold;
      newtime = reftime + new_dt;
      new_control = scITER_A;
      check_consistency();
    }

    // limit growth
    if (phase != pINIT_DC && new_dt > old_dt * OPT::trstepgrow) {
      new_dt = old_dt * OPT::trstepgrow;
      newtime = reftime + new_dt;
      new_control = scADT;
      check_consistency();
    }

    // quantize
    if (newtime < almost_fixed_time) {
      assert(new_dt >= 0);
      if (newtime > reftime + old_dt*.8
	  && newtime < reftime + old_dt*1.5
	  && newtime < almost_fixed_time
	  && reftime + old_dt <= almost_fixed_time) {
	// new_dt is close enough to old_dt.
	// use old_dt, to avoid a step change.
	new_dt = old_dt;
	newtime = reftime + new_dt;
	check_consistency();
      }else{
	// There will be a step change.
	// Try to choose one that we will keep for a while.
	// Choose new_dt to be in integer fraction of target_dt.
	double target_dt = fixed_time - reftime;
	assert(target_dt >= new_dt);
	double steps = 1 + floor((target_dt - dtmin) / new_dt);
	assert(steps > 0);
	new_dt = target_dt / steps;
	newtime = reftime + new_dt;
	check_consistency();
      }
    }

    // trap time step too small
    if (!accepted && new_dt < dtmin) {
      untested();
      new_dt = dtmin;
      newtime = reftime + new_dt;
      new_control = scSMALL;
      check_consistency();
    }

    // if all that makes it close to user_requested, make it official
    if (up_order(newtime-dtmin, time_by_user_request, newtime+dtmin)) {
      newtime = time_by_user_request;
      new_dt = newtime - reftime;
      new_control = scUSER;
      check_consistency();			   
    }
    check_consistency();
    assert(!accepted || newtime > time0);
    assert(accepted || newtime <= time0);
  }
  
  set_step_cause(new_control);

  /* got it, I think */
  
  /* check to be sure */
  if (newtime < time1 + dtmin) {
    itested();
    /* It's really bad. */
    /* Reject the most recent step, back up as much as possible, */
    /* and creep along */
    assert(!accepted);
    assert(step_cause() < scREJECT);
    assert(step_cause() >= 0);
    error(bDANGER,"very backward time step\n%s\n",
	  TR::step_cause[step_cause()]);
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime, time0, time1, time1 + dtmin);
    newtime = time1 + dtmin;
    set_step_cause(scSMALL);
    error(bERROR, "tried everything, still doesn't work, giving up\n");
  }else if (newtime <= time0 - dtmin) {
    /* Reject the most recent step. */
    /* We have faith that it will work with a smaller time step. */
    assert(!accepted);
    assert(newtime >= time1 + dtmin);
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    set_step_cause(scREJECT);
  }else if (newtime < time0 + dtmin) {
    untested();
    /* Another evaluation at the same time. */
    /* Keep the most recent step, but creep along. */
    assert(newtime > time0 - dtmin);
    error(bLOG, "zero time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    if (accepted) {
      untested();
      time1 = time0;
    }else{
      untested();
    }
    newtime = time0 + dtmin;
    set_step_cause(scZERO);
  }else{
    assert(accepted);
    assert(newtime >= time0 + dtmin);
    /* All is OK.  Moving on. */
    /* Keep value of newtime */
    time1 = time0;
  }
  time0 = newtime;
  
  /* advance event queue (maybe) */
  /* We already looked at it.  Dump what's on top if we took it. */
  while (!eq.empty() && eq.top() <= time0) {
    eq.pop();
  }
  /* BUG:: what if it is later rejected?  It's lost! */

  ++::status.control[cSTEPS];
  ++steps_total_;
  ::status.review.stop();
  return (time0 <= tstop + dtmin);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept(void)
{
  ::status.accept.start();
  set_limit();
  if (OPT::traceload) {
    while (!acceptq.empty()) {
      acceptq.back()->tr_accept();
      acceptq.pop_back();
    }
  }else{
    untested();
    acceptq.clear();
    CARD_LIST::card_list.tr_accept();
  }
  ++steps_accepted_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::reject(void)
{
  ::status.accept.start();
  acceptq.clear();
  ++steps_rejected_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
