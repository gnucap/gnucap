/*$Id: e_compon.h,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * base class for all components
 */
//testing=script 2006.07.12
#ifndef E_COMPON_H
#define E_COMPON_H
#include "s__.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// this file
class COMMON_COMPONENT;
class COMPONENT;
/*--------------------------------------------------------------------------*/
// external
class MODEL_CARD;
class CS;
class ELEMENT;
/*--------------------------------------------------------------------------*/
inline bool conchk(double o, double n,
		   double a=OPT::abstol, double r=OPT::reltol)
{
  return (std::abs(n-o) <= (r * std::abs(n)+a));
}
/*--------------------------------------------------------------------------*/
enum {CC_STATIC=27342}; // mid-sized arbitrary positive int
// pass this as an argument to a common constructor to mark it as static,
// so it won't be deleted
/*--------------------------------------------------------------------------*/
class COMMON_COMPONENT {
public:
  static void attach_common(COMMON_COMPONENT* c, COMMON_COMPONENT** to);
  static void detach_common(COMMON_COMPONENT** from);
private:
  COMMON_COMPONENT& operator=(const COMMON_COMPONENT&)
					{unreachable(); return *this;}
  explicit COMMON_COMPONENT() {unreachable();incomplete();}
protected:
  explicit COMMON_COMPONENT(const COMMON_COMPONENT& p);
  explicit COMMON_COMPONENT(int c);
public:
  virtual ~COMMON_COMPONENT();

  const MODEL_CARD* attach_model(const COMPONENT*, int=bERROR)const;
  COMMON_COMPONENT& attach(const MODEL_CARD* m) {_model = m; return *this;}
  void set_modelname(const std::string& n) {_modelname = n;}
  void parse_modelname(CS&);

  virtual COMMON_COMPONENT* clone()const = 0;
  virtual void	parse(CS&);
  virtual void	print(OMSTREAM&)const;
  virtual void	elabo3(const COMPONENT*);
  virtual COMMON_COMPONENT* deflate()	{return this;}

  virtual void	tr_eval(ELEMENT*)const;
  virtual void	ac_eval(ELEMENT*)const;
  virtual bool	has_tr_eval()const	{untested(); return false;}
  virtual bool	has_ac_eval()const	{untested(); return false;}

  virtual const char* name()const	= 0;
  virtual bool  operator==(const COMMON_COMPONENT&x)const;

  bool operator!=(const COMMON_COMPONENT& x)const {return !(*this == x);}
  const std::string&  modelname()const	{return _modelname;}
  const MODEL_CARD*   model()const	{assert(_model); return _model;}
	  bool	      has_model()const	{return _model;}
protected:
  virtual bool	parse_numlist(CS&)	{return false;}
  virtual bool	parse_params(CS&);
private:
  bool parse_param_list(CS&);
protected:
  PARAMETER<double>	_tnom_c; // specification temperature
  PARAMETER<double>	_dtemp;  // rise over enclosing temperature
  PARAMETER<double>	_temp_c; // actual temperature of device
  PARAMETER<double>	_m;	 // number of devices in parallel
private:
  std::string	_modelname;
  mutable const MODEL_CARD* _model;
  int		_attach_count;
};
/*--------------------------------------------------------------------------*/
/* note on _attach_count ...
 * The int argument is the initial _attach_count (default = 0)
 * Set it to CC_STATIC for static default versions that will never be deleted.
 * Set it to 0 (default) for auto versions, so they can be deleted.
 * A common will not be deleted on a detach if its _attach_count != 0
 * A failed assertion from the common destructor probably means
 * the common is being deleted before a device it is attached to is,
 * without being first detached.
 * This is why ~COMPONENT destroys the subckt explicitly.
 *
 * Static commons (CC_STATIC) must be in file scope, not in function scope,
 * because local statics are deleted first, before any globals.
 * BUG:: possible portability problem.  What is deletion order?
 */
/*--------------------------------------------------------------------------*/
class COMPONENT : public CARD {
protected:
  explicit   COMPONENT();
  explicit   COMPONENT(const COMPONENT& p);
	     ~COMPONENT();
public:
  virtual const char* dev_type()const = 0;
  virtual int	stop_nodes()const	{return max_nodes();}
  virtual int	max_nodes()const = 0;
  virtual int	min_nodes()const = 0;
  virtual int	int_nodes()const	{return 0;}
protected: // override virtual
  void  elabo1();
  bool	is_device()const		{return true;}
  void  map_nodes();
  void  tr_iwant_matrix();
  void  ac_iwant_matrix();
protected: // not virtual
  int	parse_nodes(CS&, int max_nodes, int min_nodes, int leave_tail,
		    int start, bool all_new = false);
  void	printnodes(OMSTREAM&)const;

  const MODEL_CARD* attach_model()const	
		{return (has_common() ? _common->attach_model(this) : 0);}
  void	attach_common(COMMON_COMPONENT*c) 
			{COMMON_COMPONENT::attach_common(c,&_common);}
  void	detach_common()	{COMMON_COMPONENT::detach_common(&_common);}
  void	deflate_common();

  bool	converged()const		{return _converged;}
  void	set_converged(bool s=true)	{_converged = s;}
  void	set_not_converged()		{_converged = false;}
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // list and queue management
  bool	is_q_for_eval()const	 {return (_q_for_eval>=iteration_tag());}
  void	mark_q_for_eval()	 {_q_for_eval = iteration_tag();}
  void	mark_always_q_for_eval() {_q_for_eval = INT_MAX;}
  // mark_as...  doesn't queue it, just marks it as queued.  It might lie.
  void	q_eval();
  void	q_load()	{SIM::loadq.push_back(this);}
  void	q_accept()	{SIM::acceptq.push_back(this);}
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // modifiers
public:
  virtual void set_parameters(const std::string& Label, CARD* Parent,
			      COMMON_COMPONENT* Common, double Value,
			      int state_count, double state[],
			      int node_count, const node_t nodes[]);
  void	set_slave();
  void	set_value(const PARAMETER<double>& v) {CARD::set_value(v);}
  void	set_value(const PARAMETER<double>& v, COMMON_COMPONENT* c);

  void	set_value(double v) {CARD::set_value(v);}
  void	set_value(double v, COMMON_COMPONENT* c);
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // querys
  bool	has_common()const {return _common;}
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // data access
  COMMON_COMPONENT*	  mutable_common()	{return _common;}
  const COMMON_COMPONENT* common()const		{return _common;}
private:
  COMMON_COMPONENT* _common;
private:
  bool _converged;	// convergence status
  int  _q_for_eval;
};
/*--------------------------------------------------------------------------*/
/* q_eval: queue this device for evaluation on the next pass,
 * with a check against doing it twice.
 */
inline void COMPONENT::q_eval()
{
  if(!is_q_for_eval()) {
    mark_q_for_eval();
    SIM::evalq_uc->push_back(this);
  }else{
    untested();
    //unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
