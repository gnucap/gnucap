/*$Id: e_elemnt.cc,v 20.7 2001/09/29 05:31:06 al Exp $ -*- C++ -*-
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
 * Base class for elements of a circuit
 */
#include "u_xprobe.h"
#include "e_aux.h"
#include "ap.h"
#include "bm.h" // includes e_elemnt.h
/*--------------------------------------------------------------------------*/
ELEMENT::ELEMENT()
  :COMPONENT(),
   _loaditer(0),
   _m0(),
   _m1(),
   _loss0(0.),
   _loss1(0.),
   _acg(0.),
   _method_u(meUNKNOWN),
   _y0(),
   _y1(),
   _y2(),
   _ev(0.) 
{
  _n = _nodes;
  assert(_y0.x == 0. && _y0.f0 == 0. && _y0.f1 == 0.);
  assert(_y1 == _y0);
  assert(_y2 == _y0);
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
   _method_u(p._method_u),
   _y0(),
   _y1(),
   _y2(),
   _ev(0.)
{
  _n = _nodes;
  for (int ii = 0;  ii < NODES_PER_BRANCH;  ++ii) {
    _n[ii] = p._n[ii];
  }
  assert(_y0.x == 0. && _y0.f0 == 0. && _y0.f1 == 0.);
  assert(_y1 == _y0);
  assert(_y2 == _y0);
}
/*--------------------------------------------------------------------------*/
bool ELEMENT::skip_dev_type(CS& cmd)
{
  return cmd.icmatch(dev_type());
}
/*--------------------------------------------------------------------------*/
void ELEMENT::parse_more_nodes(CS& cmd, int gotnodes)
{
  int inports = is_2port();
  parse_nodes(cmd, 2*inports+2, 2*inports+2, gotnodes);
  //		   max		min	     start
}
/*--------------------------------------------------------------------------*/
void ELEMENT::parse(CS& cmd)
{
  parse_Label(cmd);
  int gotnodes = parse_nodes(cmd,numnodes(),0);
  //				 max	    min
  COMMON_COMPONENT* c = 0;

  // HSPICE compatibility kluge.
  // The device type or function type could be stuck between the nodes.
  if (gotnodes < numnodes()) {
    skip_dev_type(cmd); // (redundant)
    c = EVAL_BM_ACTION_BASE::parse_func_type(cmd);
    parse_more_nodes(cmd, gotnodes);
  }

  // Normal mode.  nodes first, then data.
  if (!c) {
    skip_dev_type(cmd); // (redundant)
    c = new EVAL_BM_COND;	
  }
  assert(c);

  c->parse(cmd);

  // At this point, there is ALWAYS a common attached, which may have more
  // commons attached to it.  Try to reduce its complexity.
  // "c->deflate()" may return "c" or some simplification of "c".
  COMMON_COMPONENT* dc = c->deflate(); // dc == deflated_common
  EVAL_BM_VALUE* dvc = dynamic_cast<EVAL_BM_VALUE*>(dc);  
  {if (dvc && !dvc->has_ext_args()) {
    set_value(dvc->value());
  }else{
    attach_common(dc);
  }}
  if (dc != c) {
    delete c;
  }

  cmd.check(bDANGER, "what's this?");
}
/*--------------------------------------------------------------------------*/
void ELEMENT::print(OMSTREAM& where, int /*detail*/)const
{
  if (short_label()[0] != id_letter()) {
    where << '.' << dev_type() << ' ';
  }
  where << short_label();
  printnodes(where, numnodes());
  if (!has_common() || value() != 0) {
    where << ' ' << value();
  }
  if (has_common()) {
    common()->print(where);
  }
  where << '\n';
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_amps()const
{
  return fixzero((_loss0 * tr_outvolts() + _m0.c1 * tr_involts() + _m0.c0),
  		 _m0.c0);
}
/*--------------------------------------------------------------------------*/
double ELEMENT::tr_probe_num(CS& cmd)const
{
  {if (cmd.pmatch("Vout")) {
    return tr_outvolts();
  }else if (cmd.pmatch("VIn")) {
    return tr_involts();
  }else if (cmd.pmatch("I")) {
    return tr_amps();
  }else if (cmd.pmatch("P")) {
    return tr_amps() * tr_outvolts();
  }else if (cmd.pmatch("PD")) {
    double p = tr_amps() * tr_outvolts();
    return (p > 0.) ? p : 0.;
  }else if (cmd.pmatch("PS")) {
    double p = tr_amps() * tr_outvolts();
    return (p < 0.) ? -p : 0.;
  }else if (cmd.pmatch("INput")) {
    untested();
    return _y0.x;
  }else if (cmd.pmatch("F")) {
    if (_y0.f0 == LINEAR) {
      untested();
      return _y0.x * _y0.f1;
    }else{
      return _y0.f0;
    }
  }else if (cmd.pmatch("NV")) {
    return value();
  }else if (cmd.pmatch("EV")) {
    return _y0.f1;
  }else if (cmd.pmatch("Y")) {
    return _m0.c1;
  }else if (cmd.pmatch("EIV")) {
    untested();
    return _m0.x;
  }else if (cmd.pmatch("IOFfset")) {
    return _m0.c0;
  }else if (cmd.pmatch("IPassive")) {
    untested();
    return _m0.c1 * tr_involts();
  }else if (cmd.pmatch("ILoss")) {
    untested();
    return _loss0 * tr_outvolts();
//  }else if (cmd.pmatch("DIdt")) {
//    double i0  = (_m0.c1  * _m0.x  + _m0.c0);
//    double it1 = (mt1.f1 * mt1.x + mt1.c0);
//    return  (i0 - it1) / (time0 - time1);
//  }else if (cmd.pmatch("DTNew")) {
//    return timef - time0;
//  }else if (cmd.pmatch("DTOld")) {
//    return time0 - time1;
//  }else if (cmd.pmatch("TIMEF")) {
//    return timef;
//  }else if (cmd.pmatch("TIME")) {
//    return time0;
//  }else if (cmd.pmatch("TIMEO")) {
//    return time1;
  }else if (cmd.pmatch("R")) {
    return (_m0.c1!=0.) ? 1/_m0.c1 : DBL_MAX;
  }else if (cmd.pmatch("Z")) {
    return port_impedance(_n[OUT1], _n[OUT2], lu, (_m0.c1+_loss0));
  }else if (cmd.pmatch("ZRAW")) {
    return port_impedance(_n[OUT1], _n[OUT2], lu, 0.);
  }else{
    untested();
    return COMPONENT::tr_probe_num(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
XPROBE ELEMENT::ac_probe_ext(CS& cmd)const
{
  COMPLEX admittance = (is_source()) ? COMPLEX(0.) : _acg;
  COMPLEX source =     (is_source()) ? _acg : COMPLEX(0.);

  {if (cmd.pmatch("V")) {			/* volts (out) */
    return XPROBE(ac_outvolts());
  }else if (cmd.pmatch("VI")) {			/* volts (in) */
    return XPROBE(ac_involts());
  }else if (cmd.pmatch("I")) {			/* amps */
    return XPROBE(ac_involts() * admittance + source + ac_outvolts() * _loss0);
  }else if (cmd.pmatch("P")) {			/* complex "power" */
    COMPLEX i = ac_involts() * admittance + source + ac_outvolts() * _loss0;
    return XPROBE(ac_outvolts() * conj(i), mtREAL, 10.);
  }else if (cmd.pmatch("NV")) {			/* nominal value */
    untested();
    return XPROBE(value());
  }else if (cmd.pmatch("EV")) {			/* effective value */
    return XPROBE(_ev);
  }else if (cmd.pmatch("Y")) {			/* admittance */
    untested();
    return XPROBE(admittance, mtREAL);
  }else if (cmd.pmatch("R")) {			/* complex "resistance" */
    {if (admittance == 0.) {
      untested();
      return XPROBE(DBL_MAX);
    }else{
      return XPROBE(1. / admittance);
    }}
  }else if (cmd.pmatch("Z")) {			/* port impedance */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], acx, admittance));
  }else if (cmd.pmatch("ZRAW")) {		/* port impedance, raw */
    return XPROBE(port_impedance(_n[OUT1], _n[OUT2], acx, COMPLEX(0.)));
  }else{ 					/* bad parameter */
    return COMPONENT::ac_probe_ext(cmd);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
