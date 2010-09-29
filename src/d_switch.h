/*$Id: d_switch.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * data structures for voltage (and current) controlled switch
 */
//testing=script 2006.06.14
#ifndef D_SWITCH_H
#define D_SWITCH_H
#include "e_model.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class MODEL_SWITCH : public MODEL_CARD {
private:
  explicit	MODEL_SWITCH(const MODEL_SWITCH& p) 
				:MODEL_CARD(p) {unreachable();}
public:
  explicit	MODEL_SWITCH();
private: // override virtual
  CARD*		clone()const	{untested(); return new MODEL_SWITCH(*this);}
  void		parse_spice(CS&);
  void		elabo1();
  void		print_spice(OMSTREAM&,int)const;
public:
  PARAMETER<double> vt;		/* threshold voltage */
  PARAMETER<double> vh;		/* hysteresis voltage */
  PARAMETER<double> ron;	/* on resistance */
  PARAMETER<double> roff;	/* off resistance */
  PARAMETER<double> von;
  PARAMETER<double> voff;
  enum control_t {VOLTAGE, CURRENT};
  control_t type;	/* current or voltage controlled */
private:
  static double const _default_vt;
  static double const _default_vh;
  static double const _default_ron;
  static double const _default_roff;
};
/*--------------------------------------------------------------------------*/
class COMMON_SWITCH : public COMMON_COMPONENT {
private:
  explicit COMMON_SWITCH(const COMMON_SWITCH& p)
    :COMMON_COMPONENT(p) {}
public:
  explicit COMMON_SWITCH(int c=0)	:COMMON_COMPONENT(c) {}
  bool operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new COMMON_SWITCH(*this);}
  const char* name()const	 {untested(); return "switch";}
};
/*--------------------------------------------------------------------------*/
class SWITCH_BASE : public ELEMENT {
protected:
  explicit	SWITCH_BASE();
  explicit	SWITCH_BASE(const SWITCH_BASE& p);
protected: // override virtual
  const char* dev_type()const	= 0;
  int	   max_nodes()const	= 0;
  int	   min_nodes()const	= 0;
  int	   out_nodes()const	{untested(); return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	= 0;
  bool	   is_1port()const	{untested(); return true;}
  CARD*	   clone()const		= 0;
  void	   parse_spice(CS&);
  void     print_spice(OMSTREAM&,int)const;
  void     elabo1();
  //void   map_nodes();		//ELEMENT
  void     precalc();

  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  void	   dc_begin()		{tr_begin();}
  void	   tr_begin();
  //void   tr_restore();	//CARD/nothing
  void     dc_advance();
  void     tr_advance();
  bool	   tr_needs_eval()const	{return (SIM::phase != SIM::pTRAN);}
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  DPAIR    tr_review();
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{untested(); tr_unload_passive();}
  double   tr_involts()const	{untested(); return tr_outvolts();}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const
				{unreachable(); return tr_outvolts_limited();}
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const	//ELEMENT
  double   tr_probe_num(CS&)const;

  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void	   ac_begin()		{_ev = _y0.f1; _acg = _m0.c1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{untested(); return ac_outvolts();}
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
protected:
  std::string	 _input_label;	/*this is here instead of in Cswitch*/
  const ELEMENT* _input;	/* due to bad design and lazyness */
private:
  enum state_t {_UNKNOWN, _ON, _OFF};
  state_t	_ic;		/* initial state, belongs in common */
  double	_time_future;
  enum {_keep_time_steps = 2};
  double	_time[_keep_time_steps];
  double	_in[_keep_time_steps];
  state_t	_state[_keep_time_steps];
};
/*--------------------------------------------------------------------------*/
class DEV_VSWITCH : public SWITCH_BASE {
private:
  explicit  DEV_VSWITCH(const DEV_VSWITCH& p) :SWITCH_BASE(p) {untested();}
public:
  explicit  DEV_VSWITCH()	:SWITCH_BASE() {}
private: // override virtual
  int	    max_nodes()const	{return 4;}
  int	    min_nodes()const	{return 4;}
  int	    net_nodes()const	{return 4;}
  CARD*	    clone()const	{untested(); return new DEV_VSWITCH(*this);}
  const char* dev_type()const	{untested(); return "vswitch";}
  char	    id_letter()const	{untested(); return 'S';}
};
/*--------------------------------------------------------------------------*/
class DEV_CSWITCH : public SWITCH_BASE {
private:
  explicit  DEV_CSWITCH(const DEV_CSWITCH& p) :SWITCH_BASE(p) {untested();}
public:
  explicit  DEV_CSWITCH()	:SWITCH_BASE() {}
private: // override virtual
  int	    max_nodes()const	{return 2;}
  int	    min_nodes()const	{return 2;}
  int	    net_nodes()const	{return 2;}
  CARD*	    clone()const	{untested(); return new DEV_CSWITCH(*this);}
  void	    elabo1();
  const char* dev_type()const	{untested(); return "iswitch";}
  char	    id_letter()const	{untested(); return 'W';}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
