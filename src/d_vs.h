/*$Id: d_vs.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * fixed voltage source
 */
//testing=script 2006.07.17
#ifndef D_VS_H
#define D_VS_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class DEV_VS : public ELEMENT {
private:
  explicit DEV_VS(const DEV_VS& p) :ELEMENT(p) {}
public:
  explicit DEV_VS()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{return 'V';}
  const char* dev_type()const	{return "vsource";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   is_source()const	{return true;}
  bool	   f_is_value()const	{return true;}
  CARD*	   clone()const		{return new DEV_VS(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  //void   map_nodes();		//ELEMENT
  void	   precalc();

  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  void	   dc_begin();
  void	   tr_begin()		{dc_begin();}
  void	   tr_restore()		{dc_begin();}
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_loss(); tr_load_source();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{untested();tr_unload_source();}
  double   tr_involts()const	{return 0.;}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const {unreachable(); return 0.;}
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(CS&)const;//ELEMENT

  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void	   ac_begin()	{_loss1 = _loss0 = 1./OPT::shortckt; _acg = _ev = 0.;}
  void	   do_ac();
  void	   ac_load()		{ac_load_loss(); ac_load_source();}
  COMPLEX  ac_involts()const	{return 0.;}
  COMPLEX  ac_amps()const	{return (_acg + ac_outvolts()*_loss0);}
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
