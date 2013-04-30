/*$Id: e_node.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * circuit node class
 */
//testing=script,sparse 2006.07.11
#ifndef E_NODE_H
#define E_NODE_H
#include "u_sim_data.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC;
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
class INTERFACE LOGICVAL {
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
  
  bool is_unknown()const	{return _lv == lvUNKNOWN;}
  bool lv_future()const		{assert(_lv!=lvUNKNOWN); return _lv & 1;}
  bool lv_old()const		{assert(_lv!=lvUNKNOWN); return _lv & 2;}

  bool is_rising() const	{return _lv == lvRISING;}
  bool is_falling()const	{return _lv == lvFALLING;}

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
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  explicit NODE(const std::string& s, int n);
  ~NODE() {}

public: // raw data access (rvalues)
  int	user_number()const	{return _user_number;}
  //int	flat_number()const	{itested();return _flat_number;}
public: // simple calculated data access (rvalues)
  int	matrix_number()const	{return _sim->_nm[_user_number];}
  int	m_()const		{return matrix_number();}
public: // maniputation
  NODE&	set_user_number(int n)	{_user_number = n; return *this;}
  //NODE& set_flat_number(int n) {itested();_flat_number = n; return *this;}
  //NODE& set_matrix_number(int n){untested();_matrix_number = n;return *this;}
public: // virtuals
  double	tr_probe_num(const std::string&)const;
  XPROBE	ac_probe_ext(const std::string&)const;

  double      v0()const	{
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_v0[m_()];
  }
  double      vt1()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_vt1[m_()];
  }
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
  //double      vdc()const		{untested();return _vdc[m_()];}

  //double&     i()	{untested();return _i[m_()];}  /* lvalues */
  COMPLEX&    iac() {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
};
extern NODE ground_node;
/*--------------------------------------------------------------------------*/
class INTERFACE LOGIC_NODE : public NODE {
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
  std::string _failure_mode;

  // so it is not pure virtual
  //const	      std::string long_label()const;
public: // virtuals
  double	tr_probe_num(const std::string&)const;
  //XPROBE	ac_probe_ext(const std::string&)const;

public: // raw data access (rvalues)
  LOGICVAL lv()const			{return _lv;}
  int	   quality()const		{return _quality;}
  const std::string& failure_mode()const {return _failure_mode;}
  int	   d_iter()const		{return _d_iter;}
  int	   a_iter()const		{return _a_iter;}
  double   final_time()const		{return _final_time;}
  double   last_change_time()const	{return _lastchange;}
  const MODEL_LOGIC* process()const	{return _family;}
  double   old_last_change_time()const	{untested(); return _old_lastchange;}
  const LOGICVAL old_lv()const		{return _old_lv;}

public: // simple calculated data access (rvalues)
  bool	 lv_future()const	{return lv().lv_future();}
  bool	 is_unknown()const	{return lv().is_unknown();}
  bool	 in_transit()const	{return final_time() < NEVER;}
  bool	 is_digital()const	{return _mode == moDIGITAL;}
  bool	 is_analog()const	{return _mode == moANALOG;}
  double annotated_logic_value()const;

public: // calculated data access (rvalues)
  bool	just_reached_stable()const;

public: // raw data access (lvalues)
  void	set_quality(int q)		{_quality = q;}
  void	set_failure_mode(const std::string& f) {_failure_mode = f;}
  void	set_final_time(double t)	{_final_time = t;}
  
  void	set_d_iter()			{_d_iter = _sim->iteration_tag();}
  void	set_last_change_time()		{_lastchange = _sim->_time0;}
  void	set_last_change_time(double t)	{_lastchange = t;}
  void	set_lv(LOGICVAL v)		{_lv = v;}
  void	set_process(const MODEL_LOGIC* f) {_family = f;}

  void  store_old_last_change_time()	{_old_lastchange = last_change_time();}
  void	store_old_lv()			{_old_lv = lv();}
  void	restore_lv()			{untested(); set_lv(old_lv());}
  void	set_mode(smode_t m)		{_mode = m;}

public: // other internal
  void  set_bad_quality(const std::string& f) {
    set_quality(qBAD);
    set_failure_mode(f);
  }
  void  set_good_quality(const std::string& f = "ok") {
    set_quality(qGOOD);
    set_failure_mode(f);
  }
  void	dont_set_quality(const std::string& f = "don't know") {
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
  LOGIC_NODE&	set_a_iter()	{_a_iter = _sim->iteration_tag(); return *this;}
};
/*--------------------------------------------------------------------------*/
class INTERFACE node_t {
private:
  static bool node_is_valid(int i) {
    if (i == INVALID_NODE) {untested();
      itested();
    }else if (i < 0) {
      unreachable();
    }else if (i > NODE::_sim->_total_nodes) {
      unreachable();
    }else{
    }
    return i>=0 && i<=NODE::_sim->_total_nodes;
  }
  static int  to_internal(int n) {
    assert(node_is_valid(n));
    assert(NODE::_sim->_nm);
    return NODE::_sim->_nm[n];
  }

private:
  NODE* _nnn;
  int _ttt;		// m == nm[t] if properly set up
  int _m;		// mapped, after reordering

public:
  int	      m_()const	{return _m;}

  int	      t_()const {
    //assert(_nnn);
    //assert(_ttt == _nnn->flat_number());
    return _ttt;
  }	// e_cardlist.cc:CARD_LIST::map_subckt_nodes:436 and
	// e_node.h:node_t::map:263,265 only

  int	      e_()const {
    assert(_nnn);
    return ((_nnn) ? _nnn->user_number() : INVALID_NODE);
  }
  const NODE* n_()const {return _nnn;}
  NODE*	      n_()	{return _nnn;}

  
  const std::string  short_label()const {return ((n_()) ? (n_()->short_label()) : "?????");}
  void	set_to_ground(CARD*);
  void	new_node(const std::string&, const CARD*);
  void	new_model_node(const std::string& n, CARD* d);
  void	map_subckt_node(int* map_array, const CARD* d);
  bool	is_grounded()const {return (e_() == 0);}
  bool	is_connected()const {return (e_() != INVALID_NODE);}

  node_t&     map() {
    if (t_() != INVALID_NODE) {
      assert(_nnn);
      _m=to_internal(t_());
    }else{
      assert(_m == INVALID_NODE);
    }
    return *this;
  } // e_compon.cc:COMPONENT::map_nodes:522

  explicit    node_t();
	      node_t(const node_t&);
  explicit    node_t(NODE*);
	      ~node_t() {}

private: // raw data access (lvalues)
  LOGIC_NODE&	data()const;

public:
  //LOGIC_NODE&	    operator*()const	{untested();return data();}
  const LOGIC_NODE* operator->()const	{return &data();}
  LOGIC_NODE*	    operator->()	{return &data();}

  node_t& operator=(const node_t& p);

  bool operator==(const node_t& p) {return _nnn==p._nnn && _ttt==p._ttt && _m==p._m;}

public:
  double      v0()const {
    //assert(m_() >= 0);
    if (m_() >= 0) {
      assert(m_() <= NODE::_sim->_total_nodes);
      assert(n_());
      //assert(n_()->m_() == m_());
      //assert(n_()->v0() == NODE::_sim->_v0[m_()]);
      return NODE::_sim->_v0[m_()];
    }else{
      //BUG// in BJT model: should not get here but does.
      return 0.;
    }
  }

  COMPLEX     vac()const {
    //assert(m_() >= 0);
    if (m_() >= 0) {
      assert(m_() <= NODE::_sim->_total_nodes);
      assert(n_());
      //assert(n_()->m_() == m_());
      //assert(n_()->vac() == NODE::_ac[m_()]);
      return NODE::_sim->_ac[m_()];
    }else{untested();
      //BUG// assume v0 BUG applies here too.
      return 0.;
    }
  }

  double&     i() {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    return NODE::_sim->_i[m_()];
  }
#if 0
  COMPLEX&    iac() {untested();
    assert(n_());
    assert(n_()->m_() == m_());
    assert(n_()->iac() == NODE::_ac[m_()]);
    //return n_()->iac();
    return NODE::_sim->_ac[m_()];
  }
#endif
};
/*--------------------------------------------------------------------------*/
INTERFACE double volts_limited(const node_t& n1, const node_t& n2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
