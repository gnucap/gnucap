/*$Id: d_trln.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * transmission lines
 */
//testing=script 2006.07.17
#ifndef D_TRNLIN_H
#define D_TRNLIN_H
#include "m_wave.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
enum {NUM_INIT_COND = 4};
/*--------------------------------------------------------------------------*/
class COMMON_TRANSLINE : public COMMON_COMPONENT {
private:
  PARAMETER<double> len;	/* length multiplier */
  PARAMETER<double> R;
  PARAMETER<double> L;
  PARAMETER<double> G;
  PARAMETER<double> C;
  PARAMETER<double> z0;		/* characteristic impedance */
  PARAMETER<double> td;		/* delay time */
  PARAMETER<double> f;		/* specification frequency */
  PARAMETER<double> nl;		/* length (wavelengths) at f */
  double   ic[NUM_INIT_COND];	/* initial conditions: v1, i1, v2, i2 */
  int	   icset;		/* flag: initial condition set */
public:
  double real_z0;
  double real_td;
private:
  explicit COMMON_TRANSLINE(const COMMON_TRANSLINE& p);
public:
  explicit COMMON_TRANSLINE(int c=0);
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {return new COMMON_TRANSLINE(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&)const;
  void		elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  //void	tr_eval(ELEMENT*)const; //COMPONENT_COMMON
  //void	ac_eval(ELEMENT*)const; //COMPONENT_COMMON
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{untested(); return "transline";}
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing/ignored
  //bool  	parse_params(CS&);	//COMPONENT_COMMON/nothing/ignored
};
/*--------------------------------------------------------------------------*/
class DEV_TRANSLINE : public ELEMENT {
private:
  WAVE	 _forward;
  WAVE	 _reflect;
  double _if0; // value of current source representing incident wave
  double _ir0; // value of current source representing reflected wave
  double _if1; // val of cs rep incident wave, one load ago
  double _ir1; // val of cs rep reflected wave, one load ago
  COMPLEX _y11;// AC equiv ckt
  COMPLEX _y12;// AC equiv ckt
private:
  explicit	DEV_TRANSLINE(const DEV_TRANSLINE& p)
    :ELEMENT(p), _forward(), _reflect(), _if0(0), _ir0(0), _if1(0), _ir1(0)
    {untested();}
public:
  explicit	DEV_TRANSLINE();
private: // override virtual
  char		id_letter()const	{untested();return 'T';}
  const char*	dev_type()const		{untested(); return "tline";}
  int		max_nodes()const	{return 4;}
  int		min_nodes()const	{return 4;}
  int		out_nodes()const	{untested();return 4;}
  int		matrix_nodes()const	{return 4;}
  int		net_nodes()const	{return 4;}
  CARD*		clone()const	{untested();return new DEV_TRANSLINE(*this);}
  void		parse_spice(CS&);
  void		print_spice(OMSTREAM&,int)const;
  //void	elabo1();		//COMPONENT
  //void	map_nodes();		//ELEMENT
  void		precalc();

  void		tr_iwant_matrix();
  void		dc_begin()		{untested();}
  void		tr_begin();
  //void	tr_restore();		//CARD/nothing
  void		dc_advance()		{tr_advance();}
  void		tr_advance();
  bool		tr_needs_eval()const;
  //void	tr_queue_eval()		//ELEMENT
  bool		do_tr();
  void		tr_load();
  DPAIR 	tr_review();
  void		tr_accept();
  void		tr_unload();
  double	tr_involts()const;
  //double	tr_input()const		//ELEMENT
  double	tr_involts_limited()const;
  //double	tr_input_limited()const //ELEMENT
  //double	tr_probe_num(CS&)const;	//ELEMENT wrong???

  void		ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  //void	ac_begin();		//CARD/nothing
  void		do_ac();
  void		ac_load();
  COMPLEX	ac_involts()const;
  //XPROBE	ac_probe_ext(CS&)const;	//ELEMENT wrong??

  bool		is_2port()const		{untested(); return true;}
private:
  void		setinitcond(CS&);
};
/*--------------------------------------------------------------------------*/
inline bool DEV_TRANSLINE::tr_needs_eval()const
{
  assert(!is_q_for_eval());
  return (_if0!=_if1 || _ir0!=_ir1);
}
/*--------------------------------------------------------------------------*/
inline double DEV_TRANSLINE::tr_involts()const
{
  return dn_diff(_n[IN1].v0(), _n[IN2].v0());
}
/*--------------------------------------------------------------------------*/
inline double DEV_TRANSLINE::tr_involts_limited()const
{
  unreachable();
  return volts_limited(_n[IN1],_n[IN2]);
}
/*--------------------------------------------------------------------------*/
inline COMPLEX DEV_TRANSLINE::ac_involts()const
{untested();
  return _n[IN1]->vac() - _n[IN2]->vac();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
