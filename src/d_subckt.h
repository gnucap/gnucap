/*$Id: d_subckt.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * data structures for subcircuits
 */
//testing=script,sparse 2006.07.17
#ifndef D_SUBCKT_H
#define D_SUBCKT_H
#include "e_node.h"
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
#define PORTS_PER_SUBCKT 100
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
private:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT();
		~DEV_SUBCKT()	{--_count;}
private: // override virtual
  char		id_letter()const	{untested();return 'X';}
  const char*	dev_type()const		{untested(); return "subckt";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 1;}
  int		out_nodes()const	{untested();return _net_nodes;}
  int		matrix_nodes()const	{return 0;}
  int		net_nodes()const	{return _net_nodes;}
  CARD*		clone()const		{return new DEV_SUBCKT(*this);}
  void		parse_spice(CS&);
  void		print_spice(OMSTREAM&,int)const;
  void		elabo1();
  double	tr_probe_num(CS&)const;
public:
  static int	count()			{return _count;}
private:
  int		_net_nodes;
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
};
/*--------------------------------------------------------------------------*/
class COMMON_SUBCKT : public COMMON_COMPONENT {
private:
  explicit COMMON_SUBCKT(const COMMON_SUBCKT& p)
    :COMMON_COMPONENT(p), _params(p._params) {++_count;}
public:
  explicit COMMON_SUBCKT(int c=0)	:COMMON_COMPONENT(c) {++_count;}
	   ~COMMON_SUBCKT()		{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new COMMON_SUBCKT(*this);}
  const char*	name()const		{untested(); return "subckt";}
  static int	count()			{return _count;}
  bool parse_params(CS&);
  void print(OMSTREAM&)const;
private:
  static int	_count;
public:
  PARAM_LIST	_params;
};
/*--------------------------------------------------------------------------*/
class MODEL_SUBCKT : public COMPONENT {
private:
  explicit	MODEL_SUBCKT(const MODEL_SUBCKT&p):COMPONENT(p){unreachable();}
public:
  explicit	MODEL_SUBCKT();
		~MODEL_SUBCKT();
private: // override virtual
  char		id_letter()const	{untested();return '\0';}
  const char*   dev_type()const		{untested(); return "";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 1;}
  int		out_nodes()const	{untested();return 0;}
  int		matrix_nodes()const	{untested();return 0;}
  int		net_nodes()const	{return _net_nodes;}
  CARD*		clone()const	{untested(); return new MODEL_SUBCKT(*this);}
  void		parse_spice(CS&);
  void		print_spice(OMSTREAM&,int)const;
  bool		is_device()const	{return false;}
  bool		makes_own_scope()const  {return true;}
  void		map_nodes()		{}
public:
  static int	count()			{return _count;}
private:
  int		_net_nodes;
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
