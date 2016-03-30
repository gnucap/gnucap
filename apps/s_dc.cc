/*$Id: s_dc.cc 2016/03/25 al $ -*- C++ -*-
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
 * dc analysis top
 */
//testing=script 2016.03.25
#include "globals.h"
#include "u_status.h"
#include "u_prblst.h"
#include "u_cardst.h"
#include "e_elemnt.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DCOP : public SIM {
public:
  void	finish();
protected:
  void	fix_args(int);
  void	options(CS&, int);
private:
  void	sweep();
  void	sweep_recursive(int);
  void	first(int);
  bool	next(int);
  explicit DCOP(const DCOP&): SIM() {unreachable(); incomplete();}
protected:
  explicit DCOP();
  ~DCOP() {}
  
protected:
  enum {DCNEST = 4};
  int _n_sweeps;
  PARAMETER<double> _start[DCNEST];
  PARAMETER<double> _stop[DCNEST];
  PARAMETER<double> _step_in[DCNEST];
  double _step[DCNEST];
  bool _linswp[DCNEST];
  double* (_sweepval[DCNEST]);	/* pointer to thing to sweep, dc command */
  ELEMENT* (_zap[DCNEST]);	/* to branch to zap, for re-expand */
  CARDSTASH _stash[DCNEST];	/* store std values of elements being swept */
  bool _loop[DCNEST];		/* flag: do it again backwards */
  bool _reverse_in[DCNEST];	/* flag: sweep backwards, input */
  bool _reverse[DCNEST];	/* flag: sweep backwards, working */
  bool _cont;			/* flag: continue from previous run */
  TRACE _trace;			/* enum: show extended diagnostics */
  enum {ONE_PT, LIN_STEP, LIN_PTS, TIMES, OCTAVE, DECADE} _stepmode[DCNEST];
};
/*--------------------------------------------------------------------------*/
class DC : public DCOP {
public:
  explicit DC(): DCOP() {}
  ~DC() {}
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
  explicit DC(const DC&): DCOP() {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
class OP : public DCOP {
public:
  explicit OP(): DCOP() {}
  ~OP() {}
  void	do_it(CS&, CARD_LIST*);
private:
  void	setup(CS&);
  explicit OP(const OP&): DCOP() {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DC::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->_time0 = 0.;
  _sim->set_command_dc();
  _sim->_phase = p_INIT_DC;
  ::status.dc.reset().start();
  command_base(Cmd);
  _sim->_has_op = s_DC;
  _scope = NULL;
  ::status.dc.stop();
}
/*--------------------------------------------------------------------------*/
void OP::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->_time0 = 0.;
  _sim->set_command_op();
  _sim->_phase = p_INIT_DC;
  ::status.op.reset().start();
  command_base(Cmd);
  _sim->_has_op = s_OP;
  _scope = NULL;
  ::status.op.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DCOP::DCOP()
  :SIM(),
   _n_sweeps(1),
   _cont(false),
   _trace(tNONE)
{
  for (int ii = 0; ii < DCNEST; ++ii) {
    _loop[ii] = false;
    _reverse_in[ii] = false;
    _reverse[ii] = false;
    _step[ii]=0.;
    _linswp[ii]=true;
    _sweepval[ii]=&_sim->_genout;
    _zap[ii]=NULL; 
    _stepmode[ii] = ONE_PT;
  }
  
  //BUG// in SIM.  should be initialized there.
  //_sim->_genout=0.;
  _out=IO::mstdout;
  //_sim->_uic=false;
}
/*--------------------------------------------------------------------------*/
void DCOP::finish(void)
{
  for (int ii = 0;  ii < _n_sweeps;  ++ii) {
    if (_zap[ii]) { // component
      _stash[ii].restore();
      _zap[ii]->dec_probes();
      _zap[ii]->precalc_first();
      _zap[ii]->precalc_last();
      _zap[ii] = NULL;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void OP::setup(CS& Cmd)
{
  _sim->_temp_c = OPT::temp_c;
  _cont = false;
  _trace = tNONE;
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;

  _zap[0] = NULL;
  _sweepval[0] = &(_sim->_temp_c);

  if (Cmd.match1("'\"({") || Cmd.is_float()) {
    Cmd >> _start[0];
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> _stop[0];
    }else{
      _stop[0] = _start[0];
    }
  }else{
  }
  
  _step[0] = 0.;
  _sim->_genout = 0.;

  options(Cmd,0);

  _n_sweeps = 1;
  Cmd.check(bWARNING, "what's this?");
  _sim->_freq = 0;

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  _start[0].e_val(OPT::temp_c, _scope);
  fix_args(0);
}
/*--------------------------------------------------------------------------*/
void DC::setup(CS& Cmd)
{
  _sim->_temp_c = OPT::temp_c;
  _cont = false;
  _trace = tNONE;
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed */
  bool ploton = IO::plotset  &&  plotlist().size() > 0;

  if (Cmd.more()) {
    for (_n_sweeps = 0; Cmd.more() && _n_sweeps < DCNEST; ++_n_sweeps) {
      CARD_LIST::fat_iterator ci = findbranch(Cmd, &CARD_LIST::card_list);
      if (!ci.is_end()) {			// sweep a component
	if (ELEMENT* c = dynamic_cast<ELEMENT*>(*ci)) {
	  _zap[_n_sweeps] = c;
	}else{untested();
	  throw Exception("dc/op: can't sweep " + (**ci).long_label() + '\n');
	}
      }else if (Cmd.is_float()) {		// sweep the generator
	_zap[_n_sweeps] = NULL;
      }else{
	// leave as it was .. repeat Cmd with no args
      }
      
      if (Cmd.match1("'\"({") || Cmd.is_float()) {	// set up parameters
	_start[_n_sweeps] = "NA";
	_stop[_n_sweeps] = "NA";
	Cmd >> _start[_n_sweeps] >> _stop[_n_sweeps];
	_step[_n_sweeps] = 0.;
      }else{
	// leave it as it was .. repeat Cmd with no args
      }
      
      _sim->_genout = 0.;
      options(Cmd,_n_sweeps);
    }
  }else{ 
  }
  Cmd.check(bWARNING, "what's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  assert(_n_sweeps > 0);
  for (int ii = 0;  ii < _n_sweeps;  ++ii) {
    _start[ii].e_val(0., _scope);
    fix_args(ii);

    if (_zap[ii]) { // component
      _stash[ii] = _zap[ii];			// stash the std value
      _zap[ii]->inc_probes();			// we need to keep track of it
      _zap[ii]->set_value(_zap[ii]->value(),0);	// zap out extensions
      _zap[ii]->set_constant(false);		// so it will be updated
      _sweepval[ii] = _zap[ii]->set__value();	// point to value to patch
    }else{ // generator
      _sweepval[ii] = &_sim->_genout;			// point to value to patch
    }
  }
  _sim->_freq = 0;
}
/*--------------------------------------------------------------------------*/
void DCOP::fix_args(int Nest)
{
  _stop[Nest].e_val(_start[Nest], _scope);
  _step_in[Nest].e_val(0., _scope);
  _step[Nest] = _step_in[Nest];
  
  switch (_stepmode[Nest]) {
  case ONE_PT:
  case LIN_STEP:
    _linswp[Nest] = true;
    break;
  case LIN_PTS:untested();
    if (_step[Nest] <= 2.) {untested();
      _step[Nest] = 2.;
    }else{untested();
    }
    _linswp[Nest] = true;
    break;
  case TIMES:untested();
    if (_step[Nest] == 0.  &&  _start[Nest] != 0.) {untested();
      _step[Nest] = _stop[Nest] / _start[Nest];
    }else{untested();
    }
    _linswp[Nest] = false;
    break;
  case OCTAVE:untested();
    if (_step[Nest] == 0.) {untested();
      _step[Nest] = 1.;
    }else{untested();
    }
    _step[Nest] = pow(2.00000001, 1./_step[Nest]);
    _linswp[Nest] = false;
    break;
  case DECADE:
    if (_step[Nest] == 0.) {untested();
      _step[Nest] = 1.;
    }else{
    }
    _step[Nest] = pow(10., 1./_step[Nest]);
    _linswp[Nest] = false;
    break;
  };
  
  if (_step[Nest] == 0.) {	// prohibit log sweep from 0
    _step[Nest] = _stop[Nest] - _start[Nest];
    _linswp[Nest] = true;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DCOP::options(CS& Cmd, int Nest)
{
  _sim->_uic = _loop[Nest] = _reverse_in[Nest] = false;
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || (Cmd.match1("'\"({")	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Cmd.is_float()	&& ((Cmd >> _step_in[Nest]), (_stepmode[Nest] = LIN_STEP)))
      || (Get(Cmd, "*",		  &_step_in[Nest]) && (_stepmode[Nest] = TIMES))
      || (Get(Cmd, "+",		  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "by",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "step",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_STEP))
      || (Get(Cmd, "d{ecade}",	  &_step_in[Nest]) && (_stepmode[Nest] = DECADE))
      || (Get(Cmd, "ti{mes}",	  &_step_in[Nest]) && (_stepmode[Nest] = TIMES))
      || (Get(Cmd, "lin",	  &_step_in[Nest]) && (_stepmode[Nest] = LIN_PTS))
      || (Get(Cmd, "o{ctave}",	  &_step_in[Nest]) && (_stepmode[Nest] = OCTAVE))
      || Get(Cmd, "c{ontinue}",   &_cont)
      || Get(Cmd, "dt{emp}",	  &(_sim->_temp_c),   mOFFSET, OPT::temp_c)
      || Get(Cmd, "lo{op}", 	  &_loop[Nest])
      || Get(Cmd, "re{verse}",	  &_reverse_in[Nest])
      || Get(Cmd, "te{mperature}",&(_sim->_temp_c))
      || (Cmd.umatch("tr{ace} {=}") &&
	  (ONE_OF
	   || Set(Cmd, "n{one}",      &_trace, tNONE)
	   || Set(Cmd, "o{ff}",       &_trace, tNONE)
	   || Set(Cmd, "w{arnings}",  &_trace, tUNDER)
	   || Set(Cmd, "i{terations}",&_trace, tITERATION)
	   || Set(Cmd, "v{erbose}",   &_trace, tVERBOSE)
	   || Cmd.warn(bWARNING, 
		       "need none, off, warnings, iterations, verbose")
	   )
	  )
      || outset(Cmd,&_out)
      ;
  }while (Cmd.more() && !Cmd.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void DCOP::sweep()
{
  head(_start[0], _stop[0], " ");
  _sim->_bypass_ok = false;
  _sim->set_inc_mode_bad();
  if (_cont) {untested();
    _sim->restore_voltages();
    CARD_LIST::card_list.tr_restore();
  }else{
    _sim->clear_limit();
    CARD_LIST::card_list.tr_begin();
  }
  sweep_recursive(_n_sweeps);
}
/*--------------------------------------------------------------------------*/
void DCOP::sweep_recursive(int Nest)
{
  --Nest;
  assert(Nest >= 0);
  assert(Nest < DCNEST);

  OPT::ITL itl = OPT::DCBIAS;
  
  first(Nest);
  do {
    if (Nest == 0) {
      if (_sim->command_is_op()) {
	CARD_LIST::card_list.precalc_last();
      }else{
      }
      int converged = solve_with_homotopy(itl,_trace);
      if (!converged) {untested();
	error(bWARNING, "did not converge\n");
      }else{
      }
      ::status.accept.start();
      _sim->set_limit();
      CARD_LIST::card_list.tr_accept();
      ::status.accept.stop();
      _sim->_has_op = _sim->_mode;
      _sim->keep_voltages();
      outdata(*_sweepval[Nest]);
      itl = OPT::DCXFER;
    }else{
      sweep_recursive(Nest);
    }
  } while (next(Nest));
}
/*--------------------------------------------------------------------------*/
void DCOP::first(int Nest)
{
  assert(Nest >= 0);
  assert(Nest < DCNEST);
  assert(_start);
  assert(_sweepval);
  assert(_sweepval[Nest]);

  if (ELEMENT* c = dynamic_cast<ELEMENT*>(_zap[Nest])) {
    c->set_constant(false); 
    // because of extra precalc_last could set constant to true
    // will be obsolete, once pointer hack is fixed
  }else{
    // not needed if not sweeping an element
  }

  *_sweepval[Nest] = _start[Nest];
  _reverse[Nest] = false;
  if (_reverse_in[Nest]) {untested();
    while (next(Nest)) {untested();
      /* nothing */;
    }
    _reverse[Nest] = true;
    next(Nest);
  }else{
  }
  _sim->_phase = p_INIT_DC;
}
/*--------------------------------------------------------------------------*/
bool DCOP::next(int Nest)
{
  double sweepval = NOT_VALID;
  bool ok = false;

  if (_linswp[Nest]) {
    double fudge = _step[Nest] / 10.;
    if (_step[Nest] == 0.) {
      // not stepping
      assert(!ok);
      assert(sweepval == NOT_VALID);
    }else{
      // stepping
      if (!_reverse[Nest]) {
	sweepval = *(_sweepval[Nest]) + _step[Nest];
	fixzero(&sweepval, _step[Nest]);
	ok = in_order(_start[Nest]-fudge, sweepval, _stop[Nest]+fudge);
	if (!ok  &&  _loop[Nest]) {
	  // turn around
	  _reverse[Nest] = true;
	}else{
	  // forward
	}
      }else{
	assert(_reverse[Nest]);
	assert(!ok);
	assert(sweepval == NOT_VALID);
      }
      if (_reverse[Nest]) {
	assert(!ok);
	//assert(sweepval == NOT_VALID);
	sweepval = *(_sweepval[Nest]) - _step[Nest];
	fixzero(&sweepval, _step[Nest]);
	ok = in_order(_start[Nest]-fudge, sweepval, _stop[Nest]+fudge);
      }else{
	// not sure of status
      }
    }
  }else{
    // not linswp
    double fudge = pow(_step[Nest], .1);
    if (_step[Nest] == 1.) {untested();
      // not stepping
      assert(!ok);
      assert(sweepval == NOT_VALID);
    }else{
      if (!_reverse[Nest]) {
	sweepval = *(_sweepval[Nest]) * _step[Nest];
	ok = in_order(_start[Nest]/fudge, sweepval, _stop[Nest]*fudge);
	if (!ok  &&  _loop[Nest]) {untested();
	  // turn around
	  _reverse[Nest] = true;
	}else{
	  // forward
	}
      }else{untested();
	assert(_reverse[Nest]);
	assert(!ok);
	assert(sweepval == NOT_VALID);
      }
      if (_reverse[Nest]) {untested();
	assert(!ok);
	assert(sweepval == NOT_VALID);
	sweepval = *(_sweepval[Nest]) / _step[Nest];
	ok = in_order(_start[Nest]/fudge, sweepval, _stop[Nest]*fudge);
      }else{
	// not sure of status
      }
    }
  }
  _sim->_phase = p_DC_SWEEP;
  if (ok) {
    assert(sweepval != NOT_VALID);
    *(_sweepval[Nest]) = sweepval;
    return true;
  }else{
    //assert(sweepval == NOT_VALID);
    return false;
  }
}
/*--------------------------------------------------------------------------*/
static DC p2;
static OP p4;
static DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "dc", &p2);
static DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "op", &p4);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
