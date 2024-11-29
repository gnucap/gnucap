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
#ifndef E_LOGIC_H
#define E_LOGIC_H
#include "e_logicval.h"
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class INTERFACE COMMON_LOGIC : public COMMON_COMPONENT {
protected:
  enum {PORTS_PER_GATE = 10};
  static int	_count;
protected:
  explicit	COMMON_LOGIC(int c=0)
    :COMMON_COMPONENT(c) {++_count;}
  explicit	COMMON_LOGIC(const COMMON_LOGIC& p)
    :COMMON_COMPONENT(p) {++_count;}
public:
		~COMMON_LOGIC()			{--_count;}
  bool operator==(const COMMON_COMPONENT&)const override;
  static  int	count()				{untested();return _count;}
  virtual LOGICVAL logic_eval(const node_t*, int)const	= 0;

  void		set_param_by_index(int, std::string&, int)override;
  bool		param_is_printable(int)const override;
  std::string	param_name(int)const override;
  std::string	param_name(int,int)const override;
  std::string	param_value(int)const override;
  int param_count()const override {return (1 + COMMON_COMPONENT::param_count());}
  virtual std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < PORTS_PER_GATE);
    static std::string names[PORTS_PER_GATE] = {"out",
			"in1", "in2", "in3", "in4", "in5", "in6", "in7", "in8", "in9"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
