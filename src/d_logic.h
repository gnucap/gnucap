/*$Id: d_logic.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2006.07.17
#ifndef D_LOGIC_H
#define D_LOGIC_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_GATE = 10};
/*--------------------------------------------------------------------------*/
class DEV_LOGIC : public ELEMENT {
public:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
private:
  int		_lastchangenode;
  int		_quality;
  std::string	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[PORTS_PER_GATE];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC();
  explicit	DEV_LOGIC(const DEV_LOGIC& p);
		~DEV_LOGIC()		{--_count;}
private: // override virtuals
  char	   id_letter()const	{return 'U';}
  std::string value_name()const	{return "#";}
  bool	      print_type_in_spice()const {return true;}
  std::string dev_type()const {assert(has_common());
    return (common()->modelname() + " " + common()->name()).c_str();}
  int	   tail_size()const	{return 2;}
  int	   max_nodes()const	{return PORTS_PER_GATE;}
  int	   min_nodes()const	{return BEGIN_IN+1;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return _net_nodes;}
  CARD*	   clone()const		{return new DEV_LOGIC(*this);}
  void	   precalc_first() {ELEMENT::precalc_first(); if (subckt()) {subckt()->precalc_first();}}
  void	   expand();
  void	   precalc_last() {ELEMENT::precalc_last(); if (subckt()) {subckt()->precalc_last();}}
  //void   map_nodes();

  void	   tr_iwant_matrix();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  void	   tr_regress();
  bool	   tr_needs_eval()const;
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  TIME_PAIR tr_review();
  void	   tr_accept();
  void	   tr_unload();
  double   tr_involts()const		{unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  double   tr_involts_limited()const	{unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(const std::string&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac()	{untested();  assert(subckt());  subckt()->do_ac();}
  void	   ac_load()	{untested();  assert(subckt());  subckt()->ac_load();}
  COMPLEX  ac_involts()const		{unreachable(); return 0.;}
  COMPLEX  ac_amps()const		{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(int)const {untested();
    incomplete();
    return "";
  }
public:
  static int count()			{return _count;}
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
};
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC : public MODEL_CARD {
private:
  explicit	MODEL_LOGIC(const MODEL_LOGIC& p);
public:
  explicit MODEL_LOGIC(const DEV_LOGIC*);
	   ~MODEL_LOGIC()		{--_count;}
private: // override virtuals
  std::string	dev_type()const		{return "logic";}
  CARD*		clone()const		{return new MODEL_LOGIC(*this);}
  void		precalc_first();
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int		param_count()const	{return (13 + MODEL_CARD::param_count());}
public:
  static int	count()			{return _count;}
public:
			/* ----- digital mode ----- */
  PARAMETER<double> delay;	/* propagation delay */
			/* -- conversion parameters both ways -- */
  PARAMETER<double> vmax;	/* nominal volts for logic 1 */
  PARAMETER<double> vmin;	/* nominal volts for logic 0 */
  PARAMETER<double> unknown;	/* nominal volts for unknown (bogus) */
			/* ---- D to A conversion ---- */
  PARAMETER<double> rise;	/* rise time (time in slope) */
  PARAMETER<double> fall;	/* fall time (time in slope) */
  PARAMETER<double> rs; 	/* series resistance -- strong */
  PARAMETER<double> rw; 	/* series resistance -- weak */
			/* ---- A to D conversion ---- */
  PARAMETER<double> th1;	/* threshold for 1 as fraction of range */
  PARAMETER<double> th0;	/* threshold for 0 as fraction of range */
  	        	/* ---- quality judgement parameters ---- */
  PARAMETER<double> mr;		/* margin rise - how much worse rise can be */
  PARAMETER<double> mf;		/* margin fall - how much worse fall can be */
  PARAMETER<double> over;	/* overshoot limit - as fraction of range */
public: // calculated parameters
  double range;			/* vmax - vmin */
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
class INTERFACE COMMON_LOGIC : public COMMON_COMPONENT {
protected:
  explicit	COMMON_LOGIC(int c=0)
    :COMMON_COMPONENT(c), incount(0) {++_count;}
  explicit	COMMON_LOGIC(const COMMON_LOGIC& p)
    :COMMON_COMPONENT(p), incount(p.incount) {++_count;}
public:
		~COMMON_LOGIC()			{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  static  int	count()				{return _count;}
  virtual LOGICVAL logic_eval(const node_t*)const	= 0;
public:
  int		incount;
protected:
  static int	_count;
};
/*--------------------------------------------------------------------------*/
class LOGIC_AND : public COMMON_LOGIC {
private:
  explicit LOGIC_AND(const LOGIC_AND& p) :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_AND(*this);}
public:
  explicit LOGIC_AND(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out &= n[ii]->lv();
    }
    return out;
  }
  virtual std::string name()const	  {itested();return "and";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NAND : public COMMON_LOGIC {
private:
  explicit LOGIC_NAND(const LOGIC_NAND&p):COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const {return new LOGIC_NAND(*this);}
public:
  explicit LOGIC_NAND(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out &= n[ii]->lv();
    }
    return ~out;
  }
  virtual std::string name()const	  {itested();return "nand";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_OR : public COMMON_LOGIC {
private:
  explicit LOGIC_OR(const LOGIC_OR& p)	 :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_OR(*this);}
public:
  explicit LOGIC_OR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out |= n[ii]->lv();
    }
    return out;
  }
  virtual std::string name()const	  {itested();return "or";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NOR : public COMMON_LOGIC {
private:
  explicit LOGIC_NOR(const LOGIC_NOR& p) :COMMON_LOGIC(p) {++_count;}
  COMMON_COMPONENT* clone()const {return new LOGIC_NOR(*this);}
public:
  explicit LOGIC_NOR(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* n)const {
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {
      out |= n[ii]->lv();
    }
    return ~out;
  }
  virtual std::string name()const	  {return "nor";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_XOR : public COMMON_LOGIC {
private:
  explicit LOGIC_XOR(const LOGIC_XOR& p) :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_XOR(*this);}
public:
  explicit LOGIC_XOR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out ^= n[ii]->lv();
    }
    return out;
  }
  virtual std::string name()const	  {itested();return "xor";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_XNOR : public COMMON_LOGIC {
private:
  explicit LOGIC_XNOR(const LOGIC_XNOR&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_XNOR(*this);}
public:
  explicit LOGIC_XNOR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out ^= n[ii]->lv();
    }
    return ~out;
  }
  virtual std::string name()const	  {itested();return "xnor";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_INV : public COMMON_LOGIC {
private:
  explicit LOGIC_INV(const LOGIC_INV& p) :COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const	{return new LOGIC_INV(*this);}
public:
  explicit LOGIC_INV(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* n)const {
    return ~n[0]->lv();
  }
  virtual std::string name()const	  {return "inv";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NONE : public COMMON_LOGIC {
private:
  explicit LOGIC_NONE(const LOGIC_NONE&p):COMMON_LOGIC(p){itested();++_count;}
  COMMON_COMPONENT* clone()const {itested(); return new LOGIC_NONE(*this);}
public:
  explicit LOGIC_NONE(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t*)const {untested();
    return lvUNKNOWN;
  }
  virtual std::string name()const	  {untested();return "error";}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
