/*$Id: s_tr_swp.cc  $ -*- C++ -*-
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
 * sweep time and simulate.  output results.
 * manage event queue
 */
//testing=script 2024.09.13
#include "u_time_pair.h"
#include "u_sim_data.h"
#include "u_status.h"
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
  static std::string step_cause[] = { //
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
class TIME_t {
  static double _dtmin;
  double _t {0};
  explicit TIME_t(int, const double& T) : _t(T) {}
public:
  explicit TIME_t() : _t(0) {}
	   TIME_t(const TIME_t& T) : _t(T._t) {}
  explicit TIME_t(const double& T) : _t(std::round(T/_dtmin)) {}

  TIME_t& operator=(const TIME_t& T) {_t = T._t; return *this;}

  TIME_t operator+(const TIME_t& B)const {return TIME_t(0,_t + B._t);}
  TIME_t operator-(const TIME_t& B)const {return TIME_t(0,_t - B._t);}
  TIME_t operator*(const double& B)const {return TIME_t(0,_t * B);}
  TIME_t operator/(const double& B)const {return TIME_t(0,_t / B);}

  bool   operator>(const TIME_t& B)const {return (_t > B._t);}
  bool   operator>=(const TIME_t& B)const {return (_t >= B._t);}
  bool   operator<(const TIME_t& B)const {return (_t < B._t);}
  bool   operator<=(const TIME_t& B)const {return (_t <= B._t);}
  bool   operator==(const TIME_t& B)const {return (_t == B._t);}

  double to_double()const {return _t*_dtmin;}

  static void set_dtmin(double Dtmin) {_dtmin = Dtmin;}
};
double TIME_t::_dtmin {1e-12};
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep()
{
  _sim->_phase = p_INIT_DC;
  head(_tstart, _tstop, "Time");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  TIME_t::set_dtmin(_sim->_dtmin);
  
  if (_cont) {  // use the data from last time
    // keep event queue contents
    assert(_sim->_eq.empty() || _sim->_eq.top() >= _sim->_time0);
    _sim->_phase = p_RESTORE;
    _sim->restore_voltages();
    _scope->tr_restore();
  }else{
    while (!_sim->_eq.empty()) {untested();
      _sim->_eq.pop();
    }
    _sim->clear_limit();
    _scope->tr_begin();
  }
  
  first();
  _sim->_genout = gen();
  
  if (_sim->uic_now()) {
    advance_time();
    _sim->zero_voltages();
    _scope->do_tr();    //evaluate_models
    while (!_sim->_late_evalq.empty()) {untested(); //BUG// encapsulation violation
      _sim->_late_evalq.front()->do_tr_last();
      _sim->_late_evalq.pop_front();
    }
    _converged = true;
    _sim->_loadq.clear(); // fake solve, clear the queue
    //BUG// UIC needs further analysis.
  }else{
    _converged = solve_with_homotopy(OPT::DCBIAS,_trace);
    if (!_converged) {
      error(bWARNING, "did not converge\n");
    }else{
    }
  }
  review(); 
  _accepted = true;
  accept();
  
  {
    bool printnow = (_sim->_time0 == _tstart || _trace >= tALLTIME);
    int outflags = ofNONE;
    if (printnow) {
      outflags = ofPRINT | ofSTORE | ofKEEP;
    }else{
      outflags = ofSTORE;
    }
    outdata(_sim->_time0, outflags);
  }
  
  while (next()) {
    _sim->_bypass_ok = false;
    _sim->_phase = p_TRAN;
    _sim->_genout = gen();
    _converged = solve(OPT::TRHIGH,_trace);

    _accepted = _converged && review();

    if (_accepted) {
      assert(_converged);
      assert(_sim->_time0 <= _time_by_user_request + _sim->_dtmin);
      accept();
      if (step_cause() == scUSER) {
	assert(up_order(_sim->_time0-_sim->_dtmin, _time_by_user_request, _sim->_time0+_sim->_dtmin));
	++_stepno;
	_time_by_user_request += _tstrobe;	/* advance user time */
      }else{
      }
      assert(_sim->_time0 < _time_by_user_request);
    }else{
      reject();
      assert(_time1 < _time_by_user_request);
    }
    {
      bool printnow =
	(_trace >= tREJECTED)
	|| (_accepted && (_trace >= tALLTIME
			  || step_cause() == scUSER
			  || (!_tstrobe.has_hard_value() && _sim->_time0+_sim->_dtmin > _tstart)));
      int outflags = ofNONE;
      if (printnow) {
	outflags = ofPRINT | ofSTORE | ofKEEP;
      }else if (_accepted) {
	outflags = ofSTORE;
      }else{
      }
      outdata(_sim->_time0, outflags);
    }
    
    if (!_converged && OPT::quitconvfail) {untested();
      outdata(_sim->_time0, ofPRINT);
      throw Exception("convergence failure, giving up");
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE C)
{
  switch (C) {
  case scITER_A:untested();
      // fall through
  case scADT:untested();
      // fall through
  case scUSER:
      // fall through
  case scEVENTQ:
      // fall through
  case scSKIP:
      // fall through
  case scITER_R:
      // fall through
  case scTE:
      // fall through
  case scAMBEVENT:
      // fall through
  case scINITIAL:
    ::status.control = C;
    break;
  case scNO_ADVANCE:untested();
      // fall through
  case scZERO:untested();
      // fall through
  case scSMALL:
      // fall through
  case scREJECT:
    ::status.control += C;
    break;
  }
}
/*--------------------------------------------------------------------------*/
int TRANSIENT::step_cause()const
{
  return ::status.control;
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first()
{
  /* usually, _sim->_time0, time1 == 0, from setup */
  assert(_sim->_time0 == _time1);
  assert(_sim->_time0 <= _tstart);
  ::status.review.start();

  _stepno = 0;

  //_time_by_user_request = _sim->_time0 + _tstrobe;	/* set next user step */
  //set_step_cause(scUSER);

  if (_sim->_time0 < _tstart) {			// skip until _tstart
    set_step_cause(scINITIAL);				// suppressed 
    _time_by_user_request = _tstart;			// set first strobe
  }else{					// no skip
    set_step_cause(scUSER);				// strobe here
    _time_by_user_request = _sim->_time0 + _tstrobe;	// set next strobe
  }

  ::status.hidden_steps = 0;
  ::status.review.stop();
}
/*--------------------------------------------------------------------------*/
#define check_consistency() {						\
    trace3("", __LINE__, almost_fixed_time.to_double(), fixed_time.to_double()); \
    trace4("", __LINE__, newtime.to_double(), reftime.to_double(), new_dt.to_double()); \
    assert(almost_fixed_time <= fixed_time);				\
    assert(newtime <= fixed_time);					\
    assert(newtime <= almost_fixed_time);				\
    assert(newtime > time1);						\
    assert(newtime > reftime);						\
    assert(newtime <= TIME_t(_time_by_user_request + _sim->_dtmin));	\
    assert(newtime <= TIME_t(_time_by_user_request));			\
    assert(new_dt >= TIME_t(0.));					\
    assert(new_dt > TIME_t(0.));					\
    assert(new_dt >= TIME_t(_sim->_dtmin));				\
  }
#define check_consistency2() {						\
    trace4("", __LINE__, newtime.to_double(), reftime.to_double(), new_dt.to_double()); \
    assert(newtime > time1);						\
    assert(newtime <= TIME_t(_time_by_user_request + _sim->_dtmin));	\
    assert(newtime <= TIME_t(_time_by_user_request));			\
    assert(new_dt >= TIME_t(0.));					\
    assert(new_dt > TIME_t(0.));					\
    assert(new_dt >= TIME_t(_sim->_dtmin));				\
  }
/*--------------------------------------------------------------------------*/
/* next: go to next time step
 * Set _sim->_time0 to the next time step, store the old one in time1.
 * Try several methods.  Take the one that gives the shortest step.
 */
bool TRANSIENT::next()
{
  ::status.review.start();

  TIME_t time1(_time1);
  TIME_t time0(_sim->_time0);
  
  TIME_t reftime;
  if (_accepted) {
    reftime = time0;
    trace0("accepted");
  }else{
    reftime = time1;
    trace0("rejected");
  }
  trace3("", _time1, _sim->_time0, reftime);
  
  // start with user time step
  TIME_t newtime(_time_by_user_request);
  TIME_t new_dt = newtime - reftime;
  STEP_CAUSE new_control = scUSER;
  check_consistency2();
  
  // event queue, events that absolutely will happen
  // exact time.  NOT ok to move or omit, even by _sim->_dtmin
  // some action is associated with it.
  // At this point, use it, don't pop,
  // in case this step is rejected or not used.
  // Pop happens in accept.
  if (!_sim->_eq.empty() && TIME_t(_sim->_eq.top()) < newtime) {
    newtime = TIME_t(_sim->_eq.top());
    new_dt = newtime - reftime;
    new_control = scEVENTQ;
    check_consistency2();
  }else{
  }
  
  const TIME_t fixed_time = newtime;
  
  // device events that may not happen
  // not sure of exact time.  will be rescheduled if wrong.
  // ok to move by _sim->_dtmin.  time is not that accurate anyway.
  if (TIME_t(_time_by_ambiguous_event) < newtime) {
    newtime = TIME_t(_time_by_ambiguous_event);
    new_dt = newtime - reftime;
    new_control = scAMBEVENT;
    check_consistency2();
  }else{
  }
  
  const TIME_t almost_fixed_time = newtime;
  
  // device error estimates
  if (TIME_t(_time_by_error_estimate) < newtime) {
    newtime = TIME_t(_time_by_error_estimate);
    new_dt = newtime - reftime;
    new_control = scTE;
    check_consistency();
  }else{
  }
  
  // skip, dtmax user parameter
  if (TIME_t(_dtmax) < new_dt) {
    new_dt = TIME_t(_dtmax);
    newtime = reftime + new_dt;
    new_control = scSKIP;
    check_consistency();
  }else{
  }
  
  TIME_t old_dt = time0 - time1;
  assert(old_dt >= TIME_t(0.));
  
  if (old_dt == TIME_t(0.)) {
    // initial step -- could be either t==0 or continue
    // for the first time, just guess
    // make it 100x smaller than expected
    if (TIME_t(std::max(_dtmax/100., _sim->_dtmin)) < new_dt) {
      new_dt = TIME_t(std::max(_dtmax/100., _sim->_dtmin));
      newtime = time0 + new_dt;
      new_control = scINITIAL;
    }else{
    }
  }else if (old_dt > TIME_t(0.)) {
    // Normal step.

    if (!_converged) {
      // convergence failure
      if (old_dt / OPT::trstepshrink < new_dt) {
	assert(!_accepted);
	new_dt = old_dt / OPT::trstepshrink;
	newtime = reftime + new_dt;
	new_control = scITER_R;
      }else{untested();
      }
    }else{
    }
    
    // converged but with more iterations than we like
    if (_sim->exceeds_iteration_limit(OPT::TRLOW)) {
      if (old_dt * OPT::trstephold < new_dt) {untested();
	assert(_accepted);
	new_dt = old_dt * OPT::trstephold;
	newtime = reftime + new_dt;
	new_control = scITER_A;
	check_consistency();
      }else{
      }
    }else{
    }
    
    // limit growth
    if (old_dt * OPT::trstepgrow < new_dt) {untested();
      new_dt = old_dt * OPT::trstepgrow;
      newtime = reftime + new_dt;
      new_control = scADT;
      check_consistency();
    }else{
    }
  }else{untested();
    unreachable();
    // Negative time.  Reserve for future work.
  }
  
  if (newtime > almost_fixed_time) {untested();
    unreachable();
  }else if (newtime == TIME_t(_time_by_user_request)) {
    assert(new_control == scUSER);
  }else if (newtime == fixed_time) {
    assert(new_control == scEVENTQ);
  }else if (newtime == almost_fixed_time) {
    assert(new_control == scAMBEVENT);
  }else if (newtime < time0) {
    // Clean-up mode.  Backing up.
    // Don't mess with time stepping.
    // It's messed up enough already.
  }else{
    // Smooth analog mode.
    // It is ok to move steps to make it smoother.
    assert(reftime == time0); // moving forward
    //assert(reftime >  time1);

    // If new_dt is close enough to old_dt to still mostly meet goals,
    // Use exactly old_dt again, to have a sequence of steps exactly the same size.
    if (up_order(old_dt*.8, new_dt, old_dt*1.5) && reftime + old_dt <= almost_fixed_time) {
      assert(reftime >  time1);
      if (new_control == scTE) {
	assert(reftime >  time1);
      }else if (new_control == scSKIP) {
	assert(reftime >  time1);
      }else{untested();
	assert(reftime >  time1);
      }
      new_dt = old_dt;
      newtime = reftime + new_dt;
      if (newtime > almost_fixed_time) {untested();
	unreachable();
      }else if (newtime == TIME_t(_time_by_user_request)) {
	new_control = scUSER;
      }else if (newtime + TIME_t(_sim->_dtmin) >= TIME_t(_time_by_user_request)) {
	newtime = TIME_t(_time_by_user_request);
	new_dt = newtime - reftime;
	new_control = scUSER;
      }else if (newtime == fixed_time) {
	new_control = scEVENTQ;
      }else if (newtime == almost_fixed_time) {
	new_control = scAMBEVENT;
      }else{
      }
      check_consistency();
    }else{
      // There will be a step change.
      // Try to choose one that we will keep for a while.
      // Choose new_dt to be in integer fraction of target_dt.
      // Try to adjust time stepping to minimize changes in time step,
      // removing minor changes that are supposedly irrelevant.
      // Solution is faster when time step stays the same over multiple steps.
      if (new_control == scTE) {
      }else if (new_control == scSKIP) {
      }else if (new_control == scINITIAL) {
      }else{untested();
      }
      TIME_t target_dt = fixed_time - reftime;
      assert(target_dt >= new_dt);
      double steps = ceil(target_dt.to_double() / new_dt.to_double());
      assert(steps > 0);
      new_dt = TIME_t(target_dt.to_double() / steps);
      newtime = TIME_t(reftime.to_double() + new_dt.to_double());
      check_consistency();
    }
  }
  
  set_step_cause(new_control);

  // trap time step too small
  if (new_dt < TIME_t(_sim->_dtmin)) {untested();
    new_dt = TIME_t(_sim->_dtmin);
    newtime = reftime + new_dt;
    new_control = scSMALL;
    check_consistency();
  }else{
  }

  assert(!_accepted || newtime > time0);
  assert(_accepted  || newtime < time0);
  
  
  
  /* got it, I think */
  
  /* check to be sure */
  if (newtime < time1 + TIME_t(_sim->_dtmin)) {untested();
    /* It's really bad. */
    /* Reject the most recent step, back up as much as possible, */
    /* and creep along */
    assert(!_accepted);
    assert(step_cause() < scREJECT);
    assert(step_cause() >= 0);
    error(bDANGER,"non-recoverable " + TR::step_cause[step_cause()] + "\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime, _sim->_time0, _time1, _time1 + _sim->_dtmin);
    newtime = time1 + TIME_t(_sim->_dtmin);
    set_step_cause(scSMALL);
    //check_consistency2();
    throw Exception("tried everything, still doesn't work, giving up");
  }else if (newtime < time0) {
    /* Reject the most recent step. */
    /* We have faith that it will work with a smaller time step. */
    assert(!_accepted);
    assert(newtime >= time1 + TIME_t(_sim->_dtmin));
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime.to_double(), _sim->_time0, _time1);
    set_step_cause(scREJECT);
    _sim->mark_inc_mode_bad();
    check_consistency2();
  }else if (newtime < time0 + TIME_t(_sim->_dtmin)) {untested();
    /* Another evaluation at the same time. */
    /* Keep the most recent step, but creep along. */
    assert(newtime > time0 - TIME_t(_sim->_dtmin));
    error(bDANGER, "zero time step\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e\n", newtime, _sim->_time0, _time1);
    if (_accepted) {untested();
      _time1 = _sim->_time0;
    }else{untested();
      assert(_converged);
    }
    check_consistency2();
    newtime = time0 + TIME_t(_sim->_dtmin);
    if (newtime > TIME_t(_time_by_user_request)) {untested();
      newtime = TIME_t(_time_by_user_request);
      set_step_cause(scUSER);
    }else{untested();
    }
    set_step_cause(scZERO);
    check_consistency2();
  }else{
    assert(_accepted);
    assert(newtime >= time0 + TIME_t(_sim->_dtmin));
    /* All is OK.  Moving on. */
    /* Keep value of newtime */
    _time1 = _sim->_time0;
    check_consistency2();
  }
  _sim->_time0 = newtime.to_double();

  check_consistency2();
  ++steps_total_;
  ::status.review.stop();
  trace0("next");
  //return (_sim->_time0 <= _tstop + _sim->_dtmin);
  return (newtime <= TIME_t(_tstop + _sim->_dtmin));
}
/*--------------------------------------------------------------------------*/
bool TRANSIENT::review()
{
  ::status.review.start();
  _sim->count_iterations(iTOTAL);

  TIME_PAIR time_by = _scope->tr_review();

#if 0
  // not ready for this yet.
  _time_by_ambiguous_event = TIME_t(time_by._event).to_double();
  _time_by_error_estimate  = TIME_t(time_by._error_estimate).to_double();
#else
  double mintime    = _time1       + 2*_sim->_dtmin;
  double rejecttime = _sim->_time0 - 2*_sim->_dtmin;
  double creeptime  = _sim->_time0 + 2*_sim->_dtmin;

  if (time_by._event < mintime) {
    _time_by_ambiguous_event = mintime;
  }else{
    _time_by_ambiguous_event = time_by._event;
  }
  if (up_order(rejecttime, _time_by_ambiguous_event, creeptime)) {
    _time_by_ambiguous_event = creeptime;
  }else{
  }

  rejecttime = _sim->_time0 - 1.1*_sim->_dtmin;
  creeptime  = _sim->_time0 + 1.1*_sim->_dtmin;
  if (time_by._error_estimate < mintime) {
    _time_by_error_estimate = mintime;
  }else{
    _time_by_error_estimate = time_by._error_estimate;
  }
  if (up_order(rejecttime, _time_by_error_estimate, creeptime)) {
    _time_by_error_estimate = creeptime;
  }else{
  }
#endif
  trace4("review", _time1, _sim->_time0, _time_by_ambiguous_event, _time_by_error_estimate);

  ::status.review.stop();

  return (_time_by_error_estimate > _sim->_time0  &&  _time_by_ambiguous_event > _sim->_time0);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept()
{
  ::status.accept.start();

  // advance event queue (maybe)
  // We already looked at it.  Dump what's on top if we took it.
  // This method of disposing of used _eq events will be changed in near future.
  // This block of code will be removed.
  while (!_sim->_eq.empty() && TIME_t(_sim->_eq.top()) <= TIME_t(_sim->_time0)) {
    assert(TIME_t(_sim->_eq.top()) == TIME_t(_sim->_time0));
    trace1("eq", _sim->_eq.top());
    _sim->_eq.pop();
  }
  bool pruned = false;
  while (!_sim->_eq.empty() && TIME_t(_sim->_eq.top()) < TIME_t(_sim->_time0 + _sim->_dtmin)) {untested();
    // should be unreachable
    // if it gets here, it's a //BUG//
    // near duplicate events in the queue.  overclocked?
    trace1("eq-prune", _sim->_eq.top());
    _sim->_eq.pop();
    pruned = true;
  }
  if(pruned){untested();
    // comment out to put devices under stress
    //_sim->_eq.push(_sim->_time0 + _sim->_dtmin);
    _sim->new_event(_sim->_time0 + _sim->_dtmin, this);
  }else{
  }

  _sim->set_limit();
  if (OPT::traceload) {
    while (!_sim->_acceptq.empty()) {
      _sim->_acceptq.back()->tr_accept();
      _sim->_acceptq.pop_back();
    }
  }else{untested();
    _sim->_acceptq.clear();
    _scope->tr_accept();
  }
  ++steps_accepted_;
  _sim->_has_op = s_TRAN;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::reject()
{
  ::status.accept.start();
  _sim->_acceptq.clear();
  ++steps_rejected_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
