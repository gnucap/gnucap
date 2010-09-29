/*$Id: bmm_semi.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Spice3 compatible "semiconductor resistor and capacitor""
 */
#include "e_model.h" 
#include "bm.h"
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_BASE : public EVAL_BM_ACTION_BASE {
protected:
  double _length;
  double _width;
  double _temp;
  double _value;
  explicit EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p);
protected:
  explicit EVAL_BM_SEMI_BASE(int c=0);
  ~EVAL_BM_SEMI_BASE() {}
private: // override virtual
  COMMON_COMPONENT* clone()const = 0;
  const char*	name()const	{untested();return modelname().c_str();}
  bool ac_too()const		{untested();return false;}
  void parse(CS&);
  void print(OMSTREAM&)const;
  void expand(const COMPONENT*) = 0;
  void tr_eval(ELEMENT*)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_CAPACITOR : public EVAL_BM_SEMI_BASE {
private:
  explicit EVAL_BM_SEMI_CAPACITOR(const EVAL_BM_SEMI_CAPACITOR& p)
    :EVAL_BM_SEMI_BASE(p) {untested();}
public:
  explicit EVAL_BM_SEMI_CAPACITOR(int c=0)
    :EVAL_BM_SEMI_BASE(c) {}
  ~EVAL_BM_SEMI_CAPACITOR() {}
private: // override virtual
  COMMON_COMPONENT* clone()const 
			{untested(); return new EVAL_BM_SEMI_CAPACITOR(*this);}
  void expand(const COMPONENT*);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_RESISTOR : public EVAL_BM_SEMI_BASE {
private:
  explicit EVAL_BM_SEMI_RESISTOR(const EVAL_BM_SEMI_RESISTOR& p)
    :EVAL_BM_SEMI_BASE(p) {untested();}
public:
  explicit EVAL_BM_SEMI_RESISTOR(int c=0)
    :EVAL_BM_SEMI_BASE(c) {}
  ~EVAL_BM_SEMI_RESISTOR() {}
private: // override virtual
  COMMON_COMPONENT* clone()const
			{untested(); return new EVAL_BM_SEMI_RESISTOR(*this);}
  void expand(const COMPONENT*);
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_CAPACITOR : public MODEL_CARD {
public:
  double _cj;
  double _cjsw;
  double _narrow;
  double _defw;
  double _tc1;
  double _tc2;
public:
  explicit MODEL_SEMI_CAPACITOR();
private: // override virtual
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_CAPACITOR;}
  bool parse_front(CS&);
  void parse_params(CS&);
  void parse_finish() {}
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  void print_calculated(OMSTREAM&)const {}
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_RESISTOR : public MODEL_CARD {
public:
  double _rsh;
  double _narrow;
  double _defw;
  double _tc1;
  double _tc2;
public:
  explicit MODEL_SEMI_RESISTOR();
private: // override virtual
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_RESISTOR;}
  bool parse_front(CS&);
  void parse_params(CS&);
  void parse_finish() {}
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  void print_calculated(OMSTREAM&)const {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
