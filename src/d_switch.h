/*$Id: d_switch.h,v 22.12 2002/07/26 08:02:01 al Exp $ -*- C++ -*-
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
 * data structures for voltage (and current) controlled switch
 */
#ifndef D_SWITCH_H
#define D_SWITCH_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class MODEL_SWITCH : public MODEL_CARD {
friend class SWITCH_BASE;
private:
  explicit	MODEL_SWITCH(const MODEL_SWITCH& p) 
				:MODEL_CARD(p) {unreachable();}
public:
  explicit	MODEL_SWITCH();
private: // override virtual
  CARD*		clone()const	{untested(); return new MODEL_SWITCH(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&,int)const;
private:
  double    vt;		/* threshold voltage */
  double    vh;		/* hysteresis voltage */
  double    ron;	/* on resistance */
  double    roff;	/* off resistance */
  enum control_t {VOLTAGE, CURRENT};
  control_t type;	/* current or voltage controlled */
};
/*--------------------------------------------------------------------------*/
class COMMON_SWITCH : public COMMON_COMPONENT {
private:
  explicit COMMON_SWITCH(const COMMON_SWITCH& p)
    :COMMON_COMPONENT(p) {}
public:
  explicit COMMON_SWITCH(int c=0)	:COMMON_COMPONENT(c) {}
  bool operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new COMMON_SWITCH(*this);}
  const char* name()const		{untested(); return "switch";}
};
/*--------------------------------------------------------------------------*/
class SWITCH_BASE : public ELEMENT {
protected:
  explicit	SWITCH_BASE();
  explicit	SWITCH_BASE(const SWITCH_BASE& p);
  void		parse_sb(CS&,int);
  void		expand_sb();
private: // override virtual
  const char* dev_type()const	= 0;
  int	   max_nodes()const	{unreachable(); return 4;}
  int	   min_nodes()const	{unreachable(); return 4;}
  int	   out_nodes()const	{return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	= 0;
  bool	   is_1port()const	{return true;}
  CARD*	   clone()const		= 0;
  void	   parse(CS&)		= 0;
  void     print(OMSTREAM&,int)const;
  void     expand()		= 0;		
  //void   map_nodes();		//ELEMENT
  void     precalc();

  void	   tr_alloc_matrix()	{tr_alloc_matrix_passive();}
  void	   dc_begin();
  void	   tr_begin()		{dc_begin();}
  //void   tr_restore();	//CARD/nothing
  void     dc_advance()		{_previous_state = _current_state;}
  void     tr_advance()		{_previous_state = _current_state;}
  bool	   tr_needs_eval()	{return true;}
  //void   tr_queue_eval()	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{tr_unload_passive();}
  //double tr_amps()const	//ELEMENT
  double   tr_involts()const	{return tr_outvolts();}
  double   tr_involts_limited()const
				{unreachable(); return tr_outvolts_limited();}
  //double tr_probe_num(CS&)const;//ELEMENT

  void	   ac_alloc_matrix()	{ac_alloc_matrix_passive();}
  void	   ac_begin()		{_ev = _y0.f1; _acg = _m0.c1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{return ac_outvolts();}
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
protected:
  std::string	_input_label;		/*this is here instead of in Cswitch*/
  ELEMENT*	_input;		        /* due to bad design and lazyness */
private:
  enum state_t {_UNKNOWN, _ON, _OFF};
  state_t	_ic;		/* initial state, belongs in common */
  state_t	_current_state;	/* state 1 iter ago (may be bad) */
  state_t	_previous_state;/* state 1 time or step ago (known good) */
};
/*--------------------------------------------------------------------------*/
class DEV_VSWITCH : public SWITCH_BASE {
private:
  explicit  DEV_VSWITCH(const DEV_VSWITCH& p):SWITCH_BASE(p){untested();}
public:
  explicit  DEV_VSWITCH()	:SWITCH_BASE(){}
private: // override virtual
  int	    net_nodes()const	{return 4;}
  CARD*	    clone()const	{untested();return new DEV_VSWITCH(*this);}
  void	    parse(CS& cmd)	{parse_sb(cmd,4);}
  void	    expand()		{expand_sb();}
  const char* dev_type()const	{return "vswitch";}
  char	    id_letter()const	{return 'S';}
};
/*--------------------------------------------------------------------------*/
class DEV_CSWITCH : public SWITCH_BASE {
private:
  explicit  DEV_CSWITCH(const DEV_CSWITCH& p) :SWITCH_BASE(p){untested();}
public:
  explicit  DEV_CSWITCH()	:SWITCH_BASE(){}
private: // override virtual
  int	    net_nodes()const	{return 2;}
  CARD*	    clone()const	{untested();return new DEV_CSWITCH(*this);}
  void	    parse(CS& cmd)	{parse_sb(cmd,2);}
  void	    expand();
  const char* dev_type()const	{return "iswitch";}
  char	    id_letter()const	{return 'W';}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
