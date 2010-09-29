/*$Id: bmm_semi.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Spice3 compatible "semiconductor resistor and capacitor""
 */
//testing=script 2006.07.13
#include "e_model.h" 
#include "bm.h"
/*--------------------------------------------------------------------------*/
class EVAL_BM_SEMI_BASE : public EVAL_BM_ACTION_BASE {
protected:
  PARAMETER<double> _length;
  PARAMETER<double> _width;
  double _value;
private:
  static double const _default_length;
  static double const _default_width;
  static double const _default_value;
protected:
  explicit EVAL_BM_SEMI_BASE(const EVAL_BM_SEMI_BASE& p);
  explicit EVAL_BM_SEMI_BASE(int c=0);
  ~EVAL_BM_SEMI_BASE() {}
protected: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const = 0;
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
  bool  	parse_params(CS&);
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
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const 
			{untested(); return new EVAL_BM_SEMI_CAPACITOR(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  //void	print(OMSTREAM&)const;	//EVAL_BM_SEMI_BASE
  void		elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  //void	tr_eval(ELEMENT*)const; //EVAL_BM_SEMI_BASE
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  //const char*	name()const;		//EVAL_BM_SEMI_BASE
  //bool	ac_too()const		//EVAL_BM_SEMI_BASE/false
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing
  //bool  	parse_params(CS&);	//EVAL_BM_SEMI_BASE
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
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const
			{untested(); return new EVAL_BM_SEMI_RESISTOR(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  //void	print(OMSTREAM&)const;	//EVAL_BM_SEMI_BASE
  void		elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  //void	tr_eval(ELEMENT*)const; //EVAL_BM_SEMI_BASE
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  //const char*	name()const;		//EVAL_BM_SEMI_BASE
  //bool	ac_too()const		//EVAL_BM_SEMI_BASE/false
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing
  //bool  	parse_params(CS&);	//EVAL_BM_SEMI_BASE
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_BASE : public MODEL_CARD {
public:
  PARAMETER<double> _narrow;
  PARAMETER<double> _defw;
  PARAMETER<double> _tc1;
  PARAMETER<double> _tc2;
private:
  static double const _default_narrow;
  static double const _default_defw;
  static double const _default_tc1;
  static double const _default_tc2;
protected:
  explicit MODEL_SEMI_BASE();
protected: // override virtual
  void  elabo1();
  COMMON_COMPONENT* new_common()const {untested();return new EVAL_BM_SEMI_CAPACITOR;}
  bool parse_front(CS&) = 0;
  bool parse_params(CS&);
  void print_front(OMSTREAM&)const = 0;
  void print_params(OMSTREAM&)const;
  void print_calculated(OMSTREAM&)const {}
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_CAPACITOR : public MODEL_SEMI_BASE {
public:
  PARAMETER<double> _cj;
  PARAMETER<double> _cjsw;
private:
  static double const _default_cj;
  static double const _default_cjsw;
public:
  explicit MODEL_SEMI_CAPACITOR();
private: // override virtual
  void  elabo1();
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_CAPACITOR;}
  bool parse_front(CS&);
  bool parse_params(CS&);
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  //void print_calculated(OMSTREAM&)const {}
};
/*--------------------------------------------------------------------------*/
class MODEL_SEMI_RESISTOR : public MODEL_SEMI_BASE {
public:
  PARAMETER<double> _rsh;
private:
  static double const _default_rsh;
public:
  explicit MODEL_SEMI_RESISTOR();
private: // override virtual
  void  elabo1();
  COMMON_COMPONENT* new_common()const {return new EVAL_BM_SEMI_RESISTOR;}
  bool parse_front(CS&);
  bool parse_params(CS&);
  void print_front(OMSTREAM&)const;
  void print_params(OMSTREAM&)const;
  //void print_calculated(OMSTREAM&)const {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
