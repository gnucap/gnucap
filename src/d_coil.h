/*$Id: d_coil.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Inductance (L) component
 */
//testing=script 2006.07.17
#ifndef D_COIL_H
#define D_COIL_H
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
class DEV_INDUCTANCE : public STORAGE {
private:
  explicit DEV_INDUCTANCE(const DEV_INDUCTANCE& p) :STORAGE(p) {}
public:
  explicit DEV_INDUCTANCE()	:STORAGE() {}
public: // override virtual
  char	   id_letter()const	{return 'L';}
  const char* dev_type()const	{return "inductor";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   is_1port()const	{return true;}
  CARD*	   clone()const		{return new DEV_INDUCTANCE(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  void	   map_nodes();
  void     precalc();

  void	   tr_iwant_matrix();
  void	   dc_begin();
  void	   tr_begin();
  void	   tr_restore();
  void     dc_advance();
  void     tr_advance();
  bool     tr_needs_eval()const;
  void     tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  DPAIR    tr_review();
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload();
  double   tr_involts()const	{return tr_outvolts();}
  double   tr_input()const	{return _m0.c0 + _m0.c1 * tr_involts();}
  double   tr_involts_limited()const {return tr_outvolts_limited();}
  double   tr_input_limited()const {return _m0.c0+_m0.c1*tr_involts_limited();}
  //double tr_amps()const	//ELEMENT
  double   tr_probe_num(CS&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac();
  void	   ac_load();
  COMPLEX  ac_involts()const	{return ac_outvolts();}
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_MUTUAL_L : public COMPONENT {
private:
  explicit	DEV_MUTUAL_L(const DEV_MUTUAL_L& p);
public:
  explicit	DEV_MUTUAL_L();
private: // override virtual
  char	   id_letter()const	{untested();return 'K';}
  const char* dev_type()const	{untested(); return "mutual_inductor";}
  int	   max_nodes()const	{unreachable(); return 0;}
  int	   min_nodes()const	{unreachable(); return 0;}
  //int	   out_nodes()const	//CARD 0
  //int	   matrix_nodes()const	//CARD 0
  //int	   net_nodes()const	//CARD 0
  bool	   is_2port()const	{untested(); return true;}
  CARD*	   clone()const		{untested(); return new DEV_MUTUAL_L(*this);}
  void	   parse_spice(CS&);
  void	   print_spice(OMSTREAM& where, int detail)const;
  void	   elabo1();
  void	   map_nodes()		{}
  //void   precalc();		//CARD/nothing

  //void   dc_begin();		//CARD/nothing
  //void   tr_begin();		//CARD/nothing
  //void   tr_restore();	//CARD/nothing
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//CARD/nothing
  //void   tr_queue_eval();	//CARD/nothing
  //bool   do_tr();		//CARD/nothing
  //void   tr_load();		//CARD/nothing
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//CARD/nothing
#if 0
  double   tr_involts()const
		{untested(); return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const
			{unreachable(); return volts_limited(_n[IN1],_n[IN2]);}
#endif
  //double tr_probe_num(CS&)const;//CKT_BASE/nothing

  //void   ac_begin();		//CARD/nothing
  //void   do_ac();		//CARD/nothing
  //void   ac_load();		//CARD/nothing
  COMPLEX  ac_involts()const {untested();return _n[IN1]->vac()-_n[IN2]->vac();}
  //XPROBE ac_probe_ext(CS&)const;//CKT_BASE/nothing
private:
  std::string		_output_label;
  DEV_INDUCTANCE*	_output;
  std::string		_input_label;
  DEV_INDUCTANCE*	_input;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
