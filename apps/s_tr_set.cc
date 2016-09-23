/*$Id: s_tr_set.cc 2016/09/22 al $ -*- C++ -*-
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
 * set up transient and fourier analysis
 */
//testing=script 2016.08.01
#include "u_sim_data.h"
#include "u_prblst.h"
#include "ap.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::setup(CS&);
//	void	TRANSIENT::options(CS&);
/*--------------------------------------------------------------------------*/
/* tr_setup: transient analysis: parse command string and set options
 * 	(options set by call to tr_options)
 */
void TRANSIENT::setup(CS& Cmd)
{
  _tstart.e_val(NOT_INPUT, _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  _tstrobe.e_val(NOT_INPUT, _scope);

  if (_sim->is_first_expand()) {
    _sim->_last_time = 0;
  }else{
  }

  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    PARAMETER<double> arg1, arg2, arg3;
    Cmd >> arg1;
    if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
      Cmd >> arg2;
    }else{
    }
    if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
      Cmd >> arg3;
    }else{
    }
    
    if (arg3.has_hard_value()) {	    /* 3 args: all */
      assert(arg2.has_hard_value());
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg3.e_val(0.,_scope);
      if (arg3 == 0.) {			    /* spice (illogical) order */
	_tstart = arg3;		    	    /* _tstrobe _tstop _tstart */
	_tstop  = arg2;
	_tstrobe  = arg1;
      }else if (arg1 == 0.) {		    /* eca (logical) order: */
	_tstart = arg1;			    /* _tstart _tstop _tstrobe */
	_tstop  = arg2;				
	_tstrobe  = arg3;
      }else if (arg1 > arg3) {untested();   /* eca (logical) order: */
	_tstart = arg1;			    /* _tstart _tstop _tstrobe */
	_tstop  = arg2;				
	_tstrobe  = arg3;
      }else{				    /* spice (illogical) order */
	_tstart = arg3;		    	    /* _tstrobe _tstop _tstart */
	_tstop  = arg2;
	_tstrobe  = arg1;
      }
    }else if (arg2.has_hard_value()) {	    /* 2 args */
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg2.e_val(0.,_scope);
      if (arg1 == 0.) {		 	    /* 2 args: _tstart, _tstop */
	_tstart = arg1;
	_tstop  = arg2;
	/* _tstrobe unchanged */
      }else if (arg1 >= arg2) {		    /* 2 args: _tstop, _tstrobe */
	_tstart = _sim->_last_time;
	_tstop  = arg1;
	_tstrobe  = arg2;
      }else{				    /* 2 args: _tstrobe, _tstop */
	assert(arg1 < arg2);	   	    /* spice order */
	_tstart = "NA"; /* 0 */
	_tstop  = arg2;
	_tstrobe  = arg1;
      }
    }else{				    /* 1 arg */
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      if (arg1 > _sim->_last_time) {	    /* 1 arg: _tstop */
	_tstart = _sim->_last_time;
	_tstop  = arg1;
	/* _tstrobe unchanged */
      }else if (arg1 == 0.) {untested();    /* 1 arg: _tstart */
	double oldrange = _tstop - _tstart;
	_tstart = 0.;
	_tstop  = oldrange;
	/* _tstrobe unchanged */
      }else{untested();			     /* 1 arg: _tstrobe */
	assert(arg1 <= _sim->_last_time);
	assert(arg1 > 0.);
	double oldrange = _tstop - _tstart;
	_tstart = _sim->_last_time;
	_tstop  = _sim->_last_time + oldrange;
	_tstrobe  = arg1;
      }
    }
  }else{ /* no args */
    double oldrange = _tstop - _tstart;
    _tstart = _sim->_last_time;
    _tstop  = _sim->_last_time + oldrange;
    /* _tstrobe unchanged */
  }
  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    Cmd >> _dtmax_in;
  }else{
  }

  options(Cmd);

  _tstart.e_val(0., _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  if (_tstart < 0 || _tstop <= _tstart) {untested();
    throw Exception("transient: bad time args");
  }else{
  }

  _tstrobe.e_val(NOT_INPUT, _scope);
  if (_tstrobe <= 0.) {
    _tstrobe.set_default(NOT_INPUT);
  }else{
  }
  _tstrobe.e_val(_tstop-_tstart, _scope);

  if  (_cold || _tstart < _sim->_last_time  ||  _sim->_last_time <= 0.) {
    _cont = false;
    _time1 = _sim->_time0 = 0.;
  }else{
    _cont = true;
    _time1 = _sim->_time0 = _sim->_last_time;
  }
  _sim->_freq = ((_tstop > _tstart) ? (1 / (_tstop - _tstart)) : (0.));

  if (_dtmax_in.has_hard_value()) {
    _dtmax = _dtmax_in;
  }else if (_skip_in.has_hard_value()) {
    _dtmax = _tstrobe / double(_skip_in);
  }else{
    _dtmax = std::min(_dtmax_in, _tstrobe);
  }

  if (_dtmin_in.has_hard_value()) {untested();
    _sim->_dtmin = _dtmin_in;
  }else if (_dtratio_in.has_hard_value()) {untested();
    _sim->_dtmin = _dtmax / _dtratio_in;
  }else{
    // use larger of soft values
    _sim->_dtmin = std::max(double(_dtmin_in), _dtmax/_dtratio_in);
  }
}
/*--------------------------------------------------------------------------*/
/* tr_options: set options common to transient and fourier analysis
 */
void TRANSIENT::options(CS& Cmd)
{
  _out = IO::mstdout;
  _out.reset(); //BUG// don't know why this is needed
  _sim->_temp_c = OPT::temp_c;
  bool ploton = IO::plotset  &&  plotlist().size() > 0;
  _sim->_uic = _cold = false;
  _trace = tNONE;
  unsigned here = Cmd.cursor();
  do{
    ONE_OF
      || Get(Cmd, "c{old}",	   &_cold)
      || Get(Cmd, "dte{mp}",	   &_sim->_temp_c,  mOFFSET, OPT::temp_c)
      || Get(Cmd, "dtma{x}",	   &_dtmax_in)
      || Get(Cmd, "dtmi{n}",	   &_dtmin_in)
      || Get(Cmd, "dtr{atio}",	   &_dtratio_in)
      || Get(Cmd, "pl{ot}",	   &ploton)
      || Get(Cmd, "sk{ip}",	   &_skip_in)
      || Get(Cmd, "sta{rt}",	   &_tstart)
      || Get(Cmd, "sto{p}",	   &_tstop)
      || Get(Cmd, "str{obeperiod}",&_tstrobe)
      || Get(Cmd, "te{mperature}", &_sim->_temp_c)
      || Get(Cmd, "uic",	   &_sim->_uic)
      || (Cmd.umatch("tr{ace} {=}") &&
	  (ONE_OF
	   || Set(Cmd, "n{one}",      &_trace, tNONE)
	   || Set(Cmd, "o{ff}",       &_trace, tNONE)
	   || Set(Cmd, "w{arnings}",  &_trace, tUNDER)
	   || Set(Cmd, "a{lltime}",   &_trace, tALLTIME)
	   || Set(Cmd, "r{ejected}",  &_trace, tREJECTED)
	   || Set(Cmd, "i{terations}",&_trace, tITERATION)
	   || Set(Cmd, "v{erbose}",   &_trace, tVERBOSE)
	   || Cmd.warn(bWARNING, "need none, off, warnings, alltime, "
		       "rejected, iterations, verbose")
	   )
	  )
      || outset(Cmd,&_out)
      ;
  }while (Cmd.more() && !Cmd.stuck(&here));
  Cmd.check(bWARNING, "what's this?");

  IO::plotout = (ploton) ? IO::mstdout : OMSTREAM();
  initio(_out);

  _dtmax_in.e_val(BIGBIG, _scope);
  _dtmin_in.e_val(OPT::dtmin, _scope);
  _dtratio_in.e_val(OPT::dtratio, _scope);
  _skip_in.e_val(1, _scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
