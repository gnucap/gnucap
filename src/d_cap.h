/*$Id: d_cap.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * capacitance devices:
 *	self-capacitance (C device)
 *	trans-capacitance (non-spice charge transfer device)
 */
#ifndef D_CAP_H
#define D_CAP_H
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
class DEV_CAPACITANCE : public STORAGE {
protected:
  explicit DEV_CAPACITANCE(const DEV_CAPACITANCE& p) :STORAGE(p) {}
public:
  explicit DEV_CAPACITANCE()	:STORAGE() {}
protected: // override virtual
  char	   id_letter()const	{return 'C';}
  const char* dev_type()const	{return "capacitor";}
  int	   numnodes()const	{return 2;}
  bool	   is_1port()const	{return true;}
  CARD*	   clone()const		{return new DEV_CAPACITANCE(*this);}
  //void   parse(CS&);		//ELEMENT
  //void   print(OMSTREAM,int)const; //ELEMENT
  //void   expand();		//CARD/nothing
  //void   map_nodes();		//ELEMENT
  //void   precalc()		//STORAGE

  //void   dc_begin();		//STORAGE
  //void   tr_begin();		//STORAGE
  //void   tr_restore();	//STORAGE
  //void   dc_advance();	//STORAGE
  //void   tr_advance();	//STORAGE
  //bool   tr_needs_eval();	//STORAGE
  //void   tr_queue_eval();	//STORAGE
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  double   tr_review();
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{tr_unload_passive();}
  //double tr_amps()const	//ELEMENT
  double   tr_involts()const	{return tr_outvolts();}
  double   tr_involts_limited()const {return tr_outvolts_limited();}
  double   tr_probe_num(CS&)const;

  void	   ac_begin()		{_ev = _y0.f1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{return ac_outvolts();}
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_TRANSCAP : public DEV_CAPACITANCE {
private:
  explicit DEV_TRANSCAP(const DEV_TRANSCAP& p) :DEV_CAPACITANCE(p){untested();}
public:
  explicit DEV_TRANSCAP()	:DEV_CAPACITANCE() {}
private: // override virtual
  char     id_letter()const	{return '\0';}
  const char* dev_type()const	{return "tcap";}
  int	   numnodes()const	{return 4;}
  bool	   is_1port()const	{untested(); return false;}
  bool	   is_2port()const	{return true;}
  bool	   f_is_value()const	{untested(); return true;}
  CARD*	   clone()const		{untested(); return new DEV_TRANSCAP(*this);}
  //void   parse(CS&);		//ELEMENT
  //void   print(OMSTREAM,int)const; //ELEMENT
  //void   expand();		//CARD/nothing
  //void   map_nodes();		//ELEMENT
  //void   precalc()		//STORAGE

  //void   dc_begin();		//STORAGE
  //void   tr_begin();		//STORAGE
  //void   tr_restore();	//STORAGE
  //void   dc_advance();	//STORAGE
  //void   tr_advance();	//STORAGE
  //bool   tr_needs_eval();	//STORAGE
  //void   tr_queue_eval();	//STORAGE
  //bool   do_tr();		//DEV_CAPACITANCE
  void	   tr_load()		{tr_load_active();}
  //double tr_review();		//DEV_CAPACITANCE
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//DEV_CAPACITANCE
  //double tr_amps()const	//ELEMENT
  double   tr_involts()const	{return dn_diff(_n[IN1].v0(),_n[IN2].v0());}
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  //double tr_probe_num(CS&)const;//DEV_CAPACITANCE

  //void    ac_begin();		//DEV_CAPACITANCE
  //void    do_ac();		//DEV_CAPACITANCE
  void	    ac_load()		{untested(); ac_load_active();}
  //COMPLEX ac_involts()const;	//DEV_CAPACITANCE
  //XPROBE  ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
// BUG:: doesn't model dynamic effects of control.
class DEV_VCCAP : public DEV_CAPACITANCE {
private:
  explicit DEV_VCCAP(const DEV_VCCAP& p) :DEV_CAPACITANCE(p) {untested();}
public:
  explicit DEV_VCCAP()		:DEV_CAPACITANCE() {}
private: // override virtual
  char     id_letter()const	{return '\0';}
  const char* dev_type()const	{return "vccap";}
  int	   numnodes()const	{return 4;}
  bool	   is_2port()const	{return true;}
  bool	   f_is_value()const	{untested(); return true;}
  CARD*	   clone()const		{return new DEV_VCCAP(*this);}
  //void   parse(CS&);		//ELEMENT
  //void   print(OMSTREAM,int)const; //ELEMENT
  //void   expand();		//CARD/nothing
  //void   map_nodes();		//ELEMENT
  //void   precalc()		//STORAGE

  //void   dc_begin();		//STORAGE
  //void   tr_begin();		//STORAGE
  //void   tr_restore();	//STORAGE
  //void   dc_advance();	//STORAGE
  //void   tr_advance();	//STORAGE
  //bool   tr_needs_eval();	//STORAGE
  //void   tr_queue_eval();	//STORAGE
  bool     do_tr();
  //void   tr_load();		//DEV_CAPACITANCE
  //double tr_review();		//DEV_CAPACITANCE
  //void   tr_accept();		//CARD/nothing
  //void   tr_unload();		//DEV_CAPACITANCE
  //double tr_amps()const	//ELEMENT
  double   tr_involts()const	{return dn_diff(_n[IN1].v0(),_n[IN2].v0());}
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  //double tr_probe_num(CS&)const;//DEV_CAPACITANCE

  //void    ac_begin();		//DEV_CAPACITANCE
  //void    do_ac();		//DEV_CAPACITANCE
  //void    ac_load();		//DEV_CAPACITANCE
  //COMPLEX ac_involts()const;	//DEV_CAPACITANCE
  //XPROBE  ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_POLY_CAP : public STORAGE {
private:
  double*  _vy0; // vector form of _y0 _values;
  double*  _vy1; // vector form of _y1 _old_values;
  double*  _vi0; // vector form of _i0
  double*  _vi1; // vector form of _i1
  int	   _n_ports;
  double   _time;
private:
  explicit DEV_POLY_CAP(const DEV_POLY_CAP& p)
    :STORAGE(p) {incomplete(); unreachable();}
public:
  explicit DEV_POLY_CAP(const std::string& label, int n_ports, node_t nodes[],
		      double vy0[], double vy1[], CARD* owner=0);
private: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  const char* dev_type()const	{unreachable(); return "pcapacitor";}
  int	   numnodes()const	{unreachable(); return _n_ports*2;}
  CARD*	   clone()const	       {unreachable(); return new DEV_POLY_CAP(*this);}
  //void   parse(CS&);		//ELEMENT
  //void   print(OMSTREAM,int)const; //ELEMENT
  //void   expand();		//CARD/nothing
  //void   map_nodes();		//ELEMENT
  //void   precalc();		//STORAGE

  //void   dc_begin();		//STORAGE
  //void   tr_begin();		//STORAGE
  //void   tr_restore();	//STORAGE
  //void   dc_advance();	//STORAGE
  //void   tr_advance();	//STORAGE
  //bool   tr_needs_eval();	//STORAGE
  //void   tr_queue_eval();	//STORAGE
  bool	   do_tr();
  void	   tr_load();
  double   tr_review()		{return review(_i0.f0, _it1.f0);}
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload();
  double   tr_amps()const;
  double   tr_involts()const	{unreachable(); return NOT_VALID;}
  double   tr_involts_limited()const {unreachable(); return NOT_VALID;}
  //double tr_probe_num(CS&)const;//ELEMENT

  //void   ac_begin();		//CARD/nothing
  void	   do_ac()		{ac_load();}
  void	   ac_load();
  COMPLEX  ac_involts()const	{unreachable(); return NOT_VALID;}
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
