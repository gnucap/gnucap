/*$Id: s_tr_set.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
//testing=script 2007.11.21
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
  _tstep.e_val(NOT_INPUT, _scope);

  _cont = true;
  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    PARAMETER<double> arg1, arg2, arg3;
    Cmd >> arg1;
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg2;
    }else{itested();
    }
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg3;
    }else{
    }
    
    if (arg3.has_hard_value()) {	    /* 3 args: all */
      assert(arg2.has_hard_value());
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg3.e_val(0.,_scope);
      if (arg3 == 0.) {			    /* spice (illogical) order */
	_tstart = arg3;		    	    /* _tstep _tstop _tstart */
	_tstop  = arg2;
	_tstep  = arg1;
      }else if (arg1 == 0.) {		    /* eca (logical) order: */
	_tstart = arg1;			    /* _tstart _tstop _tstep */
	_tstop  = arg2;				
	_tstep  = arg3;
      }else if (arg1 > arg3) {untested();   /* eca (logical) order: */
	_tstart = arg1;			    /* _tstart _tstop _tstep */
	_tstop  = arg2;				
	_tstep  = arg3;
      }else{untested(); 		    /* spice (illogical) order */
	_tstart = arg3;		    	    /* _tstep _tstop _tstart */
	_tstop  = arg2;
	_tstep  = arg1;
      }
    }else if (arg2.has_hard_value()) {	    /* 2 args */
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg2.e_val(0.,_scope);
      if (arg1 == 0.) {untested(); 	    /* 2 args: _tstart, _tstop */
	_tstart = arg1;
	_tstop  = arg2;
	/* _tstep unchanged */
      }else if (arg1 >= arg2) {		    /* 2 args: _tstop, _tstep */
	_tstart = _sim->_last_time;
	_tstop  = arg1;
	_tstep  = arg2;
      }else{ /* arg1 < arg2 */		    /* 2 args: _tstep, _tstop */
	_tstart = "NA"; /* 0 */	   	    /* spice order */
	_tstop  = arg2;
	_tstep  = arg1;
      }
    }else{itested();
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      if (arg1 > _sim->_last_time) {untested();	    /* 1 arg: _tstop */
	_tstart = _sim->_last_time;
	_tstop  = arg1;
	/* _tstep unchanged */
      }else if (arg1 == 0.) {itested();	    /* 1 arg: _tstart */
	double oldrange = _tstop - _tstart;
	_tstart = 0.;
	_tstop  = oldrange;
	/* _tstep unchanged */
      }else{untested(); /* arg1 < _sim->_last_time, but not 0 */  /* 1 arg: _tstep */
	double oldrange = _tstop - _tstart;
	_tstart = _sim->_last_time;
	_tstop  = _sim->_last_time + oldrange;
	_tstep  = arg1;
      }
    }
  }else{ /* no args */
    double oldrange = _tstop - _tstart;
    _tstart = _sim->_last_time;
    _tstop  = _sim->_last_time + oldrange;
    /* _tstep unchanged */
  }
  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    Cmd >> _dtmax_in;
  }else{
  }
  options(Cmd);

  _tstart.e_val(0., _scope);
  _tstop.e_val(NOT_INPUT, _scope);
  _tstep.e_val(NOT_INPUT, _scope);

  if  (_cold || _tstart < _sim->_last_time  ||  _sim->_last_time <= 0.) {
    _cont = false;
    time1 = _sim->_time0 = 0.;
  }else{
    _cont = true;
    time1 = _sim->_time0 = _sim->_last_time;
  }
  _sim->_freq = ((_tstop > _tstart) ? (1 / (_tstop - _tstart)) : (0.));

  if (!_tstep.has_good_value()) {
    throw Exception("transient: time step is required");
  }else if (_tstep==0.) {itested();
    throw Exception("time step = 0");
  }else{
  }

  if (_dtmax_in.has_hard_value()) {
    _dtmax = _dtmax_in;
  }else if (_skip_in.has_hard_value()) {
    _dtmax = _tstep / double(_skip_in);
  }else{
    _dtmax = std::min(_dtmax_in, _tstep);
  }

  if (_dtmin_in.has_hard_value()) {
    _sim->_dtmin = _dtmin_in;
  }else if (_dtratio_in.has_hard_value()) {
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
