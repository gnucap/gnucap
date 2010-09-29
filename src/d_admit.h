/*$Id: d_admit.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * admittance devices:
 *	self-admittance (old Y device)
 *	trans-admittance (VCCS, G device)
 *	voltage controlled admittance
 *	poly admittance
 */
//testing=script 2006.07.17
#ifndef D_ADMIT_H
#define D_ADMIT_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class DEV_ADMITTANCE : public ELEMENT {
protected:
  explicit DEV_ADMITTANCE(const DEV_ADMITTANCE& p) :ELEMENT(p) {}
public:
  explicit DEV_ADMITTANCE()	:ELEMENT() {}
protected: // override virtual
  char	   id_letter()const	{return 'Y';}
  const char* dev_type()const	{return "admittance";}
  int	   max_nodes()const	{return 2;}
  int	   min_nodes()const	{return 2;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return 2;}
  bool	   is_1port()const	{return true;}
  CARD*	   clone()const		{untested();return new DEV_ADMITTANCE(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  //void   map_nodes();		//ELEMENT
  void	   precalc();

  void	   tr_iwant_matrix()	{tr_iwant_matrix_passive();}
  //void   dc_begin();		//CARD/nothing
  //void   tr_begin();		//CARD/nothing
  //void   tr_restore();	//CARD/nothing
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_passive();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{tr_unload_passive();}
  double   tr_involts()const	{return tr_outvolts();}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const {return tr_outvolts_limited();}
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const;	//ELEMENT
  //double tr_probe_num(CS&)const;//ELEMENT

  void	   ac_iwant_matrix()	{ac_iwant_matrix_passive();}
  void	   ac_begin()		{_acg = _ev = _y0.f1;}
  void	   do_ac();
  void	   ac_load()		{ac_load_passive();}
  COMPLEX  ac_involts()const	{untested();return ac_outvolts();}
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_VCCS : public DEV_ADMITTANCE {
protected:
  explicit DEV_VCCS(const DEV_VCCS& p) :DEV_ADMITTANCE(p) {}
public:
  explicit DEV_VCCS()		:DEV_ADMITTANCE() {}
protected: // override virtual
  char	   id_letter()const	{return 'G';}
  const char* dev_type()const	{return "vccs";}
  int	   max_nodes()const	{return 4;}
  int	   min_nodes()const	{return 4;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 4;}
  bool	   is_1port()const	{untested();return false;}
  bool	   is_2port()const	{return true;}
  CARD*	   clone()const		{return new DEV_VCCS(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  //void   map_nodes();		//ELEMENT
  //void   precalc();		//DEV_ADMITTANCE

  void	   tr_iwant_matrix()	{tr_iwant_matrix_active();}
  //void   dc_begin();		//CARD/nothing
  //void   tr_begin();		//CARD/nothing
  //void   tr_restore();	//CARD/nothing
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  //bool   do_tr()		//DEV_ADMITTANCE
  void	   tr_load()		{tr_load_active();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()		{untested();tr_unload_active();}
  double   tr_involts()const	{return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const {return volts_limited(_n[IN1],_n[IN2]);}
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(CS&)const;//ELEMENT

  void	   ac_iwant_matrix()	{ac_iwant_matrix_active();}
  //void   ac_begin();		//DEV_ADMITTANCE
  //void   do_ac();		//DEV_ADMITTANCE
  void	   ac_load()		{ac_load_active();}
  COMPLEX  ac_involts()const	{return _n[IN1]->vac() - _n[IN2]->vac();}
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_VCG : public DEV_VCCS {
private:
  explicit DEV_VCG(const DEV_VCG& p) :DEV_VCCS(p) {untested();}
public:
  explicit DEV_VCG()		:DEV_VCCS() {}
private: // override virtual
  char	   id_letter()const	{untested();return '\0';}
  const char* dev_type()const	{return "vcg";}
  int	   max_nodes()const	{return 4;}
  int	   min_nodes()const	{return 4;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 4;}
  bool	   is_2port()const	{untested();return true;}
  CARD*	   clone()const		{untested();return new DEV_VCG(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  //void   map_nodes();		//ELEMENT
  void	   precalc();

  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  //void   dc_begin();		//CARD/nothing
  //void   tr_begin();		//CARD/nothing
  //void   tr_restore();	//CARD/nothing
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load()		{tr_load_shunt(); tr_load_active();}
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload()	{untested(); tr_unload_shunt(); tr_unload_active();}
  //double tr_involts()const	//DEV_VCCS
  //double tr_input()const	//ELEMENT
  //double tr_involts_limited()const //DEV_VCCS
  //double tr_input_limited()const //ELEMENT
  //double tr_amps()const	//ELEMENT
  //double tr_probe_num(CS&)const;//ELEMENT

  void	    ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	    ac_begin()		{_ev = _y0.f0;  _acg = _m0.c1;}
  void	    do_ac();
  void	    ac_load()		{ac_load_loss(); ac_load_active();}
  //COMPLEX ac_involts()const	//DEV_VCCS
  //COMPLEX ac_amps()const;	//ELEMENT
  //XPROBE  ac_probe_ext(CS&)const;//ELEMENT
};
/*--------------------------------------------------------------------------*/
class DEV_CPOLY_G : public ELEMENT {
protected:
  double*  _values;
  double*  _old_values;
  int	   _n_ports;
  double   _time;
  const double** _inputs;
protected:
  explicit DEV_CPOLY_G(const DEV_CPOLY_G& p)
    :ELEMENT(p) {incomplete(); unreachable();}
public:
  explicit DEV_CPOLY_G();
  ~DEV_CPOLY_G();
protected: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  const char* dev_type()const	{unreachable(); return "cpoly_g";}
  int	   max_nodes()const	{unreachable(); return 0;}
  int	   min_nodes()const	{unreachable(); return 0;}
  int	   out_nodes()const	{untested();return 2;}
  int	   matrix_nodes()const	{return _n_ports*2;}
  int	   net_nodes()const	{return _n_ports*2;}
  CARD*	   clone()const		{unreachable(); return new DEV_CPOLY_G(*this);}
  //void   parse_spice(CS&);	//ELEMENT
  //void   print_spice(OMSTREAM,int)const; //ELEMENT
  //void   elabo1();		//COMPONENT
  //void   map_nodes();		//ELEMENT
  //void   precalc();		//CARD/nothing

  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  //void   dc_begin();		//CARD/nothing
  //void   tr_begin();		//CARD/nothing
  //void   tr_restore();	//CARD/nothing
  //void   dc_advance();	//CARD/nothing
  //void   tr_advance();	//CARD/nothing
  //bool   tr_needs_eval();	//ELEMENT
  //void   tr_queue_eval();	//ELEMENT
  bool	   do_tr();
  void	   tr_load();
  //double tr_review();		//CARD/nothing
  //void   tr_accept();		//CARD/nothing
  void	   tr_unload();
  double   tr_involts()const	{unreachable(); return NOT_VALID;}
  //double tr_input()const	//ELEMENT
  double   tr_involts_limited()const {unreachable(); return NOT_VALID;}
  //double tr_input_limited()const //ELEMENT
  double   tr_amps()const;
  //double tr_probe_num(CS&)const;//ELEMENT

  void	   ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  //void   ac_begin();		//CARD/nothing
  void	   do_ac()		{ac_load();}
  void	   ac_load();
  COMPLEX  ac_involts()const	{unreachable(); return NOT_VALID;}
  COMPLEX  ac_amps()const	{unreachable(); return NOT_VALID;}
  //XPROBE ac_probe_ext(CS&)const;//ELEMENT
public:
  void set_parameters(const std::string& Label, CARD* Parent,
		      COMMON_COMPONENT* Common, double Value,
		      int state_count, double state[],
		      int node_count, const node_t nodes[]);
  //		      const double* inputs[]=0);
protected:
  bool do_tr_con_chk_and_q();
};
/*--------------------------------------------------------------------------*/
#if 0
class DEV_FPOLY_G : public DEV_CPOLY_G {
private:
  explicit DEV_FPOLY_G(const DEV_FPOLY_G& p)
    :DEV_CPOLY_G(p) {incomplete(); unreachable();}
public:
  explicit DEV_FPOLY_G() :DEV_CPOLY_G() {untested();}
private: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  const char* dev_type()const	{unreachable(); return "fpoly_g";}
  CARD*	   clone()const		{unreachable(); return new DEV_FPOLY_G(*this);}
  bool	   do_tr();
};
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
