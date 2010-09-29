/*$Id: bmm_table.h,v 25.92 2006/06/28 15:02:53 al Exp $ -*- C++ -*-
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
 * behavioral modeling
 */
//testing=script 2006.04.18
#include "e_model.h" 
#include "bm.h"
/*--------------------------------------------------------------------------*/
class SPLINE;
/*--------------------------------------------------------------------------*/
class EVAL_BM_TABLE : public EVAL_BM_ACTION_BASE {
protected:
  explicit EVAL_BM_TABLE(const EVAL_BM_TABLE& p);
public:
  explicit EVAL_BM_TABLE(int c=0);
  ~EVAL_BM_TABLE() {}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const {untested(); return new EVAL_BM_TABLE(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const	{untested();return modelname().c_str();}
  bool		ac_too()const		{untested();return false;}
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing
  //bool	parse_params(CS&);	//EVAL_BM_ACTION_BASE
};
/*--------------------------------------------------------------------------*/
class MODEL_TABLE : public MODEL_CARD {
public:
  PARAMETER<int>    _order;
  PARAMETER<double> _below;
  PARAMETER<double> _above;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  SPLINE* _spline;
private:
  static int    const _default_order;
  static double const _default_below;
  static double const _default_above;
public:
  explicit MODEL_TABLE();
  ~MODEL_TABLE();
private: // override virtual
  void  elabo1();
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_TABLE;}
  bool parse_front(CS&);
  bool parse_params(CS&);
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  void print_calculated(OMSTREAM&)const {}
  void tr_eval(COMPONENT*)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
