/*$Id: d_logic.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx  out gnd vdd in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
#include "ap.h"
#include "u_xprobe.h"
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
extern NODE* nstat;
int DEV_LOGIC::_count = 0;
int COMMON_LOGIC::_count = -1;
static LOGIC_NONE Default_LOGIC(CC_STATIC);
/*--------------------------------------------------------------------------*/
DEV_LOGIC::DEV_LOGIC()
  :ELEMENT(),
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
   _lastchangenode(0),
   _quality(qGOOD),
   _failuremode("ok"),
   _oldgatemode(moUNKNOWN),
   _gatemode(moUNKNOWN)   
{
  untested();
  for (int ii = 0;  ii < PORTS_PER_GATE;  ++ii) {
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::parse(CS& cmd)
{
  parse_Label(cmd);
  int incount = parse_nodes(cmd, PORTS_PER_GATE, BEGIN_IN);
  if (incount <= BEGIN_IN) {
    untested();
    incount = BEGIN_IN +1;
    cmd.warn(bDANGER, "need more nodes");
  }
  incount -= BEGIN_IN;
  std::string modelname = cmd.ctos(TOKENTERM);

  COMMON_LOGIC* c = 0;
  {if     (cmd.pmatch("AND" )) {c = new LOGIC_AND;}
  else if (cmd.pmatch("NAND")) {c = new LOGIC_NAND;}
  else if (cmd.pmatch("OR"  )) {c = new LOGIC_OR;}
  else if (cmd.pmatch("NOR" )) {c = new LOGIC_NOR;}
  else if (cmd.pmatch("XOR" )) {c = new LOGIC_XOR;}
  else if (cmd.pmatch("INV" )) {c = new LOGIC_INV;}
  else {cmd.warn(bWARNING,"need and,nand,or,nor,xor,inv");c=new LOGIC_NONE;}
  }
  assert(c);
  c->incount = incount;
  c->set_modelname(modelname);
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::print(OMSTREAM& where, int)const
{
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  where << short_label();
  printnodes(where, PORTS_PER_GATE);
  where << ' ' << c->modelname() << ' ' << c->name() << '\n'; 
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::expand()
{
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(attach_model());
  if (!m) {
    untested();
    error(bERROR, "model "+c->modelname()+" is not a logic family (LOGIC)\n");
  }

  char s_incount[BUFLEN];
  sprintf(s_incount, "%u", c->incount);
  expandsubckt(c->modelname() + c->name() + s_incount);

  {if (subckt().empty()) {
    _gatemode = moDIGITAL;
    error(bDANGER, long_label() + ": no model, forcing digital\n");
  }else{
    _gatemode = OPT::mode;
    subckt().expand();
  }}

  assert(!constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::precalc()
{
  subckt().precalc();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::dc_begin()
{
  {if (subckt().empty()) {
    _gatemode = moDIGITAL;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
  }}
  NODE* outnode = &(nstat[_n[OUTNODE].m]);
  outnode->set_mode(_gatemode);
  _oldgatemode = _gatemode;
  subckt().dc_begin();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_begin()
{
  {if (subckt().empty()) {
    untested();
    _gatemode = moDIGITAL;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
  }}
  NODE* outnode = &(nstat[_n[OUTNODE].m]);
  outnode->set_mode(_gatemode);
  _oldgatemode = _gatemode;
  subckt().tr_begin();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_restore()
{
  {if (subckt().empty()) {
    untested();
    _gatemode = moDIGITAL;
  }else{
    untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
  }}
  subckt().tr_restore();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::dc_advance()
{
  if (_gatemode != _oldgatemode) {
    tr_unload();
    NODE* outnode = &(nstat[_n[OUTNODE].m]);
    outnode->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable();	 break;
  case moMIXED:   unreachable();	 break;
  case moDIGITAL: /*nothing*/		 break;
  case moANALOG:  subckt().dc_advance(); break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_LOGIC::tr_advance()
{
  NODE* outnode = &(nstat[_n[OUTNODE].m]);
  if (_gatemode != _oldgatemode) {
    tr_unload();
    outnode->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL:
    if (outnode->in_transit()) {
      q_eval();
      if (SIM::time0 >= outnode->final_time()) {
	outnode->propagate();
      }
    }
    break;
  case moANALOG:
    subckt().tr_advance();
    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC::tr_needs_eval()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); return false;
  case moMIXED:   unreachable(); return false;
  case moDIGITAL: return (SIM::phase != SIM::pTRAN);
  case moANALOG:  untested(); return subckt().tr_needs_eval();
  }
  unreachable();
  return false;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_queue_eval()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable();		     break;
  case moMIXED:   unreachable();		     break;
  case moDIGITAL: {if (tr_needs_eval()) {q_eval();}} break;
  case moANALOG:  subckt().tr_queue_eval();	     break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::tr_eval_digital()
{
  assert(_gatemode == moDIGITAL);
  NODE* outnode = &(nstat[_n[OUTNODE].m]);
  
  switch (SIM::phase) {
  case SIM::pDC_SWEEP:	untested(); tr_accept();break;
  case SIM::pINIT_DC:	tr_accept();		break;
  case SIM::pTRAN:	/*nothing*/		break;
  case SIM::pNONE:	unreachable();		break;
  }
  
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y0.x = 0.;
  _y0.f1 = outnode->to_analog(m);
  _y0.f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./m->rs;
  _m0.c0 = _y0.f1 / -m->rs;
  store_values();
  q_load();
  
  return !outnode->is_unknown();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::do_tr()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable();		    break;
  case moMIXED:   unreachable();		    break;
  case moDIGITAL: set_converged(tr_eval_digital()); break;
  case moANALOG:  set_converged(subckt().do_tr());  break;
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_load()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable();      break;
  case moMIXED:   unreachable();      break;
  case moDIGITAL: tr_load_passive();  break;
  case moANALOG:  subckt().tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC::tr_review()
{
  q_accept();
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); return NEVER;
  case moMIXED:   unreachable(); return NEVER;
  case moDIGITAL: return NEVER;
  case moANALOG:  return subckt().tr_review();
  }
  //digital mode queues events explicitly in tr_accept
  unreachable();
  return NEVER;
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
  NODE* ns[PORTS_PER_GATE];
  {
    _n[OUTNODE]->to_logic(m);
    ns[OUTNODE] = &*(_n[OUTNODE]);
    _quality = ns[OUTNODE]->quality();  /* the worst quality on this device */
    _failuremode = ns[OUTNODE]->failure_mode();    /* what is wrong with it? */
    _lastchangenode = OUTNODE;		/* which node changed most recently */
    int lastchangeiter=ns[OUTNODE]->d_iter();/* iteration # when it changed */
    trace0(long_label().c_str());
    trace2(ns[OUTNODE]->failure_mode(), OUTNODE, ns[OUTNODE]->quality());
    
    for (int ii = BEGIN_IN;  ii < port_count();  ++ii) {
      assert(ii < PORTS_PER_GATE);
      _n[ii]->to_logic(m);
      ns[ii] = &*(_n[ii]);
      if (ns[ii]->quality() < _quality) {
	_quality = ns[ii]->quality();
	_failuremode = ns[ii]->failure_mode();
      }
      if (ns[ii]->d_iter() >= lastchangeiter) {
	lastchangeiter = ns[ii]->d_iter();
	_lastchangenode = ii;
      }
      trace2(ns[ii]->failure_mode(), ii, ns[ii]->quality());
    }
    /* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
     * Otherwise, an input changed.  Need to evaluate.
     * If all quality are good, can evaluate as digital.
     * Otherwise need to evaluate as analog.
     */
    trace3(_failuremode, _lastchangenode, lastchangeiter, _quality);
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  {if (want_analog()) {
    if (_gatemode == moDIGITAL) {
      untested();
      error(bTRACE, "%s:%u:%g switch to analog, %s\n", long_label().c_str(),
	    STATUS::iter[iTOTAL], SIM::time0, _failuremode);
      _oldgatemode = _gatemode;
      _gatemode = moANALOG;
    }
    assert(_gatemode == moANALOG);
  }else{
    assert(want_digital());
    if (_gatemode == moANALOG) {
      error(bTRACE, "%s:%u:%g switch to digital\n",
	    long_label().c_str(), STATUS::iter[iTOTAL], SIM::time0);
      _oldgatemode = _gatemode;
      _gatemode = moDIGITAL;
    }
    assert(_gatemode == moDIGITAL);
    bool bypass = (SIM::bypass_ok
		   && _lastchangenode == OUTNODE
		   && SIM::phase == SIM::pTRAN);
    if (!bypass) {
      LOGICVAL newstate = c->logic_eval(&ns[BEGIN_IN]);
      //		     ^^^^^^^^^^
      {if (ns[OUTNODE]->is_unknown()) {
	assert(SIM::phase == SIM::pINIT_DC  ||  SIM::phase == SIM::pDC_SWEEP);
	ns[OUTNODE]->force_initial_value(newstate);
	/* This happens when initial DC is digital.
	 * Answers could be wrong if order in netlist is reversed 
	 */
      }else{
	switch (newstate) {
	case lvSTABLE0:	/*nothing*/		break;
	case lvRISING:  newstate=lvSTABLE0;	break;
	case lvFALLING: newstate=lvSTABLE1;	break;
	case lvSTABLE1:	/*nothing*/		break;
	case lvUNKNOWN: untested(); /*nothing*/	break;
	}
	if (newstate.lv_future() != ns[OUTNODE]->lv_future()) {
	  ns[OUTNODE]->set_event(m->delay);
	  //assert(newstate == ns[OUTNODE]->lv_future());
	  if (_lastchangenode == OUTNODE) {
	    untested();
	    error(bDANGER, "%s:%u:%g non-event state change\n",
		  long_label().c_str(), STATUS::iter[iTOTAL], SIM::time0);
	  }
	}
      }}
    }
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_unload()
{
  {if (subckt().exists()) {
    subckt().tr_unload();
  }else{
    untested();
  }}
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::ac_begin()
{
  untested();
  {if (subckt().exists()) {
    subckt().ac_begin();
  }else{
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }}
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC::tr_probe_num(CS& what)const
{
  untested();
  return nstat[_n[OUTNODE].m].tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC::ac_probe_ext(CS& what)const
{
  untested();
  return nstat[_n[OUTNODE].m].ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool COMMON_LOGIC::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_LOGIC* p = dynamic_cast<const COMMON_LOGIC*>(&x);
  bool rv = p
    && incount == p->incount
    && is_equal(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
