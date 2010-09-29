/*$Id: e_compon.h,v 24.10 2003/10/18 05:14:15 al Exp $ -*- C++ -*-
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
 * branch structure type definitions
 * device types (enumeration type?)
 */
#ifndef E_COMPON_H
#define E_COMPON_H
#include "m_cpoly.h"
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
friend void attach_common(COMMON_COMPONENT* c, COMMON_COMPONENT** to);
friend void detach_common(COMMON_COMPONENT** from);
private:
  COMMON_COMPONENT& operator=(const COMMON_COMPONENT&)
					{unreachable(); return *this;}
protected:
  explicit COMMON_COMPONENT(const COMMON_COMPONENT& p)
    :_tnom(p._tnom),_modelname(p._modelname),_model(p._model),_attach_count(0)
    {}
  explicit COMMON_COMPONENT(int c)
    :_tnom(NOT_INPUT),_modelname(), _model(0), _attach_count(c) {}
public:
  virtual ~COMMON_COMPONENT();

  virtual bool operator==(const COMMON_COMPONENT&)const = 0;
  bool operator!=(const COMMON_COMPONENT& x)const {return !(*this == x);}
  const MODEL_CARD* attach_model(const COMPONENT*)const;
  COMMON_COMPONENT& attach(const MODEL_CARD* m) {_model = m; return *this;}
  void set_modelname(const std::string& n) {_modelname = n;}
  void parse_modelname(CS&);

  virtual COMMON_COMPONENT* clone()const = 0;
  virtual void	parse(CS&)		{unreachable();}
  virtual void	print(OMSTREAM&)const	{unreachable();}
  virtual void	expand(const COMPONENT*){assert(_modelname == "");}

  virtual void	tr_eval(ELEMENT*x)const;
  virtual void	ac_eval(ELEMENT*x)const;
  virtual bool	has_tr_eval()const	{untested(); return false;}
  virtual bool	has_ac_eval()const	{untested(); return false;}
  virtual COMMON_COMPONENT* deflate()	{return this;}

  virtual const char* name()const	= 0;
  const std::string&  modelname()const	{return _modelname;}
  const MODEL_CARD*   model()const	{assert(_model); return _model;}
	  bool	      has_model()const	{return _model;}

  bool is_equal(const COMMON_COMPONENT&)const;
protected:
  double	_tnom;
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
	     ~COMPONENT()		{subckt().destroy(); detach_common();}
public:
  virtual const char* dev_type()const = 0;
  virtual int	max_nodes()const = 0;
  virtual int	min_nodes()const = 0;
  virtual int	int_nodes()const	{return 0;}
protected: // override virtual
  void  expand();
  bool	is_device()const		{return true;}
  void  map_nodes();
  void  tr_alloc_matrix();
  void  ac_alloc_matrix();
protected: // not virtual
  int	parse_nodes(CS&,int,int,int=0);
  void	printnodes(OMSTREAM&)const;

  const MODEL_CARD* attach_model()const	
		{return (has_common() ? _common->attach_model(this) : 0);}
  void	attach_common(COMMON_COMPONENT*c) {::attach_common(c,&_common);}
  void	detach_common()			{::detach_common(&_common);}
  void	deflate_common();

  bool	converged()const		{return _converged;}
  void	set_converged(bool s=true)	{_converged = s;}
  void	set_not_converged()		{_converged = false;}

  void	expandsubckt(const std::string& modelname);
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // list and queue management
  bool	is_q_for_eval() {return (_q_for_eval>=STATUS::iter[iTOTAL]);}
  void	mark_q_for_eval()	 {_q_for_eval = STATUS::iter[iTOTAL];}
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
  void	set_slave()	{mark_always_q_for_eval(); subckt().set_slave();}
  void	set_value(double v) {CARD::set_value(v);}
  void	set_value(double v, COMMON_COMPONENT* c);
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // querys
  bool	is_slave()const	  {untested(); return (_q_for_eval==INT_MAX);}
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
  {if(!is_q_for_eval()) {
    mark_q_for_eval();
    SIM::evalq_uc->push_back(this);
  }else{
    unreachable();
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
