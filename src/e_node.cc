/*$Id: e_node.cc,v 24.23 2004/02/01 21:12:55 al Exp $ -*- C++ -*-
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
 * node probes
 */
#include "u_nodemap.h"
#include "l_denoise.h"
#include "declare.h"	/*  new_event ... */
#include "d_logic.h"
#include "e_aux.h"
#include "u_xprobe.h"
#include "ap.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
//inline LOGICVAL& LOGICVAL::set_in_transition()
//		NODE::NODE();
// const string NODE::long_label()const;
//	double	NODE::tr_probe_num(CS&)const;
//	XPROBE  NODE::ac_probe_ext(CS&)const;
//	double	NODE::annotated_logic_value()const;
//inline bool	NODE::just_reached_stable()const
//	NODE&	NODE::to_logic()const;
//	double  NODE::to_analog()const;
//	NODE&   NODE::propagate();
//	NODE&	NODE::force_initial_value(bool v)
//	NODE&	NODE::set_event(double delay)
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
NODE::NODE()
  :CKT_BASE(),
   _user_number(0),
   _family(0),
   _d_iter(0),
   _a_iter(0),
   _final_time(0),
   _lastchange(0),
   _old_lastchange(0),
   _dt(0),
   _mode(moANALOG),
   _lv(),
   _old_lv(),
   _needs_analog(false)
{
  set_bad_quality("initial");
}
/*--------------------------------------------------------------------------*/
const std::string NODE::long_label()const
{
  static char buffer[BUFLEN];
  sprintf(buffer, "%u", user_number());
  return buffer;
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(CS& cmd)const
{
  {if (cmd.pmatch("V")) {
    return rint(v0()/OPT::vfloor) * OPT::vfloor;
  }else if (cmd.pmatch("Z")) {
    return port_impedance(node_t(user_number()), node_t(0), lu, 0.);
  }else if (cmd.pmatch("Logic")) {
    return annotated_logic_value();
  }else if (cmd.pmatch("LAstchange")) {
    return _lastchange;
  }else if (cmd.pmatch("FInaltime")) {
    return final_time();
  }else if (cmd.pmatch("DIter")) {
    return static_cast<double>(_d_iter);
  }else if (cmd.pmatch("AIter")) {
    return static_cast<double>(_a_iter);
  }else if (cmd.pmatch("COUNT")) {
    return static_cast<double>(_needs_analog);
  }else{
    return CKT_BASE::tr_probe_num(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
XPROBE NODE::ac_probe_ext(CS& cmd)const
{
  {if (cmd.pmatch("V")) {
    return XPROBE(vac());
  }else if (cmd.pmatch("Z")) {
    return XPROBE(port_impedance(node_t(user_number()),
				 node_t(0),acx,COMPLEX(0.)));
  }else{
    untested();
    return CKT_BASE::ac_probe_ext(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
/* annotated_logic_value:  a printable value for probe
 * that has secondary info encoded in its fraction part
 */
double NODE::annotated_logic_value()const
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
inline bool NODE::just_reached_stable()const
{
  return newly_stable[_old_lv][_lv];
}
/*--------------------------------------------------------------------------*/
/* to_logic: set up logic data for a node, if needed
 * If the logic data is already up to date, do nothing.
 * else set up: logic value (_lv) and quality.
 * Use and update _d_iter, _lastchange to keep track of what was done.
 */
NODE& NODE::to_logic(const MODEL_LOGIC* f)
{
  assert(f);
  if (_family && _family != f) {
    untested();
    set_bad_quality("logic family mismatch");
    error(bWARNING, "node " + long_label() 
	  + " logic family mismatch\nis it " + _family->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  _family = f;


  if (is_analog() &&  _d_iter < _a_iter) {
    switch (SIM::phase) {
    case SIM::pDC_SWEEP:
    case SIM::pINIT_DC:	_lastchange=_old_lastchange=0;_lv=lvUNKNOWN;break;
    case SIM::pTRAN:	break;
    case SIM::pNONE:	unreachable(); break;
    }
    _dt = SIM::time0 - _lastchange;
    {if (_dt < 0.) {
      error(bPICKY, "time moving backwards.  was %g, now %g\n",
	    _lastchange, SIM::time0);
      _dt = SIM::time0 - _old_lastchange;
      if (_dt <= 0.) {
	untested();
	error(bERROR,"internal error: time moving backwards, can't recover\n");
      }else{
	untested();
      }
      assert(_dt > 0.);
      _lv = _old_lv;			/* skip back one */
    }else{
      _old_lastchange = _lastchange;
      _old_lv = _lv;			/* save to see if it changes */
    }}
    
    double sv = v0() / _family->range;	/* new scaled voltage */
    {if (sv >= _family->th1) {		/* logic 1 */
      switch (_lv) {
      case lvSTABLE0: dont_set_quality("stable 0 to stable 1");	break;
      case lvRISING:  dont_set_quality("begin stable 1");	break;
      case lvFALLING:untested();set_bad_quality("falling to stable 1"); break;
      case lvSTABLE1: dont_set_quality("continuing stable 1");	break;
      case lvUNKNOWN: set_good_quality("initial 1");		break;
      }
      _lv = lvSTABLE1;
    }else if (sv <= _family->th0) {	/* logic 0 */
      switch (_lv) {
      case lvSTABLE0: dont_set_quality("continuing stable 0");	break;
      case lvRISING: untested();set_bad_quality("rising to stable 0");	break;
      case lvFALLING: dont_set_quality("begin stable 0");	break;
      case lvSTABLE1: dont_set_quality("stable 1 to stable 0");	break;
      case lvUNKNOWN: set_good_quality("initial 0");		break;
      }
      _lv = lvSTABLE0;
    }else{				/* transition region */
      double oldsv = vt1() / _family->range;/* old scaled voltage */
      double diff  = sv - oldsv;
      {if (diff > 0) {	/* rising */
	switch (_lv) {
	case lvSTABLE0: dont_set_quality("begin good rise");		break;
	case lvRISING:
	  {if (diff < _dt/(_family->mr * _family->rise)) {
	    set_bad_quality("slow rise");
	  }else{
	    dont_set_quality("continuing good rise");
	  }}
	  break;
	case lvFALLING: set_bad_quality("positive glitch in fall");	break;
	case lvSTABLE1: set_bad_quality("negative glitch in 1");	break;
	case lvUNKNOWN: set_bad_quality("initial rise");		break;
	}
	_lv = lvRISING;
      }else if (diff < 0) {	/* falling */
	switch (_lv) {
	case lvSTABLE0: set_bad_quality("positive glitch in 0");	break;
	case lvRISING:  set_bad_quality("negative glitch in rise");	break;
	case lvFALLING:
	  {if (-diff < _dt/(_family->mf * _family->fall)) {
	    set_bad_quality("slow fall");
	  }else{
	    dont_set_quality("continuing good fall");
	  }}
	  break;
	case lvSTABLE1:	dont_set_quality("begin good fall");		break;
	case lvUNKNOWN: set_bad_quality("initial fall");		break;
	}
	_lv = lvFALLING;
      }else{				/* hanging up in transition */
	untested();
	error(bDANGER, "inflection???\n");
	set_bad_quality("in transition but no change");
	/* state (rise/fall)  unchanged */
      }}
    }}
    if (sv > 1.+_family->over || sv < -_family->over) {/* out of range */
      untested();
      set_bad_quality("out of range");
    }
    if (just_reached_stable()) { /* A bad node gets a little better */
      improve_quality();	/* on every good transition.	   */
    }				/* Eventually, it is good enough.  */
				/* A good transition is defined as */
				/* entering a stable state from    */
				/* a transition state.		   */
    set_d_iter();
    _lastchange = SIM::time0;
    trace3(_failure_mode, _lastchange, _quality, _lv);
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
double NODE::to_analog(const MODEL_LOGIC* f)
{
  assert(f);
  if (_family && _family != f) {
    untested();
    error(bWARNING, "node " + long_label() 
	  + " logic family mismatch\nis it " + _family->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  _family = f;

  double start = NOT_VALID;
  double end = NOT_VALID;
  double risefall = NOT_VALID;
  switch (_lv) {
  case lvSTABLE0: return _family->vmin;
  case lvRISING:  untested(); 
    start=_family->vmin; end=_family->vmax; risefall=_family->rise; break;
  case lvFALLING: untested(); 
    start=_family->vmax; end=_family->vmin; risefall=_family->fall; break;
  case lvSTABLE1: return _family->vmax;
  case lvUNKNOWN: return _family->unknown;
  }
  assert(start != NOT_VALID);
  assert(end   != NOT_VALID);
  assert(risefall != NOT_VALID);

  {if (SIM::time0 <= (final_time()-risefall)) {
    return start;
  }else if (SIM::time0 >= final_time()) {
    untested();
    return end;
  }else{
    return end - ((end-start) * (final_time()-SIM::time0) / risefall);
  }}
}
/*--------------------------------------------------------------------------*/
NODE& NODE::propagate()
{
  assert(in_transit());
  {if (_lv.is_rising()) {
    _lv = lvSTABLE1;
  }else if (_lv.is_falling()) {
    _lv = lvSTABLE0;
  }else{
    //untested();
  }}
  set_d_iter();
  set_final_time(NEVER);
  _lastchange = SIM::time0;
  assert(!(in_transit()));
  return *this;
}
/*--------------------------------------------------------------------------*/
NODE& NODE::force_initial_value(LOGICVAL v)
{
  assert(SIM::phase == SIM::pINIT_DC || SIM::phase == SIM::pDC_SWEEP);
  assert(SIM::time0 == 0.);
  assert(is_unknown());
  assert(is_digital());
  _lv = v; // BUG ??
  set_good_quality("initial dc");
  set_d_iter();
  set_final_time(NEVER);
  _lastchange = SIM::time0;
  return *this;
}
/*--------------------------------------------------------------------------*/
NODE& NODE::set_event(double delay, LOGICVAL v)
{
  _lv.set_in_transition(v);
  if (SIM::phase == SIM::pTRAN  &&  in_transit()) { 
    untested();
    set_bad_quality("race");
  }
  set_d_iter();
  set_final_time(SIM::time0 + delay);
  error(bTRACE, "%s:%u:%g new event\n",
	long_label().c_str(), _d_iter, final_time());
  new_event(final_time());
  _lastchange = SIM::time0;
  return *this;
}
/*--------------------------------------------------------------------------*/
/* name2number: convert node name to node number
 * returns node number
 * cnt updated
 */
static int name2number(CS& cmd, const CARD* d)
{
  {if (OPT::named_nodes) {
    int here = cmd.cursor();
    {if (cmd.skiprparen()) {
      cmd.reset(here);
      return INVALID_NODE;
    }else{
      std::string node = cmd.ctos();
      {if (!cmd.more()) {
	cmd.reset(here);
	return INVALID_NODE;
      }else if (cmd.stuck(&here)) {
	return INVALID_NODE;
      }else{
	assert(d);
	CARD* owner = d->owner();
	CARD_LIST* cl = (owner) 
	  ? &(owner->subckt()) : &(CARD_LIST::card_list);
	NODE_MAP* map = &(cl->_nm);
	return map->new_node(node);
      }}
    }}
  }else{
    int here = cmd.cursor();
    int node = cmd.ctoi();
    {if (cmd.stuck(&here)) {
      return INVALID_NODE;
    }else{
      return node;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void node_t::parse(CS& cmd, const CARD* card)
{
  _t = _e = name2number(cmd, card);
}
/*--------------------------------------------------------------------------*/
std::string node_t::name(const CARD* d)const
{
  {if (OPT::named_nodes) {
    //untested();
    assert(d);
    CARD* owner = d->owner();
    CARD_LIST* cl = (owner) 
      ? &(owner->subckt()) : &(CARD_LIST::card_list);
    NODE_MAP* map = &(cl->_nm);
    return (*map)[e_()];
  }else{
    //untested();
    return to_string(e_());
  }}
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
OMSTREAM& operator<<(OMSTREAM& o, const node_t& n)
{
  o << n.name(0);
  return o;
}
/*--------------------------------------------------------------------------*/
/* volts_limited: transient voltage, best approximation, with limiting
 */
double volts_limited(const node_t & n1, const node_t & n2)
{
  bool limiting = false;

  double v1 = n1.v0();
  {if (v1 < CKT_BASE::_vmin) {
    limiting = true;
    v1 = CKT_BASE::_vmin;
  }else if (v1 > CKT_BASE::_vmax) {
    limiting = true;
    v1 = CKT_BASE::_vmax;
  }}

  double v2 = n2.v0();
  {if (v2 < CKT_BASE::_vmin) {
    limiting = true;
    v2 = CKT_BASE::_vmin;
  }else if (v2 > CKT_BASE::_vmax) {
    limiting = true;
    v2 = CKT_BASE::_vmax;
  }}

  if (limiting) {
    SIM::limiting = true;
    if (OPT::dampstrategy & dsRANGE) {
      SIM::fulldamp = true;
      error(bTRACE, "range limit damp\n");
    }
    if (OPT::picky <= bTRACE) {
      error(bNOERROR,"node limiting (n1,n2,dif) "
	    "was (%g %g %g) now (%g %g %g)\n",
	    n1.v0(), n2.v0(), n1.v0() - n2.v0(), v1, v2, v1-v2);
    }
  }

  return dn_diff(v1,v2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
