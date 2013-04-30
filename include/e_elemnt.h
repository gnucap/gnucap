/*$Id: e_elemnt.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * base class for simple elements
 */
//testing=script 2006.07.12
#ifndef E_ELEMNT_H
#define E_ELEMNT_H
#include "e_node.h"
#include "m_cpoly.h"
#include "l_denoise.h"
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class INTERFACE ELEMENT : public COMPONENT {
protected:
  explicit ELEMENT();
  explicit ELEMENT(const ELEMENT& p);
  ~ELEMENT() {}
  
  void	   store_values()		{assert(_y[0]==_y[0]); _y1=_y[0];}
  //void   reject_values()		{ _y0 = _y1;}
public:
  double*  set__value()			{return _value.pointer_hack();}

  bool	   skip_dev_type(CS&);
public: // override virtual
  bool	   print_type_in_spice()const {return false;}
  void	   precalc_last();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  void	   tr_regress();
  bool	   tr_needs_eval()const {/*assert(!is_q_for_eval());*/ return !is_constant();}
#if 0
  void	   tr_queue_eval()	{
    if(tr_needs_eval()) {
      q_eval();
    }else{
    }
  }
#endif
  TIME_PAIR tr_review();

  //void   map_nodes();
  void	   tr_iwant_matrix() = 0;
  void	   ac_iwant_matrix() = 0;
  double   tr_probe_num(const std::string&)const;
  XPROBE   ac_probe_ext(const std::string&)const;

protected: // inline, below
  double   dampdiff(double*, const double&);

  void	   tr_load_inode();
  void	   tr_unload_inode();
  void	   ac_load_inode();

  void	   tr_load_shunt();
  void	   tr_unload_shunt();
  void	   ac_load_shunt();

  void	   tr_load_source();
  void	   tr_unload_source();
  void	   ac_load_source();

  void	   tr_load_couple();
  void	   tr_unload_couple();
  void	   ac_load_couple();

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

  void	   tr_load_source_point(node_t& no1, double* value, double* old_value);
  void	   ac_load_source_point(node_t& no1, COMPLEX new_value);

  void	   tr_load_diagonal_point(const node_t& no1, double* value, double* old_value);
  void	   ac_load_diagonal_point(const node_t& no1, COMPLEX value);
  
  void	   tr_load_point(const node_t& no1, const node_t& no2,
			 double* value, double* old_value);
  void	   ac_load_point(const node_t& no1, const node_t& no2,
			 COMPLEX value);
  
  bool	   conv_check()const;
  bool	   has_tr_eval()const;
  bool	   has_ac_eval()const;
  bool	   using_tr_eval()const;
  bool	   using_ac_eval()const;
  void	   tr_eval();
  void	   ac_eval();

protected: // in .cc
  void	   tr_iwant_matrix_passive();
  void	   tr_iwant_matrix_active();
  void	   tr_iwant_matrix_extended();
  void	   ac_iwant_matrix_passive();
  void	   ac_iwant_matrix_active();
  void	   ac_iwant_matrix_extended();

public:
  double   tr_review_trunc_error(const FPOLY1* q);
  double   tr_review_check_and_convert(double timestep);

  double   tr_outvolts()const	{return dn_diff(_n[OUT1].v0(), _n[OUT2].v0());}
  double   tr_outvolts_limited()const{return volts_limited(_n[OUT1],_n[OUT2]);}
  COMPLEX  ac_outvolts()const	{return _n[OUT1]->vac() - _n[OUT2]->vac();}

  virtual  double  tr_involts()const		= 0;
  virtual  double  tr_input()const		{return tr_involts();}
  virtual  double  tr_involts_limited()const	= 0;
  virtual  double  tr_input_limited()const	{return tr_involts_limited();}
  virtual  double  tr_amps()const;
  virtual  COMPLEX ac_involts()const		= 0;
  virtual  COMPLEX ac_amps()const;

  virtual int order()const		{return OPT::trsteporder;}
  virtual double error_factor()const	{return OPT::trstepcoef[OPT::trsteporder];}
  int param_count()const {return (0 + COMPONENT::param_count());}
protected:
  int      _loaditer;	// load iteration number
private:
  node_t   _nodes[NODES_PER_BRANCH]; // nodes (0,1:out, 2,3:in)
public:
  CPOLY1   _m0;		// matrix parameters, new
  CPOLY1   _m1;		// matrix parameters, 1 fill ago
  double   _loss0;	// shunt conductance
  double   _loss1;
  COMPLEX  _acg;	// ac admittance matrix values
public: // commons
  COMPLEX  _ev;		// ac effective value (usually real)
  double   _dt;

  double   _time[OPT::_keep_time_steps];
  FPOLY1   _y1;		// iteration parameters, 1 iter ago
  FPOLY1   _y[OPT::_keep_time_steps]; /* charge or flux, and deriv.	*/
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline double ELEMENT::dampdiff(double* v0, const double& v1)
{
  //double diff = v0 - v1;
  assert(v0);
  assert(*v0 == *v0);
  assert(v1 == v1);
  double diff = dn_diff(*v0, v1);
  assert(diff == diff);
  if (!_sim->is_advance_or_first_iteration()) {
    diff *= _sim->_damp;
    *v0 = v1 + diff;
  }else{
  }
  return mfactor() * ((_sim->is_inc_mode()) ? diff : *v0);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_inode()
{
  double d = dampdiff(&_loss0, _loss1);
  if (d != 0.) {
    _sim->_aa.load_couple(_n[OUT1].m_(), _n[IN1].m_(), -d);
    _sim->_aa.load_couple(_n[OUT2].m_(), _n[IN1].m_(),  d);
  }else{
  }
  _loss1 = _loss0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_inode()
{untested();
  _loss0 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_inode();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_inode()
{
  _sim->_acx.load_couple(_n[OUT1].m_(), _n[IN1].m_(), -mfactor() * _loss0);
  _sim->_acx.load_couple(_n[OUT2].m_(), _n[IN1].m_(),  mfactor() * _loss0);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_shunt()
{
  double d = dampdiff(&_loss0, _loss1);
  if (d != 0.) {
    _sim->_aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), d);
  }else{
  }
  _loss1 = _loss0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_shunt()
{untested();
  _loss0 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_shunt();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_shunt()
{
  _sim->_acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), mfactor() * _loss0);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_source()
{
#if !defined(NDEBUG)
  assert(_loaditer != _sim->iteration_tag()); // double load
  _loaditer = _sim->iteration_tag();
#endif

  double d = dampdiff(&_m0.c0, _m1.c0);
  if (d != 0.) {
    if (_n[OUT2].m_() != 0) {
      _n[OUT2].i() += d;
    }else{
    }
    if (_n[OUT1].m_() != 0) {
      _n[OUT1].i() -= d;
    }else{
    }
  }else{
  }
  _m1 = _m0;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_source()
{untested();
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_source();
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_source()
{
  if (_n[OUT2].m_() != 0) {
    _n[OUT2]->iac() += mfactor() * _acg;
  }else{
  }
  if (_n[OUT1].m_() != 0) {
    _n[OUT1]->iac() -= mfactor() * _acg;
  }else{itested();
  }
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_couple()
{
  double d = dampdiff(&_m0.c1, _m1.c1);
  if (d != 0.) {
    _sim->_aa.load_couple(_n[OUT1].m_(), _n[OUT2].m_(), d);
  }else{
  }
  _m1.c1 = _m0.c1;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_couple()
{untested();
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_couple(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_couple()
{
  _sim->_acx.load_couple(_n[OUT1].m_(), _n[OUT2].m_(), mfactor() * _acg);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_passive()
{
  double d = dampdiff(&_m0.c1, _m1.c1);
  if (d != 0.) {
    _sim->_aa.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), d);
  }else{
  }
  tr_load_source(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_passive()
{
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_passive(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_passive()
{
  _sim->_acx.load_symmetric(_n[OUT1].m_(), _n[OUT2].m_(), mfactor() * _acg);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_active()
{
  double d = dampdiff(&_m0.c1, _m1.c1);
  if (d != 0.) {
    _sim->_aa.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(),
		       _n[IN1].m_(), _n[IN2].m_(), d);
  }else{
  }
  tr_load_source(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_unload_active()
{untested();
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load_active(); // includes _m1 = _m0
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_active()
{
  _sim->_acx.load_asymmetric(_n[OUT1].m_(), _n[OUT2].m_(),
		      _n[IN1].m_(), _n[IN2].m_(), mfactor() * _acg);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_extended(const node_t& no1, const node_t& no2,
				      const node_t& ni1, const node_t& ni2,
				      double* new_value, double* old_value)
{
  double d = dampdiff(new_value, *old_value);
  if (d != 0.) {
    _sim->_aa.load_asymmetric(no1.m_(), no2.m_(), ni1.m_(), ni2.m_(), d);
  }else{
  }
  *old_value = *new_value;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_extended(const node_t& no1, const node_t& no2,
				      const node_t& ni1, const node_t& ni2,
				      COMPLEX new_value)
{
  _sim->_acx.load_asymmetric(no1.m_(), no2.m_(), ni1.m_(), ni2.m_(), mfactor() * new_value);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_source_point(node_t& no1,
				   double* new_value, double* old_value)
{
  double d = dampdiff(new_value, *old_value);
  if (d != 0.) {
    if (no1.m_() != 0) {
      no1.i() += d;
    }else{
    }
  }else{
  }
  *old_value = *new_value;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_source_point(node_t& no1, COMPLEX new_value)
{itested();
  if (no1.m_() != 0) {itested();
    no1->iac() += mfactor() * new_value;
  }else{itested();
  }
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_diagonal_point(const node_t& no1,
					    double* new_value, double* old_value)
{
  double d = dampdiff(new_value, *old_value);
  if (d != 0.) {
    _sim->_aa.load_diagonal_point(no1.m_(), d);
  }else{
  }
  *old_value = *new_value;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_diagonal_point(const node_t& no1, COMPLEX new_value)
{
  _sim->_acx.load_diagonal_point(no1.m_(), mfactor() * new_value);
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_load_point(const node_t& no1, const node_t& no2,
				   double* new_value, double* old_value)
{
  double d = dampdiff(new_value, *old_value);
  if (d != 0.) {
    _sim->_aa.load_point(no1.m_(), no2.m_(), d);
  }else{
  }
  *old_value = *new_value;
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_load_point(const node_t& no1, const node_t& no2,
				   COMPLEX new_value)
{itested();
  _sim->_acx.load_point(no1.m_(), no2.m_(), mfactor() * new_value);
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::conv_check()const
{
  return conchk(_y1.f1, _y[0].f1)
    && conchk(_y1.f0, _y[0].f0)
    && conchk(_y1.x,  _y[0].x, OPT::vntol);
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::has_tr_eval()const
{
  return (has_common() && common()->has_tr_eval());
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::has_ac_eval()const
{
  return (has_common() && common()->has_ac_eval());
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::using_tr_eval()const
{
  return (has_probes() || has_tr_eval());
}
/*--------------------------------------------------------------------------*/
inline bool ELEMENT::using_ac_eval()const
{
  return (has_probes() || has_ac_eval());
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::tr_eval()
{
  if (has_tr_eval()) {
    common()->tr_eval(this);
  }else{
    // can get here if a simple device has probes
    _y[0].f1 = value();
    _y[0].f0 = _y[0].x * _y[0].f1;
  }
  set_converged(conv_check());
}
/*--------------------------------------------------------------------------*/
inline void ELEMENT::ac_eval()
{
  if (has_ac_eval()) {
    common()->ac_eval(this);
  }else{
    // can get here if a simple device has probes
    _ev = _y[0].f1;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
