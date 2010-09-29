/*$Id: e_ccsrc.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * current controlled source base
 */
#ifndef E_CCSRC_H
#define E_CCSRC_H
#include "l_denoise.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class CCSRC_BASE : public ELEMENT {
  friend class DEV_MUTUAL_L;
protected:
  explicit	CCSRC_BASE()
    :ELEMENT(), _input_label(), _input(0) {}
  explicit	CCSRC_BASE(const CCSRC_BASE& p)
    :ELEMENT(p), _input_label(p._input_label), _input(p._input) {untested();}
protected: // override virtual
  int	   numnodes()const	{return 4;}
  void	   parse_more_nodes(CS&, int);
  void	   print(OMSTREAM& where, int detail)const;
  void	   expand();
  bool	   tr_needs_eval()	{untested(); return true;}
  void	   tr_queue_eval()	{q_eval();}
  void	   tr_unload()		{untested(); tr_unload_active();}
  double   tr_involts()const
		{untested(); return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  COMPLEX  ac_involts()const {untested();return _n[IN1].vac()-_n[IN2].vac();}
  bool	   is_2port()const		{return true;}  
public:
  void	set_parameters(const std::string& Label, CARD* Parent,
		       COMMON_COMPONENT* Common, double Value,
		       const node_t& N0, const node_t& N1,
		       ELEMENT* Input);
protected:
  std::string	_input_label;
  ELEMENT*	_input;
private:
  enum	{PRINTNODES = 2};
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
