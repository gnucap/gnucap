/*$Id: d_cccs.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * current controlled current source
 */
#ifndef D_CCCS_H
#define D_CCCS_H
#include "e_ccsrc.h"
/*--------------------------------------------------------------------------*/
class DEV_CCCS : public CCSRC_BASE {
private:
  explicit DEV_CCCS(const DEV_CCCS& p) :CCSRC_BASE(p) {untested();}
public:
  explicit DEV_CCCS()		:CCSRC_BASE() {}
private: // override virtual
  char	   id_letter()const	{return 'F';}
  const char* dev_type()const	{return "cccs";}
  //int	   numnodes()const	//CCSRC_BASE=4
  CARD*	   clone()const		{untested(); return new DEV_CCCS(*this);}
  //void   parse(CS&);		//ELEMENT
  //void   print(OMSTREAM,int)const; //CCSRC_BASE
  //void   expand();		//CCSRC_BASE
  //void   map_nodes();		//ELEMENT
  void	   precalc();

  void	   dc_begin();
  void	   tr_begin()		{dc_begin();}
  void	   tr_restore()		{dc_begin();}
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//CCSRC_BASE
  //void   tr_queue_eval();	//CCSRC_BASE
  bool	   do_tr();
  void	   tr_load()		{tr_load_active();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//CCSRC_BASE
  //double tr_amps()const	//ELEMENT
  //double tr_involts()const;	//CCSRC_BASE
  //double tr_involts_limited()const;	//CCSRC_BASE
  //double tr_probe_num(CS&)const;//ELEMENT

  void	    ac_begin()		{_ev = _y0.f1;}
  void	    do_ac();
  void	    ac_load()		{unreachable(); incomplete(); /*fake*/}
  //COMPLEX ac_involts()const;	//CCSRC_BASE
  //XPROBE  ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
