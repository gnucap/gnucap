/*$Id: e_elemnt.cc 2016/09/26 al $ -*- C++ -*-
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
ELEMENT::ELEMENT(COMMON_COMPONENT* c)
  :COMPONENT(c),
   _value(0),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(0.),
   _loss1(0.),
   _acg(0.),
   _ev(0.),
   _dt(0.)
{
  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  std::fill_n(_time, int(OPT::_keep_time_steps), 0.);
}
/*--------------------------------------------------------------------------*/
ELEMENT::ELEMENT(const ELEMENT& p)
  :COMPONENT(p),
   _value(p._value),
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

  for (int ii = 0;  ii < NODES_PER_BRANCH;  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }

  assert(_y[0].x == 0. && _y[0].f0 == 0. && _y[0].f1 == 0.);
  assert(_y1 == _y[0]);

  notstd::copy_n(p._time, int(OPT::_keep_time_steps), _time);
}
/*--------------------------------------------------------------------------*/
void ELEMENT::set_value(double v, COMMON_COMPONENT* c)
{
  if (c != common()) {
    detach_common();
    attach_common(c);
  }else{
  }
  set_value(v);
}
/*--------------------------------------------------------------------------*/
int ELEMENT::set_param_by_name(std::string Name, std::string Value)
{
  if(Name == value_name()){
    _value = Value;
    return ELEMENT::param_count() - 1; // BUG?
  }else{
    return COMPONENT::set_param_by_name(Name, Value);
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::set_param_by_index(int i, std::string& Value, int offset)
{
  if (has_common()) {itested();
    COMMON_COMPONENT* c = common()->clone();
    assert(c);
    c->set_param_by_index(i, Value, offset);
    attach_common(c);
  }else{
    switch (ELEMENT::param_count() - 1 - i) {
    case 0:
      _value = Value; break;
    default:
      COMPONENT::set_param_by_index(i, Value, offset);
    }
  }
}
/*--------------------------------------------------------------------------*/
bool ELEMENT::param_is_printable(int i)const
{
  if (has_common()) {
    return COMPONENT::param_is_printable(i);
  }else{
    switch (ELEMENT::param_count() - 1 - i) {
    case 0:
      return value().has_hard_value();
    default:
      return COMPONENT::param_is_printable(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string ELEMENT::param_name(int i)const
{
  if (has_common()) {
    return COMPONENT::param_name(i);
  }else{
    switch (ELEMENT::param_count() - 1 - i) {
    case 0:  return value_name();
    default:
      return COMPONENT::param_name(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string ELEMENT::param_name(int i, int j)const
{
  if (has_common()) {untested();
    return COMPONENT::param_name(i);
  }else{
    if (j == 0) {
      return param_name(i);
    }else if (i >= ELEMENT::param_count()) {untested();
      return "";
    }else{
      return COMPONENT::param_name(i,j);
    }
  }
}
/*--------------------------------------------------------------------------*/
std::string ELEMENT::param_value(int i)const
{
  if (has_common()) {
    return COMPONENT::param_value(i);
  }else{
    switch (ELEMENT::param_count() - 1 - i) {
    case 0:
      return value().string();
    default:
      return COMPONENT::param_value(i);
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool ELEMENT::skip_dev_type(CS& cmd)
{
  return cmd.umatch(dev_type() + ' ');
}
/*--------------------------------------------------------------------------*/
void ELEMENT::precalc_last()
{
  COMPONENT::precalc_last();
  _value.e_val(0.,scope());
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
void ELEMENT::ac_begin()
{
  if (_sim->has_op() == s_NONE) {
    _y[0].x  = 0.;
    _y[0].f0 = LINEAR;
    _y[0].f1 = value();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_restore()
{
  if (_time[0] > _sim->_time0) {untested();
    // _freezetime
    incomplete();
    //BUG// wrong values in _time[]
    for (int i=0  ; i<OPT::_keep_time_steps-1; ++i) {untested();
      _time[i] = _time[i+1];
      _y[i] = _y[i+1];
    }
    _time[OPT::_keep_time_steps-1] = 0.;
    _y[OPT::_keep_time_steps-1]    = FPOLY1(0., 0., 0.);
  }else if (_time[0] == _sim->_time0) {
    // the usual continue where the last one left off
  }else{unreachable();
    // skipping ahead, not implemented
  }

  //assert(_time[0] == _sim->_time0);
  if (_time[0] != _sim->_time0) {untested();
    error(bDANGER, "//BUG// restore time mismatch.  last=%g, using=%g\n",
	  _time[0], _sim->_time0);
    //BUG// happens when continuing after a ^c,
    // when the last step was not printed
    //BUG// also happens with _freezetime
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
    assert(_time[i] <= _time[i-1]);
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
    assert(_time[i] <= _time[i-1]);
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
  trace2(long_label().c_str(), n_(OUT1).m_(), n_(OUT2).m_());

  assert(n_(OUT1).m_() != INVALID_NODE);
  assert(n_(OUT2).m_() != INVALID_NODE);
  //BUG// assert can fail as a result of some parse errors

  _sim->_aa.iwant(n_(OUT1).m_(),n_(OUT2).m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_iwant_matrix_active()
{
  assert(matrix_nodes() == 4);
  assert(is_device());
  assert(!subckt());

  assert(n_(OUT1).m_() != INVALID_NODE);
  assert(n_(OUT2).m_() != INVALID_NODE);
  assert(n_(IN1).m_() != INVALID_NODE);
  assert(n_(IN2).m_() != INVALID_NODE);
  //BUG// assert can fail as a result of some parse errors

  //_sim->_aa.iwant(n_(OUT1).m_(),n_(OUT2).m_());
  _sim->_aa.iwant(n_(OUT1).m_(),n_(IN1).m_());
  _sim->_aa.iwant(n_(OUT1).m_(),n_(IN2).m_());
  _sim->_aa.iwant(n_(OUT2).m_(),n_(IN1).m_());
  _sim->_aa.iwant(n_(OUT2).m_(),n_(IN2).m_());
  //_sim->_aa.iwant(n_(IN1).m_(),n_(IN2).m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::tr_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0;  ii < matrix_nodes();  ++ii) {
    if (n_(ii).m_() >= 0) {
      for (int jj = 0;  jj < ii ;  ++jj) {
	_sim->_aa.iwant(n_(ii).m_(),n_(jj).m_());
      }
    }else{itested();
      // node 1 is grounded or invalid
    }
  }
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_passive()
{
  trace2(long_label().c_str(), n_(OUT1).m_(), n_(OUT2).m_());
  _sim->_acx.iwant(n_(OUT1).m_(),n_(OUT2).m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_active()
{
  //_sim->_acx.iwant(n_(OUT1).m_(),n_(OUT2).m_());
  _sim->_acx.iwant(n_(OUT1).m_(),n_(IN1).m_());
  _sim->_acx.iwant(n_(OUT1).m_(),n_(IN2).m_());
  _sim->_acx.iwant(n_(OUT2).m_(),n_(IN1).m_());
  _sim->_acx.iwant(n_(OUT2).m_(),n_(IN2).m_());
  //_sim->_acx.iwant(n_(IN1).m_(),n_(IN2).m_());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::ac_iwant_matrix_extended()
{
  assert(is_device());
  assert(!subckt());
  assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0;  ii < matrix_nodes();  ++ii) {
    if (n_(ii).m_() >= 0) {
      for (int jj = 0;  jj < ii ;  ++jj) {
	_sim->_acx.iwant(n_(ii).m_(),n_(jj).m_());
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
  }else if (Umatch(x, "eiv ")) {
    return _m0.x;
  }else if (Umatch(x, "y ")) {
    return _m0.c1;
  }else if (Umatch(x, "is{tamp} ")) {itested();
    return _m0.f0();
  }else if (Umatch(x, "iof{fset} ")) {
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
    return port_impedance(n_(OUT1), n_(OUT2), _sim->_aa, mfactor()*(_m0.c1+_loss0));
  }else if (Umatch(x, "zraw ")) {
    return port_impedance(n_(OUT1), n_(OUT2), _sim->_aa, 0.);
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
    }else{
      return XPROBE(1. / admittance);
    }
  }else if (Umatch(x, "z ")) {			/* port impedance */
    return XPROBE(port_impedance(n_(OUT1), n_(OUT2), _sim->_acx, mfactor()*admittance));
  }else if (Umatch(x, "zraw ")) {		/* port impedance, raw */
    return XPROBE(port_impedance(n_(OUT1), n_(OUT2), _sim->_acx, COMPLEX(0.)));
  }else{ 					/* bad parameter */
    return COMPONENT::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_review_trunc_error(const FPOLY1* q)
{
  double timestep;
  if (_time[0] <= 0.) {
    // DC, I know nothing
    timestep = NEVER;
  }else{
    int error_deriv; // which derivative to use for error estimate
    if (order() >= OPT::_keep_time_steps - 2) {
      error_deriv = OPT::_keep_time_steps - 1;
    }else if (order() < 0) {untested();
      error_deriv = 1;
    }else{
      error_deriv = order()+1;
    }
    for (int i=error_deriv; i>0; --i) {
      if (_time[i-1] <= _time[i]) {
	// not enough info to use that derivative, use a lower order derivative
	error_deriv = i-1;
      }else{
      }
    }
    assert(error_deriv > 0);
    assert(error_deriv < OPT::_keep_time_steps);
    for (int i=error_deriv; i>0; --i) {
      assert(_time[i] < _time[i-1]);
    }
    
    double c[OPT::_keep_time_steps];
    for (int i=0; i<OPT::_keep_time_steps; ++i) {
      c[i] = q[i].f0;
    }
    derivatives(c, OPT::_keep_time_steps, _time);
    // now c[i] is i'th derivative
    
    assert(OPT::_keep_time_steps >= 5);
    trace4(("ts " + long_label()).c_str(), error_deriv, error_factor(),
	   OPT::trsteporder, OPT::trstepcoef[OPT::trsteporder] );
    trace5("time", _time[0], _time[1], _time[2], _time[3], _time[4]);
    trace5("charge", q[0].f0, q[1].f0, q[2].f0, q[3].f0, q[4].f0);
    trace5("deriv", c[0], c[1], c[2], c[3], c[4]);
    
    if (c[error_deriv] == 0) {
      // avoid divide by zero
      timestep = NEVER;
    }else{
      double chargetol = std::max(OPT::chgtol,
				  OPT::reltol * std::max(std::abs(q[0].f0), std::abs(q[1].f0)));
      double tol = OPT::trtol * chargetol;
      double denom = error_factor() * std::abs(c[error_deriv]);
      assert(tol > 0.);
      assert(denom > 0.);
      switch (error_deriv) { // pow is slow.
      case 1:  timestep = tol / denom; break;
      case 2:  timestep = sqrt(tol / denom); break;
      case 3:  timestep = cbrt(tol / denom); break;
      default: timestep = pow((tol / denom), 1./(error_deriv)); break;
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
	error(bTRACE, "initial step rejected:" + long_label() + '\n');
	error(bTRACE, "new=%g  old=%g  required=%g\n",
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
void ELEMENT::obsolete_move_parameters_from_common(const COMMON_COMPONENT* dc)
{
  assert(dc);

  _value   = dc->value();
  // _mfactor = dc->mfactor();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
