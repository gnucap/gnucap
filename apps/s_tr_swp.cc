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
//testing=hitcount 2024.10.01
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
  double _t {0}; // used as a signed integer.  keeping double for overflow.
  explicit TIME_t(int, const double& T) : _t(T) {assert(std::floor(T)==std::ceil(T));}
public:
  explicit TIME_t() : _t(0) {}
	   TIME_t(const TIME_t& T) : _t(T._t) {}
  explicit TIME_t(const double& T) : _t(std::round(T/_dtmin)) {}
  explicit TIME_t(const int64_t& T) : _t(double(T)) {untested();}

  TIME_t& operator=(const TIME_t& T) {_t = T._t; return *this;}

  TIME_t operator+(const TIME_t& B)const {return TIME_t(0,_t + B._t);}
  TIME_t operator-(const TIME_t& B)const {return TIME_t(0,_t - B._t);}

  TIME_t operator*(const double& B)const {return TIME_t(0,std::round(_t * B));}
  TIME_t operator/(const double& B)const {return TIME_t(0,std::round(_t / B));}

  TIME_t operator*(const int64_t& B)const {return TIME_t(0,(_t * double(B)));}
  TIME_t operator/(const int64_t& B)const {return TIME_t(0,std::floor(_t / double(B)));}

  bool   operator>(const TIME_t& B)const {return (_t > B._t);}
  bool   operator>=(const TIME_t& B)const {return (_t >= B._t);}
  bool   operator<(const TIME_t& B)const {return (_t < B._t);}
  bool   operator<=(const TIME_t& B)const {return (_t <= B._t);}
  bool   operator==(const TIME_t& B)const {return (_t == B._t);}

  double to_double()const {return _t*_dtmin;}
  int64_t ticks()const	  {return int64_t(_t);}

  static void set_dtmin(double Dtmin) {_dtmin = Dtmin;}
};
double TIME_t::_dtmin {OPT::dtmin};
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep()
{//321
  _sim->_phase = p_INIT_DC;
  head(_tstart, _tstop, "Time");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  TIME_t::set_dtmin(_sim->_dtmin);
  
  if (_cont) {//29	// use the data from last time
    // keep event queue contents
    assert(_sim->_eq.empty() || _sim->_eq.top() >= _sim->_time0);
    _sim->_phase = p_RESTORE;
    _sim->restore_voltages();
    _scope->tr_restore();
  }else{//292
    while (!_sim->_eq.empty()) {untested();
      _sim->_eq.pop();
    }
    _sim->clear_limit();
    _scope->tr_begin();
  }
  
  first();
  _sim->_genout = gen();
  
  if (_sim->uic_now()) {//12
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
  }else{//309
    _converged = solve_with_homotopy(OPT::DCBIAS,_trace);
    if (!_converged) {//3
      error(bWARNING, "did not converge\n");
    }else{//306
    }
  }
  review(); 
  _accepted = true;
  accept();
  
  {//321
    bool printnow = (_sim->_time0 == _tstart || _trace >= tALLTIME);
    int outflags = ofNONE;
    if (printnow) {//320
      outflags = ofPRINT | ofSTORE | ofKEEP;
    }else{//1
      outflags = ofSTORE;
    }
    outdata(_sim->_time0, outflags);
  }
  
  while (next()) {//43128
    _sim->_bypass_ok = false;
    _sim->_phase = p_TRAN;
    _sim->_genout = gen();
    _converged = solve(OPT::TRHIGH,_trace);

    _accepted = _converged && review();

    if (_accepted) {//42421
      assert(_converged);
      assert(_sim->_time0 <= _time_by_user_request + _sim->_dtmin);
      accept();
      if (step_cause() == scUSER) {//27333
	assert(up_order(_sim->_time0-_sim->_dtmin, _time_by_user_request, _sim->_time0+_sim->_dtmin));
	++_stepno;
	_time_by_user_request += _tstrobe;	/* advance user time */
      }else{//15088
      }
      assert(_sim->_time0 < _time_by_user_request);
    }else{//707
      reject();
      assert(_time1 < _time_by_user_request);
    }
    {//43128
      bool printnow =
	(_trace >= tREJECTED)
	|| (_accepted && (_trace >= tALLTIME
			  || step_cause() == scUSER
			  || (!_tstrobe.has_hard_value() && _sim->_time0+_sim->_dtmin > _tstart)));
      int outflags = ofNONE;
      if (printnow) {//32959
	outflags = ofPRINT | ofSTORE | ofKEEP;
      }else if (_accepted) {//9551
	outflags = ofSTORE;
      }else{//618
      }
      outdata(_sim->_time0, outflags);
    }
    
    if (!_converged && OPT::quitconvfail) {untested();
      outdata(_sim->_time0, ofPRINT);
      throw Exception("convergence failure, giving up");
    }else{//43128
    }
  }
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE C)
{//44477
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
{//232823
  return ::status.control;
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first()
{//321
  /* usually, _sim->_time0, time1 == 0, from setup */
  assert(_sim->_time0 == _time1);
  assert(_sim->_time0 <= _tstart);
  ::status.review.start();

  _stepno = 0;

  //_time_by_user_request = _sim->_time0 + _tstrobe;	/* set next user step */
  //set_step_cause(scUSER);

  if (_sim->_time0 < _tstart) {//2		// skip until _tstart
    set_step_cause(scINITIAL);				// suppressed 
    _time_by_user_request = _tstart;			// set first strobe
  }else{//319					// no skip
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
    assert(newtime == reftime + new_dt);				\
    assert(newtime <= TIME_t(_time_by_user_request + _sim->_dtmin));	\
    assert(newtime <= TIME_t(_time_by_user_request));			\
    assert(new_dt >= TIME_t(0.));					\
    assert(new_dt > TIME_t(0.));					\
    assert(new_dt >= TIME_t(_sim->_dtmin));				\
  }
#define check_consistency2() {						\
    trace4("", __LINE__, newtime.to_double(), reftime.to_double(), new_dt.to_double()); \
    assert(newtime > time1);						\
    assert(newtime > reftime);						\
    assert(newtime == reftime + new_dt);				\
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
{//43449
  ::status.review.start();

  TIME_t time1(_time1);
  TIME_t time0(_sim->_time0);
  
  TIME_t reftime;
  if (_accepted) {//42742
    reftime = time0;
    trace0("accepted");
  }else{//707
    reftime = time1;
    trace0("rejected");
  }
  trace3("", _time1, _sim->_time0, reftime.to_double());
  
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
  if (!_sim->_eq.empty() && TIME_t(_sim->_eq.top()) < newtime) {//338
    newtime = TIME_t(_sim->_eq.top());
    new_dt = newtime - reftime;
    new_control = scEVENTQ;
    check_consistency2();
  }else{//43111
  }
  
  const TIME_t fixed_time = newtime;
  
  // device events that may not happen
  // not sure of exact time.  will be rescheduled if wrong.
  // ok to move by _sim->_dtmin.  time is not that accurate anyway.
  if (TIME_t(_time_by_ambiguous_event) < newtime) {//3079
    newtime = TIME_t(_time_by_ambiguous_event);
    new_dt = newtime - reftime;
    new_control = scAMBEVENT;
    check_consistency2();
  }else{//40370
  }
  
  const TIME_t almost_fixed_time = newtime;
  check_consistency();
  
  // device error estimates
  if (TIME_t(_time_by_error_estimate) < newtime) {//12904
    newtime = TIME_t(_time_by_error_estimate);
    new_dt = newtime - reftime;
    new_control = scTE;
    check_consistency();
  }else{//30545
  }
  
  // skip, dtmax user parameter
  if (TIME_t(_dtmax) < new_dt) {//1169
    new_dt = TIME_t(_dtmax);
    newtime = reftime + new_dt;
    new_control = scSKIP;
    check_consistency();
  }else{//42280
  }
  
  TIME_t old_dt = time0 - time1;
  assert(old_dt >= TIME_t(0.));
  
  if (old_dt == TIME_t(0.)) {//321
    // initial step -- could be either t==0 or continue
    // for the first time, just guess
    // make it 100x smaller than expected
    if (TIME_t(std::max(_dtmax/100., _sim->_dtmin)) < new_dt) {//298
      new_dt = TIME_t(std::max(_dtmax/100., _sim->_dtmin));
      newtime = time0 + new_dt;
      new_control = scINITIAL;
    }else{//23
    }
  }else if (old_dt > TIME_t(0.)) {//43128
    // Normal step.

    if (!_converged) {//31
      // convergence failure
      if (old_dt / OPT::trstepshrink < new_dt) {//31
	assert(!_accepted);
	new_dt = old_dt / OPT::trstepshrink;
	newtime = reftime + new_dt;
	new_control = scITER_R;
      }else{untested();
      }
    }else{//43097
    }
    
    // converged but with more iterations than we like
    if (_sim->exceeds_iteration_limit(OPT::TRLOW)) {//1050
      if (old_dt * OPT::trstephold < new_dt) {untested();
	assert(_accepted);
	new_dt = old_dt * OPT::trstephold;
	newtime = reftime + new_dt;
	new_control = scITER_A;
	check_consistency();
      }else{//1050
      }
    }else{//42078
    }
    
    // limit growth
    if (old_dt * OPT::trstepgrow < new_dt) {untested();
      new_dt = old_dt * OPT::trstepgrow;
      newtime = reftime + new_dt;
      new_control = scADT;
      check_consistency();
    }else{//43128
    }
  }else{untested();
    unreachable();
    // Negative time.  Reserve for future work.
  }
  
  if (newtime > almost_fixed_time) {untested();
    unreachable();
  }else if (newtime == TIME_t(_time_by_user_request)) {//27273
    assert(new_control == scUSER);
  }else if (newtime == fixed_time) {//274
    assert(new_control == scEVENTQ);
  }else if (newtime == almost_fixed_time) {//1897
    assert(new_control == scAMBEVENT);
  }else if (newtime < time0) {//658
    // Clean-up mode.  Backing up.
    // Don't mess with time stepping.
    // It's messed up enough already.
  }else{//13347
    // Smooth analog mode.
    // It is ok to move steps to make it smoother.
    assert(reftime == time0); // moving forward

    // If new_dt is close enough to old_dt to still mostly meet goals,
    // Use exactly old_dt again, to have a sequence of steps exactly the same size.
    if (up_order(old_dt*.8, new_dt, old_dt*1.5) && reftime + old_dt <= almost_fixed_time) {//8083
      if (new_control == scTE) {//6983
      }else if (new_control == scSKIP) {//1100
      }else{untested();
      }
      if (new_dt == old_dt) {//442
      }else{//7641
      }
      new_dt = old_dt;
      newtime = reftime + new_dt;
      if (newtime > almost_fixed_time) {untested();
	unreachable();
      }else if (newtime == TIME_t(_time_by_user_request)) {//433
	new_control = scUSER;
      }else if (newtime == fixed_time) {//2
	new_control = scEVENTQ;
      }else if (newtime == almost_fixed_time) {//4
	new_control = scAMBEVENT;
      }else{//7644
      }
      check_consistency();
    }else{//5264
      // There will be a step change.
      // Try to choose one that we will keep for a while.
      // Choose new_dt to be in integer fraction of target_dt.
      // Try to adjust time stepping to minimize changes in time step,
      // removing minor changes that are supposedly irrelevant.
      // Solution is faster when time step stays the same over multiple steps.
      if (new_control == scTE) {//4910
      }else if (new_control == scSKIP) {//56
      }else if (new_control == scINITIAL) {//298
      }else{untested();
      }
      TIME_t target_dt = fixed_time - reftime;
      assert(target_dt >= new_dt);

      //double steps = ceil(target_dt.to_double() / new_dt.to_double());
      //new_dt = TIME_t(target_dt.to_double() / steps);
      //newtime = TIME_t(reftime.to_double() + new_dt.to_double());

      int64_t steps = target_dt.ticks() / new_dt.ticks();
      TIME_t try_dt;
      if (new_dt * steps == target_dt) {//386
	// exact.  keep new_dt, no change
      }else if (((try_dt = target_dt / (steps+1)) * (steps+1)) == target_dt) {//2263
	// add 1 step, now exact.
	new_dt = try_dt;
      }else if (((try_dt = target_dt / (steps+2)) * (steps+2)) == target_dt) {//917
	// add 2 steps, now exact.
	new_dt = try_dt;
      }else{//1698
	// it won't be exact.
	// always short, bump dt, now always goes past.
	try_dt = target_dt / (steps+1) + TIME_t(_sim->_dtmin);
	if (try_dt == new_dt) {//477
	}else if (try_dt < new_dt) {//1221
	}else{untested();
	}
	assert(try_dt * (steps+1) > target_dt);
	new_dt = try_dt;
      }
      assert(new_dt == TIME_t(new_dt.to_double()));
      newtime = reftime + new_dt;
      check_consistency();
    }
  }
  
  set_step_cause(new_control);

  // trap time step too small
  if (new_dt < TIME_t(_sim->_dtmin)) {untested();
    unreachable();
    error(bDANGER,"non-recoverable " + TR::step_cause[step_cause()] + "\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime.to_double(), _sim->_time0, _time1, _time1 + _sim->_dtmin);
    new_dt = TIME_t(_sim->_dtmin);
    newtime = reftime + new_dt;
    _sim->mark_inc_mode_bad();
    set_step_cause(scSMALL);
    check_consistency();
  }else{//43449
  }
  
  // trap exact duplicate time
  if (newtime == time0) {untested();
    unreachable();
    error(bDANGER, "zero time step\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime.to_double(), _sim->_time0, _time1);
    if (_accepted) {untested();
      _time1 = _sim->_time0;
    }else{untested();
      assert(_converged);
    }
    new_dt = TIME_t(_sim->_dtmin);
    newtime = time0 + new_dt;
    _sim->mark_inc_mode_bad();
    set_step_cause(scZERO);
    check_consistency();
  }else{//43449
  }

  assert(!_accepted || newtime > time0);
  assert(_accepted  || newtime < time0);

  if (newtime < time0) {//707
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime.to_double(), _sim->_time0, _time1);
    assert(reftime == time1);
    _sim->mark_inc_mode_bad();
    set_step_cause(scREJECT);
  }else{//42742
    assert(_accepted);
    assert(newtime > time0);
    assert(reftime == time0);
    // All is OK.  Moving on.
  }

  check_consistency();
  _time1 = reftime.to_double();
  _sim->_time0 = newtime.to_double();

  ++steps_total_;
  ::status.review.stop();
  trace0("next");
  return (newtime <= TIME_t(_tstop + _sim->_dtmin));
}
/*--------------------------------------------------------------------------*/
bool TRANSIENT::review()
{//43418
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

  if (time_by._event < mintime) {//99
    _time_by_ambiguous_event = mintime;
  }else{//43319
    _time_by_ambiguous_event = time_by._event;
  }
  if (up_order(rejecttime, _time_by_ambiguous_event, creeptime)) {//234
    _time_by_ambiguous_event = creeptime;
  }else{//43184
  }

  rejecttime = _sim->_time0 - 1.1*_sim->_dtmin;
  creeptime  = _sim->_time0 + 1.1*_sim->_dtmin;
  if (time_by._error_estimate < mintime) {//24
    _time_by_error_estimate = mintime;
  }else{//43394
    _time_by_error_estimate = time_by._error_estimate;
  }
  if (up_order(rejecttime, _time_by_error_estimate, creeptime)) {//25
    _time_by_error_estimate = creeptime;
  }else{//43393
  }
#endif
  trace4("review", _time1, _sim->_time0, _time_by_ambiguous_event, _time_by_error_estimate);

  ::status.review.stop();

  return (_time_by_error_estimate > _sim->_time0  &&  _time_by_ambiguous_event > _sim->_time0);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept()
{//42742
  ::status.accept.start();

  // advance event queue (maybe)
  // We already looked at it.  Dump what's on top if we took it.
  // This method of disposing of used _eq events will be changed in near future.
  // This block of code will be removed.
  while (!_sim->_eq.empty() && TIME_t(_sim->_eq.top()) <= TIME_t(_sim->_time0)) {//272
    assert(TIME_t(_sim->_eq.top()) == TIME_t(_sim->_time0));
    trace1("eq", _sim->_eq.top());
    _sim->_eq.pop();
  }

  _sim->set_limit();
  if (OPT::traceload) {//42742
    while (!_sim->_acceptq.empty()) {//13857
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
{//707
  ::status.accept.start();
  _sim->_acceptq.clear();
  ++steps_rejected_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
