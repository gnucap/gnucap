/*$Id: d_trln.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * Transmission line. (ideal lossless.)
 */
//testing=script,sparse 2006.07.17
#include "m_wave.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
enum {NUM_INIT_COND = 4};
/*--------------------------------------------------------------------------*/
class COMMON_TRANSLINE : public COMMON_COMPONENT {
private:
  PARAMETER<double> len;	/* length multiplier */
  PARAMETER<double> R;
  PARAMETER<double> L;
  PARAMETER<double> G;
  PARAMETER<double> C;
  PARAMETER<double> z0;		/* characteristic impedance */
  PARAMETER<double> td;		/* delay time */
  PARAMETER<double> f;		/* specification frequency */
  PARAMETER<double> nl;		/* length (wavelengths) at f */
  double   ic[NUM_INIT_COND];	/* initial conditions: v1, i1, v2, i2 */
  int	   icset;		/* flag: initial condition set */
public:
  double real_z0;
  double real_td;
private:
  explicit COMMON_TRANSLINE(const COMMON_TRANSLINE& p);
public:
  explicit COMMON_TRANSLINE(int c=0);
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new COMMON_TRANSLINE(*this);}

  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int param_count()const {return (9 + COMMON_COMPONENT::param_count());}
public:
  void		precalc_first(const CARD_LIST*);
  void		precalc_last(const CARD_LIST*);
  std::string	name()const		{untested(); return "transline";}
};
/*--------------------------------------------------------------------------*/
class DEV_TRANSLINE : public ELEMENT {
private:
  WAVE	 _forward;
  WAVE	 _reflect;
  double _if0; // value of current source representing incident wave
  double _ir0; // value of current source representing reflected wave
  double _if1; // val of cs rep incident wave, one load ago
  double _ir1; // val of cs rep reflected wave, one load ago
  COMPLEX _y11;// AC equiv ckt
  COMPLEX _y12;// AC equiv ckt
private:
  explicit	DEV_TRANSLINE(const DEV_TRANSLINE& p)
    :ELEMENT(p), _forward(), _reflect(), _if0(0), _ir0(0), _if1(0), _ir1(0) {}
public:
  explicit	DEV_TRANSLINE();
private: // override virtual
  char		id_letter()const	{return 'T';}
  std::string   value_name()const	{return "#";}
  std::string	dev_type()const		{itested(); return "tline";}
  int		max_nodes()const	{return 4;}
  int		min_nodes()const	{return 4;}
  int		matrix_nodes()const	{return 4;}
  int		net_nodes()const	{return 4;}
  CARD*		clone()const		{return new DEV_TRANSLINE(*this);}
  void		precalc_last();
  void		tr_iwant_matrix();
  void		tr_begin();
  void		dc_advance();
  void		tr_advance();
  void		tr_regress();
  bool		tr_needs_eval()const;
  bool		do_tr();
  void		tr_load();
  TIME_PAIR 	tr_review();
  void		tr_accept();
  void		tr_unload();
  double	tr_involts()const;
  double	tr_involts_limited()const;
  void		ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void		do_ac();
  void		ac_load();
  COMPLEX	ac_involts()const;

  std::string port_name(int i)const {itested();
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"t1", "b1", "t2", "b2"};
    return names[i];
  }
private:
  void		setinitcond(CS&);
};
/*--------------------------------------------------------------------------*/
inline bool DEV_TRANSLINE::tr_needs_eval()const
{
  assert(!is_q_for_eval());
  return (_if0!=_if1 || _ir0!=_ir1);
}
/*--------------------------------------------------------------------------*/
inline double DEV_TRANSLINE::tr_involts()const
{
  return dn_diff(_n[IN1].v0(), _n[IN2].v0());
}
/*--------------------------------------------------------------------------*/
inline double DEV_TRANSLINE::tr_involts_limited()const
{
  unreachable();
  return volts_limited(_n[IN1],_n[IN2]);
}
/*--------------------------------------------------------------------------*/
inline COMPLEX DEV_TRANSLINE::ac_involts()const
{untested();
  return _n[IN1]->vac() - _n[IN2]->vac();
}
/*--------------------------------------------------------------------------*/
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
void COMMON_TRANSLINE::set_param_by_index(int I, std::string& Value, int Offset)
{
  switch (COMMON_TRANSLINE::param_count() - 1 - I) {
  case 0:  len = Value; break;
  case 1:  R = Value; break;
  case 2:  L = Value; break;
  case 3:  G = Value; break;
  case 4:  C = Value; break;
  case 5:  z0 = Value; break;
  case 6:  td = Value; break;
  case 7:  f = Value; break;
  case 8:  nl = Value; break;
  default: COMMON_COMPONENT::set_param_by_index(I, Value, Offset); break;
  }
  //BUG// does not print IC
}
/*--------------------------------------------------------------------------*/
bool COMMON_TRANSLINE::param_is_printable(int I)const
{
  switch (COMMON_TRANSLINE::param_count() - 1 - I) {
  case 0:  return len.has_hard_value();
  case 1:  return R.has_hard_value();
  case 2:  return L.has_hard_value();
  case 3:  return G.has_hard_value();
  case 4:  return C.has_hard_value();
  case 5:  return z0.has_hard_value();
  case 6:  return td.has_hard_value();
  case 7:  return f.has_hard_value();
  case 8:  return nl.has_hard_value();
  default: return COMMON_COMPONENT::param_is_printable(I);
  }
  //BUG// does not print IC
#if 0
  if (icset) {untested();
    o << " IC=";
    for (int i = 0;  i < NUM_INIT_COND;  ++i) {untested();
      o << ic[i] << ' ';
    }
  }else{
  }
#endif
}
/*--------------------------------------------------------------------------*/
std::string COMMON_TRANSLINE::param_name(int I)const
{
  switch (COMMON_TRANSLINE::param_count() - 1 - I) {
  case 0:  return "len";
  case 1:  return "r";
  case 2:  return "l";
  case 3:  return "g";
  case 4:  return "c";
  case 5:  return "z0";
  case 6:  return "td";
  case 7:  return "f";
  case 8:  return "nl";
  default: return COMMON_COMPONENT::param_name(I);
  }
  //BUG// does not print IC
}
/*--------------------------------------------------------------------------*/
std::string COMMON_TRANSLINE::param_name(int I, int j)const
{
  if (j == 0) {
    return param_name(I);
  }else if (I >= COMMON_COMPONENT::param_count()) {
    switch (COMMON_TRANSLINE::param_count() - 1 - I) {
    case 5:  return (j==1) ? "z" : (j==2) ? "zo" : "";
    case 6:  return (j==1) ? "d" : (j==2) ? "delay" : "";
    case 7:  return (j==1) ? "freq" : "";
    default: return "";
    }
  }else{
    return COMMON_COMPONENT::param_name(I, j);
  }
  //BUG// does not print IC
}
/*--------------------------------------------------------------------------*/
std::string COMMON_TRANSLINE::param_value(int I)const
{
  switch (COMMON_TRANSLINE::param_count() - 1 - I) {
  case 0:  return len.string();
  case 1:  return R.string();
  case 2:  return L.string();
  case 3:  return G.string();
  case 4:  return C.string();
  case 5:  return z0.string();
  case 6:  return td.string();
  case 7:  return f.string();
  case 8:  return nl.string();
  default: return COMMON_COMPONENT::param_value(I);
  }
  //BUG// does not print IC
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_first(Scope);
  len.e_val(_default_len, Scope);
  R.e_val(_default_R, Scope);
  L.e_val(_default_L, Scope);
  G.e_val(_default_G, Scope);
  C.e_val(_default_C, Scope);
  z0.e_val(_default_z0, Scope);
  td.e_val(_default_td, Scope);
  f.e_val(_default_f, Scope);
  nl.e_val(_default_nl, Scope);
}
/*--------------------------------------------------------------------------*/
void COMMON_TRANSLINE::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_last(Scope);
  { // real_td
    if (td.has_hard_value()) {untested();
      real_td = len * td;
      if (f.has_hard_value()) {untested(); // check for conflicts
	if (!conchk(td, nl/f, OPT::vntol)) {untested();
	  error(bDANGER, "td, f&nl conflict.  using td\n");
	}else{untested();
	}
      }else{untested();
      }
    }else if (f.has_hard_value()) {
      real_td = len * nl / f;      
    }else if (L.has_hard_value() && C.has_hard_value()) {untested();
      real_td = len * sqrt(L * C);
    }else{untested();
      assert(real_td == NOT_INPUT);
      error(bDANGER, "can't determine length\n");
    }
  }
  
  { // real_z0
    if (z0.has_hard_value()) {
      real_z0 = z0;
      if (L.has_hard_value() && C.has_hard_value()) {untested();
	error(bDANGER, "redundant specification both Z0 and LC, using Z0\n");
      }else{
      }
    }else{untested();
      if (L.has_hard_value() && C.has_hard_value()) {untested();
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
void DEV_TRANSLINE::precalc_last()
{
  ELEMENT::precalc_last();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _forward.set_delay(c->real_td);
  _reflect.set_delay(c->real_td);
  set_converged();
  assert(!is_constant());
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::tr_iwant_matrix()
{
  _sim->_aa.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_aa.iwant(_n[IN1].m_(), _n[IN2].m_());
  _sim->_lu.iwant(_n[OUT1].m_(),_n[OUT2].m_());
  _sim->_lu.iwant(_n[IN1].m_(), _n[IN2].m_());
}
/*--------------------------------------------------------------------------*/
/* first setup, initial dc, empty the lines
 */
void DEV_TRANSLINE::tr_begin()
{
  ELEMENT::tr_begin();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _forward.initialize().push(0.-c->real_td, 0.).push(0., 0.);
  _reflect.initialize().push(0.-c->real_td, 0.).push(0., 0.);
}
/*--------------------------------------------------------------------------*/
/* before anything else .. see what is coming out
 * _if0 = output current ..
 * The "wave" class stores voltages, but we need currents,
 * because the simulator uses the Norton equivalent circuit.
 * This makes the Thevenin to Norton conversion.
 */
void DEV_TRANSLINE::dc_advance()
{
  ELEMENT::dc_advance();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _if0 = _forward.v_out(_sim->_time0).f0/c->real_z0;
  _ir0 = _reflect.v_out(_sim->_time0).f0/c->real_z0;
}
void DEV_TRANSLINE::tr_advance()
{
  ELEMENT::tr_advance();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _if0 = _forward.v_out(_sim->_time0).f0/c->real_z0;
  _ir0 = _reflect.v_out(_sim->_time0).f0/c->real_z0;
}
void DEV_TRANSLINE::tr_regress()
{
  ELEMENT::tr_regress();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  _if0 = _forward.v_out(_sim->_time0).f0/c->real_z0;
  _ir0 = _reflect.v_out(_sim->_time0).f0/c->real_z0;
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
  //BUG// explicit mfactor
  double lvf = NOT_VALID; // load value, forward
  double lvr = NOT_VALID; // load value, reflected
  if (!_sim->is_inc_mode()) {
    const COMMON_TRANSLINE* c = prechecked_cast<const COMMON_TRANSLINE*>(common());
    assert(c);
    _sim->_aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), mfactor()/c->real_z0);
    _sim->_aa.load_symmetric(_n[IN1].m_(),  _n[IN2].m_(),  mfactor()/c->real_z0);
    lvf = _if0;
    lvr = _ir0;
  }else{
    lvf = dn_diff(_if0, _if1);
    lvr = dn_diff(_ir0, _ir1);
  }
  if (lvf != 0.) {
    if (_n[OUT1].m_() != 0) {
      _n[OUT1].i() += mfactor() * lvf;
    }else{untested();
    }
    if (_n[OUT2].m_() != 0) {untested();
      _n[OUT2].i() -= mfactor() * lvf;
    }else{
    }
  }else{
  }
  if (lvr != 0.) {
    if (_n[IN1].m_() != 0) {
      _n[IN1].i() += mfactor() * lvr;
    }else{untested();
    }
    if (_n[IN2].m_() != 0) {untested();
      _n[IN2].i() -= mfactor() * lvr;
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
TIME_PAIR DEV_TRANSLINE::tr_review()
{
  q_accept();
  const COMMON_TRANSLINE* c=prechecked_cast<const COMMON_TRANSLINE*>(common());
  assert(c);
  return TIME_PAIR(_sim->_time0 + c->real_td, NEVER); // ok to miss the spikes, for now
}
/*--------------------------------------------------------------------------*/
/* after this step is all done, determine the reflections and send them on.
 */
void DEV_TRANSLINE::tr_accept()
{
  trace1(short_label().c_str(), _sim->_time0);
  _reflect.push(_sim->_time0, _forward.v_reflect(_sim->_time0, tr_outvolts()));
  _forward.push(_sim->_time0, _reflect.v_reflect(_sim->_time0, tr_involts()));
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
  double lenth = _sim->_freq * c->real_td * 4;  /* length in quarter waves */
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
}
/*--------------------------------------------------------------------------*/
void DEV_TRANSLINE::ac_load()
{
  //BUG// explicit mfactor
  _sim->_acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), mfactor()*_y11);
  _sim->_acx.load_symmetric(_n[IN1].m_(),  _n[IN2].m_(),  mfactor()*_y11);
  _sim->_acx.load_asymmetric(_n[OUT1].m_(),_n[OUT2].m_(), _n[IN2].m_(),  _n[IN1].m_(),
			     mfactor()*_y12);
  _sim->_acx.load_asymmetric(_n[IN1].m_(), _n[IN2].m_(), _n[OUT2].m_(), _n[OUT1].m_(),
			     mfactor()*_y12);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_TRANSLINE p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "T|tline", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
