/*$Id: e_node.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * circuit node class
 */
//testing=script,sparse 2006.07.11
#ifndef E_NODE_H
#define E_NODE_H
#include "s__.h"
#include "e_card.h"
//#include "e_base.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC;
class XPROBE;
/*--------------------------------------------------------------------------*/
enum {
  OUT1 = 0,
  OUT2 = 1,
  IN1 = 2,
  IN2 = 3,
  NODES_PER_BRANCH = 4,
  INVALID_NODE = -1
};
#define	qBAD	 (0)
#define qGOOD	 (OPT::transits)
/*--------------------------------------------------------------------------*/
enum _LOGICVAL {lvSTABLE0,lvRISING,lvFALLING,lvSTABLE1,lvUNKNOWN};
enum {lvNUM_STATES = lvUNKNOWN+1};
/*--------------------------------------------------------------------------*/
class LOGICVAL {
private:
  _LOGICVAL _lv;
  static const _LOGICVAL or_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL xor_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL and_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL not_truth[lvNUM_STATES];
public:
  LOGICVAL() :_lv(lvUNKNOWN)			{}
  LOGICVAL(const LOGICVAL& p)	:_lv(p._lv)	{}
  LOGICVAL(_LOGICVAL p)		:_lv(p)		{}
  ~LOGICVAL() {}

  operator _LOGICVAL()const {return static_cast<_LOGICVAL>(_lv);}
  
  LOGICVAL& operator=(_LOGICVAL p)	 {_lv=p; return *this;}
  LOGICVAL& operator=(const LOGICVAL& p) {_lv=p._lv; return *this;}

  LOGICVAL& operator&=(LOGICVAL p)
	{untested(); _lv = and_truth[_lv][p._lv]; return *this;}
  LOGICVAL& operator|=(LOGICVAL p)
	{_lv = or_truth[_lv][p._lv]; return *this;}
  LOGICVAL  operator^=(LOGICVAL p)
	{untested(); _lv = xor_truth[_lv][p._lv]; return *this;}
  LOGICVAL  operator~()const	{return not_truth[_lv];}
  
  const bool is_unknown()const	{return _lv == lvUNKNOWN;}
  const bool lv_future()const	{assert(_lv!=lvUNKNOWN); return _lv & 1;}
  const bool lv_old()const	{assert(_lv!=lvUNKNOWN); return _lv & 2;}

  const bool is_rising() const	{return _lv == lvRISING;}
  const bool is_falling()const	{return _lv == lvFALLING;}

  LOGICVAL& set_in_transition(LOGICVAL newval);
};
/*--------------------------------------------------------------------------*/
class NODE : public CKT_BASE {
private:
  int	_user_number;
  //int	_flat_number;
  //int	_matrix_number;
protected:
  explicit NODE();
private: // inhibited
  explicit NODE(const NODE& p);
public:
  explicit NODE(const NODE* p);
  explicit NODE(const std::string& s, int n);
  ~NODE() {}

public: // raw data access (rvalues)
  int	user_number()const	{return _user_number;}
  int	e_()const		{return user_number();}
  //int	flat_number()const	{untested();return _flat_number;}
  //int	t_()const		{untested();return flat_number();}
public: // simple calculated data access (rvalues)
  int	matrix_number()const	{return SIM::nm[_user_number];}
  int	m_()const		{return matrix_number();}
public: // maniputation
  NODE&	set_user_number(int n)	{_user_number = n;   return *this;}
  //NODE& set_flat_number(int n) {untested();_flat_number = n;   return *this;}
  //NODE& set_matrix_number(int n){untested();_matrix_number = n;return *this;}
public: // virtuals
  double	tr_probe_num(CS&)const;
  XPROBE	ac_probe_ext(CS&)const;

  double      v0()const			{return SIM::v0 [m_()];}
  double      vt1()const		{return SIM::vt1[m_()];}
  COMPLEX     vac()const		{return SIM::ac [m_()];}
  //double      vdc()const		{untested();return SIM::vdc[m_()];}

  //double&     i()			{untested();return SIM::i[m_()];}	/* lvalues */
  COMPLEX&    iac()			{return SIM::ac[m_()];}
};
extern NODE ground_node;
/*--------------------------------------------------------------------------*/
class LOGIC_NODE : public NODE {
private:
  const MODEL_LOGIC *_family;	/* logic family */
  int 	      _d_iter;		/* iteration of last update - digital */
  int 	      _a_iter;		/* iteration of last update - analog */
  double      _final_time;	/* time logic transition attains final state */
  double      _lastchange;	/* time of last change */
  double      _old_lastchange;	/* in case it rejects a step */
  smode_t     _mode;		/* simulation mode (analog or digital)*/
  LOGICVAL    _lv;		/* "logic value" (real type is LOGICVAL) */
  LOGICVAL    _old_lv;		/* in case it rejects a step */
  int	      _quality;		/* quality of digital mode */
  const char* _failure_mode;

  // so it is not pure virtual
  //const	      std::string long_label()const;
public: // virtuals
  double	tr_probe_num(CS&)const;
  //XPROBE	ac_probe_ext(CS&)const;

public: // raw data access (rvalues)
  const LOGICVAL lv()const		{return _lv;}
  const int	quality()const		{return _quality;}
  const char*	failure_mode()const	{return _failure_mode;}
  const int	d_iter()const		{return _d_iter;}
  const int	a_iter()const		{return _a_iter;}
  const double	final_time()const	{return _final_time;}
  const double	last_change_time()const	{return _lastchange;}
  const MODEL_LOGIC* process()const	{return _family;}
  const double old_last_change_time()const{untested(); return _old_lastchange;}
  const LOGICVAL old_lv()const		{return _old_lv;}

public: // simple calculated data access (rvalues)
  const bool	lv_future()const	{return lv().lv_future();}
  const bool	is_unknown()const	{return lv().is_unknown();}
  const bool	in_transit()const	{return final_time() < NEVER;}
  const bool	is_digital()const	{return _mode == moDIGITAL;}
  const bool	is_analog()const	{return _mode == moANALOG;}
  const double	annotated_logic_value()const;

public: // calculated data access (rvalues)
  const bool	just_reached_stable()const;

public: // raw data access (lvalues)
  void	set_quality(int q)		{_quality = q;}
  void	set_failure_mode(const char* f) {_failure_mode = f;}
  void	set_final_time(double t)	{_final_time = t;}
  
  void	set_d_iter()			{_d_iter = iteration_tag();}
  void	set_last_change_time()		{_lastchange = SIM::time0;}
  void	set_last_change_time(double t)	{_lastchange = t;}
  void	set_lv(LOGICVAL v)		{_lv = v;}
  void	set_process(const MODEL_LOGIC* f) {_family = f;}

  void  store_old_last_change_time()	{_old_lastchange = last_change_time();}
  void	store_old_lv()			{_old_lv = lv();}
  void	restore_lv()			{untested(); set_lv(old_lv());}
  void	set_mode(smode_t m)		{_mode = m;}

public: // other internal
  void  set_bad_quality(const char* f) {
    set_quality(qBAD);
    set_failure_mode(f);
  }
  void  set_good_quality(const char* f = "ok") {
    set_quality(qGOOD);
    set_failure_mode(f);
  }
  void	dont_set_quality(const char* f = "don't know") {
    set_failure_mode(f);
  }
  void	improve_quality() {
    if (quality() < qGOOD) {
      ++_quality;
    }
  }

public: // action, used by logic
  void	      set_event(double delay, LOGICVAL v);
  void	      force_initial_value(LOGICVAL v);
  void	      propagate();
  double      to_analog(const MODEL_LOGIC*f);
  void	      to_logic(const MODEL_LOGIC*f);

private: // inhibited
  explicit LOGIC_NODE(const LOGIC_NODE&):NODE(){incomplete();unreachable();}
public: // general use
  explicit LOGIC_NODE();
	   ~LOGIC_NODE() {}

public: // matrix
  LOGIC_NODE&	set_a_iter()	{_a_iter = iteration_tag(); return *this;}
};
/*--------------------------------------------------------------------------*/
class node_t {
private:
  static bool node_is_valid(int i) {
    if (i>=0) {
    }else{
      untested();
    }
    if (i<=::status.total_nodes) {
    }else{
      untested();
    }
    return i>=0 && i<=::status.total_nodes;
  }
  static int  to_internal(int n) {
    assert(node_is_valid(n));
    assert(SIM::nm);
    return SIM::nm[n];
  }

private:
  int _m;		// mapped, after reordering
  int _t;		// m == nm[t] if properly set up
  NODE* _n;

public:
  int	      m_()const	{return _m;}
  int	      t_()const {return _t;} // d_subckt.cc:271 only
  int	      e_()const {return ((_n) ? _n->e_() : INVALID_NODE);}
  const NODE* n_()const {untested();return _n;}
  NODE*	      n_()	{return _n;}

  void	      parse(CS&, CARD*);
  void	      set_to_0(CARD*);
  void	      new_model_node()		{_t = ::status.newnode_model();}
  void	      map_subckt_node(int* map_array);
  bool	     is_valid()const {return node_is_valid(_t) && _m==to_internal(_t);}
  //bool      is_unconnected()const	{untested(); return !_n;}
  node_t&     map()			{_m=to_internal(_t); return *this;}

  explicit    node_t();
	      node_t(const node_t&);
  explicit    node_t(NODE*);
	      ~node_t() {}

private: // raw data access (lvalues)
  LOGIC_NODE&	data()const
    {assert(is_valid());extern LOGIC_NODE*nstat;assert(nstat);return nstat[m_()];}
public:
  //LOGIC_NODE&	    operator*()const	{untested(); return data();}
  const LOGIC_NODE* operator->()const	{return &data();}
  LOGIC_NODE*	    operator->()	{return &data();}

public:
  double      v0()const {
    //assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->v0() == SIM::v0[m_()]);
    //return n_()->v0();
    return SIM::v0[m_()];
  }    
#if 0
  COMPLEX     vac()const {
    assert(n_());
    assert(n_()->m_() == m_());
    assert(n_()->vac() == SIM::ac[m_()]);
    //return n_()->vac();
    return SIM::ac[m_()];
  }
#endif
  double&     i() {
    //assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->i() == SIM::i[m_()]);
    //return n_()->i();
    return SIM::i[m_()];
  }
#if 0
  COMPLEX&    iac() {
    assert(n_());
    assert(n_()->m_() == m_());
    assert(n_()->iac() == SIM::ac[m_()]);
    //return n_()->iac();
    return SIM::ac[m_()];
  }
#endif
};
/*--------------------------------------------------------------------------*/
double volts_limited(const node_t& n1, const node_t& n2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
