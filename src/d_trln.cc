/*$Id: d_trln.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * Transmission line. (ideal lossless.)
 */
//testing=script,sparse 2006.07.17
#include "d_trln.h"
/*--------------------------------------------------------------------------*/
const double _default_len (1);
const double _default_R   (0);
const double _default_L   (NOT_INPUT);
const double _default_G   (0);
const double _default_C   (NOT_INPUT);
const double _default_z0  (50.);
const double _default_td  (NOT_INPUT);
const double _default_f   (NOT_INPUT);
const double _default_nl  (0.25);
const double LINLENTOL = .000001;
static COMMON_TRANSLINE Default_TRANSLINE(CC_STATIC);
/*--------------------------------------------------------------------------*/
COMMON_TRANSLINE::COMMON_TRANSLINE(int c)
  :COMMON_COMPONENT(c),
   len(_default_len),
   R(_default_R),
   L(_default_L),
   G(_default_G),
   C(_default_C),
   z0(_default_z0),
   td(_default_td),
   f(_default_f),
   nl(_default_nl),
   icset(false),
   real_z0(NOT_INPUT),
   real_td(NOT_INPUT)
{
  for (int i = 0;  i < NUM_INIT_COND;  ++i) {
    ic[i] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
COMMON_TRANSLINE::COMMON_TRANSLINE(const COMMON_TRANSLINE& p)
  :COMMON_COMPONENT(p),
   len(p.len),
   R(p.R),
   L(p.L),
   G(p.G),
   C(p.C),
   z0(p.z0),
   td(p.td),
   f(p.f),
   nl(p.nl),
   icset(p.icset),
   real_z0(p.real_z0),
   real_td(p.real_td)
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
    && len == p->len
    && R == p->R
    && L == p->L
    && G == p->G
    && C == p->C
    && z0 == p->z0
    && td == p->td
    && f == p->f
    && nl == p->nl
    && icset == p->icset
    && COMMON_COMPONENT::operator==(x);
  if (rv) {
    for (int i=0; i<NUM_INIT_COND; ++i) {
      rv &= ic[i] == p->ic[i];
    }
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::parse(CS& cmd)
{
  int here = cmd.cursor();
  do{
    ONE_OF
      || get(cmd, "LEN",  &len)
      || get(cmd, "R",    &R)
      || get(cmd, "L",    &L)
      || get(cmd, "G",    &G)
      || get(cmd, "C",    &C)
      || get(cmd, "Z0",   &z0)
      || get(cmd, "Zo",   &z0)
      || get(cmd, "Delay",&td)
      || get(cmd, "TD",   &td)
      || get(cmd, "Freq", &f)
      || get(cmd, "Nl",   &nl)
      ;
    if (cmd.pmatch("Ic")) {untested();
      icset = true;
      for (int i = 0;  i < NUM_INIT_COND;  ++i) {untested();
	ic[i] = cmd.ctof();
      }
    }else{
    }
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "what's this?");
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::print(OMSTREAM& o)const
{
  o.setfloatwidth(7);
  if (len.has_value())  {untested(); o << "  len="<< len;}
  if (R.has_value())	{untested(); o << "  R="  << R;}
  if (L.has_value())    {untested(); o << "  L="  << L;}
  if (G.has_value())    {untested(); o << "  G="  << G;}
  if (C.has_value())    {untested(); o << "  C="  << C;}
  if (z0.has_value())   {o << "  Z0=" << z0;}
  if (td.has_value())   {untested(); o << "  TD=" << td;}
  if (f.has_value())    {o << "  F="  << f;}
  if (nl.has_value())   {o << "  NL=" << nl;}
  if (icset) {untested();
    o << "  IC=";
    for (int i = 0;  i < NUM_INIT_COND;  ++i) {untested();
      o << ic[i] << ' ';
    }
  }else{
  }
  o << '\n';
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::elabo3(const COMPONENT* c)
{
  assert(c);
  const CARD_LIST* par_scope = c->scope();
  assert(par_scope);
  COMMON_COMPONENT::elabo3(c);
  len.e_val(_default_len, par_scope);
  R.e_val(_default_R, par_scope);
  L.e_val(_default_L, par_scope);
  G.e_val(_default_G, par_scope);
  C.e_val(_default_C, par_scope);
  z0.e_val(_default_z0, par_scope);
  td.e_val(_default_td, par_scope);
  f.e_val(_default_f, par_scope);
  nl.e_val(_default_nl, par_scope);

  { // real_td
    if (td.has_value()) {untested();
      real_td = len * td;
      if (f.has_value()) {untested(); // check for conflicts
	if (!conchk(td, nl/f, OPT::vntol)) {untested();
	  error(bDANGER, "td, f&nl conflict.  using td\n");
	}else{untested();
	}
      }else{untested();
      }
    }else if (f.has_value()) {
      real_td = len * nl / f;      
    }else if (L.has_value() && C.has_value()) {untested();
      real_td = len * sqrt(L * C);
    }else{untested();
      assert(real_td == NOT_INPUT);
      error(bDANGER, "can't determine length\n");
    }
  }
  
  { // real_z0
    if (z0.has_value()) {
      real_z0 = z0;
      if (L.has_value() && C.has_value()) {untested();
	error(bDANGER, "redundant specification both Z0 and LC, using Z0\n");
      }else{
      }
    }else{untested();
      if (L.has_value() && C.has_value()) {untested();
	real_z0 = sqrt(L / C);
      }else{untested();
	assert(_default_z0 == 50.);
	error(bDANGER, "can't determine Z0, assuming 50\n");
	real_z0 = _default_z0;
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
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
void DEV_TRANSLINE::parse_spice(CS& cmd)
{
  assert(has_common());
  COMMON_TRANSLINE* c = prechecked_cast<COMMON_TRANSLINE*>(common()->clone());
  assert(c);

  parse_Label(cmd);
  parse_nodes(cmd, max_nodes(), min_nodes(), 0/*no model_name*/, 0/*start*/);
  c->parse(cmd);
  attach_common(c);
  set_converged();
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::print_spice(OMSTREAM& o, int)const
{
  const COMMON_TRANSLINE*c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);

  o << short_label();
  printnodes(o);
  c->print(o);
}
/*--------------------------------------------------------------------------*/
/* precalc: called once to set things up.
 */
void DEV_TRANSLINE::precalc()
{
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _forward.set_delay(c->real_td);
  _reflect.set_delay(c->real_td);
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_iwant_matrix()
{
  aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  aa.iwant(_n[IN1].m_(), _n[IN2].m_());
  lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  lu.iwant(_n[IN1].m_(), _n[IN2].m_());
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
 * _if0 = output current ..
 * The "wave" class stores voltages, but we need currents,
 * because the simulator uses the Norton equivalent circuit.
 * This makes the Thevenin to Norton conversion.
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
  //if (_if0 != _if1  ||  _ir0 != _ir1) {
  if (!conchk(_if0, _if1, OPT::abstol, OPT::reltol*.01)
       || !conchk(_ir0, _ir1, OPT::abstol, OPT::reltol*.01)) {
    q_load();
  }else{
  }
  assert(converged());
  return true;
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_load()
{
  double lvf = NOT_VALID; // load value, forward
  double lvr = NOT_VALID; // load value, reflected
  if (!SIM::inc_mode) {
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
  }
  if (lvf != 0.) {
    if (_n[OUT1].m_() != 0) {
      _n[OUT1].i() += lvf;
    }else{untested();
    }
    if (_n[OUT2].m_() != 0) {untested();
      _n[OUT2].i() -= lvf;
    }else{
    }
  }else{
  }
  if (lvr != 0.) {
    if (_n[IN1].m_() != 0) {
      _n[IN1].i() += lvr;
    }else{untested();
    }
    if (_n[IN2].m_() != 0) {untested();
      _n[IN2].i() -= lvr;
    }else{
    }
  }else{
  }
  _if1 = _if0;
  _ir1 = _ir0;
}
/*--------------------------------------------------------------------------*/
/* limit the time step to no larger than a line length.
 */
DPAIR DEV_TRANSLINE::tr_review()
{
  q_accept();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  return DPAIR(SIM::time0 + c->real_td, NEVER); // ok to miss the spikes, for now
}
/*--------------------------------------------------------------------------*/
/* after this step is all done, determine the reflections and send them on.
 */
void DEV_TRANSLINE::tr_accept()
{
  trace1(short_label().c_str(), SIM::time0);
  _reflect.push(SIM::time0, _forward.v_reflect(SIM::time0, tr_outvolts()));
  _forward.push(SIM::time0, _reflect.v_reflect(SIM::time0, tr_involts()));
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_unload()
{untested();
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
  }else{
  }
  lenth *= (M_PI_2);	/* now in radians */
  
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
