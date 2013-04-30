/*$Id: e_elemnt.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
//testing=script 2006.07.12
#include "m_divdiff.h"
#include "u_xprobe.h"
#include "e_aux.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
ELEMENT::ELEMENT()
  :COMPONENT(),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(0.),
   _loss1(0.),
   _acg(0.),
   _ev(0.),
   _dt(0.)
{
  _n = _nodes;
  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  std::fill_n(_time, int(OPT::_keep_time_steps), 0.);
}
/*--------------------------------------------------------------------------*/
ELEMENT::ELEMENT(const ELEMENT& p)
  :COMPONENT(p),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(p._loss0),
   _loss1(p._loss1),
   _acg(0.),
   _ev(0.),
   _dt(0.)
{
  trace0(long_label().c_str());
  _n = _nodes;
  if (p._n == p._nodes) {
    for (int ii = 0;  ii < NODES_PER_BRANCH;  ++ii) {
      _n[ii] = p._n[ii];
    }
  }else{
    assert(p._nodes);
    // the constructor for a derived class will take care of it
  }
  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  notstd::copy_n(p._time, int(OPT::_keep_time_steps), _time);
}
/*--------------------------------------------------------------------------*/
bool ELEMENT::skip_dev_type(CS& cmd)
{
  return cmd.umatch(dev_type() + ' ');
}
/*--------------------------------------------------------------------------*/
void ELEMENT::precalc_last()
{
  COMPONENT::precalc_last();

  //BUG// This is needed for AC analysis without doing op (or dc or tran ...) first.
  // Something like it should be moved to ac_begin.
  if (_sim->is_first_expand()) {
    _y[0].x  = 0.;
    _y[0].f0 = LINEAR;
    _y[0].f1 = value();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_begin()
{
  _time[0] = 0.;
  _y[0].x  = 0.;
  _y[0].f0 = LINEAR;
  _y[0].f1 = value();
  _y1 = _y[0];
  for (int i=1; i<OPT::_keep_time_steps; ++i) {
    _time[i] = 0.;
    _y[i] = FPOLY1(0., 0., 0.);
  }
  _dt = NOT_VALID;
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_restore()
{
  if (_time[0] > _sim->_time0) {itested();
    for (int i=0  ; i<OPT::_keep_time_steps-1; ++i) {itested();
      _time[i] = _time[i+1];
      _y[i] = _y[i+1];
    }
    _time[OPT::_keep_time_steps-1] = 0.;
    _y[OPT::_keep_time_steps-1]    = FPOLY1(0., 0., 0.);
  }else if (_time[0] == _sim->_time0) {
  }else{untested();
  }

  //assert(_time[0] == _sim->_time0);
  if (_time[0] != _sim->_time0) {itested();
    error(bDANGER, "//BUG// restore time mismatch.  last=%g, using=%g\n",
	  _time[0], _sim->_time0);
    //BUG// happens when continuing after a ^c,
    // when the last step was not printed
    // _time[0] is the non-printed time.  _sim->_time0 is the printed time.
  }else{
  }

  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::dc_advance()
{
  assert(_sim->_time0 == 0.); // DC

  for (int i=OPT::_keep_time_steps-1; i>=0; --i) {
    assert(_time[i] == 0.);
  }

  _dt = NOT_VALID;
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_advance()
{
  assert(_time[0] < _sim->_time0); // moving forward
  
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
    _time[i] = _time[i-1];
    _y[i] = _y[i-1];
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_regress()
{
  assert(_time[0] >= _sim->_time0); // moving backwards
  assert(_time[1] <= _sim->_time0); // but not too far backwards

  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    assert(_time[i] < _time[i-1] || _time[i] == 0.);
  }
  _time[0] = _sim->_time0;

  _dt = _time[0] - _time[1];
}
/*--------------------------------------------------------------------------*/
TIME_PAIR ELEMENT::tr_review()
{
  COMPONENT::tr_review();
  if (order() > 0 && _y[0].f0 != LINEAR) {
    double timestep = tr_review_trunc_error(_y);
    double newtime = tr_review_check_and_convert(timestep);
    _time_by.min_error_estimate(newtime);
  }else{
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_iwant_matrix_passive()
{
  assert(matrix_nodes() == 2);
  assert(is_device());
  //assert(!subckt()); ok for subckt to exist for logic
  trace2(long_label().c_str(), _n[OUT1].m_(), _n[OUT2].m_());

  assert(_n[OUT1].m_() != INVALID_NODE);
  assert(_n[OUT2].m_() != INVALID_NODE);

  _sim->_aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_iwant_matrix_active()
{
  assert(matrix_nodes() == 4);
  assert(is_device());
  assert(!subckt());

  assert(_n[OUT1].m_() != INVALID_NODE);
  assert(_n[OUT2].m_() != INVALID_NODE);
  assert(_n[IN1].m_() != INVALID_NODE);
  assert(_n[IN2].m_() != INVALID_NODE);

  //_sim->_aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_aa.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_aa.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_aa.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_aa.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_aa.iwant(_n[IN1].m_(),_n[IN2].m_());

  //_sim->_lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_lu.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_lu.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_lu.iwant(_n[IN1].m_(),_n[IN2].m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0;  ii < matrix_nodes();  ++ii) {
    if (_n[ii].m_() >= 0) {
      for (int jj = 0;  jj < ii ;  ++jj) {
	_sim->_aa.iwant(_n[ii].m_(),_n[jj].m_());
	_sim->_lu.iwant(_n[ii].m_(),_n[jj].m_());
      }
    }else{itested();
      // node 1 is grounded or invalid
    }
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_passive()
{
  trace2(long_label().c_str(), _n[OUT1].m_(), _n[OUT2].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[OUT2].m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_active()
{
  //_sim->_acx.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[IN1].m_());
  _sim->_acx.iwant(_n[OUT1].m_(),_n[IN2].m_());
  _sim->_acx.iwant(_n[OUT2].m_(),_n[IN1].m_());
  _sim->_acx.iwant(_n[OUT2].m_(),_n[IN2].m_());
  //_sim->_acx.iwant(_n[IN1].m_(),_n[IN2].m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0;  ii < matrix_nodes();  ++ii) {
    if (_n[ii].m_() >= 0) {
      for (int jj = 0;  jj < ii ;  ++jj) {
	_sim->_acx.iwant(_n[ii].m_(),_n[jj].m_());
      }
    }else{itested();
      // node 1 is grounded or invalid
    }
  }
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_amps()const
{
  trace5("", _loss0, tr_outvolts(), _m0.c1, tr_involts(), _m0.c0);
  return fixzero((_loss0 * tr_outvolts() + _m0.c1 * tr_involts() + _m0.c0),
  		 _m0.c0);
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v{out} ")) {
    return tr_outvolts();
  }else if (Umatch(x, "vi{n} ")) {
    return tr_involts();
  }else if (Umatch(x, "i ")) {
    return tr_amps();
  }else if (Umatch(x, "p ")) {
    return tr_amps() * tr_outvolts();
  }else if (Umatch(x, "pd ")) {
    double p = tr_amps() * tr_outvolts();
    return (p > 0.) ? p : 0.;
  }else if (Umatch(x, "ps ")) {
    double p = tr_amps() * tr_outvolts();
    return (p < 0.) ? -p : 0.;
  }else if (Umatch(x, "in{put} ")) {
    return _y[0].x;
  }else if (Umatch(x, "f ")) {
    if (_y[0].f0 == LINEAR) {itested();
      return _y[0].x * _y[0].f1;
    }else{
      return _y[0].f0;
    }
  }else if (Umatch(x, "ev |df ")) {
    return _y[0].f1;
  }else if (Umatch(x, "nv ")) {
    return value();
  }else if (Umatch(x, "eiv ")) {untested();
    return _m0.x;
  }else if (Umatch(x, "y ")) {
    return _m0.c1;
  }else if (Umatch(x, "is{tamp} ")) {
    return _m0.f0();
  }else if (Umatch(x, "iof{fset} ")) {itested();
    return _m0.c0;
  }else if (Umatch(x, "ip{assive} ")) {itested();
    return _m0.c1 * tr_involts();
  }else if (Umatch(x, "il{oss} ")) {untested();
    return _loss0 * tr_outvolts();
  }else if (Umatch(x, "dt ")) {
    return _dt;
  }else if (Umatch(x, "dtr{equired} ")) {
    return ((_time_by._error_estimate - _time[0]) > 0)
      ? _time_by._error_estimate - _time[0]
      : _time_by._error_estimate - _time[1];
  }else if (Umatch(x, "time ")) {untested();
    return _time[0];
  }else if (Umatch(x, "timeo{ld} ")) {untested();
    return _time[1];
  //}else if (Umatch(x, "didt ")) {untested();
    //double i0  = (_m0.c1  * _m0.x  + _m0.c0);
    //double it1 = (mt1.f1 * mt1.x + mt1.c0);
    //return  (i0 - it1) / (_sim->_time0 - _time1);
  }else if (Umatch(x, "r ")) {
    return (_m0.c1!=0.) ? 1/_m0.c1 : MAXDBL;
  }else if (Umatch(x, "z ")) {
    return port_impedance(_n[OUT1], _n[OUT2], _sim->_lu, mfactor()*(_m0.c1+_loss0));
  }else if (Umatch(x, "zraw ")) {
    return port_impedance(_n[OUT1], _n[OUT2], _sim->_lu, 0.);
  }else{
    return COMPONENT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
COMPLEX ELEMENT::ac_amps()const
{
  assert(!is_source());
  return (ac_involts() * _acg + ac_outvolts() * _loss0);
}
/*--------------------------------------------------------------------------*/
XPROBE ELEMENT::ac_probe_ext(const std::string& x)const
{
  COMPLEX admittance = (is_source()) ? _loss0 : _acg+_loss0;

  if (Umatch(x, "v{out} ")) {			/* volts (out) */
    return XPROBE(ac_outvolts());
  }else if (Umatch(x, "vin ")) {		/* volts (in) */
    return XPROBE(ac_involts());
  }else if (Umatch(x, "i ")) {			/* amps */
    return XPROBE(ac_amps());
  }else if (Umatch(x, "p ")) {			/* complex "power" */
    return XPROBE(ac_outvolts() * conj(ac_amps()), mtREAL, 10.);
  }else if (Umatch(x, "nv ")) {untested();	/* nominal value */
    return XPROBE(value());
  }else if (Umatch(x, "ev ")) {			/* effective value */
    return XPROBE(_ev);
  }else if (Umatch(x, "y ")) {untested();		/* admittance */
    return XPROBE(admittance, mtREAL);
  }else if (Umatch(x, "r ")) {			/* complex "resistance" */
    if (admittance == 0.) {untested();
      return XPROBE(MAXDBL);
    }else{untested();
      return XPROBE(1. / admittance);
    }
  }else if (Umatch(x, "z ")) {			/* port impedance */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], _sim->_acx, mfactor()*admittance));
  }else if (Umatch(x, "zraw ")) {		/* port impedance, raw */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], _sim->_acx, COMPLEX(0.)));
  }else{ 					/* bad parameter */
    return COMPONENT::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_review_trunc_error(const FPOLY1* q)
{
  int error_deriv = order()+1;
  double timestep;
  if (_time[0] <= 0.) {
    // DC, I know nothing
    timestep = NEVER;
  }else if (_time[error_deriv] <= 0.) {
    // first few steps, I still know nothing
    // repeat whatever step was used the first time
    timestep = _dt;
  }else{
    for (int i=error_deriv; i>0; --i) {
      assert(_time[i] < _time[i-1]); // || _time[i] == 0.);
    }

    double c[OPT::_keep_time_steps];
    for (int i=0; i<OPT::_keep_time_steps; ++i) {
      c[i] = q[i].f0;
    }
    assert(error_deriv < OPT::_keep_time_steps);
    derivatives(c, OPT::_keep_time_steps, _time);
    // now c[i] is i'th derivative
    
    assert(OPT::_keep_time_steps >= 5);
    trace0(("ts" + long_label()).c_str());
    trace5("time", _time[0], _time[1], _time[2], _time[3], _time[4]);
    trace5("charge", q[0].f0, q[1].f0, q[2].f0, q[3].f0, q[4].f0);
    trace5("deriv", c[0], c[1], c[2], c[3], c[4]);
    
    if (c[error_deriv] == 0) {
      timestep = NEVER;
    }else{
      double chargetol = std::max(OPT::chgtol,
	OPT::reltol * std::max(std::abs(q[0].f0), std::abs(q[1].f0)));
      double tol = OPT::trtol * chargetol;
      double denom = error_factor() * std::abs(c[error_deriv]);
      assert(tol > 0.);
      assert(denom > 0.);
      switch (error_deriv) { // pow is slow.
      case 1:	timestep = tol / denom;		break;
      case 2:	timestep = sqrt(tol / denom);	break;
      case 3:	timestep = cbrt(tol / denom);	break;
      default:	timestep = pow((tol / denom), 1./(error_deriv)); break;
      }
      trace4("", chargetol, tol, denom, timestep);
    }
  }
  assert(timestep > 0.);
  return timestep;
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_review_check_and_convert(double timestep)
{
  double time_future;
  if (timestep == NEVER) {
    time_future = NEVER;
  }else{
    if (timestep < _sim->_dtmin) {
      timestep = _sim->_dtmin;
    }else{
    }

    if (timestep < _dt * OPT::trreject) {
      if (_time[order()] == 0) {
	error(bWARNING, "initial step rejected:" + long_label() + '\n');
	error(bWARNING, "new=%g  old=%g  required=%g\n",
	      timestep, _dt, _dt * OPT::trreject);
      }else{
	error(bTRACE, "step rejected:" + long_label() + '\n');
	error(bTRACE, "new=%g  old=%g  required=%g\n",
	      timestep, _dt, _dt * OPT::trreject);
      }
      time_future = _time[1] + timestep;
      trace3("reject", timestep, _dt, time_future);
    }else{
      time_future = _time[0] + timestep;
      trace3("accept", timestep, _dt, time_future);
    }
  }
  assert(time_future > 0.);
  assert(time_future > _time[1]);
  return time_future;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
