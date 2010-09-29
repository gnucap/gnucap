/*$Id: e_elemnt.h,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * branch structure type definitions
 * device types (enumeration type?)
 */
#ifndef E_ELEMNT_H
#define E_ELEMNT_H
#include "l_denoise.h"
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class ELEMENT : public COMPONENT {
  friend class DEV_CCCS;
  friend class DEV_CCVS;
protected:
  explicit ELEMENT();
  explicit ELEMENT(const ELEMENT& p);
  
  void	   store_values()		{_y2=_y1; _y1=_y0;}
  //void   reject_values()		{untested(); _y0 = _y1;}

  bool			skip_dev_type(CS&);
  virtual void		parse_more_nodes(CS&, int);
protected: // override virtual
  void	   parse(CS&);
  void	   print(OMSTREAM&,int)const;
  bool	   tr_needs_eval()	{return !constant();}
  void	   tr_queue_eval()	{if(tr_needs_eval()) {q_eval();}}
  void	   map_nodes() {assert(!subckt().exists()); COMPONENT::map_nodes();}
  void	   tr_alloc_matrix() = 0;
  void	   ac_alloc_matrix() = 0;

protected: // inline, below
  double   dampdiff(double&, const double&, bool);

  void	   tr_load_shunt();
  void	   tr_unload_shunt();
  void	   ac_load_shunt();

  void	   tr_load_loss();
  void	   ac_load_loss();

  void	   tr_load_source();
  void	   tr_unload_source();
  void	   ac_load_source();

  void	   tr_load_passive();
  void	   tr_unload_passive();
  void	   ac_load_passive();

  void	   tr_load_active();
  void	   tr_unload_active();
  void	   ac_load_active();

  void	   tr_load_extended(const node_t& no1, const node_t& no2,
			    const node_t& ni1, const node_t& ni2,
			    double* value, double* old_value);
  void	   ac_load_extended(const node_t& no1, const node_t& no2,
			    const node_t& ni1, const node_t& ni2,
			    COMPLEX value);

  bool	   conv_check()const;
  bool	   using_tr_eval()const;
  bool	   has_tr_eval()const;
  bool	   has_ac_eval()const;
  void	   tr_eval();
  void	   ac_eval();
protected: // in .cc
  void	   tr_alloc_matrix_passive();
  void	   tr_alloc_matrix_active();
  void	   tr_alloc_matrix_extended();
  void	   ac_alloc_matrix_passive();
  void	   ac_alloc_matrix_active();
  void	   ac_alloc_matrix_extended();

  double   tr_outvolts()const	{return dn_diff(_n[OUT1].v0(), _n[OUT2].v0());}
  double   tr_outvolts_limited()const{return volts_limited(_n[OUT1],_n[OUT2]);}
  COMPLEX  ac_outvolts()const	{return _n[OUT1].vac() - _n[OUT2].vac();}
  double   tr_probe_num(CS&)const;
  XPROBE   ac_probe_ext(CS&)const;

  virtual  double  tr_amps()const;
  virtual  double  tr_involts()const = 0;
  virtual  double  tr_involts_limited()const = 0;
  virtual  COMPLEX ac_involts()const = 0;
private:
  int      _loaditer;	// load iteration number
  node_t   _nodes[NODES_PER_BRANCH]; // nodes (0,1:out, 2,3:in)
protected:
  CPOLY1   _m0;		// matrix parameters, new
  CPOLY1   _m1;		// matrix parameters, 1 fill ago
  double   _loss0;	// shunt conductance
  double   _loss1;
  COMPLEX  _acg;	// ac admittance matrix values
  method_t _method_u;	// method to use for this part per user
public: // commons
  FPOLY1   _y0;		// iteration parameters, new
  FPOLY1   _y1;		// iteration parameters, 1 iter ago
  FPOLY1   _y2;		// iteration parameters, 2 iter ago
  COMPLEX  _ev;		// ac effective value (usually real)
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline double ELEMENT::dampdiff(double& v0, const double& v1, bool damp)
{
  //double diff = v0 - v1;
  double diff = dn_diff(v0, v1);
  if (STATUS::iter[iSTEP] > 1 && damp) {
    diff *= SIM::damp;
    v0 = v1 + diff;
  }
  return (SIM::inc_mode) ? diff : v0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_shunt()
{
  double d = dampdiff(_loss0, _loss1, true);
  if (d != 0.) {
    aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), d);
  }
  _loss1 = _loss0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_shunt()
{
  _loss0 = 0.;
  if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }
  tr_load_shunt();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_shunt()
{
  acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), _loss0);
}
/*--------------------------------------------------------------------------*/
 inline void ELEMENT::tr_load_loss()
{
  tr_load_shunt();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_loss()
{
  ac_load_shunt();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_source()
{
#ifndef NDEBUG
  if (_loaditer == STATUS::iter[iTOTAL]) {
    error(bDANGER, long_label() + " internal error: double load\n");
  }
  _loaditer = STATUS::iter[iTOTAL];
#endif

  double dc0 = dampdiff(_m0.c0, _m1.c0, true);
  if (dc0 != 0.) {
    if (_n[OUT2].m_() != 0) {
      _n[OUT2].i() += dc0;
    }
    if (_n[OUT1].m_() != 0) {
      _n[OUT1].i() -= dc0;
    }
  }
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_source()
{
  _m0.c0 = _m0.c1 = 0.;
  {if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }else{
    untested();
  }}
  tr_load_source();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_source()
{
  if (_n[OUT2].m_() != 0) {
    _n[OUT2].iac() += _acg;
  }
  if (_n[OUT1].m_() != 0) {
    _n[OUT1].iac() -= _acg;
  }
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_passive()
{
  double d = dampdiff(_m0.c1, _m1.c1, true);
  if (d != 0.) {
    aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), d);
  }
  tr_load_source();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_passive()
{
  _m0.c0 = _m0.c1 = 0.;
  if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }
  tr_load_passive();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_passive()
{
  acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), _acg);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_active()
{
  double d = dampdiff(_m0.c1, _m1.c1, true);
  if (d != 0.) {
    aa.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(), _n[IN1].m_(), _n[IN2].m_(), d);
  }
  tr_load_source();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_active()
{
  _m0.c0 = _m0.c1 = 0.;
  {if (SIM::inc_mode) {
    SIM::inc_mode = tsBAD;
  }else{
    untested();
  }}
  tr_load_active();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_active()
{
  acx.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(), _n[IN1].m_(), _n[IN2].m_(), _acg);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_extended(const node_t& no1, const node_t& no2,
				      const node_t& ni1, const node_t& ni2,
				      double* value, double* old_value)
{
  double d = dampdiff(*value, *old_value, true);
  if (d != 0.) {
    aa.load_asymmetric(no1.m_(), no2.m_(), ni1.m_(), ni2.m_(), d);
  }
  *old_value = *value;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_extended(const node_t& no1, const node_t& no2,
				      const node_t& ni1, const node_t& ni2,
				      COMPLEX value)
{
  acx.load_asymmetric(no1.m_(), no2.m_(), ni1.m_(), ni2.m_(), value);
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::conv_check()const
{
  return conchk(_y1.f1, _y0.f1)
    && conchk(_y1.f0, _y0.f0)
    && conchk(_y1.x,  _y0.x);
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::has_tr_eval()const
{
  return (has_common() && common()->has_tr_eval());
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::using_tr_eval()const
{
  return (has_probes() || has_tr_eval());
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::has_ac_eval()const
{
  return (has_probes() || (has_common() && common()->has_ac_eval()));
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_eval()
{
  {if (has_common() && common()->has_tr_eval()) {
    common()->tr_eval(this);
  }else{
    // can get here if a simple device has probes
    _y0.f1 = value();
    _y0.f0 = _y0.x * _y0.f1;
  }}
  set_converged(conv_check());
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_eval()
{
  {if (has_common() && common()->has_ac_eval()) {
    common()->ac_eval(this);
  }else{
    // can get here if a simple device has probes
    _ev = _y0.f1;
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
