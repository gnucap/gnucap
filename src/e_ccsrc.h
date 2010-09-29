/*$Id: e_ccsrc.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * current controlled source base
 */
//testing=script 2006.07.12
#ifndef E_CCSRC_H
#define E_CCSRC_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class CCSRC_BASE : public ELEMENT {
private:
protected:
  explicit	CCSRC_BASE()
    :ELEMENT(), _input_label(), _input(0) {}
  explicit	CCSRC_BASE(const CCSRC_BASE& p)
    :ELEMENT(p), _input_label(p._input_label), _input(p._input) {untested();}
  ~CCSRC_BASE() {}
protected: // override virtual
  int	   stop_nodes()const	{return 2;}
  int	   max_nodes()const	{untested();return 4;}
  int	   min_nodes()const	{return 4;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 2;}
  void	   parse_more_nodes(CS&, int);
  void	   print_spice(OMSTREAM& o, int detail)const;
  void	   elabo1();
  bool	   tr_needs_eval()const	{assert(!is_q_for_eval()); return true;}
  //void   tr_queue_eval()	//ELEMENT
  void	   tr_unload()		{untested(); tr_unload_active();}
  double   tr_involts()const	{untested();return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_input()const	{untested(); return _input->tr_amps();}
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  double   tr_input_limited()const {return _input->tr_amps();}
  COMPLEX  ac_involts()const {untested();return _n[IN1]->vac()-_n[IN2]->vac();}
  bool	   is_2port()const		{untested();return true;}  
public:
  void	set_parameters_cc(const std::string& Label, CARD* Parent,
		       COMMON_COMPONENT* Common, double Value,
		       const node_t& N0, const node_t& N1,
		       ELEMENT* Input);
protected:
  std::string	 _input_label;
  const ELEMENT* _input;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
