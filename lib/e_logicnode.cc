/*$Id: e_logicnode.cc $ -*- C++ -*-
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "e_logicmod.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
LOGIC_NODE::LOGIC_NODE()
  :NODE(),
   _family(0),
   _d_iter(-1), // initially d_iter is older than a_iter
   _a_iter(0),
   _final_time(0),
   _lastchange(0),
   _old_lastchange(0),
   _mode(moANALOG),
   _lv(),
   _old_lv(),
   _quality(qBAD),
   _failure_mode("initial")
{
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "l{ogic} ")) {
    return annotated_logic_value();
  }else if (Umatch(x, "la{stchange} ")) {
    return _lastchange;
  }else if (Umatch(x, "fi{naltime} ")) {
    return final_time();
  }else if (Umatch(x, "di{ter} ")) {untested();
    return static_cast<double>(_d_iter);
  }else if (Umatch(x, "ai{ter} ")) {untested();
    return static_cast<double>(_a_iter);
  }else{
    return NODE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
/* annotated_logic_value:  a printable value for probe
 * that has secondary info encoded in its fraction part
 */
double LOGIC_NODE::annotated_logic_value()const
{
  return (_lv + (.1 * (OPT::transits - quality())) + (.01 * (2 - _mode)));
}
/*--------------------------------------------------------------------------*/
static bool newly_stable[lvUNKNOWN+1][lvUNKNOWN+1] = { // oldlv, _lv
  /*	   s0	  rise   fall	s1     u */
  /* s0 */{false, false, false, true,  false},
  /*rise*/{false, false, false, true,  false},
  /*fall*/{true,  false, false, false, false},
  /* s1 */{true,  false, false, false, false},
  /* u  */{true,  false, false, true,  false}
};
/*--------------------------------------------------------------------------*/
inline bool LOGIC_NODE::just_reached_stable()const
{
  return newly_stable[old_lv()][lv()];
}
/*--------------------------------------------------------------------------*/
/* to_logic: set up logic data for a node, if needed
 * If the logic data is already up to date, do nothing.
 * else set up: logic value (_lv) and quality.
 * Use and update _d_iter, _lastchange to keep track of what was done.
 */
void LOGIC_NODE::to_logic(const MODEL_LOGIC*f)
{
  assert(f);
  if (process() && process() != f) {untested();
    set_bad_quality("logic process mismatch");
    error(bWARNING, "node " + long_label() 
	  + " logic process mismatch\nis it " + process()->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  if (is_analog() &&  d_iter() < a_iter()) {
    if (_sim->analysis_is_restore()) {untested();
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
      set_last_change_time(0);
      store_old_last_change_time();
      set_lv(lvUNKNOWN);
    }else{
    }
    double dt = _sim->_time0 - last_change_time();
    if (dt < 0.) {untested();
      error(bPICKY, "time moving backwards.  was %g, now %g\n",
	    last_change_time(), _sim->_time0);
      dt = _sim->_time0 - old_last_change_time();
      if (dt <= 0.) {untested();
	throw Exception("internal error: time moving backwards, can't recover");
      }else{untested();
      }
      assert(dt > 0.);
      set_lv(old_lv());			/* skip back one */
    }else{
      store_old_last_change_time();
      store_old_lv();			/* save to see if it changes */
    }
    
    double sv = v0() / process()->range;	/* new scaled voltage */
    if (sv >= process()->th1) {		/* logic 1 */
      switch (lv()) {
      case lvSTABLE0: dont_set_quality("stable 0 to stable 1");	break;
      case lvRISING:  dont_set_quality("begin stable 1");	break;
      case lvFALLING:untested();set_bad_quality("falling to stable 1"); break;
      case lvSTABLE1: dont_set_quality("continuing stable 1");	break;
      case lvUNKNOWN: set_good_quality("initial 1");		break;
      }
      set_lv(lvSTABLE1);
    }else if (sv <= process()->th0) {	/* logic 0 */
      switch (lv()) {
      case lvSTABLE0: dont_set_quality("continuing stable 0");	break;
      case lvRISING: untested();set_bad_quality("rising to stable 0");	break;
      case lvFALLING: dont_set_quality("begin stable 0");	break;
      case lvSTABLE1: dont_set_quality("stable 1 to stable 0");	break;
      case lvUNKNOWN: set_good_quality("initial 0");		break;
      }
      set_lv(lvSTABLE0);
    }else{				/* transition region */
      double oldsv = vt1() / process()->range;/* old scaled voltage */
      double diff  = sv - oldsv;
      if (diff > 0) {	/* rising */
	switch (lv()) {
	case lvSTABLE0:
	  dont_set_quality("begin good rise");
	  break;
	case lvRISING:
	  if (diff < dt/(process()->mr * process()->rise)) {
	    set_bad_quality("slow rise");
	  }else{
	    dont_set_quality("continuing good rise");
	  }
	  break;
	case lvFALLING:
	  untested();
	  set_bad_quality("positive glitch in fall");
	  break;
	case lvSTABLE1:
	  untested();
	  set_bad_quality("negative glitch in 1");
	  break;
	case lvUNKNOWN:
	  set_bad_quality("initial rise");
	  break;
	}
	set_lv(lvRISING);
      }else if (diff < 0) {	/* falling */
	switch (lv()) {
	case lvSTABLE0:
	  untested();
	  set_bad_quality("positive glitch in 0");
	  break;
	case lvRISING:
	  set_bad_quality("negative glitch in rise");
	  break;
	case lvFALLING:
	  if (-diff < dt/(process()->mf * process()->fall)) {
	    set_bad_quality("slow fall");
	  }else{
	    dont_set_quality("continuing good fall");
	  }
	  break;
	case lvSTABLE1:
	  dont_set_quality("begin good fall");
	  break;
	case lvUNKNOWN:
	  untested();
	  set_bad_quality("initial fall");
	  break;
	}
	set_lv(lvFALLING);
      }else{				/* hanging up in transition */
	untested();
	error(bDANGER, "inflection???\n");
	set_bad_quality("in transition but no change");
	/* state (rise/fall)  unchanged */
      }
    }
    if (sv > 1.+process()->over || sv < -process()->over) {/* out of range */
      set_bad_quality("out of range");
    }
    if (just_reached_stable()) { /* A bad node gets a little better */
      improve_quality();	/* on every good transition.	   */
    }				/* Eventually, it is good enough.  */
				/* A good transition is defined as */
				/* entering a stable state from    */
				/* a transition state.		   */
    set_d_iter();
    set_last_change_time();
    trace3(_failure_mode.c_str(), _lastchange, _quality, _lv);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::to_analog(const MODEL_LOGIC* f)
{
  assert(f);
  if (process() && process() != f) {untested();
    error(bWARNING, "node " + long_label() 
	  + " logic process mismatch\nis it " + process()->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  double start = NOT_VALID;
  double end = NOT_VALID;
  double risefall = NOT_VALID;
  switch (lv()) {
  case lvSTABLE0:
    return process()->vmin;
  case lvRISING:
    start = process()->vmin;
    end = process()->vmax;
    risefall = process()->rise;
    break;
  case lvFALLING:
    start = process()->vmax;
    end = process()->vmin;
    risefall = process()->fall;
    break;
  case lvSTABLE1:
    return process()->vmax;
  case lvUNKNOWN:
    return process()->unknown;
  }
  assert(start != NOT_VALID);
  assert(end   != NOT_VALID);
  assert(risefall != NOT_VALID);

  if (_sim->_time0 <= (final_time()-risefall)) {
    return start;
  }else if (_sim->_time0 >= final_time()) {
    untested();
    return end;
  }else{
    return end - ((end-start) * (final_time()-_sim->_time0) / risefall);
  }
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::propagate()
{
  assert(in_transit());
  if (lv().is_rising()) {
    set_lv(lvSTABLE1);
  }else if (lv().is_falling()) {
    set_lv(lvSTABLE0);
  }else{
    // lv no change
  }
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
  assert(!(in_transit()));
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::unpropagate()
{
  set_final_time(last_change_time());
  set_last_change_time(old_last_change_time());
  set_lv(old_lv());
  set_d_iter();
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::force_initial_value(LOGICVAL v)
{
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{untested();
  }
  assert(_sim->analysis_is_static() || _sim->analysis_is_restore());
  assert(_sim->_time0 == 0.);
  assert(is_unknown());
  assert(is_digital());
  set_lv(v); // BUG ??
  set_good_quality("initial dc");
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::set_event(double delay, LOGICVAL v)
{
  _lv.set_in_transition(v);
  if (_sim->analysis_is_tran_dynamic()  &&  in_transit()) {
    set_bad_quality("race");
  }else{
    // normal good quality event
    // leaving quality as it was
  }
  set_d_iter();
  set_final_time(_sim->_time0 + delay);
  if (OPT::picky <= bTRACE) {
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
