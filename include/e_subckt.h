/*$Id: e_subckt.h,v 26.126 2009/10/16 05:29:28 al Exp $ -*- C++ -*-
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
 * base class for elements made of subcircuits
 */
//testing=script 2006.07.12
#ifndef E_SUBCKT_H
#define E_SUBCKT_H
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class BASE_SUBCKT : public COMPONENT {
protected:
  explicit BASE_SUBCKT()
    :COMPONENT() {}
  explicit BASE_SUBCKT(const BASE_SUBCKT& p)
    :COMPONENT(p) {}
  ~BASE_SUBCKT() {}
protected: // override virtual
  //char  id_letter()const		//CARD/null
  std::string dev_type()const {assert(common()); return common()->modelname();}
  int	  tail_size()const		{return 1;}
  //int	  max_nodes()const		//COMPONENT/null
  //int	  num_nodes()const		//COMPONENT/null
  //int	  min_nodes()const		//COMPONENT/null
  int     matrix_nodes()const		{return 0;}
  int     net_nodes()const		{return _net_nodes;}
  //CARD* clone()const			//CARD/null
  //void  precalc_first()	{assert(subckt()); subckt()->precalc();}
  //void  expand()			//COMPONENT
  //void  precalc_last()	{assert(subckt()); subckt()->precalc();}
  //void  map_nodes();
  void	  tr_begin()	{assert(subckt()); subckt()->tr_begin();}
  void	  tr_restore()	{assert(subckt()); subckt()->tr_restore();}
  void	  dc_advance()	{assert(subckt()); subckt()->dc_advance();}
  void	  tr_advance()	{assert(subckt()); subckt()->tr_advance();}
  void	  tr_regress()	{assert(subckt()); subckt()->tr_regress();}
  bool	  tr_needs_eval()const
	{assert(subckt()); return subckt()->tr_needs_eval();}
  void	  tr_queue_eval() {assert(subckt()); subckt()->tr_queue_eval();}
  bool	  do_tr()
	{assert(subckt());set_converged(subckt()->do_tr());return converged();}
  void	  tr_load()	{assert(subckt()); subckt()->tr_load();}
  TIME_PAIR tr_review()	{assert(subckt()); return _time_by = subckt()->tr_review();}
  void	  tr_accept()	{assert(subckt()); subckt()->tr_accept();}
  void	  tr_unload()	{assert(subckt()); subckt()->tr_unload();}
  void	  ac_begin()	{assert(subckt()); subckt()->ac_begin();}
  void	  do_ac()	{assert(subckt()); subckt()->do_ac();}
  void	  ac_load()	{assert(subckt()); subckt()->ac_load();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
