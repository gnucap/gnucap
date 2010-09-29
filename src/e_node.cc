/*$Id: e_node.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "d_subckt.h"
#include "u_nodemap.h"
#include "d_logic.h"
#include "e_aux.h"
#include "u_xprobe.h"
//#include "e_node.h"
/*--------------------------------------------------------------------------*/
double volts_limited(const node_t & n1, const node_t & n2);
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::or_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvRISING,  lvRISING,  lvRISING,  lvSTABLE1, lvRISING},
  {lvFALLING, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvSTABLE1, lvSTABLE1, lvSTABLE1, lvSTABLE1, lvSTABLE1},
  {lvUNKNOWN, lvRISING,  lvUNKNOWN, lvSTABLE1, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::xor_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvRISING,  lvFALLING, lvRISING,  lvFALLING, lvUNKNOWN},
  {lvFALLING, lvRISING,  lvFALLING, lvRISING,  lvUNKNOWN},
  {lvSTABLE1, lvFALLING, lvRISING,  lvSTABLE0, lvUNKNOWN},
  {lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::and_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvSTABLE0, lvSTABLE0, lvSTABLE0, lvSTABLE0},
  {lvSTABLE0, lvRISING,  lvFALLING, lvRISING,  lvUNKNOWN},
  {lvSTABLE0, lvFALLING, lvFALLING, lvFALLING, lvFALLING},
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvSTABLE0, lvUNKNOWN, lvFALLING, lvUNKNOWN, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::not_truth[lvNUM_STATES] = {
  lvSTABLE1, lvFALLING, lvRISING,  lvSTABLE0, lvUNKNOWN  
};
/*--------------------------------------------------------------------------*/
static _LOGICVAL prop_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN},
  {lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN},
  {lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvSTABLE1, lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
inline LOGICVAL& LOGICVAL::set_in_transition(LOGICVAL newval)
{
  _lv = prop_truth[_lv][newval];
  assert(_lv != lvUNKNOWN);
  return *this;
}
/*--------------------------------------------------------------------------*/
LOGIC_NODE::LOGIC_NODE()
  :NODE(),
   _family(0),
   _d_iter(0),
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
/* default constructor : unconnected, don't use
 */
NODE::NODE()
  :CKT_BASE(),
   _user_number(INVALID_NODE)
   //_flat_number(INVALID_NODE),
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
/* copy constructor : user data only
 */
NODE::NODE(const NODE& p)
  :CKT_BASE(p),
   _user_number(p._user_number)
   //_flat_number(INVALID_NODE),
   //_matrix_number(INVALID_NODE)
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 */
NODE::NODE(const NODE* p)
  :CKT_BASE(*p),
   _user_number(p->_user_number)
   //_flat_number(INVALID_NODE),
   //_matrix_number(INVALID_NODE)
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* usual initializing constructor : name and index
 */
NODE::NODE(const std::string& s, int n)
  :CKT_BASE(s),
   _user_number(n)
   //_flat_number(INVALID_NODE),
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
node_t::node_t()
  :_m(INVALID_NODE),
   _t(INVALID_NODE),
   _n(0)
{
}
node_t::node_t(const node_t& p)
  :_m(p._m),
   _t(p._t),
   _n(p._n)
{
}
node_t::node_t(NODE* n)
  :_m(INVALID_NODE),
   _t(INVALID_NODE),
   _n(n)
{
  _t = n->user_number();
  map();
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(CS& cmd)const
{
  int here = cmd.cursor();
  if (cmd.pmatch("V")) {
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (cmd.pmatch("Z")) {
    return port_impedance(node_t(const_cast<NODE*>(this)),
			  node_t(&ground_node), lu, 0.);
  }else if (ONE_OF
	    || cmd.pmatch("Logic")
	    || cmd.pmatch("LAstchange")
	    || cmd.pmatch("FInaltime")
	    || cmd.pmatch("DIter")
	    || cmd.pmatch("AIter")
	    || cmd.pmatch("COUNT")
	    ) {
    extern LOGIC_NODE* nstat;
    assert(nstat);
    cmd.reset(here);
    return nstat[matrix_number()].tr_probe_num(cmd);
#if 0
  }else if (cmd.pmatch("MDY")) {
    untested();
    // matrix diagonal admittance
    return aa.d(m_(),m_());
  }else if (cmd.pmatch("MDZ")) {
    untested();
    // matrix diagonal impedance
    return 1/aa.d(m_(),m_());
#endif
  }else if (cmd.pmatch("ZERO")) {
    untested();
    // fake probe: 0.0
    return 0.0;
  }else if (cmd.pmatch("PDZ")) {
    untested();
    // fake probe 1/0 .. positive divide by zero = Infinity
    CS x1("ZERO");
    double z1 = tr_probe_num(x1);
    return 1.0/z1;
  }else if (cmd.pmatch("NDZ")) {
    untested();
    // fake probe -1/0 .. negative divide by zero = -Infinity
    CS x1("ZERO");
    double z1 = tr_probe_num(x1);
    return -1.0/z1;
  }else if (cmd.pmatch("NAN")) {
    untested();
    // fake probe 0/0 = NaN
    CS x1("ZERO");
    CS x2("ZERO");
    double z1 = tr_probe_num(x1);
    double z2 = tr_probe_num(x2);
    return z1/z2;
  }else{
    untested();
    return CKT_BASE::tr_probe_num(cmd);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::tr_probe_num(CS& cmd)const
{
  if (cmd.pmatch("Logic")) {
    return annotated_logic_value();
  }else if (cmd.pmatch("LAstchange")) {
    untested();
    return _lastchange;
  }else if (cmd.pmatch("FInaltime")) {
    untested();
    return final_time();
  }else if (cmd.pmatch("DIter")) {
    untested();
    return static_cast<double>(_d_iter);
  }else if (cmd.pmatch("AIter")) {
    untested();
    return static_cast<double>(_a_iter);
  }else{
    return NODE::tr_probe_num(cmd);
  }
}
/*--------------------------------------------------------------------------*/
XPROBE NODE::ac_probe_ext(CS& cmd)const
{
  if (cmd.pmatch("V")) {
    return XPROBE(vac());
  }else if (cmd.pmatch("Z")) {
    return XPROBE(port_impedance(node_t(const_cast<NODE*>(this)),
				 node_t(&ground_node), acx, COMPLEX(0.)));
  }else{
    untested();
    return CKT_BASE::ac_probe_ext(cmd);
  }
}
/*--------------------------------------------------------------------------*/
/* annotated_logic_value:  a printable value for probe
 * that has secondary info encoded in its fraction part
 */
const double LOGIC_NODE::annotated_logic_value()const
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
inline const bool LOGIC_NODE::just_reached_stable()const
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
  if (process() && process() != f) {
    untested();
    set_bad_quality("logic process mismatch");
    error(bWARNING, "node " + long_label() 
	  + " logic process mismatch\nis it " + process()->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  if (is_analog() &&  d_iter() < a_iter()) {
    switch (SIM::phase) {
    case SIM::pDC_SWEEP:
    case SIM::pINIT_DC:
      set_last_change_time(0);
      store_old_last_change_time();
      set_lv(lvUNKNOWN);
      break;
    case SIM::pTRAN:
      break;
    case SIM::pNONE:
      unreachable();
      break;
    }
    double dt = SIM::time0 - last_change_time();
    if (dt < 0.) {
      untested();
      error(bPICKY, "time moving backwards.  was %g, now %g\n",
	    last_change_time(), SIM::time0);
      dt = SIM::time0 - old_last_change_time();
      if (dt <= 0.) {
	untested();
	error(bERROR,"internal error: time moving backwards, can't recover\n");
      }else{
	untested();
      }
      assert(dt > 0.);
      restore_lv();			/* skip back one */
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
	    untested();
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
    trace3(_failure_mode, _lastchange, _quality, _lv);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::to_analog(const MODEL_LOGIC* f)
{
  assert(f);
  if (process() && process() != f) {
    untested();
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
    untested(); 
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

  if (SIM::time0 <= (final_time()-risefall)) {
    return start;
  }else if (SIM::time0 >= final_time()) {
    untested();
    return end;
  }else{
    untested();
    return end - ((end-start) * (final_time()-SIM::time0) / risefall);
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
void LOGIC_NODE::force_initial_value(LOGICVAL v)
{
  assert(SIM::phase == SIM::pINIT_DC || SIM::phase == SIM::pDC_SWEEP);
  assert(SIM::time0 == 0.);
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
  if (SIM::phase == SIM::pTRAN  &&  in_transit()) { 
    untested();
    set_bad_quality("race");
  }
  set_d_iter();
  set_final_time(SIM::time0 + delay);
  if (OPT::picky <= bTRACE) {
    untested();
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  SIM::new_event(final_time());
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void node_t::parse(CS& cmd, CARD* d)
{
  int here = cmd.cursor();
  std::string node_name = cmd.ctos();
  if (cmd.stuck(&here)) {
    untested();
    // didn't move, probably a terminator.
    _t = INVALID_NODE;
  }else{
    // legal node name, store it.
    assert(d);
    NODE_MAP* map = d->scope()->nodes();
    assert(map);
    _n = map->new_node(node_name);
    _t = _n->user_number();
  }
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_0(CARD* d)
{
  untested();
  assert(d);
  const CARD* owner = (dynamic_cast<const MODEL_SUBCKT*>(d))
    ? d			// subckt header, makes its own scope
    : d->owner();	// normal element, owner determines scope
  const CARD_LIST* sc_ope = (owner) 
    ? (owner->subckt())		// in a subckt, or is a subckt
    : &(CARD_LIST::card_list);	// root circuit
  assert(sc_ope);
  assert(sc_ope == d->scope());
  NODE_MAP* map = sc_ope->nodes();
  assert(map);
  _n = (*map)["0"];
  _t = 0;
}
/*--------------------------------------------------------------------------*/
void node_t::map_subckt_node(int* m)
{
  assert(e_() != INVALID_NODE);
  assert(e_() >= 0);	/* bad node? */
  assert(m[e_()] >= 0);	/* node map,all mapped and valid */
  _t = m[e_()];
}
/*--------------------------------------------------------------------------*/
/* volts_limited: transient voltage, best approximation, with limiting
 */
double volts_limited(const node_t & n1, const node_t & n2)
{
  bool limiting = false;

  double v1 = n1.v0();
  assert(v1 == v1);
  if (v1 < CKT_BASE::_vmin) {
    limiting = true;
    v1 = CKT_BASE::_vmin;
  }else if (v1 > CKT_BASE::_vmax) {
    limiting = true;
    v1 = CKT_BASE::_vmax;
  }

  double v2 = n2.v0();
  assert(v2 == v2);
  if (v2 < CKT_BASE::_vmin) {
    limiting = true;
    v2 = CKT_BASE::_vmin;
  }else if (v2 > CKT_BASE::_vmax) {
    limiting = true;
    v2 = CKT_BASE::_vmax;
  }

  if (limiting) {
    SIM::limiting = true;
    if (OPT::dampstrategy & dsRANGE) {
      SIM::fulldamp = true;
      error(bTRACE, "range limit damp\n");
    }
    if (OPT::picky <= bTRACE) {
      untested();
      error(bNOERROR,"node limiting (n1,n2,dif) "
	    "was (%g %g %g) now (%g %g %g)\n",
	    n1.v0(), n2.v0(), n1.v0() - n2.v0(), v1, v2, v1-v2);
    }
  }

  return dn_diff(v1,v2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
