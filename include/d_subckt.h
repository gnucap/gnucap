/*$Id: d_subckt.h,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * data structures for subcircuits
 */
//testing=script,sparse 2006.07.17
#ifndef D_SUBCKT_H
#define D_SUBCKT_H
#include "e_node.h"
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
#define PORTS_PER_SUBCKT 100
//BUG// fixed limit on number of ports
/*--------------------------------------------------------------------------*/
class INTERFACE MODEL_SUBCKT : public COMPONENT {
private:
  explicit	MODEL_SUBCKT(const MODEL_SUBCKT&p);
public:
  explicit	MODEL_SUBCKT();
		~MODEL_SUBCKT();
public: // override virtual
  char		id_letter()const	{untested();return '\0';}
  CARD*	clone_instance()const;
  bool		print_type_in_spice()const {unreachable(); return false;}
  std::string   value_name()const	{incomplete(); return "";}
  std::string   dev_type()const		{untested(); return "";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{untested();return 0;}
  int		net_nodes()const	{return _net_nodes;}
  CARD*		clone()const		{return new MODEL_SUBCKT(*this);}
  bool		is_device()const	{return false;}
  void		precalc_first()		{}
  void		expand()		{}
  void		precalc_last()		{}
  bool		makes_own_scope()const  {return true;}
  void		map_nodes()		{}
  CARD_LIST*	   scope()		{return subckt();}
  const CARD_LIST* scope()const		{return subckt();}

  std::string port_name(int)const {
    return "";
  }
public:
  static int	count()			{return _count;}

private:
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
};
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
  friend class MODEL_SUBCKT;
private:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT();
		~DEV_SUBCKT()	{--_count;}
private: // override virtual
  char		id_letter()const	{return 'X';}
  bool		print_type_in_spice()const {return true;}
  std::string   value_name()const	{return "#";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{return 0;}
  int		net_nodes()const	{return _net_nodes;}
  CARD*		clone()const		{return new DEV_SUBCKT(*this);}
  void		precalc_first();
  void		expand();
  void		precalc_last();
  double	tr_probe_num(const std::string&)const;
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}

  std::string port_name(int i)const {itested();
    if (_parent) {itested();
      return _parent->port_value(i);
    }else{itested();
      return "";
    }
  }
public:
  static int	count()			{return _count;}
private:
  const MODEL_SUBCKT* _parent;
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
};
/*--------------------------------------------------------------------------*/
class INTERFACE COMMON_SUBCKT : public COMMON_COMPONENT {
private:
  explicit COMMON_SUBCKT(const COMMON_SUBCKT& p)
    :COMMON_COMPONENT(p), _params(p._params) {++_count;}
public:
  explicit COMMON_SUBCKT(int c=0)	:COMMON_COMPONENT(c) {++_count;}
	   ~COMMON_SUBCKT()		{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new COMMON_SUBCKT(*this);}
  std::string	name()const		{itested(); return "subckt";}
  static int	count()			{return _count;}

  void set_param_by_name(std::string Name, std::string Value) {_params.set(Name, Value);}
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int param_count()const
	{return (static_cast<int>(_params.size()) + COMMON_COMPONENT::param_count());}

  void		precalc_first(const CARD_LIST*);
  void		precalc_last(const CARD_LIST*);
private:
  static int	_count;
public:
  PARAM_LIST	_params;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
