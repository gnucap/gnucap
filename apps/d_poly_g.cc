/*$Id: d_poly_g.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * DEV_CPOLY_G
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
//testing=script 2006.07.17
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CPOLY_G : public ELEMENT {
protected:
  double*  _values;
  double*  _old_values;
  int	   _n_ports;
  double   _time;
  const double** _inputs;
protected:
  explicit DEV_CPOLY_G(const DEV_CPOLY_G& p);
public:
  explicit DEV_CPOLY_G();
  ~DEV_CPOLY_G();
protected: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  std::string value_name()const	{incomplete(); return "";}
  std::string dev_type()const	{unreachable(); return "cpoly_g";}
  int	   max_nodes()const	{return net_nodes();}
  int	   min_nodes()const	{return net_nodes();}
  int	   matrix_nodes()const	{return _n_ports*2;}
  int	   net_nodes()const	{return _n_ports*2;}
  CARD*	   clone()const		{return new DEV_CPOLY_G(*this);}
  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  bool	   do_tr();
  void	   tr_load();
  void	   tr_unload();
  double   tr_involts()const	{unreachable(); return NOT_VALID;}
  double   tr_involts_limited()const {unreachable(); return NOT_VALID;}
  double   tr_amps()const;
  void	   ac_iwant_matrix()	{ac_iwant_matrix_extended();}
  void	   ac_load();
  COMPLEX  ac_involts()const	{itested(); return NOT_VALID;}
  COMPLEX  ac_amps()const	{itested(); return NOT_VALID;}

  std::string port_name(int)const {untested();
    incomplete();
    unreachable();
    return "";
  }
public:
  void set_parameters(const std::string& Label, CARD* Parent,
		      COMMON_COMPONENT* Common, double Value,
		      int state_count, double state[],
		      int node_count, const node_t nodes[]);
  //		      const double* inputs[]=0);
protected:
  bool do_tr_con_chk_and_q();
};
/*--------------------------------------------------------------------------*/
#if 0
class DEV_FPOLY_G : public DEV_CPOLY_G {
private:
  explicit DEV_FPOLY_G(const DEV_FPOLY_G& p)
    :DEV_CPOLY_G(p) {incomplete(); unreachable();}
public:
  explicit DEV_FPOLY_G() :DEV_CPOLY_G() {untested();}
private: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  std::string dev_type()const	{unreachable(); return "fpoly_g";}
  CARD*	   clone()const		{unreachable(); return new DEV_FPOLY_G(*this);}
  bool	   do_tr();
};
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G::DEV_CPOLY_G(const DEV_CPOLY_G& p)
  :ELEMENT(p),
   _values(NULL),
   _old_values(NULL),
   _n_ports(p._n_ports),
   _time(NOT_VALID),
   _inputs(NULL)
{
  // not really a copy .. only valid to copy a default
  // too lazy to do it right, and that's all that is being used
  // to do it correctly requires a deep copy
  // just filling in defaults is better than a shallow copy, hence this:
  assert(!p._values);
  assert(!p._old_values);
  assert(p._n_ports == 0);
  assert(!p._inputs);
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G::DEV_CPOLY_G()
  :ELEMENT(),
   _values(NULL),
   _old_values(NULL),
   _n_ports(0),
   _time(NOT_VALID),
   _inputs(NULL)
{
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G::~DEV_CPOLY_G()
{
  delete [] _old_values;
  if (net_nodes() > NODES_PER_BRANCH) {
    delete [] _n;
  }else{
    // it is part of a base class
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_G::do_tr_con_chk_and_q()
{
  q_load();

  assert(_old_values);
  set_converged(conchk(_time, _sim->_time0));
  _time = _sim->_time0;
  for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_old_values[i], _values[i]));
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_G::do_tr()
{
  assert(_values);
  _m0 = CPOLY1(0., _values[0], _values[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
#if 0
bool DEV_FPOLY_G::do_tr()
{untested();
  assert(_values);
  double c0 = _values[0];
  if (_inputs) {untested();
    untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      c0 -= *(_inputs[i]) * _values[i];
      trace4("", i, *(_inputs[i]), _values[i], *(_inputs[i]) * _values[i]);
    }
  }else{untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      c0 -= volts_limited(_n[2*i-2],_n[2*i-1]) * _values[i];
      trace4("", i, volts_limited(_n[2*i-2],_n[2*i-1]), _values[i],
	     volts_limited(_n[2*i-2],_n[2*i-1]) * _values[i]);
    }
  }
  trace2("", _values[0], c0);
  _m0 = CPOLY1(0., c0, _values[1]);

  return do_tr_con_chk_and_q();
}
#endif
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::tr_load()
{
  tr_load_passive();
  _old_values[0] = _values[0];
  _old_values[1] = _values[1];
  for (int i=2; i<=_n_ports; ++i) {
    tr_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1], &(_values[i]), &(_old_values[i]));
  }
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::tr_unload()
{
  std::fill_n(_values, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CPOLY_G::tr_amps()const
{
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _values[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_G::ac_load()
{
  _acg = _values[1];
  ac_load_passive();
  for (int i=2; i<=_n_ports; ++i) {
    ac_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1], _values[i]);
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void DEV_CPOLY_G::set_parameters(const std::string& Label, CARD *Owner,
				 COMMON_COMPONENT *Common, double Value,
				 int n_states, double states[],
				 int n_nodes, const node_t nodes[])
  //				 const double* inputs[])
{
  bool first_time = (net_nodes() == 0);

  set_label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  if (first_time) {
    _n_ports = n_nodes/2; // sets num_nodes() = _n_ports*2
    assert(_n_ports == n_states-1);

    assert(!_old_values);
    _old_values = new double[n_states];

    if (net_nodes() > NODES_PER_BRANCH) {
      // allocate a bigger node list
      _n = new node_t[net_nodes()];
    }else{
      // use the default node list, already set
    }      
  }else{
    assert(_n_ports == n_states-1);
    assert(_old_values);
    assert(net_nodes() == n_nodes);
    // assert could fail if changing the number of nodes after a run
  }

  //_inputs = inputs;
  _inputs = 0;
  _values = states;
  std::fill_n(_values, n_states, 0.);
  std::fill_n(_old_values, n_states, 0.);
  notstd::copy_n(nodes, net_nodes(), _n);
  assert(net_nodes() == _n_ports * 2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CPOLY_G p4;
DISPATCHER<CARD>::INSTALL d4(&device_dispatcher, "cpoly_g", &p4);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
