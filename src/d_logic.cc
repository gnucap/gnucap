/*$Id: d_logic.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx  out gnd vdd in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=script,sparse 2006.07.17
#include "d_subckt.h"
#include "u_xprobe.h"
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
int DEV_LOGIC::_count = 0;
int COMMON_LOGIC::_count = -1;
static LOGIC_NONE Default_LOGIC(CC_STATIC);
/*--------------------------------------------------------------------------*/
DEV_LOGIC::DEV_LOGIC()
  :ELEMENT(),
   _net_nodes(0),
   _lastchangenode(0),
   _quality(qGOOD),
   _failuremode("ok"),
   _oldgatemode(moUNKNOWN),
   _gatemode(moUNKNOWN)   
{
  attach_common(&Default_LOGIC);
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC::DEV_LOGIC(const DEV_LOGIC& p)
  :ELEMENT(p),
   _net_nodes(p._net_nodes),
   _lastchangenode(0),
   _quality(qGOOD),
   _failuremode("ok"),
   _oldgatemode(moUNKNOWN),
   _gatemode(moUNKNOWN)   
{untested();
  assert(max_nodes() == PORTS_PER_GATE);
  for (int ii = 0;  ii < max_nodes();  ++ii) {untested();
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::parse_spice(CS& cmd)
{
  parse_Label(cmd);
  _net_nodes = parse_nodes(cmd, max_nodes(), min_nodes(), 
			   2/*family,gate_type*/, 0/*start*/);
  if (_net_nodes <= BEGIN_IN) {untested();
    incomplete(); // redundant????
    _net_nodes = BEGIN_IN +1;
    cmd.warn(bDANGER, "need more nodes");
  }else{
  }
  assert(net_nodes() == _net_nodes);
  int incount = net_nodes() - BEGIN_IN;
  std::string modelname = cmd.ctos(TOKENTERM);

  COMMON_LOGIC* c = 0;
  if      (cmd.pmatch("AND" )) {untested();c = new LOGIC_AND;}
  else if (cmd.pmatch("NAND")) {untested();c = new LOGIC_NAND;}
  else if (cmd.pmatch("OR"  )) {untested();c = new LOGIC_OR;}
  else if (cmd.pmatch("NOR" )) {c = new LOGIC_NOR;}
  else if (cmd.pmatch("XOR" )) {untested();c = new LOGIC_XOR;}
  else if (cmd.pmatch("XNOR")) {untested();c = new LOGIC_XNOR;}
  else if (cmd.pmatch("INV" )) {c = new LOGIC_INV;}
  else {untested();
    cmd.warn(bWARNING,"need and,nand,or,nor,xor,xnor,inv");
    c=new LOGIC_NONE;
  }
  
  assert(c);
  c->incount = incount;
  c->set_modelname(modelname);
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::print_spice(OMSTREAM& where, int)const
{untested();
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  where << short_label();
  printnodes(where);
  where << ' ' << c->modelname() << ' ' << c->name() << '\n'; 
}
/*--------------------------------------------------------------------------*/
inline const CARD* find_subckt_model(CARD* scope, const std::string& modelname)
{
  const CARD* model = 0;
  {
    model = scope->find_looking_out(modelname, bDEBUG);
    if (!model) {
      error(bPICKY, "can't find subckt: " + modelname + '\n');
      return NULL;
    }else if(!dynamic_cast<const MODEL_SUBCKT*>(model)) {untested();
      error(bDANGER, modelname + " is not a subckt\n");
      return NULL;
    }else{
    }
  }
  return model;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::elabo1()
{
  ELEMENT::elabo1();
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(attach_model());
  if (!m) {untested();
    error(bERROR, "model "+c->modelname()+" is not a logic family (LOGIC)\n");
  }else{
  }

  std::string subckt_name(c->modelname()+c->name()+to_string(c->incount));
  const CARD* model = find_subckt_model(this /*scope*/, subckt_name);
  if (!model) {
    _gatemode = moDIGITAL;
    error(bDANGER, long_label() + ": no model, forcing digital\n");
  }else{
    _gatemode = OPT::mode;
    new_subckt();
    subckt()->shallow_copy(model->subckt());
    subckt()->set_owner(this);
    subckt()->map_subckt_nodes(model, this);
    subckt()->elabo2();
  }

  assert(!is_constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::map_nodes()
{
  COMPONENT::map_nodes();
  if (subckt()) {
    subckt()->map_nodes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::precalc()
{
  if (subckt()) {
    subckt()->precalc();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::dc_begin()
{
  if (!subckt()) {
    _gatemode = moDIGITAL;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
    subckt()->dc_begin();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_begin()
{
  if (!subckt()) {untested();
    _gatemode = moDIGITAL;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
    subckt()->tr_begin();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_restore()
{untested();
  if (!subckt()) {untested();
    _gatemode = moDIGITAL;
  }else{untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    subckt()->tr_restore();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::dc_advance()
{
  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moANALOG:  assert(subckt()); subckt()->dc_advance(); break;
  case moDIGITAL:
    if (_n[OUTNODE]->in_transit()) {
      //q_eval(); evalq is not used for DC
      _n[OUTNODE]->propagate();
    }else{
    }
    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_LOGIC::tr_advance()
{
  if (_gatemode != _oldgatemode) {
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moANALOG:  assert(subckt()); subckt()->tr_advance(); break;
  case moDIGITAL:
    if (_n[OUTNODE]->in_transit()) {
      q_eval();
      if (SIM::time0 >= _n[OUTNODE]->final_time()) {
	_n[OUTNODE]->propagate();
      }else{untested();
      }
    }else{
    }
    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC::tr_needs_eval()const
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL:
    //assert(!is_q_for_eval());
    return (SIM::phase != SIM::pTRAN);
  case moANALOG:
    untested();
    assert(!is_q_for_eval());
    assert(subckt());
    return subckt()->tr_needs_eval();
  }
  unreachable();
  return false;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_queue_eval()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:	  unreachable(); break;
  case moDIGITAL: ELEMENT::tr_queue_eval(); break;
  case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::tr_eval_digital()
{
  assert(_gatemode == moDIGITAL);
  
  switch (SIM::phase) {
  case SIM::pDC_SWEEP:	tr_accept();	break;
  case SIM::pINIT_DC:	tr_accept();	break;
  case SIM::pTRAN:	/*nothing*/	break;
  case SIM::pNONE:	unreachable();	break;
  }
  
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y0.x = 0.;
  _y0.f1 = _n[OUTNODE]->to_analog(m);
  _y0.f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./m->rs;
  _m0.c0 = _y0.f1 / -m->rs;
  set_converged(conv_check());
  store_values();
  q_load();
  
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::do_tr()
{  
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL: set_converged(tr_eval_digital()); break;
  case moANALOG:  assert(subckt()); set_converged(subckt()->do_tr()); break;
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_load()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL: tr_load_passive(); break;
  case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
DPAIR DEV_LOGIC::tr_review()
{
  q_accept();
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); return DPAIR(NEVER, NEVER);
  case moMIXED:   unreachable(); return DPAIR(NEVER, NEVER);
  case moDIGITAL: return DPAIR(NEVER, NEVER);
  case moANALOG:  assert(subckt()); return subckt()->tr_review();
  }
  //digital mode queues events explicitly in tr_accept
  unreachable();
  return DPAIR(NEVER, NEVER);
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC::tr_accept()
{
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  assert(PORTS_PER_GATE == max_nodes());
  {
    _n[OUTNODE]->to_logic(m);
    _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
    _failuremode = _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
    _lastchangenode = OUTNODE;		/* which node changed most recently */
    int lastchangeiter=_n[OUTNODE]->d_iter();/* iteration # when it changed */
    trace0(long_label().c_str());
    trace2(_n[OUTNODE]->failure_mode(), OUTNODE, _n[OUTNODE]->quality());
    
    for (int ii = BEGIN_IN;  ii < net_nodes();  ++ii) {
      _n[ii]->to_logic(m);
      if (_n[ii]->quality() < _quality) {
	_quality = _n[ii]->quality();
	_failuremode = _n[ii]->failure_mode();
      }else{
      }
      if (_n[ii]->d_iter() >= lastchangeiter) {
	lastchangeiter = _n[ii]->d_iter();
	_lastchangenode = ii;
      }else{
      }
      trace2(_n[ii]->failure_mode(), ii, _n[ii]->quality());
    }
    /* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
     * Otherwise, an input changed.  Need to evaluate.
     * If all quality are good, can evaluate as digital.
     * Otherwise need to evaluate as analog.
     */
    trace3(_failuremode, _lastchangenode, lastchangeiter, _quality);
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  if (want_analog()) {
    if (_gatemode == moDIGITAL) {untested();
      error(bTRACE, "%s:%u:%g switch to analog, %s\n", long_label().c_str(),
	    iteration_tag(), SIM::time0, _failuremode);
      _oldgatemode = _gatemode;
      _gatemode = moANALOG;
    }else{
    }
    assert(_gatemode == moANALOG);
  }else{
    assert(want_digital());
    if (_gatemode == moANALOG) {
      error(bTRACE, "%s:%u:%g switch to digital\n",
	    long_label().c_str(), iteration_tag(), SIM::time0);
      _oldgatemode = _gatemode;
      _gatemode = moDIGITAL;
    }else{
    }
    assert(_gatemode == moDIGITAL);
    if (!SIM::bypass_ok
	|| _lastchangenode != OUTNODE
	|| SIM::phase != SIM::pTRAN) {
      LOGICVAL future_state = c->logic_eval(&_n[BEGIN_IN]);
      //		         ^^^^^^^^^^
      if ((_n[OUTNODE]->is_unknown())
	  && (SIM::phase == SIM::pINIT_DC || SIM::phase == SIM::pDC_SWEEP)) {
	_n[OUTNODE]->force_initial_value(future_state);
	/* This happens when initial DC is digital.
	 * Answers could be wrong if order in netlist is reversed 
	 */
      }else if (future_state != _n[OUTNODE]->lv()) {
	assert(future_state != lvUNKNOWN);
	switch (future_state) {
	case lvSTABLE0:	/*nothing*/		break;
	case lvRISING:  future_state=lvSTABLE0;	break;
	case lvFALLING: future_state=lvSTABLE1;	break;
	case lvSTABLE1:	/*nothing*/		break;
	case lvUNKNOWN: unreachable();		break;
	}
	/* This handling of rising and falling may seem backwards.
	 * These states occur when the value has been contaminated 
	 * by another pending action.  The "old" value is the
	 * value without this contamination.
	 * This code is planned for replacement as part of VHDL/Verilog
	 * conversion, so the kluge stays in for now.
	 */
	assert(future_state.lv_old() == future_state.lv_future());
	if (_n[OUTNODE]->lv() == lvUNKNOWN
	    || future_state.lv_future() != _n[OUTNODE]->lv_future()) {
	  _n[OUTNODE]->set_event(m->delay, future_state);
	  //assert(future_state == _n[OUTNODE].lv_future());
	  if (_lastchangenode == OUTNODE) {
	    unreachable();
	    error(bDANGER, "%s:%u:%g non-event state change\n",
		  long_label().c_str(), iteration_tag(), SIM::time0);
	  }else{
	  }
	}else{
	}
      }else{
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::do_ac()
{untested();
  assert(subckt());
  subckt()->do_ac();
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC::tr_probe_num(CS& what)const
{
  return _n[OUTNODE]->tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC::ac_probe_ext(CS& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::want_analog()const
{
  return subckt() &&
    ((OPT::mode == moANALOG) || (OPT::mode == moMIXED && _quality != qGOOD));
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_LOGIC* p = dynamic_cast<const COMMON_LOGIC*>(&x);
  bool rv = p
    && incount == p->incount
    && COMMON_COMPONENT::operator==(x);
  if (rv) {untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
