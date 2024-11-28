/*$Id: d_coil.cc 2016/03/23 al $ -*- C++ -*-
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
 * inductors
 * y.x = amps,  y.f0 = flux,  ev = y.f1 = henrys
 * q = y history in time
 * i.x = amps,  i.f0 = volts,      i.f1 = ohms
 * m.x = volts, m.c0 = amps, acg = m.c1 = mhos
 */
//testing=script 2008.10.09
#include "globals.h"
#include "e_subckt.h"
#include "e_ccsrc.h"
#include "e_storag.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_INDUCTANCE : public STORAGE {
protected:
  explicit DEV_INDUCTANCE(const DEV_INDUCTANCE& p) 
    :STORAGE(p), _c_model(p._c_model) {}
public:
  explicit DEV_INDUCTANCE()
    :STORAGE(), _c_model(false) {}
public: // override virtual
  char	   id_letter()const override	{return 'L';}
  std::string value_name()const override {return "l";}
  std::string dev_type()const override	{return "inductor";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 2;}
  int	   int_nodes()const override     {return (!_c_model) ? 0 : 1;}
  int	   matrix_nodes()const override	{return net_nodes() + int_nodes();}

  bool	   has_inode()const override	{return _c_model;}
  bool	   has_iv_probe()const override  {return true;}
  bool	   use_obsolete_callback_parse()const override {return true;}
  CARD*	   clone()const override		{return new DEV_INDUCTANCE(*this);}
  void     expand()override;
  void     precalc_last()override;
  void	   tr_iwant_matrix()override;
  void     tr_begin()override;
  bool	   do_tr()override;
  void	   tr_load()override;
  void	   tr_unload()override;
  double   tr_involts()const override	{return tr_outvolts();}
  double   tr_input()const override;
  double   tr_involts_limited()const override {return tr_outvolts_limited();}
  double   tr_input_limited()const override;
  double   tr_amps()const override;
  double   tr_probe_num(const std::string&)const override;
  void	   ac_iwant_matrix()override;
  void	   ac_begin()override;
  void	   do_ac()override;
  void	   ac_load()override;
  COMPLEX  ac_involts()const override	{return ac_outvolts();}
  COMPLEX  ac_amps()const override;

  std::string port_name(int i)const override {itested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"p", "n"};
    return names[i];
  }

  bool _c_model;
};
/*--------------------------------------------------------------------------*/
class DEV_MUTUAL_L : public DEV_INDUCTANCE {
private:
  std::string	  _output_label;
  DEV_INDUCTANCE* _output;
  std::string	  _input_label;
  DEV_INDUCTANCE* _input;
  double _lm;
  double _mf0_c0;	// matrix parameters, new
  double _mf1_c0;	// matrix parameters, 1 fill ago
  double _mr0_c0;	// matrix parameters, new
  double _mr1_c0;	// matrix parameters, 1 fill ago
  FPOLY1 _yf1;		// iteration parameters, 1 iter ago
  FPOLY1 _yf[OPT::_keep_time_steps];
  FPOLY1 _if[OPT::_keep_time_steps];
  FPOLY1 _yr1;		// iteration parameters, 1 iter ago
  FPOLY1 _yr[OPT::_keep_time_steps];
  FPOLY1 _ir[OPT::_keep_time_steps];
private:
  explicit	DEV_MUTUAL_L(const DEV_MUTUAL_L& p);
public:
  explicit	DEV_MUTUAL_L();
private: // override virtual
  char	   id_letter()const override	{return 'K';}
  bool	   print_type_in_spice()const override {return false;}
  std::string value_name()const override {return "k";}
  std::string dev_type()const override	{untested(); return "mutual_inductor";}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return 0;}
  int	   num_current_ports()const override {return 2;}
  bool	   has_iv_probe()const override  {untested(); return false;}
  bool	   use_obsolete_callback_parse()const override {return false;}
  CARD*	   clone()const override		{return new DEV_MUTUAL_L(*this);}
  void     expand_first()override;
  void	   expand_last()override;
  void	   precalc_last()override;
  void     tr_iwant_matrix()override	{tr_iwant_matrix_passive();}
  void     tr_begin()override;
  void     dc_advance()override;
  void     tr_advance()override;
  bool     do_tr()override		{_sim->_late_evalq.push_back(this); return true;}
  bool     do_tr_last()override;
  void	   tr_load()override;
  TIME_PAIR tr_review()override		{return TIME_PAIR(NEVER,NEVER);}
  void	   tr_unload()override;
  double   tr_input()const override		{return tr_involts();}
  double   tr_input_limited()const override	{untested(); return tr_involts_limited();}
  double   tr_amps()const override		{untested(); return _loss0 * tr_outvolts();}
  double   tr_probe_num(const std::string&)const override;

  void	   ac_iwant_matrix()override	{ac_iwant_matrix_passive();}
  void	   ac_load()override;
  COMPLEX  ac_amps()const override	{untested(); return _loss0 * ac_outvolts();}

  int	   set_port_by_name(std::string& Name, std::string& Value)override
		{untested(); return COMPONENT::set_port_by_name(Name,Value);}
  void	   set_port_by_index(int Index, std::string& Value)override
		{set_current_port_by_index(Index, Value);}
  bool	   node_is_connected(int i)const override {
    switch (i) {
    case 0:  return _output_label != "";
    case 1:  return _input_label != "";
    default: unreachable(); return false;
    }
  }

  std::string port_name(int)const override {untested();
    return "";
  }
  std::string current_port_name(int i)const override {untested();
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"l1", "l2"};
    return names[i];
  }
  const std::string current_port_value(int i)const override {
    switch (i) {
    case 0:  return _output_label;
    case 1:  return _input_label;
    default: unreachable(); return COMPONENT::current_port_value(i);
    }
  }
  void set_current_port_by_index(int i, const std::string& s) override {
    switch (i) {
    case 0:  _output_label = s;	break;
    case 1:  _input_label = s;	break;
    default: unreachable();	break;
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_MUTUAL_L::DEV_MUTUAL_L()
  :DEV_INDUCTANCE(),
   _output_label(),
   _output(0),
   _input_label(),
   _input(0),
   _lm(NOT_INPUT),
   _mf0_c0(0.),
   _mf1_c0(0.),
   _mr0_c0(0.),
   _mr1_c0(0.)
{
  _c_model = true;
  assert(_yf[0].x == 0. && _yf[0].f0 == 0. && _yf[0].f1 == 0.);
  assert(_yf1 == _yf[0]);
  assert(_yr[0].x == 0. && _yr[0].f0 == 0. && _yr[0].f1 == 0.);
  assert(_yr1 == _yr[0]);
}
/*--------------------------------------------------------------------------*/
DEV_MUTUAL_L::DEV_MUTUAL_L(const DEV_MUTUAL_L& p)
  :DEV_INDUCTANCE(p),
   _output_label(p._output_label),
   _output(p._output),
   _input_label(p._input_label),
   _input(p._input),
   _lm(p._lm),
   _mf0_c0(0.),
   _mf1_c0(0.),
   _mr0_c0(0.),
   _mr1_c0(0.)
{
  _c_model = true;
  assert(_yf[0].x == 0. && _yf[0].f0 == 0. && _yf[0].f1 == 0.);
  assert(_yf1 == _yf[0]);
  assert(_yr[0].x == 0. && _yr[0].f0 == 0. && _yr[0].f1 == 0.);
  assert(_yr1 == _yr[0]);
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::precalc_last()
{
  STORAGE::precalc_last();
  // Something like it should be moved to ac_begin.
  if (_sim->has_op() == s_NONE) {
    _y[0].x  = 0.;
    _y[0].f0 = LINEAR;
    _y[0].f1 = value();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::expand()
{
  STORAGE::expand();
  if (_sim->is_first_expand()) {
    if (!_c_model) {
      //n_(IN1).set_to_ground(this);
    }else{
      n_(IN1).new_model_node(long_label() + ".i", this);
    }
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::expand_first()
{
  _output = dynamic_cast<DEV_INDUCTANCE*>(find_in_my_scope(_output_label));
  if (!_output) {
    throw Exception_Type_Mismatch(long_label(), _output_label, "inductor");
  }else{
    _output->_c_model = true;
  }

  _input = dynamic_cast<DEV_INDUCTANCE*>(find_in_my_scope(_input_label));
  if (!_input) {
    throw Exception_Type_Mismatch(long_label(), _input_label, "inductor");
  }else{
    _input->_c_model = true;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::expand_last()
{
  STORAGE::expand(); // skip DEV_INDUCTANCE
  if (_sim->is_first_expand()) {
    n_(OUT2) = _input->n_(IN1);
    n_(OUT1) = _output->n_(IN1);
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::precalc_last()
{
  _output->precalc_last();
  _input->precalc_last();

  DEV_INDUCTANCE::precalc_last();

  double l1 = _output->value();
  double l2 = _input->value();
  _lm = value() * sqrt(l1 * l2);
  trace3(long_label().c_str(), l1, l2, _lm);

  if (_sim->has_op() == s_NONE) {
    assert(_y[0].x  == 0.);
    assert(_y[0].f0 == LINEAR);
    _y[0].f1 = -_lm; // override
    _yf[0] = _yr[0] = _y[0];
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_iwant_matrix()
{
  if (!_c_model) {
    tr_iwant_matrix_passive();
  }else{
    assert(matrix_nodes() == 3);
    
    assert(n_(OUT1).m_() != INVALID_NODE);
    assert(n_(OUT2).m_() != INVALID_NODE);
    assert(n_(IN1).m_() != INVALID_NODE);
    
    _sim->_aa.iwant(n_(OUT1).m_(),n_(IN1).m_());
    _sim->_aa.iwant(n_(OUT2).m_(),n_(IN1).m_());
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_begin()
{
  STORAGE::tr_begin();
  _loss1 = _loss0 = ((!_c_model) ? 0. : 1.);
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::tr_begin()
{
  DEV_INDUCTANCE::tr_begin();
  assert(_y[0].x  == 0.);
  assert(_y[0].f0 == LINEAR);
  _y[0].f1 = -_lm; // override
  _y1 = _y[0];

  for (int i = 0;  i < OPT::_keep_time_steps;  ++i) {
    _if[i] = _ir[i] = FPOLY1(0., 0., 0.);
  }
  _mf1_c0 = _mf0_c0 = _mr1_c0 = _mr0_c0 = 0.;
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::dc_advance()
{
  STORAGE::dc_advance();

  for (int i = 1;  i < OPT::_keep_time_steps;  ++i) {
    _if[i] = _if[0];
    _ir[i] = _ir[0];
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::tr_advance()
{
  STORAGE::tr_advance();
  
  for (int i=OPT::_keep_time_steps-1; i>0; --i) {
    _yf[i] = _yf[i-1];
    _yr[i] = _yr[i-1];
    _if[i] = _if[i-1];
    _ir[i] = _ir[i-1];
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_INDUCTANCE::do_tr()
{
  if (using_tr_eval()) {
    _y[0].x = tr_input_limited(); // _m0.c0 + _m0.c1 * x;
    tr_eval();
    if ((!_c_model) && (_y[0].f1 == 0.)) {untested();
      error(bDANGER, long_label() + ": short circuit,  L = 0\n");
      _y[0].f1 = OPT::shortckt;
      set_converged(conv_check());
    }else{
    }
  }else{
    _y[0].x = tr_input(); // _m0.c0 + _m0.c1 * x;
    assert(_y[0].f1 == value());
    _y[0].f0 = _y[0].x * _y[0].f1;
    assert(converged());
  }
  store_values();
  q_load();
  
  // i is really voltage ..
  // _i[0].x = current, _i[0].f0 = voltage, _i[0].f1 = ohms
  _i[0] = differentiate(_y, _i, _time, _method_a);
  
  if (!_c_model) {
    _m0.x = NOT_VALID;
    _m0.c1 = 1 / ((_i[0].c1()==0) ? OPT::shortckt : _i[0].c1());
    _m0.c0 = -_i[0].c0() * _m0.c1;
  }else{
    //_m0 = -CPOLY1(_i[0]);
    _m0.x = NOT_VALID;
    _m0.c1 = -_loss0 * _loss0 * _i[0].c1();
    _m0.c0 =  _loss0 * _loss0 * _i[0].c0();
  }

  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_MUTUAL_L::do_tr_last()
{
  double l1 = _output->_y[0].f1;
  double l2 = _input->_y[0].f1;
  _lm = value() * sqrt(l1 * l2);

  _y[0].x = n_(OUT1).v0() - n_(OUT2).v0(); // really current
  _y[0].f1 = -_lm;
  _y[0].f0 = _y[0].x * _y[0].f1;	   // flux = I * L
  trace3("", _y[0].x, _y[0].f0, _y[0].f1);
  store_values();
  _i[0] = differentiate(_y, _i, _time, _method_a);  // really voltage, v = df/dt
  trace3("", _i[0].x, _i[0].f0, _i[0].f1);
  _m0.x = NOT_VALID;
  _m0.c1 = -_loss0 * _loss0 * _i[0].c1();
  _m0.c0 = -_loss0 * _loss0 * _i[0].c0();
  trace3("", _m0.x, _m0.c0, _m0.c1);

  _yf[0].x = n_(OUT1).v0();
  _yf[0].f1 = -_lm;
  _yf[0].f0 = _yf[0].x * _yf[0].f1;
  trace3("", _yf[0].x, _yf[0].f0, _yf[0].f1);
  assert(_yf[0]==_yf[0]); // store_values();
  _yf1=_yf[0];		  // store_values();
  _if[0] = differentiate(_yf, _if, _time, _method_a);
  trace3("", _if[0].x, _if[0].f0, _if[0].f1);
  _mf0_c0 = -_loss0 * _loss0 * _if[0].c0();
  
  _yr[0].x = n_(OUT2).v0();
  _yr[0].f1 = -_lm;
  _yr[0].f0 = _yr[0].x * _yr[0].f1;
  trace3("", _yr[0].x, _yr[0].f0, _yr[0].f1);
  assert(_yr[0]==_yr[0]); // store_values();
  _yr1=_yr[0];		  // store_values();
  _ir[0] = differentiate(_yr, _ir, _time, _method_a);
  trace3("", _ir[0].x, _ir[0].f0, _ir[0].f1);
  _mr0_c0 = -_loss0 * _loss0 * _ir[0].c0();

  q_load();
  return true;
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_load()
{
  if (!_c_model) {
    tr_load_passive();
  }else{
    tr_load_inode();
    tr_load_diagonal_point(n_(IN1), &_m0.c1, &_m1.c1);
    tr_load_source_point(n_(IN1), &_m0.c0, &_m1.c0);
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::tr_load()
{
  tr_load_couple();
  tr_load_source();
  tr_load_source_point(n_(OUT2), &_mr0_c0, &_mr1_c0);
  tr_load_source_point(n_(OUT1), &_mf0_c0, &_mf1_c0);
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_unload()
{untested();
  _loss0 = _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load();
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::tr_unload()
{untested();
  tr_unload_couple();
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_input()const
{
  if (!_c_model) {
    return _m0.c0 + _m0.c1 * tr_involts();
  }else{
    return n_(IN1).v0();
  }
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_input_limited()const
{
  if (!_c_model) {
    return _m0.c0 + _m0.c1 * tr_involts_limited();
  }else{
    return n_(IN1).v0();
  }
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_amps()const
{
  if (!_c_model) {
    return fixzero((_m0.c1 * tr_involts() + _m0.c0), _m0.c0);
  }else{
    return _loss0 * n_(IN1).v0();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_iwant_matrix()
{
  if (!_c_model) {
    ac_iwant_matrix_passive();
  }else{
    assert(matrix_nodes() == 3);
    
    assert(n_(OUT1).m_() != INVALID_NODE);
    assert(n_(OUT2).m_() != INVALID_NODE);
    assert(n_(IN1).m_() != INVALID_NODE);
    
    _sim->_acx.iwant(n_(OUT1).m_(),n_(IN1).m_());
    _sim->_acx.iwant(n_(OUT2).m_(),n_(IN1).m_());
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_begin()
{
  // STORAGE::ac_begin();

  _loss1 = _loss0 = ((!_c_model) ? 0. : 1.);
  _ev = _y[0].f1;
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::do_ac()
{
  if (using_ac_eval()) {
    ac_eval();
  }else{
    assert(_ev == _y[0].f1);
    assert(dynamic_cast<DEV_MUTUAL_L*>(this) || has_tr_eval() || _ev == double(value()));
  }
  if (!_c_model) {
    if (_ev * _sim->_jomega == 0.) {untested();
      _acg = 1. / OPT::shortckt;
    }else{
      _acg = 1. / (_ev * _sim->_jomega);
    }
  }else{
    _acg =  -_loss0 * _loss0 * _ev * _sim->_jomega;
  }
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_load()
{
  if (!_c_model) {
    ac_load_passive();
  }else{
    ac_load_inode();
    ac_load_diagonal_point(n_(IN1), _acg);
  }
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::ac_load()
{
  ac_load_couple();
}
/*--------------------------------------------------------------------------*/
COMPLEX DEV_INDUCTANCE::ac_amps()const
{
  if (!_c_model) {
    return (ac_involts() * _acg);
  }else{
    return  _loss0 * n_(IN1).vac();
  }
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "flux ")) {untested();
    return _y[0].f0;
  }else if (Umatch(x, "ind{uctance} |l ")) {untested();
    return _y[0].f1;
  }else if (Umatch(x, "dldt ")) {untested();
    return (_y[0].f1 - _y[1].f1) / _dt;
  }else if (Umatch(x, "dl ")) {untested();
    return (_y[0].f1 - _y[1].f1);
  }else if (Umatch(x, "dfdt ")) {untested();
    return (_y[0].f0 - _y[1].f0) / _dt;
  }else if (Umatch(x, "dflux ")) {untested();
    return (_y[0].f0 - _y[1].f0);
  }else{
    return STORAGE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
double DEV_MUTUAL_L::tr_probe_num(const std::string& x)const
{untested();
  if (Umatch(x, "fflux ")) {untested();
    return _yf[0].f0;
  }else if (Umatch(x, "rflux ")) {untested();
    return _yr[0].f0;
  }else if (Umatch(x, "fiof{fset} ")) {untested();
    return _mf0_c0;
  }else if (Umatch(x, "riof{fset} ")) {untested();
    return _mr0_c0;
  }else{untested();
    return DEV_INDUCTANCE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_MUTUAL_L   p1;
DEV_INDUCTANCE p2;
DISPATCHER<CARD>::INSTALL
  d1(&device_dispatcher, "K|mutual_inductor", &p1),
  d2(&device_dispatcher, "L|inductor",        &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
