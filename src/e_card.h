/*$Id: e_card.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * base class for anything in a netlist or circuit file
 */
//testing=script 2006.07.12
#ifndef E_CARD_H
#define E_CARD_H
#include "u_parameter.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
// this file
class CARD;
/*--------------------------------------------------------------------------*/
// external
class node_t;
class CARD_LIST;
class CS;
class COMMON_COMPONENT;
/*--------------------------------------------------------------------------*/
class CARD : public CKT_BASE {
private:
  mutable int	_evaliter;	// model eval iteration number
  CARD_LIST*	_subckt;
  CARD*		_owner;
public:
  node_t*	_n;
private:
  PARAMETER<double> _value;		// value, for simple parts
  bool		_constant;	// eval stays the same every iteration
  //--------------------------------------------------------------------
public:   				// traversal functions
  CARD* find_in_my_scope(const std::string& name, int warn);
  const CARD* find_in_parent_scope(const std::string& name, int warn)const;
  const CARD* find_looking_out(const std::string& name, int warn)const;
  //--------------------------------------------------------------------
public:					// virtuals. -- the important stuff
  virtual char	 id_letter()const	= 0;
  virtual CARD*	 clone()const		= 0;
  virtual void	 parse_spice(CS&)	= 0;
  virtual void	 print_spice(OMSTREAM&,int)const = 0;
  virtual void	 elabo1()		{_value.e_val(0.,scope());}
  virtual void	 map_nodes()		{}
  virtual void	 precalc()		{}
  virtual void	 tr_iwant_matrix()	{}
  virtual void	 dc_begin()		{}
  virtual void	 tr_begin()		{}
  virtual void	 tr_restore()		{}
  virtual void	 dc_advance()		{}
  virtual void	 tr_advance()		{}
  virtual bool	 tr_needs_eval()const	{return false;}
  virtual void	 tr_queue_eval()	{}
  virtual bool	 do_tr()		{return true;}
  virtual void	 tr_load()		{}
  virtual DPAIR	 tr_review()		{return DPAIR(NEVER,NEVER);}
  virtual void	 tr_accept()		{}
  virtual void	 tr_unload()		{untested();}
  //virtual void tr_print_trace(OMSTREAM&)const {untested();}
  virtual void	 ac_iwant_matrix()	{}
  virtual void	 ac_begin()		{}
  virtual void	 do_ac()		{}
  virtual void	 ac_load()		{untested();}
  //--------------------------------------------------------------------
  // virtuals defined in base, not overridden here
  // shown here only to complete the list
  //virtual double tr_probe_num(CS&)const;
  //virtual XPROBE ac_probe_ext(CS&)const;
  //--------------------------------------------------------------------
protected:				// create and destroy.
  explicit CARD();
  explicit CARD(const CARD&);
public:
  virtual  ~CARD()			{delete _subckt;}
  //--------------------------------------------------------------------
public:					// query functions.
  static double	probe(const CARD*,const std::string&);
  int		connects_to(const node_t& node)const;
  CARD_LIST*		scope();
  const CARD_LIST*	scope()const;
  //--------------------------------------------------------------------
public:					// query functions. deferred inline
  bool	evaluated()const;
  //--------------------------------------------------------------------
public:					// query functions. virtual constant
  //virtual int	max_nodes()const	// in component
  //virtual int	min_nodes()const	// in component
  virtual int	out_nodes()const	{untested();return 0;}
  virtual int	matrix_nodes()const	{return 0;}
  virtual int	net_nodes()const	{untested();return 0;}
  //virtual int	int_nodes()const	// in component
  virtual bool	is_1port()const		{return false;}
  virtual bool	is_2port()const		{return false;}
  virtual bool	is_source()const	{return false;}
  virtual bool	f_is_value()const	{return false;}
  virtual bool	is_device()const	{untested();return false;}
  virtual bool	makes_own_scope()const  {return false;}
  /*virtual*/ const std::string long_label()const;
  //--------------------------------------------------------------------
public:					// query functions.
  CARD_LIST*	     subckt()		{return _subckt;}
  const CARD_LIST*   subckt()const	{return _subckt;}
  const PARAMETER<double>& value()const	{return _value;}
  CARD*		     owner()const	{return _owner;}
  bool		     is_constant()const	{return _constant;}
  //--------------------------------------------------------------------
public:					// modifiers.
  virtual void	set_slave()	{untested(); assert(!subckt());}

  void  set_value(const PARAMETER<double>& v)	{_value = v;}
  void  set_value(double v)			{_value = v;}
public:
  virtual void  set_value(double v, COMMON_COMPONENT*)
				{unreachable(); set_value(v);}
  double* set__value()		{return _value.pointer_hack();}
public:
  void	  set_owner(CARD* o)	{assert(!_owner||_owner==o); _owner=o;}
  void	  set_constant(bool c)	{_constant = c;}

  void	  new_subckt();
};
CARD_LIST::fat_iterator findbranch(CS&,CARD_LIST::fat_iterator);
/*--------------------------------------------------------------------------*/
inline bool CARD::evaluated()const
{
  if (_evaliter == iteration_tag()) {
    return true;
  }else{
    _evaliter = iteration_tag();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
inline CARD_LIST::fat_iterator findbranch(CS& cmd, CARD_LIST* cl)
{
  return findbranch(cmd, CARD_LIST::fat_iterator(cl));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
