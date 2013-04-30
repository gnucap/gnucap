/*$Id: e_node.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
#include "u_nodemap.h"
#include "d_logic.h"
#include "e_aux.h"
#include "u_xprobe.h"
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
   //_flat_number(INVALID_NODE)
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
/* copy constructor : user data only
 */
NODE::NODE(const NODE& p)
  :CKT_BASE(p),
   _user_number(p._user_number)
   //_flat_number(p._flat_number)
   //_matrix_number(INVALID_NODE)
{
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 * supposedly not used, but used by a required function that is also not used
 */
NODE::NODE(const NODE* p)
  :CKT_BASE(*p),
   _user_number(p->_user_number)
   //_flat_number(p->_flat_number)
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
   //_flat_number(n)
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
node_t::node_t()
  :_nnn(0),
   _ttt(INVALID_NODE),
   _m(INVALID_NODE)
{
}
node_t::node_t(const node_t& p)
  :_nnn(p._nnn),
   _ttt(p._ttt),
   _m(p._m)
{
  //assert(_ttt == _nnn->flat_number());
}
node_t::node_t(NODE* n)
  :_nnn(n),
   _ttt(n->user_number()),
   _m(to_internal(n->user_number()))
{
  //assert(_ttt == _nnn->flat_number());
}
node_t& node_t::operator=(const node_t& p)
{
  if (p._nnn) {
    //assert(p._ttt == p._nnn->flat_number());
  }else{
    assert(p._ttt == INVALID_NODE);
    assert(p._m   == INVALID_NODE);
  }
  _nnn   = p._nnn;
  _ttt = p._ttt;
  _m   = p._m;
  return *this;
}
/*--------------------------------------------------------------------------*/
LOGIC_NODE& node_t::data()const
{
  assert(CKT_BASE::_sim->_nstat);
  return CKT_BASE::_sim->_nstat[m_()];
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_lu, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    assert(_sim->_nstat);
    return _sim->_nstat[matrix_number()].tr_probe_num(x);
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.d(m_(),m_());
  }else if (Umatch(x, "mdz ")) {
    // matrix diagonal impedance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return 1/aaa.d(m_(),m_());
  }else if (Umatch(x, "zero ")) {
    // fake probe: 0.0
    return 0.0;
  }else if (Umatch(x, "pdz ")) {
    // fake probe 1/0 .. positive divide by zero = Infinity
    double z1 = tr_probe_num("zero ");
    return 1.0/z1;
  }else if (Umatch(x, "ndz ")) {
    // fake probe -1/0 .. negative divide by zero = -Infinity
    double z1 = tr_probe_num("zero ");
    return -1.0/z1;
  }else if (Umatch(x, "nan ")) {
    // fake probe 0/0 = NaN
    double z1 = tr_probe_num("zero ");
    double z2 = tr_probe_num("zero ");
    return z1/z2;
  }else{itested();
    return CKT_BASE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "l{ogic} ")) {
    return annotated_logic_value();
  }else if (Umatch(x, "la{stchange} ")) {untested();
    return _lastchange;
  }else if (Umatch(x, "fi{naltime} ")) {untested();
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
XPROBE NODE::ac_probe_ext(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    return XPROBE(vac());
  }else if (Umatch(x, "z ")) {
    return XPROBE(port_impedance(node_t(const_cast<NODE*>(this)),
				 node_t(&ground_node), _sim->_acx, COMPLEX(0.)));
  }else{itested();
    return CKT_BASE::ac_probe_ext(x);
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

  if (_sim->_time0 <= (final_time()-risefall)) {
    return start;
  }else if (_sim->_time0 >= final_time()) {
    untested();
    return end;
  }else{
    untested();
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
  if (_sim->analysis_is_tran_dynamic()  &&  in_transit()) {untested();
    set_bad_quality("race");
  }
  set_d_iter();
  set_final_time(_sim->_time0 + delay);
  if (OPT::picky <= bTRACE) {untested();
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_ground(CARD* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  assert(d);

  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  _nnn = (*Map)["0"];
  _ttt = 0;
  assert(_nnn);
}
/*--------------------------------------------------------------------------*/
/* new_node: a raw new node, as when a netlist is parsed
 */
void node_t::new_node(const std::string& node_name, const CARD* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  assert(d);

  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  _nnn = Map->new_node(node_name);
  _ttt = _nnn->user_number();
  assert(_nnn);
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * Not really a model_node, but a node in the subckt that is made
 * in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 */
void node_t::new_model_node(const std::string& node_name, CARD* d)
{
  new_node(node_name, d);
  _ttt = CKT_BASE::_sim->newnode_model();
  //assert(_ttt == _nnn->flat_number());
}
/*--------------------------------------------------------------------------*/
void node_t::map_subckt_node(int* m, const CARD* d)
{
  assert(m);
  assert(e_() >= 0);
  if (node_is_valid(m[e_()])) {
    _ttt = m[e_()];
  }else{untested();
    throw Exception(d->long_label() + ": need more nodes");
  }
  //_nnn->set_flat_number(_ttt);
  assert(node_is_valid(_ttt));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
