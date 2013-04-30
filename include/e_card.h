/*$Id: e_card.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * base class for anything in a netlist or circuit file
 */
//testing=script 2007.07.13
#ifndef E_CARD_H
#define E_CARD_H
#include "e_base.h"
/*--------------------------------------------------------------------------*/
// this file
class CARD;
/*--------------------------------------------------------------------------*/
// external
class node_t;
class CARD_LIST;
class PARAM_LIST;
class LANGUAGE;
class TIME_PAIR;
/*--------------------------------------------------------------------------*/
class INTERFACE CARD : public CKT_BASE {
private:
  mutable int	_evaliter;	// model eval iteration number
  CARD_LIST*	_subckt;
  CARD* 	_owner;
  bool		_constant;	// eval stays the same every iteration
protected:
  node_t*	_n;
public:
  int		_net_nodes;	// actual number of "nodes" in the netlist
  //--------------------------------------------------------------------
public:   				// traversal functions
  CARD* find_in_my_scope(const std::string& name);
  const CARD* find_in_my_scope(const std::string& name)const;
  const CARD* find_in_parent_scope(const std::string& name)const;
  const CARD* find_looking_out(const std::string& name)const;
  //--------------------------------------------------------------------
protected: // create and destroy.
  explicit CARD();
  explicit CARD(const CARD&);
public:
  virtual  ~CARD();
  virtual CARD*	 clone()const		{unreachable(); return NULL;}
  virtual CARD*	 clone_instance()const  {return clone();}
  //--------------------------------------------------------------------
public:	// "elaborate"
  virtual void	 precalc_first()	{}
  virtual void	 expand_first()		{}
  virtual void	 expand()		{}
  virtual void	 expand_last()		{}
  virtual void	 precalc_last()		{}
  virtual void	 map_nodes()		{}
  //--------------------------------------------------------------------
public:	// dc-tran
  virtual void	 tr_iwant_matrix()	{}
  virtual void	 tr_begin()		{}
  virtual void	 tr_restore()		{}
  virtual void	 dc_advance()		{}
  virtual void	 tr_advance()		{}
  virtual void	 tr_regress()		{}
  virtual bool	 tr_needs_eval()const	{return false;}
  virtual void	 tr_queue_eval()	{}
  virtual bool	 do_tr()		{return true;}
  virtual bool	 do_tr_last()		{return true;}
  virtual void	 tr_load()		{}
  virtual TIME_PAIR tr_review();	//{return TIME_PAIR(NEVER,NEVER);}
  virtual void	 tr_accept()		{}
  virtual void	 tr_unload()		{untested();}
  //--------------------------------------------------------------------
public:	// ac
  virtual void	 ac_iwant_matrix()	{}
  virtual void	 ac_begin()		{}
  virtual void	 do_ac()		{}
  virtual void	 ac_load()		{}
  //--------------------------------------------------------------------
public:	// state, aux data
  virtual char id_letter()const	{unreachable(); return '\0';}
  virtual int  net_nodes()const	{untested();return 0;}
  virtual bool is_device()const	{return false;}
  virtual void set_slave()	{untested(); assert(!subckt());}
	  bool evaluated()const;

  void	set_constant(bool c)	{_constant = c;}
  bool	is_constant()const	{return _constant;}
  //--------------------------------------------------------------------
public: // owner, scope
  virtual CARD_LIST*	   scope();
  virtual const CARD_LIST* scope()const;
  virtual bool		   makes_own_scope()const  {return false;}

  CARD*		owner()		   {return _owner;}
  const CARD*	owner()const	   {return _owner;}
  void		set_owner(CARD* o) {assert(!_owner||_owner==o); _owner=o;}
  //--------------------------------------------------------------------
public: // subckt
  CARD_LIST*	     subckt()		{return _subckt;}
  const CARD_LIST*   subckt()const	{return _subckt;}
  void	  new_subckt();
  void	  new_subckt(const CARD* model, CARD* owner, const CARD_LIST* scope, PARAM_LIST* p);
  void	  renew_subckt(const CARD* model, CARD* owner, const CARD_LIST* scope, PARAM_LIST* p);
  //--------------------------------------------------------------------
public:	// type
  virtual std::string dev_type()const	{unreachable(); return "";}
  virtual void set_dev_type(const std::string&);
  //--------------------------------------------------------------------
public:	// label -- in CKT_BASE
  // non-virtual void set_label(const std::string& s) //BASE
  // non-virtual const std::string& short_label()const //BASE
  /*virtual*/ const std::string long_label()const; // no further override
  //--------------------------------------------------------------------
public:	// ports -- mostly defer to COMPONENT
  node_t& n_(int i)const;
  int     connects_to(const node_t& node)const;
  //--------------------------------------------------------------------
public: // parameters
  virtual void set_param_by_name(std::string, std::string);
  virtual void set_param_by_index(int i, std::string&, int offset)
				{untested(); throw Exception_Too_Many(i, 0, offset);}
  virtual int  param_count_dont_print()const	   {return 0;}
  virtual int  param_count()const		   {return 0;}
  virtual bool param_is_printable(int)const	   {untested(); return false;}
  virtual std::string param_name(int)const	   {return "";}
  virtual std::string param_name(int i,int j)const {return (j==0) ? param_name(i) : "";}
  virtual std::string param_value(int)const	   {untested(); return "";}
  virtual std::string value_name()const = 0;
  //--------------------------------------------------------------------
public:	// obsolete -- do not use in new code
  virtual bool use_obsolete_callback_parse()const {return false;}
  virtual bool use_obsolete_callback_print()const {return false;}
  virtual void print_args_obsolete_callback(OMSTREAM&,LANGUAGE*)const {unreachable();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
