/*$Id: s_ac.cc 2016/09/22 al $ -*- C++ -*-
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
 * ac analysis top
 */
//testing=script 2008.08.06
#include "globals.h"
#include "u_sim_data.h"
#include "u_status.h"
#include "u_parameter.h"
#include "u_prblst.h"
#include "e_cardlist.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class AC : public SIM {
  PARAMETER<double> _start;	// sweep start frequency
  PARAMETER<double> _stop;	// sweep stop frequency
  PARAMETER<double> _step_in;	// step size, as input
  double _step;			// printed step size
  bool	_linswp;		// flag: use linear sweep (vs log sweep)
  bool	_prevopppoint;  	// flag: use previous op point
  enum {ONE_PT, LIN_STEP, LIN_PTS, TIMES, OCTAVE, DECADE} _stepmode;
public:
  explicit AC():
    SIM(),
    _start(),
    _stop(),
    _step_in(),
    _step(0.),
    _linswp(false),
    _prevopppoint(false),
    _stepmode(ONE_PT)
  {}
private:
  explicit AC(const AC&a) :
    SIM(a),
    _start(a._start),
    _stop(a._stop),
    _step_in(a._step_in),
    _step(a._step),
    _linswp(a._linswp),
    _prevopppoint(a._prevopppoint),
    _stepmode(a._stepmode)
  {untested();}
  // CARD* clone()const override {return new AC(*this);}
public:
  ~AC() {}
public:
  void	do_it(CS&, CARD_LIST*)override;
  void	setup(CS&)override;
  void	allocate()override;
  void	sweep()override;
  void	first();
  bool	next();
  void	solve();
  void	final()override		{_scope->ac_final();}
  void	finish()override	{}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void AC::allocate()
{
  assert(_sim);
  _sim->alloc_vectors();
  _sim->_acx.reallocate();
  _sim->_acx.set_min_pivot(OPT::pivtol);
}
/*--------------------------------------------------------------------------*/
void AC::do_it(CS& Cmd, CARD_LIST* Scope)
{
  assert(Scope);
  if (Scope == &CARD_LIST::card_list) {
  }else{untested();
  }
  _scope = Scope;
  _sim->set_command_ac();
  command_base(Cmd);
  _scope = nullptr;
  ::status.ac.stop();
  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static int needslinfix;	// flag: lin option needs patch later (spice compat)
/*--------------------------------------------------------------------------*/
void AC::setup(CS& Cmd)
{
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed
  
  double temp = _scope->params()->temperature();
  double temp_k;
  if(temp!=NOT_INPUT){
    // Don't set temperature.  Keep whatever was there before,
    // from "op" or whatever.
    temp_k = temp;
  }else{
    temp_k = OPT::temp_k;
  }

  bool ploton = IO::plotset  &&  plotlist().size() > 0;

  ONE_OF
    || (Get(Cmd, "*",		&_step_in) && (_stepmode = TIMES))
    || (Get(Cmd, "+",		&_step_in) && (_stepmode = LIN_STEP))
    || (Get(Cmd, "by",		&_step_in) && (_stepmode = LIN_STEP))
    || (Get(Cmd, "step",	&_step_in) && (_stepmode = LIN_STEP))
    || (Get(Cmd, "d{ecade}",	&_step_in) && (_stepmode = DECADE))
    || (Get(Cmd, "ti{mes}",	&_step_in) && (_stepmode = TIMES))
    || (Get(Cmd, "lin",		&_step_in) && (_stepmode = LIN_PTS))
    || (Get(Cmd, "o{ctave}",	&_step_in) && (_stepmode = OCTAVE));
  
  if (Cmd.match1("'\"({") || Cmd.is_float()) {
    Cmd >> _start;
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> _stop;
    }else{
      _stop = _start;
    }
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      _stepmode = LIN_STEP;
      Cmd >> _step_in;
    }else{
    }
  }
  
  size_t here = Cmd.cursor();
  do{
    ONE_OF
      || (Get(Cmd, "*",		  &_step_in) && (_stepmode = TIMES))
      || (Get(Cmd, "+",		  &_step_in) && (_stepmode = LIN_STEP))
      || (Get(Cmd, "by",	  &_step_in) && (_stepmode = LIN_STEP))
      || (Get(Cmd, "step",	  &_step_in) && (_stepmode = LIN_STEP))
      || (Get(Cmd, "d{ecade}",	  &_step_in) && (_stepmode = DECADE))
      || (Get(Cmd, "ti{mes}",	  &_step_in) && (_stepmode = TIMES))
      || (Get(Cmd, "lin",	  &_step_in) && (_stepmode = LIN_PTS))
      || (Get(Cmd, "o{ctave}",	  &_step_in) && (_stepmode = OCTAVE))
      || Get(Cmd, "dt{emp}",	  &temp_k, mOFFSET, OPT::temp_k)
      || Get(Cmd, "pl{ot}",	  &ploton)
      || Get(Cmd, "pr{evoppoint}",&_prevopppoint)
      || Get(Cmd, "sta{rt}",	  &_start)
      || Get(Cmd, "sto{p}",	  &_stop)
      || Get(Cmd, "te{mperature}",&temp_k, mOFFSET, P_CELSIUS0)
      || Get(Cmd, "$temperature", &temp_k)
      || outset(Cmd,&_out)
      ;
  }while (Cmd.more() && !Cmd.stuck(&here));
  Cmd.check(bWARNING, "what's this??");

  assert(_scope);
  _start.e_val(0., _scope->params());
  _stop.e_val(0., _scope->params());
  _step_in.e_val(0., _scope->params());
  _step = _step_in;

  switch (_stepmode) {
  case ONE_PT:
  case LIN_STEP:
    needslinfix = false;
    _linswp = true;
    break;
  case LIN_PTS:untested();
    if (_step <= 2.) {untested();// need to fix step, later
      _step = 2.;		// do it at the end of setup
    }else{untested();		// a kluge, but this is a patch
    }
    needslinfix = true;		// and I am too lazy to do it
    _linswp = true;		// right.
    break;
  case TIMES:
    if (_step == 0.  &&  _start != 0.) {untested();
      _step = _stop / _start;
    }else{
    }
    needslinfix = false;
    _linswp = false;
    break;
  case OCTAVE:
    if (_step == 0.) {untested();
      _step = 1.;
    }else{
    }
    _step = pow(2.00000001, 1./_step);
    needslinfix = false;
    _linswp = false;
    break;
  case DECADE:
    if (_step == 0.) {
      _step = 1.;
    }else{
    }
    _step = pow(10., 1./_step);
    needslinfix = false;
    _linswp = false;
    break;
  };

  if (needslinfix) {untested();		// LIN option is # of points.
    assert(_step >= 2);			// Must compute step after 
    _step=(_stop-_start)/(_step-1.);	// reading start and stop,
    needslinfix = false;		// but step must be read first
  }else{			// for Spice compatibility
  }		
  if (_step==0.) {
    _step = _stop - _start;
    _linswp = true;
  }else{
  }

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);
  _scope->params()->set("$temperature", temp_k);
}
/*--------------------------------------------------------------------------*/
void AC::solve()
{
  _sim->_acx.zero();
  std::fill_n(_sim->_ac, _sim->_total_nodes+1, 0.);

  ::status.load.start();
  _sim->count_iterations(iTOTAL);
  _scope->do_ac();
  while (!_sim->_late_evalq.empty()) { //BUG// encapsulation violation
    _sim->_late_evalq.front()->do_ac_last();
    _sim->_late_evalq.pop_front();
  }
  _scope->ac_load();
  ::status.load.stop();

  ::status.lud.start();
  _sim->_acx.lu_decomp();
  ::status.lud.stop();

  ::status.back.start();
  _sim->_acx.fbsub(_sim->_ac);
  ::status.back.stop();
}
/*--------------------------------------------------------------------------*/
void AC::sweep()
{
  head(_start, _stop, "Freq");
  first();
  _scope->ac_begin();
  try {
    do {
      _sim->_jomega = COMPLEX(0., _sim->_freq * M_TWO_PI);
      solve();
      outdata(_sim->_freq, ofPRINT | ofSTORE);
      _sim->_has_op = s_AC;
    } while (next());
  }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
  }
}
/*--------------------------------------------------------------------------*/
void AC::first()
{
  _sim->_freq = _start;
}
/*--------------------------------------------------------------------------*/
bool AC::next()
{
  double realstop = (_linswp)
    ? _stop - _step/100.
    : _stop / pow(_step,.01);
  if (!in_order(double(_start), _sim->_freq, realstop)) {
    return false;
  }else{
  }

  _sim->_freq = (_linswp)
    ? _sim->_freq + _step
    : _sim->_freq * _step;
  if (in_order(_sim->_freq, double(_start), double(_stop))) {
    return false;
  }else{
    return true;
  }
}
/*--------------------------------------------------------------------------*/
static AC p1;
static DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "ac|`ac", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
