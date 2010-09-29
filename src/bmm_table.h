/*$Id: bmm_table.h,v 24.5 2003/04/27 01:05:05 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * behavioral modeling
 */
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
  COMMON_COMPONENT* clone()const {untested(); return new EVAL_BM_TABLE(*this);}
  const char*	name()const	{untested();return modelname().c_str();}
  bool ac_too()const		{untested();return false;}
  void parse(CS&);
  void print(OMSTREAM&)const;
  void expand(const COMPONENT*);
  void tr_eval(ELEMENT*)const;
};
/*--------------------------------------------------------------------------*/
class MODEL_TABLE : public MODEL_CARD {
public:
  int	 _order;
  double _below;
  double _above;
  std::vector<std::pair<double,double> > _table;
  SPLINE* _spline;
public:
  explicit MODEL_TABLE();
private: // override virtual
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_TABLE;}
  bool parse_front(CS&);
  bool parse_params(CS&);
  void parse_finish();
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  void print_calculated(OMSTREAM&)const {}
  void tr_eval(COMPONENT*)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
