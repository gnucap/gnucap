/*$Id: s_fo.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * tran and fourier commands -- top
 * performs transient analysis, silently, then fft.
 * outputs results of fft
 */
//testing=script 2007.11.21
#include "u_sim_data.h"
#include "u_status.h"
#include "m_phase.h"
#include "declare.h"	/* plclose, plclear, fft */
#include "u_prblst.h"
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class FOURIER : public TRANSIENT {
public:
  void	do_it(CS&, CARD_LIST*);
  explicit FOURIER():
    TRANSIENT(),
    _fstart(0.),
    _fstop(0.),
    _fstep(0.),
    _timesteps(0),
    _fdata(NULL)
  {
  }
  ~FOURIER() {}
private:
  explicit FOURIER(const FOURIER&): TRANSIENT() {unreachable(); incomplete();}
  std::string status()const {return "";}
  void	setup(CS&);	/* s_fo_set.cc */
  void	fftallocate();
  void	fftunallocate();
  void	foout();	/* s_fo_out.cc */
  void	fohead(const PROBE&);
  void	foprint(COMPLEX*);
  void	store_results(double); // override virtual
private:
  PARAMETER<double> _fstart;	/* user start frequency */
  PARAMETER<double> _fstop;	/* user stop frequency */
  PARAMETER<double> _fstep;	/* fft frequecncy step */
  int    _timesteps;	/* number of time steps in tran analysis, incl 0 */
  COMPLEX** _fdata;	/* storage to allow postprocessing */
};
/*--------------------------------------------------------------------------*/
static	int	to_pow_of_2(double);
static  int	stepnum(double,double,double);
static	COMPLEX	find_max(COMPLEX*,int,int);
static	double  db(COMPLEX);
/*--------------------------------------------------------------------------*/
void FOURIER::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->set_command_fourier();
  reset_timers();
  ::status.four.reset().start();
  
  try {
    _sim->init();
    _sim->alloc_vectors();    
    _sim->_aa.reallocate();
    _sim->_aa.dezero(OPT::gmin);
    _sim->_aa.set_min_pivot(OPT::pivtol);    
    _sim->_lu.reallocate();
    _sim->_lu.dezero(OPT::gmin);
    _sim->_lu.set_min_pivot(OPT::pivtol);
    
    setup(Cmd);
    fftallocate();
    
    ::status.set_up.stop();
    switch (ENV::run_mode) {untested();
    case rPRE_MAIN:	unreachable();		break;
    case rBATCH:	untested();
    case rINTERACTIVE:  itested();
    case rSCRIPT:	sweep(); foout();	break;
    case rPRESET:	untested(); /*nothing*/ break;
    }
  }catch (Exception& e) {itested();
    error(bDANGER, e.message() + '\n');
  }
  
  fftunallocate();
  _sim->unalloc_vectors();
  _sim->_lu.unallocate();
  _sim->_aa.unallocate();
  
  ::status.four.stop();
  ::status.total.stop();
  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* store: stash time domain data in preparation for Fourier Transform
 */
void FOURIER::store_results(double X)
{
  TRANSIENT::store_results(X);

  if (step_cause() == scUSER) {
    int ii = 0;
    for (PROBELIST::const_iterator
	   p=printlist().begin();  p!=printlist().end();  ++p) {
      assert(_stepno < _timesteps);
      _fdata[ii][_stepno] = p->value();
      ++ii;
    }
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
/* foout:  print out the results of the transform
 */
void FOURIER::foout()
{
  plclose();
  plclear();
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=printlist().begin();  p!=printlist().end();  ++p) {
    fohead(*p);
    fft(_fdata[ii], _timesteps-1,  0);
    foprint(_fdata[ii]);
    ++ii;
  }
}
/*--------------------------------------------------------------------------*/
/* fo_head: print output header
 * arg is index into probe array, to select probe name
 */
void FOURIER::fohead(const PROBE& Prob)
{
  _out.form("# %-10s", Prob.label().c_str())
    << "--------- actual ---------  -------- relative --------\n"
    << "#freq       "
    << "value        dB      phase  value        dB      phase\n";
}
/*--------------------------------------------------------------------------*/
/* fo_print: print results of fourier analysis
 * for all points at single probe
 */
void FOURIER::foprint(COMPLEX *Data)
{
  int startstep = stepnum(0., _fstep, _fstart);
  assert(startstep >= 0);
  int stopstep  = stepnum(0., _fstep, _fstop );
  assert(stopstep < _timesteps);
  COMPLEX maxvalue = find_max(Data, std::max(1,startstep), stopstep);
  if (maxvalue == 0.) {untested();
    maxvalue = 1.;
  }else{
  }
  Data[0] /= 2;
  for (int ii = startstep;  ii <= stopstep;  ++ii) {
    double frequency = _fstep * ii;
    assert(ii >= 0);
    assert(ii < _timesteps);
    COMPLEX unscaled = Data[ii];
    COMPLEX scaled = unscaled / maxvalue;
    unscaled *= 2;
    _out.form("%s%s%7.2f %8.3f %s%7.2f %8.3f\n",
	     ftos(frequency,    11,5,_out.format()),
        ftos(std::abs(unscaled),11,5,_out.format()),
	     db(unscaled),
	     phase(unscaled*COMPLEX(0.,1)),
        ftos(std::abs(scaled),  11,5,_out.format()),
	     db(scaled),
	     phase(scaled) ) ;
  }
}
/*--------------------------------------------------------------------------*/
/* stepnum: return step number given its frequency or time
 */
static int stepnum(double Start, double Step, double Here)
{
  return int((Here-Start)/Step + .5);
}
/*--------------------------------------------------------------------------*/
/* find_max: find the max magnitude in a COMPLEX array
 */
static COMPLEX find_max(COMPLEX *Data, int Start, int Stop)
{
  COMPLEX maxvalue = 0.;
  for (int ii = Start;  ii <= Stop;  ++ii) {
    if (std::abs(Data[ii]) > std::abs(maxvalue)) {
      maxvalue = Data[ii];
    }else{
    }
  }
  return maxvalue;
}
/*--------------------------------------------------------------------------*/
static double db(COMPLEX Value)
{
  return  20. * log10(std::max(std::abs(Value),VOLTMIN));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* fo_setup: fourier analysis: parse command string and set options
 * 	(options set by call to TRANSIENT::options)
 */
void FOURIER::setup(CS& Cmd)
{
  _cont = true;
  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    PARAMETER<double> arg1, arg2, arg3;
    Cmd >> arg1;
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg2;
    }else{
    }
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg3;
    }else{
    }
    
    if (arg3.has_hard_value()) {	    /* 3 args: all */
      assert(arg2.has_hard_value());
      assert(arg1.has_hard_value());
      _fstart = arg1;
      _fstop  = arg2;
      _fstep  = arg3;
    }else if (arg2.has_hard_value()) {untested(); /* 2 args: start = 0 */
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg2.e_val(0.,_scope);
      if (arg1 >= arg2) {untested();	    /* 2 args: stop, step */
	_fstart = "NA";		    	    /* 	(stop > step) */
	_fstop  = arg1;
	_fstep  = arg2;
      }else{untested(); /* arg1 < arg2 */    /* 2 args: step, stop */
	_fstart = "NA";
	_fstop  = arg2;
	_fstep  = arg1;
      }
    }else{
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      if (arg1 == 0.) {untested();	    /* 1 arg: start */
	_fstart = 0.;
	/* _fstop unchanged */
	/* _fstep unchanged */
      }else{untested();			    /* 1 arg: step */
	_fstart = "NA";
	_fstop  = "NA";
	_fstep  = arg1;
      }
    }
  }else{itested();
    /* else (no args) : no change */
  }

  options(Cmd);

  _fstart.e_val(0., _scope);
  _fstep.e_val(0., _scope);
  _fstop.e_val(OPT::harmonics * _fstep, _scope);
  
  if (_fstep == 0.) {itested();
    throw Exception("frequency step = 0");
  }else{
  }
  if (_fstop == 0.) {untested();
    _fstop = OPT::harmonics * _fstep;
  }else{
  }

  _timesteps = to_pow_of_2(_fstop*2 / _fstep) + 1;
  if (_cold  ||  _sim->_last_time <= 0.) {
    _cont = false;
    _tstart = 0.;
  }else{
    _cont = true;
    _tstart = _sim->_last_time;
  }
  _tstop = _tstart + 1. / _fstep;
  _tstep = 1. / _fstep / (_timesteps-1);
  time1 = _sim->_time0 = _tstart;

  _sim->_freq = _fstep;

  _dtmax = std::min(double(_dtmax_in), _tstep / double(_skip_in));
  if (_dtmin_in.has_hard_value()) {
    _sim->_dtmin = _dtmin_in;
  }else if (_dtratio_in.has_hard_value()) {
    _sim->_dtmin = _dtmax / _dtratio_in;
  }else{
    // use smaller of soft values
    _sim->_dtmin = std::min(double(_dtmin_in), _dtmax/_dtratio_in);
  }
}
/*--------------------------------------------------------------------------*/
/* allocate:  allocate space for fft
 */
void FOURIER::fftallocate()
{
  int probes = printlist().size();
  _fdata = new COMPLEX*[probes];
  for (int ii = 0;  ii < probes;  ++ii) {
    _fdata[ii] = new COMPLEX[_timesteps+100];
  }
}
/*--------------------------------------------------------------------------*/
/* unallocate:  unallocate space for fft
 */
void FOURIER::fftunallocate()
{
  if (_fdata) {
    for (int ii = 0;  ii < printlist().size();  ++ii) {
      delete [] _fdata[ii];
    }
    delete [] _fdata;
    _fdata = NULL;
  }else{itested();
  }
}
/*--------------------------------------------------------------------------*/
/* to_pow_of_2: round up to nearest power of 2
 * example: z=92 returns 128
 */
static int to_pow_of_2(double Z)
{
  int x = static_cast<int>(floor(Z));
  int y;
  for (y = 1; x > 0; x >>= 1) {
    y <<= 1;
  }
  return y;
}   
/*--------------------------------------------------------------------------*/
static FOURIER p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "fourier", &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
