/*$Id: d_trln.cc,v 22.10 2002/07/25 06:26:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
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
 * Transmission line. (ideal lossless.)
 */
#include "ap.h"
#include "d_trln.h"
/*--------------------------------------------------------------------------*/
const double LINLENTOL = .000001;
const double default_nl = 0.25;
const double default_z0 = 50;
const char* default_z0_string = "50";
static COMMON_TRANSLINE Default_TRANSLINE(CC_STATIC);
/*--------------------------------------------------------------------------*/
COMMON_TRANSLINE::COMMON_TRANSLINE(int c)
  :COMMON_COMPONENT(c),
   real_z0(NOT_INPUT),
   real_td(NOT_INPUT),
   len(1.),
   R(0.),
   L(NOT_INPUT),
   G(0.),
   C(NOT_INPUT),
   z0(NOT_INPUT),
   td(NOT_INPUT),
   f(NOT_INPUT),
   nl(default_nl),
   icset(false)
{
  for (int i = 0;  i < NUM_INIT_COND;  ++i) {
    ic[i] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
COMMON_TRANSLINE::COMMON_TRANSLINE(const COMMON_TRANSLINE& p)
  :COMMON_COMPONENT(p),
   real_z0(p.real_z0),
   real_td(p.real_td),
   len(p.len),
   R(p.R),
   L(p.L),
   G(p.G),
   C(p.C),
   z0(p.z0),
   td(p.td),
   f(p.f),
   nl(p.nl),
   icset(p.icset)
{
  for (int i = 0;  i < NUM_INIT_COND;  ++i) {
    ic[i] = p.ic[i];
  }
}
/*--------------------------------------------------------------------------*/
bool COMMON_TRANSLINE::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_TRANSLINE* p = dynamic_cast<const COMMON_TRANSLINE*>(&x);
  bool rv = p
    && real_z0 == p->real_z0
    && real_td == p->real_td
    && icset == p->icset
    && R == p->R
    && G == p->G
    && is_equal(x);
  if (rv) {
    untested();
    {for (int i=0; i<NUM_INIT_COND; ++i) {
      rv &= ic[i] == p->ic[i];
    }}
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    get(cmd, "LEN", &len);
    get(cmd, "Mult",&len);
    get(cmd, "R",   &R);
    get(cmd, "L",   &L);
    get(cmd, "G",   &G);
    get(cmd, "C",   &C);
    get(cmd, "Z0",  &z0);
    get(cmd, "Zo",  &z0);
    get(cmd, "TD",  &td);
    get(cmd, "Freq",&f,  mPOSITIVE);
    get(cmd, "Nl",  &nl, mPOSITIVE);
    if (cmd.pmatch("Ic")) {
      untested();
      icset = true;
      for (int i = 0;  i < NUM_INIT_COND;  ++i) {
	untested();
	ic[i] = cmd.ctof();
      }
    }
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");

  {if (td != NOT_INPUT) {
    untested();
    real_td = len * td;
    // have td.  check for conflicts.
    {if (f != NOT_INPUT) {
      {if (!conchk(td, nl/f, OPT::vntol)) {
	untested();
	cmd.warn(bDANGER, 0, "td, f&nl conflict.  using td");
      }else{
	untested();
      }}
    }else{
      untested();
    }}
  }else if (f != NOT_INPUT) {
    // try to calculate td by nl/f
    real_td = len * nl / f;      
  }else if (L != NOT_INPUT && C != NOT_INPUT) {
    untested();
    real_td = len * sqrt(L * C);
  }else{
    untested();
    cmd.warn(bERROR, 0, "can't determine length, giving up");
  }}

  {if (z0 != NOT_INPUT) {
    real_z0 = z0;
    if (L != NOT_INPUT && C != NOT_INPUT) {
      untested();
      cmd.warn(bDANGER, 0, "redundant specification both Z0 and LC, using Z0");
    }
  }else{
    untested();
    {if (L != NOT_INPUT && C != NOT_INPUT) {
      untested();
      real_z0 = sqrt(L / C);
    }else{
      untested();
      assert(default_z0 == 50.);
      cmd.warn(bDANGER, 0, "can't determine Z0, assuming 50");
      real_z0 = default_z0;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::print(OMSTREAM& o)const
{
  o.setfloatwidth(7);
  if (len != 1.)
    o << "  len=" << len;
  if (R != 0.)
    o << "  R=" << R;
  if (L != NOT_INPUT)
    o << "  L=" << L;
  if (G != 0.)
    o << "  G=" << G;
  if (C != NOT_INPUT)
    o << "  C=" << C;
  if (z0 != NOT_INPUT)
    o << "  Z0=" << z0;
  if (td != NOT_INPUT)
    o << "  TD=" << td;
  if (f != NOT_INPUT)
    o << "  F="  << f;
  if (nl != default_nl)
    o << "  NL=" << nl;
  if (icset) {
    untested();
    o << "  IC=";
    for (int i = 0;  i < NUM_INIT_COND;  ++i) {
      untested();
      o << ic[i] << ' ';
    }
  }
  o << '\n';
}
/*--------------------------------------------------------------------------*/
DEV_TRANSLINE::DEV_TRANSLINE()
  :ELEMENT(),
   _forward(), _reflect(),
   _if0(0), _ir0(0), _if1(0), _ir1(0),
   _y11(), _y12()
{
  attach_common(&Default_TRANSLINE);
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::parse(CS& cmd)
{
  assert(has_common());
  COMMON_TRANSLINE* c = prechecked_cast<COMMON_TRANSLINE*>(common()->clone());
  assert(c);

  parse_Label(cmd);
  parse_nodes(cmd, max_nodes(), min_nodes());
  c->parse(cmd);
  attach_common(c);
  set_converged();
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::print(OMSTREAM& o, int)const
{
  const COMMON_TRANSLINE*c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);

  o << short_label();
  printnodes(o);
  c->print(o);
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::precalc()
{
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _forward.set_delay(c->real_td);
  _reflect.set_delay(c->real_td);
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_alloc_matrix()
{
  extern NODE* nstat; // yuck
  
  aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  aa.iwant(_n[IN1].m_(), _n[IN2].m_());
  lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  lu.iwant(_n[IN1].m_(), _n[IN2].m_());

  nstat[_n[OUT1].m_()].set_needs_analog();
  nstat[_n[OUT2].m_()].set_needs_analog();
  nstat[_n[IN1].m_()].set_needs_analog();
  nstat[_n[IN2].m_()].set_needs_analog();
}
/*--------------------------------------------------------------------------*/
/* first setup, initial dc, empty the lines
 */
void DEV_TRANSLINE::tr_begin()
{
  _forward.initialize();
  _reflect.initialize();
}
/*--------------------------------------------------------------------------*/
/* before anything else .. see what is coming out
 */
void DEV_TRANSLINE::tr_advance()
{
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _if0 = _forward.v_out(SIM::time0)/c->real_z0;
  _ir0 = _reflect.v_out(SIM::time0)/c->real_z0;
}
/*--------------------------------------------------------------------------*/
/* usually nothing, always converged.  It is all done in advance and accept.
 * UNLESS ... it is a very short line .. then we fake it here.
 * very short line means delay is less than internal time step.
 */
bool DEV_TRANSLINE::do_tr()
{
  // code to deal with short lines goes here.
  //{if (_if0 != _if1  ||  _ir0 != _ir1) {
  {if (!conchk(_if0, _if1, OPT::abstol, OPT::reltol*.01)
       || !conchk(_ir0, _ir1, OPT::abstol, OPT::reltol*.01)) {
    q_load();
  }else{
    //untested();
  }}
  assert(converged());
  return true;
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_load()
{
  double lvf = NOT_VALID; // load value, forward
  double lvr = NOT_VALID; // load value, reflected
  {if (!SIM::inc_mode) {
    const COMMON_TRANSLINE* 
      c = prechecked_cast<const COMMON_TRANSLINE*>(common());
    assert(c);
    aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), 1/c->real_z0);
    aa.load_symmetric(_n[IN1].m_(),  _n[IN2].m_(),  1/c->real_z0);
    lvf = _if0;
    lvr = _ir0;
  }else{
    lvf = dn_diff(_if0, _if1);
    lvr = dn_diff(_ir0, _ir1);
  }}
  if (lvf != 0.) {
    {if (_n[OUT1].m_() != 0) {
      _n[OUT1].i() += lvf;
    }else{
      untested();
    }}
    if (_n[OUT2].m_() != 0) {
      untested();
      _n[OUT2].i() -= lvf;
    }
  }
  if (lvr != 0.) {
    {if (_n[IN1].m_() != 0) {
      _n[IN1].i() += lvr;
    }else{
      untested();
    }}
    if (_n[IN2].m_() != 0) {
      untested();
      _n[IN2].i() -= lvr;
    }
  }
  _if1 = _if0;
  _ir1 = _ir0;
}
/*--------------------------------------------------------------------------*/
/* limit the time step to no larger than a line length.
 */
double DEV_TRANSLINE::tr_review()
{
  q_accept();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  return SIM::time0 + c->real_td; // ok to miss the spikes, for now
}
/*--------------------------------------------------------------------------*/
/* after this step is all done, determine the reflections and send them on.
 */
void DEV_TRANSLINE::tr_accept()
{
  _reflect.push(SIM::time0, _forward.v_reflect(SIM::time0, tr_outvolts()));
  _forward.push(SIM::time0, _reflect.v_reflect(SIM::time0, tr_involts()));
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_unload()
{
  untested();
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::do_ac()
{
  const COMMON_TRANSLINE*c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  double lenth = SIM::freq * c->real_td * 4;  /* length in quarter waves */
  double dif = lenth - floor(lenth+.5);	/* avoid divide by zero if close to */
  if (std::abs(dif) < LINLENTOL) {	/* resonance by tweeking a little */
    error(bDEBUG,
	  long_label() + ": transmission line too close to resonance\n");
    lenth = (dif<0.) ? floor(lenth+.5)-LINLENTOL : floor(lenth+.5)+LINLENTOL;
  }
  lenth *= (kPId2);	/* now in radians */
  
  _y12 = COMPLEX(0., -1. / (c->real_z0 * sin(lenth)));
  _y11 = COMPLEX(0., tan(lenth/2) / c->real_z0) + _y12;
  ac_load();
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::ac_load()
{
  acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), _y11);
  acx.load_symmetric(_n[IN1].m_(),  _n[IN2].m_(),  _y11);
  acx.load_asymmetric(_n[OUT1].m_(),_n[OUT2].m_(), _n[IN2].m_(),  _n[IN1].m_(),  _y12);
  acx.load_asymmetric(_n[IN1].m_(), _n[IN2].m_(),  _n[OUT2].m_(), _n[OUT1].m_(), _y12);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
