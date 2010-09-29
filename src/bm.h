/*$Id: bm.h,v 24.21 2004/01/21 15:58:10 al Exp $ -*- C++ -*-
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
 * behavioral modeling base
 */
#ifndef E_BM_H
#define E_BM_H
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
class SPLINE;
/*--------------------------------------------------------------------------*/
class EVAL_BM_BASE : public COMMON_COMPONENT {
protected:
  explicit	EVAL_BM_BASE(int c=0) :COMMON_COMPONENT(c) {}
  explicit	EVAL_BM_BASE(const EVAL_BM_BASE& p)
    :COMMON_COMPONENT(p) {}
private: // override virtual
  bool operator==(const COMMON_COMPONENT&)const{/*incomplete();*/return false;}
  bool		has_tr_eval()const	{return true;}
  bool		has_ac_eval()const	{return true;}
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_ACTION_BASE : public EVAL_BM_BASE {
protected:
  double _bandwidth;
  double _delay;
  double _phase;
  double _ooffset;
  double _ioffset;
  double _scale;
  double _tc1;
  double _tc2;
  double _ic;
  bool   _needs_ac_eval;
  bool	 _has_ext_args;
protected:
  explicit	EVAL_BM_ACTION_BASE(int c=0);
  explicit	EVAL_BM_ACTION_BASE(const EVAL_BM_ACTION_BASE& p);
		~EVAL_BM_ACTION_BASE() {}
  void		print_base(OMSTREAM&)const;
  double	temp_adjust()const;
  void		tr_final_adjust(FPOLY1* y, bool f_is_value)const;
  void		tr_finish_tdv(ELEMENT* d, double val)const;
  void		ac_final_adjust(COMPLEX* y)const;
  void		ac_final_adjust_with_temp(COMPLEX* y)const;
  double	uic(double x)const	{return (SIM::uic_now()) ? _ic : x;}
  double	ioffset(double x)const	{return uic(x) + _ioffset;}	
public: // override virtual
  void		ac_eval(ELEMENT* d)const;
  virtual bool	ac_too()const = 0;
  void		parse(CS&);
private: // override virtual
  bool		has_ac_eval()const	{return true;}
protected: // new virtual
  virtual void	parse_front()		{}
  virtual void	parse_numlist(CS&)	{unreachable();}
  virtual bool	parse_params(CS&);
  virtual void	parse_finish();
public:
  bool		has_ext_args()const	{return _has_ext_args;}
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
  COMMON_COMPONENT* clone()const {untested(); return new EVAL_BM_COND(*this);}
  const char*	name()const		{unreachable(); return "????";}
  void		parse(CS&);
  void		print(OMSTREAM&)const;
  void		expand(const COMPONENT*c)
  {
    for (int i = 1; i < sCOUNT; ++i) {
      assert(_func[i]);
      _func[i]->expand(c);
    }
  }
  void		tr_eval(ELEMENT*d)const
		    {assert(_func[SIM::mode]); _func[SIM::mode]->tr_eval(d);}
  void		ac_eval(ELEMENT*d)const
		    {assert(_func[sAC]); _func[sAC]->ac_eval(d);}
  COMMON_COMPONENT* deflate();
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
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_MODEL(*this);}
  const char*	name()const		{return modelname().c_str();}
  bool		ac_too()const		{return true;}
  void		parse(CS&);
  void		print(OMSTREAM&)const;
  void		expand(const COMPONENT*);
  void		tr_eval(ELEMENT*d)const {assert(_func); _func->tr_eval(d);}
  void		ac_eval(ELEMENT*d)const {assert(_func); _func->ac_eval(d);}
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
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_COMPLEX(*this);}
  const char*	name()const		{return "COMPLEX";}
  bool		ac_too()const		{return true;}
  void		parse_numlist(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*)const;
  void		ac_eval(ELEMENT*)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_EXP : public EVAL_BM_ACTION_BASE {
private:
  double _iv;	 // initial value
  double _pv;	 // pulsed value
  double _td1;   // rise delay
  double _tau1;  // rise time constant
  double _td2;   // fall delay
  double _tau2;  // fall time constant
  double _period;// repeat period
  double _end;	 // marks the end of the list
  explicit	EVAL_BM_EXP(const EVAL_BM_EXP& p);
public:
  explicit      EVAL_BM_EXP(int c=0);
		~EVAL_BM_EXP()		{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_EXP(*this);}
  const char*	name()const		{return "EXP";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		parse_finish();
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_FIT : public EVAL_BM_ACTION_BASE {
private:
  int	 _order;
  double _below;
  double _above;
  double _delta;
  int    _smooth;
  std::vector<std::pair<double,double> > _table;
  SPLINE* _spline;
  explicit	EVAL_BM_FIT(const EVAL_BM_FIT& p);
public:
  explicit      EVAL_BM_FIT(int c=0);
		~EVAL_BM_FIT();
private: // override virtual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_FIT(*this);}
  const char*	name()const		{return "FIT";}
  bool		ac_too()const		{return false;}
  void		parse_front();
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		parse_finish();
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_GENERATOR : public EVAL_BM_ACTION_BASE {
private:
  explicit	EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p);
public:
  explicit      EVAL_BM_GENERATOR(int c=0);
		~EVAL_BM_GENERATOR()	{}
private: // override virtual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_GENERATOR(*this);}
  const char*	name()const		{return "GENERATOR";}
  bool		ac_too()const		{return true;}
  void		parse_numlist(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
  void		ac_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_POLY : public EVAL_BM_ACTION_BASE {
private:
  double _max;
  double _min;
  bool   _abs;
  std::vector<double> _c;
  explicit	EVAL_BM_POLY(const EVAL_BM_POLY& p);
public:
  explicit      EVAL_BM_POLY(int c=0);
		~EVAL_BM_POLY()		{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_POLY(*this);}
  const char*	name()const		{return "POLY";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_POSY : public EVAL_BM_ACTION_BASE {
private:
  double _max;
  double _min;
  bool   _abs;
  bool	 _odd;
  bool	 _even;
  std::vector<std::pair<double,double> > _table;
  explicit	EVAL_BM_POSY(const EVAL_BM_POSY& p);
public:
  explicit      EVAL_BM_POSY(int c=0);
		~EVAL_BM_POSY()		{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_POSY(*this);}
  const char*	name()const		{return "POSY";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_PULSE : public EVAL_BM_ACTION_BASE {
private:
  double _iv;
  double _pv;
  double _delay;
  double _rise;
  double _fall;
  double _width;
  double _period;
  double _end;
  explicit	EVAL_BM_PULSE(const EVAL_BM_PULSE& p);
public:
  explicit      EVAL_BM_PULSE(int c=0);
		~EVAL_BM_PULSE()	{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_PULSE(*this);}
  const char*	name()const		{return "PULSE";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		parse_finish();
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_PWL : public EVAL_BM_ACTION_BASE {
private:
  double _delta;
  int    _smooth;
  std::vector<std::pair<double,double> > _table;
  SPLINE* _spline;  
  explicit	EVAL_BM_PWL(const EVAL_BM_PWL& p);
public:
  explicit      EVAL_BM_PWL(int c=0);
		~EVAL_BM_PWL()		{}
private: // override virtual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_PWL(*this);}
  const char*	name()const		{return "PWL";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SFFM : public EVAL_BM_ACTION_BASE {
private:
  double _offset;
  double _amplitude;
  double _carrier;
  double _modindex;
  double _signal;
  double _end;
  explicit	EVAL_BM_SFFM(const EVAL_BM_SFFM& p);
public:
  explicit      EVAL_BM_SFFM(int c=0);
		~EVAL_BM_SFFM()		{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_SFFM(*this);}
  const char*	name()const		{return "SFFM";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_SIN : public EVAL_BM_ACTION_BASE {
private:
  double _offset;
  double _amplitude;
  double _frequency;
  double _delay;
  double _damping;
  double _end;
  explicit	EVAL_BM_SIN(const EVAL_BM_SIN& p);
public:
  explicit      EVAL_BM_SIN(int c=0);
		~EVAL_BM_SIN()		{}
private: // override vitrual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_SIN(*this);}
  const char*	name()const		{return "SIN";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_TANH : public EVAL_BM_ACTION_BASE {
private:
  double _gain;
  double _limit;
  explicit	EVAL_BM_TANH(const EVAL_BM_TANH& p);
public:
  explicit      EVAL_BM_TANH(int c=0);
		~EVAL_BM_TANH()		{}
private: // override virtual
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_TANH(*this);}
  const char*	name()const		{return "TANH";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  bool		parse_params(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
class EVAL_BM_VALUE : public EVAL_BM_ACTION_BASE {
private:
  double _value;
  explicit	EVAL_BM_VALUE(const EVAL_BM_VALUE& p);
public:
  explicit      EVAL_BM_VALUE(int c=0);
		~EVAL_BM_VALUE()	{}
  double	value()const		{return _value;}
private: // override virtual
  COMMON_COMPONENT* clone()const {untested(); return new EVAL_BM_VALUE(*this);}
  const char*	name()const		{return "VALUE";}
  bool		ac_too()const		{return false;}
  void		parse_numlist(CS&);
  void		print(OMSTREAM&)const;
  void		tr_eval(ELEMENT*d)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
