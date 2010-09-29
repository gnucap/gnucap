/*$Id: d_logic.h,v 22.12 2002/07/26 08:02:01 al Exp $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
#ifndef D_LOGIC_H
#define D_LOGIC_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC : public MODEL_CARD {
friend class DEV_LOGIC;
public:
  explicit	MODEL_LOGIC();
		~MODEL_LOGIC()		{--_count;}
private: // override virtuals
  CARD*		clone()const		{return new MODEL_LOGIC(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&,int)const;
public:
  static int	count()			{return _count;}
private:
  explicit	MODEL_LOGIC(const MODEL_LOGIC& p):MODEL_CARD(p){unreachable();}
public:
			/* ----- digital mode ----- */
  double     delay;	/* propagation delay */
			/* -- conversion parameters both ways -- */
  double     vmax;	/* nominal volts for logic 1 */
  double     vmin;	/* nominal volts for logic 0 */
  double     unknown;	/* nominal volts for unknown (bogus) */
  double     range;	/* vmax - vmin */
  			/* ---- D to A conversion ---- */
  double     rise;	/* rise time (time in slope) */
  double     fall;	/* fall time (time in slope) */
  double     rs;	/* series resistance -- strong */
  double     rw;	/* series resistance -- weak */
  	        	/* ---- A to D conversion ---- */
  double     th1;	/* threshold for 1 as fraction of range */
  double     th0;	/* threshold for 0 as fraction of range */
  	        	/* ---- quality judgement parameters ---- */
  double     mr;	/* margin rise - how much worse rise can be */
  double     mf;	/* margin fall - how much worse fall can be */
  double     over;	/* overshoot limit - as fraction of range */
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_GATE = 10};
/*--------------------------------------------------------------------------*/
class DEV_LOGIC : public ELEMENT {
public:
  explicit	DEV_LOGIC();
  explicit	DEV_LOGIC(const DEV_LOGIC& p);
		~DEV_LOGIC()		{--_count;}
private: // override virtuals
  char	   id_letter()const	{return 'U';}
  const char* dev_type()const
    {untested(); assert(has_common()); return common()->name();}
  int	   max_nodes()const	{return PORTS_PER_GATE;}
  int	   min_nodes()const	{unreachable(); incomplete(); return 2;}
  int	   out_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return _num_nodes;}
  CARD*	   clone()const		{untested(); return new DEV_LOGIC(*this);}
  void	   parse(CS&);
  void	   print(OMSTREAM&,int)const;
  void	   expand();
  void	   map_nodes()	{COMPONENT::map_nodes(); subckt().map_nodes();}
  void	   precalc();

  void	   tr_alloc_matrix();
  void	   dc_begin();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  bool	   tr_needs_eval();
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  double   tr_review();
  void	   tr_accept();
  void	   tr_unload();
  //double tr_amps()const		//ELEMENT
  double   tr_involts()const		{unreachable(); return 0;}
  double   tr_involts_limited()const	{unreachable(); return 0;}
  double   tr_probe_num(CS&)const;

  void	   ac_alloc_matrix();
  void	   ac_begin();
  void	   do_ac()			{untested(); subckt().do_ac();}
  void	   ac_load()			{unreachable();}
  COMPLEX  ac_involts()const		{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(CS&)const;
public:
  static int count()			{return _count;}
private:
  bool	   tr_eval_digital();
  bool	   want_analog()const{return subckt().exists() &&
    ((OPT::mode == moANALOG) || (OPT::mode == moMIXED && _quality != qGOOD));}
  bool	   want_digital()const{return subckt().empty() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));}
private:
  int		_num_nodes;
  int		_lastchangenode;
  int		_quality;
  const char*	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
  node_t	nodes[PORTS_PER_GATE];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
  enum {OUTNODE=0, GND_NODE=1, PWR_NODE=2, ENABLE=3, BEGIN_IN=4}; /* node labels */
};
/*--------------------------------------------------------------------------*/
class COMMON_LOGIC : public COMMON_COMPONENT {
protected:
  explicit	COMMON_LOGIC(int c=0)
    :COMMON_COMPONENT(c), incount(0) {++_count;}
  explicit	COMMON_LOGIC(const COMMON_LOGIC& p)
    :COMMON_COMPONENT(p), incount(p.incount) {untested(); ++_count;}
public:
		~COMMON_LOGIC()			{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  static  int	count()				{return _count;}
  virtual LOGICVAL logic_eval(NODE**)const	= 0;
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
  LOGICVAL logic_eval(NODE** ns)const {untested(); LOGICVAL out(lvSTABLE1);
    for (int ii=0; ii<incount; ++ii){out &= ns[ii]->lv();} return out;}
  virtual const char* name()const	  {return "and";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NAND : public COMMON_LOGIC {
private:
  explicit LOGIC_NAND(const LOGIC_NAND&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_NAND(*this);}
public:
  explicit LOGIC_NAND(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(NODE** ns)const {untested(); LOGICVAL out(lvSTABLE1);
    for (int ii=0; ii<incount; ++ii){out &= ns[ii]->lv();} return ~out;}
  virtual const char* name()const	  {return "nand";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_OR : public COMMON_LOGIC {
private:
  explicit LOGIC_OR(const LOGIC_OR& p)	 :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_OR(*this);}
public:
  explicit LOGIC_OR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(NODE** ns)const {untested(); LOGICVAL out(lvSTABLE0);
    for (int ii=0; ii<incount; ++ii){out |= ns[ii]->lv();} return out;}
  virtual const char* name()const	  {return "or";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NOR : public COMMON_LOGIC {
private:
  explicit LOGIC_NOR(const LOGIC_NOR& p) :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_NOR(*this);}
public:
  explicit LOGIC_NOR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(NODE** ns)const {untested(); LOGICVAL out(lvSTABLE0);
    for (int ii=0; ii<incount; ++ii){out |= ns[ii]->lv();} return ~out;}
  virtual const char* name()const	  {return "nor";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_XOR : public COMMON_LOGIC {
private:
  explicit LOGIC_XOR(const LOGIC_XOR& p) :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_XOR(*this);}
public:
  explicit LOGIC_XOR(int c=0)		  :COMMON_LOGIC(c) {untested();}
  LOGICVAL logic_eval(NODE** ns)const
    {untested(); return ns[0]->lv() ^ ns[1]->lv();}
  virtual const char* name()const	  {return "xor";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_INV : public COMMON_LOGIC {
private:
  explicit LOGIC_INV(const LOGIC_INV& p) :COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_INV(*this);}
public:
  explicit LOGIC_INV(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(NODE** ns)const	  {return ~ns[0]->lv();}
  virtual const char* name()const	  {return "inv";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NONE : public COMMON_LOGIC {
private:
  explicit LOGIC_NONE(const LOGIC_NONE&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const {untested(); return new LOGIC_NONE(*this);}
public:
  explicit LOGIC_NONE(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(NODE**)const	  {untested(); return lvUNKNOWN;}
  virtual const char* name()const	  {return "error";}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
