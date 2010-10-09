/*$Id: d_poly_cap.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
 * DEV_CPOLY_CAP
 * number of nodes = 2*n_ports
 * number of val, ov = n_ports+1
 * val[0] is the constant part, val[1] is self admittance,
 *   val[2+] are transadmittances, up to n_ports
 * node[0] and node[1] are the output.
 * node[2] up are inputs.
 * node[2*i] and node[2*i+1] correspond to val[i+1]
 */
//testing=script 2006.07.17
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_CPOLY_CAP : public STORAGE {
protected:
  double*  _vy0; // vector form of _y0 _values; charge, capacitance
  double*  _vy1; // vector form of _y1 _old_values;
  double*  _vi0; // vector form of _i0; current, difference conductance
  double*  _vi1; // vector form of _i1
  int	   _n_ports;
  double   _load_time;
  const double** _inputs;
protected:
  explicit DEV_CPOLY_CAP(const DEV_CPOLY_CAP& p);
public:
  explicit DEV_CPOLY_CAP();
  ~DEV_CPOLY_CAP();
protected: // override virtual
  char	   id_letter()const	{unreachable(); return '\0';}
  std::string value_name()const	{incomplete(); return "";}
  std::string dev_type()const	{unreachable(); return "cpoly_cap";}
  int	   max_nodes()const	{return net_nodes();}
  int	   min_nodes()const	{return net_nodes();}
  int	   matrix_nodes()const	{return _n_ports*2;}
  int	   net_nodes()const	{return _n_ports*2;}
  CARD*	   clone()const	        {unreachable();return new DEV_CPOLY_CAP(*this);}
  void	   tr_iwant_matrix()	{tr_iwant_matrix_extended();}
  bool	   tr_needs_eval()const	{/*assert(!is_q_for_eval());*/ return true;}
  bool	   do_tr();
  void	   tr_load();
  TIME_PAIR tr_review()		{return _time_by.reset();}//BUG//review(_i0.f0, _it1.f0);}
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
class DEV_FPOLY_CAP : public DEV_CPOLY_CAP {
private:
  explicit DEV_FPOLY_CAP(const DEV_FPOLY_CAP& p)
    :DEV_CPOLY_CAP(p) {}
public:
  explicit DEV_FPOLY_CAP() :DEV_CPOLY_CAP() {}
private: // override virtual
  char	   id_letter()const  {unreachable(); return '\0';}
  std::string dev_type()const{unreachable(); return "fpoly_cap";}
  CARD*	   clone()const	     {return new DEV_FPOLY_CAP(*this);}
  bool	   do_tr();
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_CPOLY_CAP::DEV_CPOLY_CAP(const DEV_CPOLY_CAP& p)
  :STORAGE(p),
   _vy0(NULL),
   _vy1(NULL),
   _vi0(NULL),
   _vi1(NULL),
   _n_ports(p._n_ports),
   _load_time(NOT_VALID),
   _inputs(NULL)
{
  // not really a copy .. only valid to copy a default
  // too lazy to do it right, and that's all that is being used
  // to do it correctly requires a deep copy
  // just filling in defaults is better than a shallow copy, hence this:
  assert(!p._vy0);
  assert(!p._vy1);
  assert(!p._vi0);
  assert(!p._vi1);
  assert(p._n_ports == 0);
  assert(!p._inputs);
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_CAP::DEV_CPOLY_CAP()
  :STORAGE(),
   _vy0(NULL),
   _vy1(NULL),
   _vi0(NULL),
   _vi1(NULL),
   _n_ports(0),
   _load_time(NOT_VALID),
   _inputs(NULL)
{
}
/*--------------------------------------------------------------------------*/
DEV_CPOLY_CAP::~DEV_CPOLY_CAP() 
{
  delete [] _vy1;
  delete [] _vi0;
  delete [] _vi1;
  if (net_nodes() > NODES_PER_BRANCH) {untested();
    delete [] _n;
  }else{
    // it is part of a base class
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_CAP::do_tr_con_chk_and_q()
{
  q_load();

  assert(_vy1);
  set_converged(conchk(_load_time, _sim->_time0));
  _load_time = _sim->_time0;
  for (int i=0; converged() && i<=_n_ports; ++i) {
    set_converged(conchk(_vy1[i], _vy0[i]));
  }
  set_converged();
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_CPOLY_CAP::do_tr()
{untested();
  incomplete();
  _m0 = CPOLY1(0., _vi0[0], _vi0[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
bool DEV_FPOLY_CAP::do_tr()
{
  assert((_time[0] == 0) || (_vy0[0] == _vy0[0]));

  _y[0].x  = tr_outvolts();
  _y[0].f0 = _vy0[0];
  _y[0].f1 = _vy0[1];
  
  _i[0] = differentiate(_y, _i, _time, _method_a);
  _vi0[0] = _i[0].f0;
  _vi0[1] = _i[0].f1;
  assert(_vi0[0] == _vi0[0]);
  
  if (_inputs) {untested();
    for (int i=1; i<=_n_ports; ++i) {untested();
      _vi0[i] = tr_c_to_g(_vy0[i], _vi0[i]);
      _vi0[0] -= *(_inputs[i]) * _vi0[i];
    }
  }else{
    for (int i=1; i<=_n_ports; ++i) {
      _vi0[i] = tr_c_to_g(_vy0[i], _vi0[i]);
      _vi0[0] -= volts_limited(_n[2*i-2],_n[2*i-1]) * _vi0[i];
      assert(_vi0[i] == _vi0[i]);
      assert(_vi0[0] == _vi0[0]);
    }
  }
  for (int i=0; i<=_n_ports; ++i) {
    assert(_vi0[i] == _vi0[i]);
  }
  
  _m0 = CPOLY1(0., _vi0[0], _vi0[1]);
  return do_tr_con_chk_and_q();
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::tr_load()
{
  for (int i=0; i<=_n_ports; ++i) {
    assert(_vi0[i] == _vi0[i]);
  }
  tr_load_passive();
  _vi1[0] = _vi0[0];
  _vi1[1] = _vi0[1];
  for (int i=2; i<=_n_ports; ++i) {
    tr_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1], &(_vi0[i]), &(_vi1[i]));
  }
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::tr_unload()
{untested();
  std::fill_n(_vi0, _n_ports+1, 0.);
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load();
}
/*--------------------------------------------------------------------------*/
double DEV_CPOLY_CAP::tr_amps()const
{untested();
  double amps = _m0.c0;
  for (int i=1; i<=_n_ports; ++i) {untested();
    amps += dn_diff(_n[2*i-2].v0(),_n[2*i-1].v0()) * _vi0[i];
  }
  return amps;
}
/*--------------------------------------------------------------------------*/
void DEV_CPOLY_CAP::ac_load()
{
  _acg = _vy0[1] * _sim->_jomega;
  ac_load_passive();
  for (int i=2; i<=_n_ports; ++i) {
    ac_load_extended(_n[OUT1], _n[OUT2], _n[2*i-2], _n[2*i-1], _vy0[i] * _sim->_jomega);
  }
}
/*--------------------------------------------------------------------------*/
/* set: set parameters, used in model building
 */
void DEV_CPOLY_CAP::set_parameters(const std::string& Label, CARD *Owner,
				   COMMON_COMPONENT *Common, double Value,
				   int n_states, double states[],
				   int n_nodes, const node_t nodes[])
  //				   const double* inputs[])
{
  bool first_time = (net_nodes() == 0);

  set_label(Label);
  set_owner(Owner);
  set_value(Value);
  attach_common(Common);

  if (first_time) {
    _n_ports = n_nodes/2; // sets num_nodes() = _n_ports*2
    assert(_n_ports == n_states-1);

    assert(!_vy1);
    assert(!_vi0);
    assert(!_vi1);
    _vy1 = new double[n_states]; 
    _vi0 = new double[n_states];
    _vi1 = new double[n_states];

    if (net_nodes() > NODES_PER_BRANCH) {untested();
      // allocate a bigger node list
      _n = new node_t[net_nodes()];
    }else{
      // use the default node list, already set
    }      
  }else{itested();
    assert(_n_ports == n_states-1);
    assert(_vy1);
    assert(_vi0);
    assert(_vi1);
    assert(net_nodes() == n_nodes);
    // assert could fail if changing the number of nodes after a run
  }

  //_inputs = inputs;
  _inputs = 0;
  _vy0 = states;
  std::fill_n(_vy0, n_states, 0.);
  std::fill_n(_vy1, n_states, 0.);
  std::fill_n(_vi0, n_states, 0.);
  std::fill_n(_vi1, n_states, 0.);
  notstd::copy_n(nodes, net_nodes(), _n);
  assert(net_nodes() == _n_ports * 2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_FPOLY_CAP p4;
DISPATCHER<CARD>::INSTALL
  d4(&device_dispatcher, "fpoly_cap", &p4);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
