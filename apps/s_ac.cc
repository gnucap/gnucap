/*$Id: s_ac.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
#include "u_sim_data.h"
#include "u_status.h"
#include "u_parameter.h"
#include "u_prblst.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class AC : public SIM {
public:
  void	do_it(CS&, CARD_LIST*);

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

  ~AC() {}
private:
  explicit AC(const AC&):SIM() {unreachable(); incomplete();}
  void	sweep();
  void	first();
  bool	next();
  void	solve();
  void	clear();
  void	setup(CS&);
private:
  PARAMETER<double> _start;	// sweep start frequency
  PARAMETER<double> _stop;	// sweep stop frequency
  PARAMETER<double> _step_in;	// step size, as input
  double _step;			// printed step size
  bool	_linswp;		// flag: use linear sweep (vs log sweep)
  bool	_prevopppoint;  	// flag: use previous op point
  enum {ONE_PT, LIN_STEP, LIN_PTS, TIMES, OCTAVE, DECADE} _stepmode;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void AC::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->set_command_ac();
  reset_timers();
  ::status.ac.reset().start();

  _sim->init();
  _sim->alloc_vectors();
  _sim->_acx.reallocate();
  _sim->_acx.set_min_pivot(OPT::pivtol);
  
  setup(Cmd);
  ::status.set_up.stop();
  switch (ENV::run_mode) {
  case rPRE_MAIN:	unreachable();	break;
  case rBATCH:		itested();
  case rINTERACTIVE:	itested();
  case rSCRIPT:		sweep();	break;
  case rPRESET:		/*nothing*/	break;
  }
  _sim->_acx.unallocate();
  _sim->unalloc_vectors();
  
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
  
  //temp_c = OPT::temp_c;
  // Don't set temperature.  Keep whatever was there before,
  // from "op" or whatever.

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
  
  unsigned here = Cmd.cursor();
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
      || Get(Cmd, "dt{emp}",	  &_sim->_temp_c,  mOFFSET, OPT::temp_c)
      || Get(Cmd, "pl{ot}",	  &ploton)
      || Get(Cmd, "pr{evoppoint}",&_prevopppoint)
      || Get(Cmd, "sta{rt}",	  &_start)
      || Get(Cmd, "sto{p}",	  &_stop)
      || Get(Cmd, "te{mperature}",&_sim->_temp_c)
      || outset(Cmd,&_out)
      ;
  }while (Cmd.more() && !Cmd.stuck(&here));
  Cmd.check(bWARNING, "what's this??");

  _start.e_val(0., _scope);
  _stop.e_val(0., _scope);
  _step_in.e_val(0., _scope);
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
  case TIMES:untested();
    if (_step == 0.  &&  _start != 0.) {untested();
      _step = _stop / _start;
    }else{untested();
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
}
/*--------------------------------------------------------------------------*/
void AC::solve()
{
  _sim->_acx.zero();
  std::fill_n(_sim->_ac, _sim->_total_nodes+1, 0.);

  ::status.load.start();
  _sim->count_iterations(iTOTAL);
  CARD_LIST::card_list.do_ac();
  CARD_LIST::card_list.ac_load();
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
  CARD_LIST::card_list.ac_begin();
  do {
    _sim->_jomega = COMPLEX(0., _sim->_freq * M_TWO_PI);
    solve();
    outdata(_sim->_freq);
  } while (next());
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
static DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "ac", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
