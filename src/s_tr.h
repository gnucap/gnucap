/*$Id: s_tr.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Transient analysis
 */
//testing=script,complete 2006.07.14
#ifndef S_TR_H
#define S_TR_H
#include "s__.h"
/*--------------------------------------------------------------------------*/
class TRANSIENT : public SIM {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:
  enum STEP_CAUSE {
    scUSER      = 1,	/* user requested				*/
    scEVENTQ    = 2,	/* an "event" from the queue			*/
    scSKIP      = 3,	/* effect of "skip" parameter			*/
    scITER_R    = 4,	/* iter count exceeds itl4 (reducing)		*/
    scITER_A    = 5,	/* iter count exceeds itl3 (holding)		*/
    scTE	= 6,	/* truncation error, or device stuff		*/
    scAMBEVENT	= 7,	/* ambiguous event				*/
    scADT	= 8,	/* by iter count limited by max(rdt, 2*adt)	*/
    scINITIAL	= 9,	/* initial guess				*/
    scREJECT    = 10,	/* rejected previous time step			*/
    scZERO      = 20,	/* fixed zero time step				*/
    scSMALL     = 30,	/* time step too small				*/
    scNO_ADVANCE= 100	/* after all that it still didn't advance	*/
  };
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
public:
  explicit TRANSIENT():
    SIM(),
    tstart(0.),
    tstop(0.),
    tstep(0.),
    dtmax(0.),
    dtratio(0.),
    skip(0),
    cold(false),
    cont(false),
    echo(false),
    trace(tNONE),
    time_suggested_by_review(0.),
    control(scINITIAL),
    time_by_iteration_count(0.),
    time_by_user_request(0.),
    time_by_error_estimate(0.),
    time_by_ambiguous_event(0.),
    converged(false),
    accepted(false)
  {
  }
  ~TRANSIENT() {}
public:
  void	command(CS&);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:		// s_tr_rev.cc
  bool	review();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:		// s_tr_set.cc
  void	setup(CS&);
protected:
  void	options(CS&);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:		// s_tr_swp.cc
  void	sweep();
private:
  void	set_step_cause(STEP_CAUSE);
public:
  int	step_cause()const;
  void	first();
  bool	next();
  void	accept();
  void	reject();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:
  bool	is_step_rejected()const {return (step_cause() > scREJECT);}
  explicit TRANSIENT(const TRANSIENT&): SIM() {unreachable(); incomplete();}
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  double tstart;	// sweep start time
  double tstop;		// sweep stop time
  double tstep;		// printed step size
  double dtmax;		// max internal step size (step / skip)
  double dtratio;	// ratio of max/min dt
  int skip;		// fixed step size: internal steps per external
  bool cold;		// flag: start time=0, all voltages=0
  bool cont;		// flag: continue from previous run
private:
  bool echo;		// flag: echo the input when using input data file
  TRACE trace;		// enum: show extended diagnostics
  double time_suggested_by_review;	// guess at best time for next step
  STEP_CAUSE control;	// why this time (enum)
  double time_by_iteration_count;
  double time_by_user_request;
  double time_by_error_estimate;
  double time_by_ambiguous_event;
  bool converged;
  bool accepted;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  static int steps_accepted_;
  static int steps_rejected_;
  static int steps_total_;
public:
  static int steps_accepted() {return steps_accepted_;}
  static int steps_rejected() {return steps_rejected_;}
  static int steps_total()    {return steps_total_;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
