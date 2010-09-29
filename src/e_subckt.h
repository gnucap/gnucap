/*$Id: e_subckt.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * base class for elements made of subcircuits
 */
#ifndef E_SUBCKT_H
#define E_SUBCKT_H
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class BASE_SUBCKT : public COMPONENT {
protected:
  explicit BASE_SUBCKT()		     :COMPONENT() {}
  explicit BASE_SUBCKT(const BASE_SUBCKT& p) :COMPONENT(p){}
protected: // override virtual
  //char  id_letter()const		//CARD/null
  //const char* dev_type()const		//COMPONENT/null
  //int	  numnodes()const		//COMPONENT/null
  //CARD* clone()const			//CARD/null
  //void  parse(CS&)			//CARD/null
  //void  print(OMSTREAM&,int)const	//CARD/null
  void	  expand()	{subckt().expand();}
  void	  map_nodes()	{COMPONENT::map_nodes(); subckt().map_nodes();}
  void	  precalc()	{subckt().precalc();}
  void	  dc_begin()	{subckt().dc_begin();}
  void	  tr_begin()	{subckt().tr_begin();}
  void	  tr_restore()	{subckt().tr_restore();}
  void	  dc_advance()	{subckt().dc_advance();}
  void	  tr_advance()	{subckt().tr_advance();}
  bool	  tr_needs_eval(){return subckt().tr_needs_eval();}
  void	  tr_queue_eval(){subckt().tr_queue_eval();}
  bool	  do_tr()	{set_converged(subckt().do_tr()); return converged();}
  void	  tr_load()	{subckt().tr_load();}
  double  tr_review()	{return subckt().tr_review();}
  void	  tr_accept()	{subckt().tr_accept();}
  void	  tr_unload()	{subckt().tr_unload();}
  void	  ac_begin()	{subckt().ac_begin();}
  void	  do_ac()	{subckt().do_ac();}
  void	  ac_load()	{unreachable(); subckt().ac_load();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
