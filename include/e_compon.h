/*$Id: e_compon.h  $ -*- C++ -*-
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
 * base class for all components
 */
//testing=script 2007.07.13
#ifndef E_COMPON_H
#define E_COMPON_H
#include "u_sim_data.h"
#include "u_time_pair.h"
#include "u_parameter.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// this file
class COMMON_COMPONENT;
class COMPONENT;
/*--------------------------------------------------------------------------*/
static const int sysparams_count = 8;
/*--------------------------------------------------------------------------*/
// external
class MODEL_CARD;
class CS;
class ELEMENT;
class CARD_LIST;
/*--------------------------------------------------------------------------*/
inline bool conchk(double o, double n,
		   double a=OPT::abstol, double r=OPT::reltol)
{
  return (std::abs(n-o) <= (r * std::abs(n) + a));
}
/*--------------------------------------------------------------------------*/
struct Exception_Precalc :public Exception{
  Exception_Precalc(const std::string& Message) 
    :Exception(Message) {
  }
};
/*--------------------------------------------------------------------------*/
enum {CC_STATIC=27342}; // mid-sized arbitrary positive int
// pass this as an argument to a common constructor to mark it as static,
// so it won't be deleted
/*--------------------------------------------------------------------------*/
class INTERFACE COMMON_COMPONENT : public CKT_BASE {
protected: // probably obsolete
  PARAMETER<double>	_tnom_c;  // specification temperature
  PARAMETER<double>	_dtemp;   // rise over enclosing temperature
  PARAMETER<double>	_temp_c;  // actual temperature of device
  PARAMETER<double>	_value; // not counted in param_count
  std::string	_modelname;
private:
  mutable const MODEL_CARD* _model;
  int		_attach_count;
public:
  static void attach_common(COMMON_COMPONENT* c, COMMON_COMPONENT** to);
  static void detach_common(COMMON_COMPONENT** from);
  bool is_shared()const {return _attach_count > 1;}
private:
  COMMON_COMPONENT& operator=(const COMMON_COMPONENT&)
			      {unreachable(); return *this;}
  explicit COMMON_COMPONENT() : CKT_BASE() {unreachable();incomplete();}
protected:
  explicit COMMON_COMPONENT(const COMMON_COMPONENT& p);
  explicit COMMON_COMPONENT(int c);
public:
  virtual ~COMMON_COMPONENT();

  void attach_model(const COMPONENT*)const;
  COMMON_COMPONENT& attach(const MODEL_CARD* m) {_model = m; return *this;}
  void set_modelname(const std::string& n) {_modelname = n;}
  void parse_modelname(CS&);

  virtual COMMON_COMPONENT* clone()const = 0;

  virtual bool use_obsolete_callback_parse()const {return false;}
  virtual bool use_obsolete_callback_print()const {return false;}
  virtual void parse_common_obsolete_callback(CS&);
  virtual void print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;
  virtual bool has_parse_params_obsolete_callback()const {return false;}
  virtual bool is_trivial()const {return false;}

  virtual bool param_is_printable(int)const;
  virtual std::string param_name(int)const;
  virtual std::string param_name(int,int)const;
  virtual std::string param_value(int)const;
  virtual int  set_param_by_name(std::string, std::string);
  int Set_param_by_name(std::string, std::string); //BUG// see implementation
  virtual void set_param_by_index(int, std::string&, int);
  virtual int param_count()const {return 3;}
public:
  virtual void precalc_first(const CARD_LIST*)	{}
  virtual void expand(const COMPONENT*)		{}
  virtual COMMON_COMPONENT* deflate()		{return this;}
  virtual void precalc_last(const CARD_LIST*);

  virtual void	tr_eval(ELEMENT*)const;
  virtual void	ac_eval(ELEMENT*)const;
  virtual TIME_PAIR tr_review(COMPONENT*)const {return TIME_PAIR(NEVER,NEVER);}
  virtual void  tr_accept(COMPONENT*)const	{}
  virtual bool	has_tr_eval()const	{untested(); return false;}
  virtual bool	has_ac_eval()const	{untested(); return false;}

  virtual bool	parse_numlist(CS&);
  virtual bool	parse_params_obsolete_callback(CS&);
  virtual void  skip_type_tail(CS&)const {}

  virtual std::string name()const	= 0;
  virtual bool  operator==(const COMMON_COMPONENT&x)const;

  bool operator!=(const COMMON_COMPONENT& x)const {return !(*this == x);}
  std::string	      modelname()const	{return _modelname;}
  const MODEL_CARD*   model()const	{assert(_model); return _model;}
  bool		      has_model()const	{return _model;}
  void set_value(double v) {_value = v;}
  const PARAMETER<double>& value()const {return _value;}
private:
  bool parse_param_list(CS&);
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
 * //BUG// possible portability problem.  What is deletion order?
 */
/*--------------------------------------------------------------------------*/
class HS_PARAM;
/*--------------------------------------------------------------------------*/
class INTERFACE COMPONENT : public CARD {
private:
  COMMON_COMPONENT* _common{nullptr};
  HS_PARAM* _hsparam{nullptr}; // possibly indirect. later.
private:
  double _mfactor_fixed;	// composite, including subckt mfactor
  bool	 _converged;
  int	 _q_for_eval;
public:
  TIME_PAIR _time_by;
  //--------------------------------------------------------------------
protected: // create and destroy.
  explicit   COMPONENT(COMMON_COMPONENT* c=nullptr);
  explicit   COMPONENT(const COMPONENT& p);
	     ~COMPONENT();
  //--------------------------------------------------------------------
public:	// "elaborate"
  void	precalc_first() override;
  void	expand() override;
  void	precalc_last() override;
  virtual bool is_valid()const	{return true;}
  //--------------------------------------------------------------------
public:	// dc-tran
  void      tr_iwant_matrix() override;
  void      tr_queue_eval() override;
  TIME_PAIR tr_review() override;
  void      tr_accept() override;
  double    tr_probe_num(const std::string&)const override;
  //--------------------------------------------------------------------
public:	// ac
  void  ac_iwant_matrix() override;
  //--------------------------------------------------------------------
public:	// state, aux data
  bool	is_device()const override	{return true;}
  void	set_slave()override;
  void  map_nodes()override;
  virtual const std::string current_probe_name()const {untested(); return "";}
  static double volts_limited(const node_t& n1, const node_t& n2);
  bool	converged()const		{return _converged;}
  void	set_converged(bool s=true)	{_converged = s;}
  void	set_not_converged()		{_converged = false;}

  double mfactor()const {
    assert(_mfactor_fixed != NOT_VALID);
#ifndef NDEBUG
    if (const COMPONENT* o = dynamic_cast<const COMPONENT*>(owner())) {
      assert(_mfactor_fixed == o->mfactor() * my_mfactor());
    }else{
      assert(!owner());
      assert(_mfactor_fixed == my_mfactor());
    }
#endif
    return _mfactor_fixed;
  }
  //--------------------------------------------------------------------
  // list and queue management
  bool	is_q_for_eval()const	 {return (_q_for_eval >= _sim->iteration_tag());}
  void	mark_q_for_eval()	 {_q_for_eval = _sim->iteration_tag();}
  void	mark_always_q_for_eval() {_q_for_eval = INT_MAX;}
  void	q_eval();
  void	q_load()		 {_sim->_loadq.push_back(this);}
  void	q_accept()		 {_sim->_acceptq.push_back(this);}
  //--------------------------------------------------------------------
  // model
  const MODEL_CARD* find_model(const std::string& name)const;
  void attach_model()const	{assert(has_common()); _common->attach_model(this);}
  //--------------------------------------------------------------------
  // common
  COMMON_COMPONENT* mutable_common()	  {return _common;}
  const COMMON_COMPONENT* common()const	  {return _common;}
  bool	has_common()const		  {return _common;}
  void	attach_common(COMMON_COMPONENT*c) {COMMON_COMPONENT::attach_common(c,&_common);}
  void	detach_common()			  {COMMON_COMPONENT::detach_common(&_common);}
  void	deflate_common();
  //--------------------------------------------------------------------
public:	// type
  void  set_dev_type(const std::string& new_type) override;
  //--------------------------------------------------------------------
public:	// ports
  virtual std::string port_name(int)const = 0;
  virtual int  set_port_by_name(std::string& name, std::string& value);
  virtual void set_port_by_index(int index, std::string& value);
  bool port_exists(int i)const {return i < net_nodes();}
  const std::string port_value(int i)const;
  void	set_port_to_ground(int index);

  virtual std::string current_port_name(int)const {return "";}
  virtual const std::string current_port_value(int)const;
  virtual void set_current_port_by_index(int, const std::string&) {unreachable();}    
  bool current_port_exists(int i)const	{return i < num_current_ports();}

  virtual int	max_nodes()const	{unreachable(); return 0;}
  virtual int	min_nodes()const	{unreachable(); return 0;}
  virtual int	num_current_ports()const {return 0;}
  virtual int	tail_size()const	{return 0;}

  int	net_nodes()const override	{itested();return 0;} //override
  virtual int	ext_nodes()const	{return max_nodes();}
  virtual int	int_nodes()const	{return 0;}
  virtual int	matrix_nodes()const	{return 0;}

  virtual bool	has_inode()const	{return false;}
  virtual bool	has_iv_probe()const	{return false;}
  virtual bool	is_source()const	{return false;}
  virtual bool	f_is_value()const	{return false;}

  bool		node_is_grounded(int i)const;
  virtual bool	node_is_connected(int i)const;
  //--------------------------------------------------------------------
public: // parameters
  int  set_param_by_name(std::string, std::string) override;
  void set_param_by_index(int, std::string&, int) override;
  int  param_count()const override {
    if (has_common()) {
      return sysparams_count + common()->param_count();
    }else{
      return sysparams_count + CARD::param_count();
    }
  }
  bool param_is_printable(int)const override;
  std::string param_name(int)const override;
  std::string param_name(int,int)const override;
  std::string param_value(int)const override;

  virtual void set_parameters(const std::string& Label, CARD* Parent,
			      COMMON_COMPONENT* Common, double Value,
			      int state_count, double state[],
			      int node_count, const node_t nodes[]);
private: // implementation
  int set_hsparam(std::string const&, std::string const&);
  HS_PARAM& hsparam();
protected: // former _mfactor
  void set_mfactor(double);
  double my_mfactor()const;
  //--------------------------------------------------------------------
public:	// obsolete -- do not use in new code
  virtual bool print_type_in_spice()const = 0;
  bool use_obsolete_callback_parse()const override;
  bool use_obsolete_callback_print()const override;
  void print_args_obsolete_callback(OMSTREAM&, LANGUAGE*)const override;
  virtual void obsolete_set_value(double) {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
