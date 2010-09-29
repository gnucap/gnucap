/*$Id: e_node.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * circuit node class
 */
#ifndef E_NODE_H
#define E_NODE_H
#include "u_status.h" // time critical inline
#include "u_opt.h" // time critical inline
#include "constant.h"
#include "e_base.h"
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
enum _LOGICVAL {lvSTABLE0=0,lvRISING=1,lvFALLING=2,lvSTABLE1=3,lvUNKNOWN=4};
/*--------------------------------------------------------------------------*/
class LOGICVAL {
public:
private:
  int _lv;
  LOGICVAL& fix()
    {if (_lv < lvSTABLE0 || _lv > lvSTABLE1){_lv = lvUNKNOWN;} return *this;}
  LOGICVAL(int p) :_lv(p)			{fix();}
public:
  LOGICVAL() :_lv(lvUNKNOWN)			{}
  LOGICVAL(const LOGICVAL& p)	:_lv(p._lv)	{}
  LOGICVAL(_LOGICVAL p)		:_lv(p)		{}

  LOGICVAL& operator=(_LOGICVAL p)	{_lv=p; return *this;}
  LOGICVAL& operator=(const LOGICVAL& p){_lv=p._lv; return *this;}
  LOGICVAL& operator&=(LOGICVAL p) {untested(); _lv &= p._lv; return fix();}
  LOGICVAL& operator|=(LOGICVAL p) {untested(); _lv |= p._lv; return fix();}
  LOGICVAL operator~()const {return (_lv==lvUNKNOWN) ?lvUNKNOWN :((~_lv)&3);}
  LOGICVAL operator^(LOGICVAL p)const{untested();
    return (_lv==lvUNKNOWN||p._lv==lvUNKNOWN) ? lvUNKNOWN : _lv ^ p._lv;}

  bool is_in_transition()const
			{untested(); return _lv==lvRISING||_lv==lvFALLING;}
  bool is_stable()const {untested(); return _lv==lvSTABLE0||_lv==lvSTABLE1;}
  bool is_unknown()const{return _lv == lvUNKNOWN;}
  bool lv_old()const	{/*assert(_lv!=lvUNKNOWN);*/ return _lv & 2;}
  bool lv_future()const	{assert(_lv!=lvUNKNOWN); return _lv & 1;}

  bool is_stable0()const{untested(); return _lv == lvSTABLE0;}
  bool is_rising() const{return _lv == lvRISING;}
  bool is_falling()const{return _lv == lvFALLING;}
  bool is_stable1()const{untested(); return _lv == lvSTABLE1;}

  operator _LOGICVAL()const {return static_cast<_LOGICVAL>(_lv);}

  LOGICVAL& set_in_transition();
};
/*--------------------------------------------------------------------------*/
class NODE : public CKT_BASE {
private:
  int	      _user_number;	/* external node number (redundant) */
  const MODEL_LOGIC *_family;	/* logic family */
  int 	      _d_iter;		/* iteration of last update - digital */
  int 	      _a_iter;		/* iteration of last update - analog */
  double      _final_time;	/* time logic transition attains final state */
  double      _lastchange;	/* time of last change */
  double      _old_lastchange;	/* in case it rejects a step */
  double      _dt;		/* time diff, for conversion */
  smode_t     _mode;		/* simulation mode (analog or digital)*/
  LOGICVAL    _lv;		/* "logic value" (real type is LOGICVAL) */
  LOGICVAL    _old_lv;		/* in case it rejects a step */
  int	      _quality;		/* quality of digital mode */
  int	      _needs_analog;	/* analog info requested (count/flag) */
  const char* _failure_mode;
public: // virtual-override, also allow direct access
  const	      std::string long_label()const;
  double      tr_probe_num(CS&)const;
  XPROBE      ac_probe_ext(CS&)const;
private:
  double      annotated_logic_value()const;

  NODE&	      improve_quality()
			{if (_quality < qGOOD){++_quality;} return *this;}
  NODE&	      set_bad_quality(const char* f)
			{_quality = qBAD; _failure_mode = f; return *this;}
  NODE&	      set_good_quality(const char* f = "ok")
			{_quality = qGOOD; _failure_mode = f; return *this;}
  NODE&	      dont_set_quality(const char* f = "don't know")
			{_failure_mode = f; return *this;}

  bool	      just_reached_stable()const;

  NODE&	      set_d_iter()	{_d_iter=STATUS::iter[iTOTAL]; return *this;}

  int	      user_number()const	{return _user_number;}
  int	      internal_number()const	{return SIM::nm[_user_number];}

  bool	      is_digital()const		{return _mode == moDIGITAL;}
  bool	      is_analog()const		{return _mode == moANALOG;}
public:
  static int  to_internal(int i)	{return SIM::nm[i];}
private:
  double      v0()const			{return SIM::v0 [internal_number()];}
  double      vt1()const		{return SIM::vt1[internal_number()];}
  double      vdc()const		{return SIM::vdc[internal_number()];}
  COMPLEX     vac()const		{return SIM::ac [internal_number()];}
private: // inhibited
  explicit    NODE(const NODE& p)	:CKT_BASE(p) {unreachable();}
public: // general use
  explicit    NODE();
	      ~NODE()			{}
public: // exported to init and probe
  NODE&	      set_number(int n)		{_user_number = n; return *this;}
  NODE&	      set_needs_analog()	{++_needs_analog; return *this;}
  bool	      needs_analog()const	{return _needs_analog > 0;}
public: // exported to matrix
  NODE&	      set_a_iter()	{_a_iter = STATUS::iter[iTOTAL]; return *this;}
public: // exported to logic devices
  int	      d_iter()const		{return _d_iter;}
  bool	      is_unknown()const		{return _lv.is_unknown();}
  bool        lv_old()const		{return _lv.lv_old();}
  bool        lv_future()const		{return _lv.lv_future();}
  LOGICVAL    lv()const			{return _lv;}
  const char* failure_mode()const	{return _failure_mode;}
  int	      quality()const		{return _quality;}
  double      final_time()const		{return _final_time;}
  bool	      in_transit()const		{return _final_time < NEVER;}

  NODE&	      force_initial_value(LOGICVAL v);
  NODE&	      to_logic(const MODEL_LOGIC*);
  double      to_analog(const MODEL_LOGIC*);
  NODE&	      set_final_time(double t)	{_final_time = t; return *this;}
  NODE&	      set_mode(smode_t m)	{_mode = m; return *this;}
  NODE&	      set_event(double delay);
  NODE&	      propagate();
};
/*--------------------------------------------------------------------------*/
class node_t {
public:
  int m;		// mapped, after reordering
  int t;		// m == nm[t] if properly set up
  int e;		// external, user numbers, t = subs exp
private:
  static bool node_is_valid(int i) {return i>=0 && i<=STATUS::total_nodes;}
  static int  to_internal(int i)
		{assert(node_is_valid(i)); assert(SIM::nm); return SIM::nm[i];}
public:
  bool	      is_valid()const
		{return node_is_valid(t) && m==to_internal(t);}
  node_t&     map()			{m=to_internal(t); return *this;}
  explicit    node_t() : m(INVALID_NODE), t(INVALID_NODE), e(INVALID_NODE) {}
	      node_t(const node_t& p)	:m(p.m), t(p.t), e(p.e) {}
  explicit    node_t(int ee)		:m(INVALID_NODE), t(ee), e(ee) {map();}
  node_t&     operator=(int n)		{t=e=n; return map();}
  NODE&	      operator*()const
    {assert(is_valid());extern NODE*nstat;assert(nstat);return nstat[m];}
  NODE*	      operator->()const
    {assert(is_valid());extern NODE*nstat;assert(nstat);return&(nstat[m]);}
  double      v0()const			{return SIM::v0[m];}	/* rvalues */
  double      vdc()const		{return SIM::vdc[m];}
  COMPLEX     vac()const		{return SIM::ac[m];}
  double&     i()			{return SIM::i[m];}	/* lvalues */
  COMPLEX&    iac()			{return SIM::ac[m];}
};
OMSTREAM& operator<<(OMSTREAM& o, const node_t& n);
/*--------------------------------------------------------------------------*/
double volts_limited(const node_t & n1, const node_t & n2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
