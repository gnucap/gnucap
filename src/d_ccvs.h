/*$Id: d_ccvs.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * current controlled voltage source
 */
//testing=script 2006.07.17
#ifndef D_CCVS_H
#define D_CCVS_H
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
class DEV_CCVS : public CCSRC_BASE {
private:
  explicit DEV_CCVS(const DEV_CCVS& p) :CCSRC_BASE(p) {untested();}
public:
  explicit DEV_CCVS()		:CCSRC_BASE() {}
private: // override virtual
  char	   id_letter()const	{untested();return 'H';}
  const char* dev_type()const	{return "ccvs";}
  //int	   max_nodes()const	//CCSRC_BASE=4
  //int	   num_nodes()const	//CCSRC_BASE=4
  //int	   min_nodes()const	//CCSRC_BASE=4
  //int	   out_nodes()const	//CCSRC_BASE=2
  //int	   matrix_nodes()const	//CCSRC_BASE=4
  //int	   net_nodes()const	//CCSRC_BASE=2
  CARD*	   clone()const		{untested();return new DEV_CCVS(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //CCSRC_BASE
  //void   elabo1();		//CCSRC_BASE
  //void   map_nodes();		//ELEMENT
  void	   precalc();

  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  void	   dc_begin();
  void	   tr_begin()		{dc_begin();}
  void	   tr_restore()		{untested();dc_begin();}
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//CCSRC_BASE
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_loss(); tr_load_active();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//CCSRC_BASE
  //double tr_involts()const;	//CCSRC_BASE
  //double tr_input()const	//CCSRC_BASE
  //double tr_involts_limited()const;	//CCSRC_BASE
  //double tr_input_limited()const //CCSRC_BASE
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(CS&)const;//ELEMENT

  void	    ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	    ac_begin()		{_loss1=_loss0=1./OPT::shortckt; _ev = _y0.f1;}
  void	    do_ac();
  void	    ac_load()		{unreachable(); incomplete(); /*fake*/}
  //COMPLEX ac_involts()const;	//CCSRC_BASE
  COMPLEX   ac_amps()const	{untested(); return ELEMENT::ac_amps();}
  //XPROBE  ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
