/*$Id: d_logic.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2006.07.17
#ifndef D_LOGIC_H
#define D_LOGIC_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC : public MODEL_CARD {
public:
  explicit	MODEL_LOGIC();
		~MODEL_LOGIC()		{--_count;}
private: // override virtuals
  CARD*		clone()const		{untested();return new MODEL_LOGIC(*this);}
  void		parse_spice(CS&);
  void		elabo1();
  void		print_spice(OMSTREAM&,int)const;
public:
  static int	count()			{return _count;}
private:
  explicit	MODEL_LOGIC(const MODEL_LOGIC& p):MODEL_CARD(p){unreachable();}
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
enum {PORTS_PER_GATE = 10};
/*--------------------------------------------------------------------------*/
class DEV_LOGIC : public ELEMENT {
private:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
  int		_net_nodes;
  int		_lastchangenode;
  int		_quality;
  const char*	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[PORTS_PER_GATE];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC();
  explicit	DEV_LOGIC(const DEV_LOGIC& p);
		~DEV_LOGIC()		{--_count;}
private: // override virtuals
  char	   id_letter()const	{untested();return 'U';}
  const char* dev_type()const
    {untested(); assert(has_common()); return common()->name();}
  int	   max_nodes()const	{return PORTS_PER_GATE;}
  int	   min_nodes()const	{return BEGIN_IN+1;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return _net_nodes;}
  CARD*	   clone()const		{untested(); return new DEV_LOGIC(*this);}
  void	   parse_spice(CS&);
  void	   print_spice(OMSTREAM&,int)const;
  void	   elabo1();
  void	   map_nodes();
  void	   precalc();

  void	   tr_iwant_matrix();
  void	   dc_begin();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  bool	   tr_needs_eval()const;
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  DPAIR    tr_review();
  void	   tr_accept();
  void	   tr_unload();
  double   tr_involts()const		{unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  double   tr_involts_limited()const	{unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(CS&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac();
  void	   ac_load()			{unreachable();}
  COMPLEX  ac_involts()const		{unreachable(); return 0.;}
  COMPLEX  ac_amps()const		{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(CS&)const;
public:
  static int count()			{return _count;}
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
};
/*--------------------------------------------------------------------------*/
class COMMON_LOGIC : public COMMON_COMPONENT {
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
  virtual const char* name()const	  {untested();return "and";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NAND : public COMMON_LOGIC {
private:
  explicit LOGIC_NAND(const LOGIC_NAND&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_NAND(*this);}
public:
  explicit LOGIC_NAND(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(const node_t* n)const {untested();
    LOGICVAL out(n[0]->lv());
    for (int ii=1; ii<incount; ++ii) {untested();
      out &= n[ii]->lv();
    }
    return ~out;
  }
  virtual const char* name()const	  {untested();return "nand";}
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
  virtual const char* name()const	  {untested();return "or";}
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
  virtual const char* name()const	  {return "nor";}
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
  virtual const char* name()const	  {untested();return "xor";}
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
  virtual const char* name()const	  {untested();return "xnor";}
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
  virtual const char* name()const	  {return "inv";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NONE : public COMMON_LOGIC {
private:
  explicit LOGIC_NONE(const LOGIC_NONE&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_NONE(*this);}
public:
  explicit LOGIC_NONE(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t*)const {untested();
    return lvUNKNOWN;
  }
  virtual const char* name()const	  {untested();return "error";}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
