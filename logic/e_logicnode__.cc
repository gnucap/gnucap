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
 * legacy logic node
 */
#include "e_logicmod.h"
#include "e_logicnode.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC;
#define	qBAD	 (0)
#define qGOOD	 (OPT::transits)
/*--------------------------------------------------------------------------*/
class LOGIC_NODE : public LOGIC_NODE_BASE {
  const MODEL_LOGIC *_family{nullptr};	/* logic family */
  int	      _quality{qBAD};		/* quality of digital mode */
  std::string _failure_mode{"initial"};
public:
  LOGIC_NODE(int i=INVALID_NODE) : LOGIC_NODE_BASE(i) {}
private:
  LOGIC_NODE(LOGIC_NODE const&p) : LOGIC_NODE_BASE(p) {}
  LOGIC_NODE_BASE* clone()const override {return new LOGIC_NODE(*this);}
public: // CARD
  void tr_begin()override;
  double tr_probe_num(const std::string& x)const override;
public:
  int quality()const {return _quality;}
  const std::string& failure_mode()const {return _failure_mode;}
private: // internal
  void	set_quality(int q)		{_quality = q;}
  void	set_failure_mode(const std::string& f) {_failure_mode = f;}
  void	set_process(const MODEL_LOGIC* f) {_family = f;}
  const MODEL_LOGIC* process()const	{return _family;}

private: // calculated data access (rvalues)
  bool	just_reached_stable()const;
  double annotated_logic_value()const;

public: // action, used by logic
  double      to_analog(const MODEL_LOGIC*f) const;
  void	      to_logic(const MODEL_LOGIC*f, double input);
  void	      to_logic(const MODEL_LOGIC*f){itested();
    to_logic(f, v0());
  }
  void	      set_event(double delay, LOGICVAL v, CARD* c=nullptr);

public: // other internal
  void  set_bad_quality(const std::string& f) {
    set_quality(qBAD);
    set_failure_mode(f);
  }
  void  set_good_quality(const std::string& f = "ok") {
    set_quality(qGOOD);
    set_failure_mode(f);
  }
  void	dont_set_quality(const std::string& f = "don't know") {
    set_failure_mode(f);
  }
  void	improve_quality() {
    if (quality() < qGOOD) {
      ++_quality;
    }
  }

}logic;
DISPATCHER<NODE>::INSTALL d3(&node_dispatcher, "__logic|__hybrid", &logic);
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::tr_begin()
{
  LOGIC_NODE_BASE::tr_begin();
  _family = nullptr;
  if(is_digital()){
    _quality = qGOOD;
  }else{
    _quality = qBAD;
    _failure_mode = "initial";
  }
  _mode = moANALOG;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static bool newly_stable[lvXX+1][lvXX+1] = { // oldlv, _lv
  /*	   s0	  rise   fall	s1     u */
  /* s0 */{false, false, false, true,  false},
  /*rise*/{false, false, false, true,  false},
  /*fall*/{true,  false, false, false, false},
  /* s1 */{true,  false, false, false, false},

  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},

  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},

  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false},
  /* u  */{true,  false, false, true,  false}
};
/*--------------------------------------------------------------------------*/
inline bool LOGIC_NODE::just_reached_stable() const
{
  return newly_stable[old_lv()][lv()];
}
/*--------------------------------------------------------------------------*/
/* to_logic: set up logic data for a node, if needed
 * If the logic data is already up to date, do nothing.
 * else set up: logic value (_lv, using set_lv()) and quality.
 * Use and update _d_iter, _lastchange to keep track of what was done.
 */
void LOGIC_NODE::to_logic(const MODEL_LOGIC*f, double input)
{
  if (is_analog()){
    set_a_iter();
  }else{
  }
  assert(f);
  if (process() && process() != f) {untested();
    set_bad_quality("logic process mismatch");
    error(bWARNING, "node " + long_label()
	  + " logic process mismatch\nis it " + process()->long_label()
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  if (is_analog() &&  d_iter() < a_iter()) {
    if (_sim->analysis_is_restore()) {
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
      set_last_change_time(0);
      store_old_last_change_time();
      set_lv(lvXX);
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

    double sv = input / process()->range;	/* new scaled voltage */
    if (sv >= process()->th1) {		/* logic 1 */
      switch (lv()) {
      case lv00: dont_set_quality("stable 0 to stable 1");	break;
      case lv01:  dont_set_quality("begin stable 1");	break;
      case lv10:untested();set_bad_quality("falling to stable 1"); break;
      case lv11: dont_set_quality("continuing stable 1");	break;
      case lv0Z: untested(); // fall-through
      case lv0X: untested(); // fall-through
      case lv1Z: untested(); // fall-through
      case lv1X: untested(); // fall-through
      case lvZ0: untested(); // fall-through
      case lvX0: untested(); // fall-through
      case lvZ1: untested(); // fall-through
      case lvX1: untested(); // fall-through
      case lvZZ: untested(); // fall-through
      case lvZX: untested(); // fall-through
      case lvXZ: untested(); // fall-through
      case lvXX: set_good_quality("initial 1");		break;
      }
      set_lv(lv11);
    }else if (sv <= process()->th0) {	/* logic 0 */
      switch (lv()) {
      case lv00: dont_set_quality("continuing stable 0");	break;
      case lv01: untested();set_bad_quality("rising to stable 0");	break;
      case lv10: dont_set_quality("begin stable 0");	break;
      case lv11: dont_set_quality("stable 1 to stable 0");	break;
      case lv0Z: untested(); // fall-through
      case lv0X: untested(); // fall-through
      case lv1Z: untested(); // fall-through
      case lv1X: untested(); // fall-through
      case lvZ0: untested(); // fall-through
      case lvX0: untested(); // fall-through
      case lvZ1: untested(); // fall-through
      case lvX1: untested(); // fall-through
      case lvZZ: untested(); // fall-through
      case lvZX: untested(); // fall-through
      case lvXZ: untested(); // fall-through
      case lvXX: set_good_quality("initial 0");		break;
      }
      set_lv(lv00);
    }else{				/* transition region */
      double oldsv = vt1() / process()->range;/* old scaled voltage */
      double diff  = sv - oldsv;
      if (diff > 0) {	/* rising */
	switch (lv()) {
	case lv00:
	  dont_set_quality("begin good rise");
	  break;
        case lvZ1: untested(); // fall-through
        case lvX1: untested(); // fall-through
	case lv01:
	  if (diff < dt/(process()->mr * process()->rise)) {
	    set_bad_quality("slow rise");
	  }else{
	    dont_set_quality("continuing good rise");
	  }
	  break;
        case lvZ0: untested(); // fall-through
        case lvX0: untested(); // fall-through
	case lv10:untested();
	  set_bad_quality("positive glitch in fall");
	  break;
	case lv11:untested();
	  set_bad_quality("negative glitch in 1");
	  break;
        case lv0Z: untested(); // fall-through
        case lv0X: untested(); // fall-through
        case lv1Z: untested(); // fall-through
        case lv1X: untested(); // fall-through
        case lvZZ: untested(); // fall-through
        case lvZX: untested(); // fall-through
        case lvXZ: untested(); // fall-through
	case lvXX:
	  set_bad_quality("initial rise");
	  break;
	}
	set_lv(lv01);
      }else if (diff < 0) {	/* falling */
	switch (lv()) {
	case lv00:untested();
	  untested();
	  set_bad_quality("positive glitch in 0");
	  break;
        case lvZ1: untested(); // fall-through
        case lvX1: untested(); // fall-through
	case lv01:
	  set_bad_quality("negative glitch in rise");
	  break;
        case lvZ0: untested(); // fall-through
        case lvX0: untested(); // fall-through
	case lv10:
	  if (-diff < dt/(process()->mf * process()->fall)) {
	    set_bad_quality("slow fall");
	  }else{
	    dont_set_quality("continuing good fall");
	  }
	  break;
	case lv11:
	  dont_set_quality("begin good fall");
	  break;
        case lv0Z: untested(); // fall-through
        case lv0X: untested(); // fall-through
        case lv1Z: untested(); // fall-through
        case lv1X: untested(); // fall-through
        case lvZZ: untested(); // fall-through
        case lvZX: untested(); // fall-through
        case lvXZ: untested(); // fall-through
	case lvXX:untested();
	  set_bad_quality("initial fall");
	  break;
	}
	set_lv(lv10);
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
    // trace3(_failure_mode.c_str(), _lastchange, _quality, _lv);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::to_analog(const MODEL_LOGIC* f)const
{
  assert(f);
  if (process() && process() != f) {untested();
    error(bWARNING, "node " + long_label()
	  + " logic process mismatch\nis it " + process()->long_label()
	  + " or " + f->long_label() + "?\n");
  }
  // set_process(f);

  double start = NOT_VALID;
  double end = NOT_VALID;
  double risefall = NOT_VALID;
  switch (lv()) {
  case lv00:
    return process()->vmin;
  case lvZ1: untested(); // fall-through
  case lvX1: // fall-through
  case lv01:
    start = process()->vmin;
    end = process()->vmax;
    risefall = process()->rise;
    break;
  case lvZ0: untested(); // fall-through
  case lvX0: // fall-through
  case lv10:
    start = process()->vmax;
    end = process()->vmin;
    risefall = process()->fall;
    break;
  case lv11:
    return process()->vmax;
  case lv0Z: untested(); // fall-through
  case lv0X: untested(); // fall-through
  case lv1Z: untested(); // fall-through
  case lv1X: untested(); // fall-through
  case lvZZ: untested(); // fall-through
  case lvZX: untested(); // fall-through
  case lvXZ: untested(); // fall-through
  case lvXX:
    return process()->unknown;
  }
  assert(start != NOT_VALID);
  assert(end   != NOT_VALID);
  assert(risefall != NOT_VALID);

  if (_sim->_time0 <= (final_time()-risefall)) {
    return start;
  }else if (_sim->_time0 >= final_time()) { untested();
    return end;
  }else{
    return end - ((end-start) * (final_time()-_sim->_time0) / risefall);
  }
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::set_event(double delay, LOGICVAL v, CARD* Device)
{
  if (Device){
  }else{ itested();
  }
  _lv.set_in_transition(v);
  if (_sim->analysis_is_tran_dynamic()  &&  in_transit()) {
    set_bad_quality("race");
  }else{
    // normal good quality event
    // leaving quality as it was
  }
  set_d_iter();
  double ft = CKT_BASE::_sim->new_event(_sim->_time0 + delay, Device?:this);
  trace4("set_event", _sim->_time0, delay, ft, _sim->_dtmin);
  set_final_time(ft);
  if (OPT::picky <= bTRACE) {
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  set_last_change_time();
  if(!subckt()){
  }else if(is_cm_hack()){ untested();
    subckt()->tr_accept();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* annotated_logic_value:  a printable value for probe
 * that has secondary info encoded in its fraction part
 */
inline double LOGIC_NODE::annotated_logic_value()const
{
  return (double(_lv) + (.1 * (OPT::transits - quality())) + (.01 * (2 - _mode)));
}
/*--------------------------------------------------------------------------*/
inline double LOGIC_NODE::tr_probe_num(const std::string& x) const
{
  if (Umatch(x, "l{ogic} ")) {
    return annotated_logic_value();
  }else{
    return LOGIC_NODE_BASE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
static LOGIC_NODE logic_ground_(0);
/*--------------------------------------------------------------------------*/
struct node_l : node_t {
  LOGIC_NODE*       operator->()      { return &data(); }
  LOGIC_NODE const* operator->()const { return &data(); }
private:
  LOGIC_NODE& data() const {
    if(n_()->m_() == 0){
      return logic_ground_;
    }else{
    }
    auto n = prechecked_cast<LOGIC_NODE const*>(n_());
    assert(n);
    return *const_cast<LOGIC_NODE*>(n);
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
