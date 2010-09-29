/*$Id: bm.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
protected: // override virtual
  bool operator==(const COMMON_COMPONENT&)const{/*incomplete();*/return false;}
  bool		has_tr_eval()const	{return true;}
  bool		has_ac_eval()const	{return true;}
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_ACTION_BASE : public EVAL_BM_BASE {
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
  bool	 _has_ext_args;
protected:
  explicit	EVAL_BM_ACTION_BASE(int c=0);
  explicit	EVAL_BM_ACTION_BASE(const EVAL_BM_ACTION_BASE& p);
		~EVAL_BM_ACTION_BASE() {}
  double	temp_adjust()const;
  void		tr_final_adjust(FPOLY1* y, bool f_is_value)const;
  void		tr_finish_tdv(ELEMENT* d, double val)const;
  void		ac_final_adjust(COMPLEX* y)const;
  void		ac_final_adjust_with_temp(COMPLEX* y)const;
  double	uic(double x)const	{return (SIM::uic_now()) ? _ic : x;}
  double	ioffset(double x)const	{return uic(x) + _ioffset;}	
public: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  //COMPONENT_COMMON* clone()const;	//COMPONENT_COMMON=0
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void		elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  //void	tr_eval(ELEMENT*)const; //COMPONENT_COMMON
  void		ac_eval(ELEMENT*)const;
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  //const char*	name()const		//COMPONENT_COMMON=0
  virtual bool	ac_too()const = 0;
protected: // override virtual
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing
  bool  	parse_params(CS&);
public:
  bool		has_ext_args()const;
  static COMMON_COMPONENT* parse_func_type(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_COND : public EVAL_BM_BASE {
private:
  COMMON_COMPONENT* _func[sCOUNT];
  bool _set[sCOUNT];
  explicit	EVAL_BM_COND(const EVAL_BM_COND& p);
public:
  explicit	EVAL_BM_COND(int c=0);
		~EVAL_BM_COND();
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COND(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&)const;
  void		elabo3(const COMPONENT*c);
  COMMON_COMPONENT* deflate();
  void		tr_eval(ELEMENT*d)const
		    {assert(_func[SIM::mode]); _func[SIM::mode]->tr_eval(d);}
  void		ac_eval(ELEMENT*d)const
		    {assert(_func[sAC]); _func[sAC]->ac_eval(d);}
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{unreachable(); return "????";}
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing/ignored
  //bool  	parse_params(CS&);	//COMPONENT_COMMON/nothing/ignored
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EVAL_BM_MODEL : public EVAL_BM_ACTION_BASE {
private:
  std::string	_arglist;
  COMMON_COMPONENT* _func;
  explicit	EVAL_BM_MODEL(const EVAL_BM_MODEL& p);
public:
  explicit      EVAL_BM_MODEL(int c=0);
		~EVAL_BM_MODEL()	{detach_common(&_func);}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_MODEL(*this);}
  void		parse(CS&);
  void		print(OMSTREAM&)const;
  void		elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*d)const {assert(_func); _func->tr_eval(d);}
  void		ac_eval(ELEMENT*d)const {assert(_func); _func->ac_eval(d);}
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{untested();return modelname().c_str();}
  bool		ac_too()const		{return true;}
  //bool	parse_numlist(CS&);	//COMPONENT_COMMON/nothing/ignored
  //bool  	parse_params(CS&);	//EVAL_BM_ACTION_BASE/ignored
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_COMPLEX : public EVAL_BM_ACTION_BASE {
private:
  COMPLEX _value;
  explicit	EVAL_BM_COMPLEX(const EVAL_BM_COMPLEX& p);
public:
  explicit      EVAL_BM_COMPLEX(int c=0);
		~EVAL_BM_COMPLEX()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COMPLEX(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  //void	elabo3(const COMPONENT*); //EVAL_BM_ACTION_BASE
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  void		ac_eval(ELEMENT*)const;
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "COMPLEX";}
  bool		ac_too()const		{untested();return true;}
  bool		parse_numlist(CS&);
  //bool  	parse_params(CS&);	//EVAL_BM_ACTION_BASE
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_EXP : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _iv;	// initial value
  PARAMETER<double> _pv;	// pulsed value
  PARAMETER<double> _td1;	// rise delay
  PARAMETER<double> _tau1;	// rise time constant
  PARAMETER<double> _td2;	// fall delay
  PARAMETER<double> _tau2;	// fall time constant
  PARAMETER<double> _period;	// repeat period
  PARAMETER<double> _end;	// marks the end of the list
  explicit	EVAL_BM_EXP(const EVAL_BM_EXP& p);
public:
  explicit      EVAL_BM_EXP(int c=0);
		~EVAL_BM_EXP()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_EXP(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "EXP";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_FIT : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<int>    _order;
  PARAMETER<double> _below;
  PARAMETER<double> _above;
  PARAMETER<double> _delta;
  PARAMETER<int>    _smooth;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  SPLINE* _spline;
  explicit	EVAL_BM_FIT(const EVAL_BM_FIT& p);
public:
  explicit      EVAL_BM_FIT(int c=0);
		~EVAL_BM_FIT();
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_FIT(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "FIT";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_GENERATOR : public EVAL_BM_ACTION_BASE {
private:
  explicit	EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p);
public:
  explicit      EVAL_BM_GENERATOR(int c=0);
		~EVAL_BM_GENERATOR()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_GENERATOR(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  //void	elabo3(const COMPONENT*); //EVAL_BM_ACTION_BASE
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  void		ac_eval(ELEMENT*)const;
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "GENERATOR";}
  bool		ac_too()const		{return true;}
  bool		parse_numlist(CS&);
  //bool	parse_params(CS&);	//EVAL_BM_ACTION_BASE
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_POLY : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _min;
  PARAMETER<double> _max;
  PARAMETER<bool>   _abs;
  std::vector<PARAMETER<double> > _c;
  explicit	EVAL_BM_POLY(const EVAL_BM_POLY& p);
public:
  explicit      EVAL_BM_POLY(int c=0);
		~EVAL_BM_POLY()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_POLY(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "POLY";}
  bool		ac_too()const		{untested();return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_POSY : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _min;
  PARAMETER<double> _max;
  PARAMETER<bool>   _abs;
  PARAMETER<bool>   _odd;
  PARAMETER<bool>   _even;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _table;
  explicit	EVAL_BM_POSY(const EVAL_BM_POSY& p);
public:
  explicit      EVAL_BM_POSY(int c=0);
		~EVAL_BM_POSY()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_POSY(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "POSY";}
  bool		ac_too()const		{untested();return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_PULSE : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _iv;
  PARAMETER<double> _pv;
  PARAMETER<double> _delay;
  PARAMETER<double> _rise;
  PARAMETER<double> _fall;
  PARAMETER<double> _width;
  PARAMETER<double> _period;
  PARAMETER<double> _end;
  explicit	EVAL_BM_PULSE(const EVAL_BM_PULSE& p);
public:
  explicit      EVAL_BM_PULSE(int c=0);
		~EVAL_BM_PULSE()	{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_PULSE(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "PULSE";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_PWL : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _delta;
  PARAMETER<int>    _smooth;
  std::vector<std::pair<PARAMETER<double>,PARAMETER<double> > > _raw_table;
  std::vector<DPAIR> _num_table;
  explicit	EVAL_BM_PWL(const EVAL_BM_PWL& p);
public:
  explicit      EVAL_BM_PWL(int c=0);
		~EVAL_BM_PWL()		{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_PWL(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "PWL";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SFFM : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _offset;
  PARAMETER<double> _amplitude;
  PARAMETER<double> _carrier;
  PARAMETER<double> _modindex;
  PARAMETER<double> _signal;
  PARAMETER<double> _end;
  explicit	EVAL_BM_SFFM(const EVAL_BM_SFFM& p);
public:
  explicit      EVAL_BM_SFFM(int c=0);
		~EVAL_BM_SFFM()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_SFFM(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "SFFM";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SIN : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _offset;
  PARAMETER<double> _amplitude;
  PARAMETER<double> _frequency;
  PARAMETER<double> _delay;
  PARAMETER<double> _damping;
  PARAMETER<double> _end;
  explicit	EVAL_BM_SIN(const EVAL_BM_SIN& p);
public:
  explicit      EVAL_BM_SIN(int c=0);
		~EVAL_BM_SIN()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_SIN(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "SIN";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_TANH : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _gain;
  PARAMETER<double> _limit;
  explicit	EVAL_BM_TANH(const EVAL_BM_TANH& p);
public:
  explicit      EVAL_BM_TANH(int c=0);
		~EVAL_BM_TANH()		{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_TANH(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{return "TANH";}
  bool		ac_too()const		{untested();return false;}
  bool		parse_numlist(CS&);
  bool		parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_VALUE : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _value;
  explicit	EVAL_BM_VALUE(const EVAL_BM_VALUE& p);
public:
  explicit      EVAL_BM_VALUE(int c=0);
		~EVAL_BM_VALUE()	{}
  const PARAMETER<double>& value()const {return _value;}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_VALUE(*this);}
  //void	parse(CS&);		//COMPONENT_COMMON
  void		print(OMSTREAM&)const;
  void  	elabo3(const COMPONENT*);
  //COMMON_COMPONENT* deflate();	//COMPONENT_COMMON/nothing
  void		tr_eval(ELEMENT*)const;
  //void	ac_eval(ELEMENT*)const; //EVAL_BM_ACTION_BASE
  //bool	has_tr_eval()const;	//EVAL_BM_BASE/true
  //bool	has_ac_eval()const;	//EVAL_BM_BASE/true
  const char*	name()const		{untested();return "VALUE";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool  	parse_params(CS&);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
