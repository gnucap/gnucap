/*$Id: bm.h,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * behavioral modeling base
 */
//testing=script 2006.07.13
#ifndef E_BM_H
#define E_BM_H
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
class SPLINE;
class FPOLY1;
/*--------------------------------------------------------------------------*/
class EVAL_BM_BASE : public COMMON_COMPONENT {
protected:
  explicit	EVAL_BM_BASE(int c=0) 
    :COMMON_COMPONENT(c) {}
  explicit	EVAL_BM_BASE(const EVAL_BM_BASE& p)
    :COMMON_COMPONENT(p) {}
		~EVAL_BM_BASE() {}
protected: // override virtual
  bool operator==(const COMMON_COMPONENT&)const{/*incomplete();*/return false;}
  bool		has_tr_eval()const	{return true;}
  bool		has_ac_eval()const	{return true;}
  bool use_obsolete_callback_parse()const {return true;}
  bool use_obsolete_callback_print()const {return true;}
  bool has_parse_params_obsolete_callback()const {return true;}
};
/*--------------------------------------------------------------------------*/
class INTERFACE EVAL_BM_ACTION_BASE : public EVAL_BM_BASE {
protected:
  PARAMETER<double> _bandwidth;
  PARAMETER<double> _delay;
  PARAMETER<double> _phase;
  PARAMETER<double> _ooffset;
  PARAMETER<double> _ioffset;
  PARAMETER<double> _scale;
  PARAMETER<double> _tc1;
  PARAMETER<double> _tc2;
  PARAMETER<double> _ic;
protected:
  explicit	EVAL_BM_ACTION_BASE(int c=0);
  explicit	EVAL_BM_ACTION_BASE(const EVAL_BM_ACTION_BASE& p);
		~EVAL_BM_ACTION_BASE() {}
  double	temp_adjust()const;
  void		tr_final_adjust(FPOLY1* y, bool f_is_value)const;
  void		tr_finish_tdv(ELEMENT* d, double val)const;
  void		ac_final_adjust(COMPLEX* y)const;
  void		ac_final_adjust_with_temp(COMPLEX* y)const;
  double	uic(double x)const	{return (CKT_BASE::_sim->uic_now()) ? _ic : x;}
  double	ioffset(double x)const	{return uic(x) + _ioffset;}	
public: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  //COMPONENT_COMMON* clone()const;	//COMPONENT_COMMON=0
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		ac_eval(ELEMENT*)const;
  virtual bool	ac_too()const = 0;
protected: // override virtual
  bool  	parse_params_obsolete_callback(CS&);
public:
  bool		has_ext_args()const;
  static COMMON_COMPONENT* parse_func_type(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EVAL_BM_VALUE : public EVAL_BM_ACTION_BASE {
private:
  explicit	EVAL_BM_VALUE(const EVAL_BM_VALUE& p):EVAL_BM_ACTION_BASE(p) {}
public:
  explicit      EVAL_BM_VALUE(int c=0) :EVAL_BM_ACTION_BASE(c) {}
		~EVAL_BM_VALUE()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_VALUE(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  bool		is_trivial()const;

  void		precalc_first(const CARD_LIST*);
  void		tr_eval(ELEMENT*)const;
  std::string	name()const		{itested();return "VALUE";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool  	parse_params_obsolete_callback(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
