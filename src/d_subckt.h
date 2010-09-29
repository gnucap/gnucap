/*$Id: d_subckt.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * data structures for subcircuits
 */
#ifndef D_SUBCKT_H
#define D_SUBCKT_H
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
#define NODESPERSUBCKT 1000
#define PORTS_PER_SUBCKT 100
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
private:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT();
		~DEV_SUBCKT()	{--_count;}
private: // override virtual
  char		id_letter()const	{return 'X';}
  const char*	dev_type()const		{untested(); return "subckt";}
  int		numnodes()const		{untested(); return 0;}
  CARD*		clone()const		{return new DEV_SUBCKT(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&,int)const;
  void		expand();
  double	tr_probe_num(CS&)const;
public:
  static int	count()			{return _count;}
private:
  static int	_count;
  node_t	_nodes[PORTS_PER_SUBCKT];
};
/*--------------------------------------------------------------------------*/
class COMMON_SUBCKT : public COMMON_COMPONENT {
private:
  explicit COMMON_SUBCKT(const COMMON_SUBCKT& p)
    :COMMON_COMPONENT(p){++_count;}
public:
  explicit COMMON_SUBCKT(int c=0)	:COMMON_COMPONENT(c){++_count;}
	   ~COMMON_SUBCKT()		{--_count;}
  bool operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new COMMON_SUBCKT(*this);}
  const char*	name()const		{untested(); return "subckt";}
  static int	count()			{return _count;}
private:
  static int	_count;
};
/*--------------------------------------------------------------------------*/
class MODEL_SUBCKT : public COMPONENT {
private:
  explicit	MODEL_SUBCKT(const MODEL_SUBCKT&p):COMPONENT(p){unreachable();}
public:
  explicit	MODEL_SUBCKT();
		~MODEL_SUBCKT();
private: // override virtual
  char		id_letter()const	{return '\0';}
  const char*   dev_type()const	{untested(); return "";}
  int	        numnodes()const	{untested(); return 0;}
  CARD*		clone()const	{untested(); return new MODEL_SUBCKT(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&,int)const;
  bool		is_device()const	{return false;}
  void		map_nodes()		{}
public:
  static int	count()			{return _count;}
private:
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
