/*$Id: d_coil.cc,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * inductors
 * y.x = amps,  y.f0 = flux,  ev = y.f1 = henrys
 * q = y history in time
 * i.x = amps,  i.f0 = volts,      i.f1 = ohms
 * m.x = volts, m.c0 = amps, acg = m.c1 = mhos
 */
#include "d_cccs.h"
#include "ap.h"
#include "d_coil.h"
/*--------------------------------------------------------------------------*/
DEV_MUTUAL_L::DEV_MUTUAL_L()
  :COMPONENT(),
   _output_label(),
   _output(0),
   _input_label(),
   _input(0)
{
}
/*--------------------------------------------------------------------------*/
DEV_MUTUAL_L::DEV_MUTUAL_L(const DEV_MUTUAL_L& p)
  :COMPONENT(p),
   _output_label(p._output_label),
   _output(p._output),
   _input_label(p._input_label),
   _input(p._input)
{
  untested();
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::parse(CS& cmd)
{
  parse_Label(cmd);
  _output_label = cmd.ctos(TOKENTERM);
  _output_label[0] = toupper(_output_label[0]);
  _input_label = cmd.ctos(TOKENTERM);
  _input_label[0] = toupper(_input_label[0]);
  set_value(cmd.ctof());
}
/*--------------------------------------------------------------------------*/
void DEV_MUTUAL_L::print(OMSTREAM& where, int)const
{
  where << short_label();
  {if (_output) {
    where << ' ' << _output->short_label();
  }else{
    where << "  " << _output_label;
  }}
  {if (_input) {
    where << ' ' << _input->short_label();
  }else{
    where << "  " << _input_label;
  }}
  where.setfloatwidth(7) << ' ' << value() << '\n';
}
/*--------------------------------------------------------------------------*/
// replace both primary and secondary (both simple L's)
// with the CCCS - L equivalent
// only works for 2.
void DEV_MUTUAL_L::expand()
{
  _output = find_in_scope(_output_label);
  _input  = find_in_scope(_input_label);
  _output->subckt().destroy();
  _input->subckt().destroy();

  double l1 = _output->value();
  double l2 = _input->value();
  double lm  = value() * sqrt(l1 * l2);
  double det = l1 * l2 - lm * lm;
  
  DEV_INDUCTANCE* pri = dynamic_cast<DEV_INDUCTANCE*>(_output->clone());
  if (!pri) {
    untested();
    error(bERROR, 
	  long_label() + ": " + _output_label + " is not an inductor\n");
  }
  if (pri->has_common()) {
    error(bERROR, long_label() + ": " + _output_label
	  + " must be linear, giving up\n");
  }
  pri->set_owner(_output);
  pri->set_value(det / l2);
  _output->subckt().push_back(pri);

  DEV_INDUCTANCE* sec = dynamic_cast<DEV_INDUCTANCE*>(_input->clone());
  if (!sec) {
    untested();
    error(bERROR, 
	  long_label() + ": " + _input_label + " is not an inductor\n");
  }
  if (sec->has_common()) {
    error(bERROR, long_label() + ": " + _input_label
	  + " must be linear, giving up\n");
  }
  sec->set_owner(_input);
  sec->set_value(det / l1);
  _input->subckt().push_back(sec);

  DEV_CCCS* sub = new DEV_CCCS;
  assert(sub);
  sub->set_parameters_cc("F" + sec->short_label(), _output, NULL,
		      -lm / l1, _output->_n[OUT1], _output->_n[OUT2], sec);
  _output->subckt().push_back(sub).expand();
  
  sub = new DEV_CCCS;
  assert(sub);
  sub->set_parameters_cc("F" + pri->short_label(), _input, NULL,
		      -lm / l2, _input->_n[OUT1], _input->_n[OUT2], pri);
  _input->subckt().push_back(sub).expand();

  assert(!constant()); /* because of integration */
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::map_nodes()
{
  {if (subckt().exists()) {
    subckt().map_nodes();
  }else{
  }}
  COMPONENT::map_nodes();
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_alloc_matrix()
{
  {if (subckt().exists()) {
    subckt().tr_alloc_matrix();
  }else{
    tr_alloc_matrix_passive();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::precalc()
{
  {if (subckt().exists()) {
    subckt().precalc();
  }else{
    STORAGE::precalc();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::dc_begin()
{
  {if (subckt().exists()) {
    untested();
    subckt().dc_begin();
  }else{
    STORAGE::dc_begin();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_begin()
{
  {if (subckt().exists()) {
    subckt().tr_begin();
  }else{
    STORAGE::tr_begin();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_restore()
{
  {if (subckt().exists()) {
    untested();
    subckt().tr_restore();
  }else{
    STORAGE::tr_restore();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::dc_advance()
{
  {if (subckt().exists()) {
    subckt().dc_advance();
  }else{
    STORAGE::dc_advance();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_advance()
{
  {if (subckt().exists()) {
    subckt().tr_advance();
  }else{
    STORAGE::tr_advance();
  }}
}
/*--------------------------------------------------------------------------*/
bool DEV_INDUCTANCE::tr_needs_eval()
{
  {if (subckt().exists()) {
    untested();
    return subckt().tr_needs_eval();
  }else{
    return true;
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_queue_eval()
{
  {if (subckt().exists()) {
    subckt().tr_queue_eval();
  }else{
    q_eval();
  }}
}
/*--------------------------------------------------------------------------*/
bool DEV_INDUCTANCE::do_tr()
{
  {if (subckt().exists()) {
    set_converged(subckt().do_tr());
  }else{
    double x = NOT_VALID;
    {if (using_tr_eval()) {
      x = tr_involts_limited();
      _y0.x = _m0.c0 + _m0.c1 * x;
      tr_eval();
      if (_y0.f1 == 0.) {
	untested();
	error(bDANGER, long_label() + ": short circuit,  L = 0\n");
	_y0.f1 = OPT::shortckt;
	set_converged(conv_check());
      }
    }else{
      x = _n[OUT1].v0() - _n[OUT2].v0();
      _y0.x = _m0.c0 + _m0.c1 * x;
      assert(_y0.f1 == value());
      _y0.f0 = _y0.x * _y0.f1;
      assert(converged());
    }}
    store_values();
    q_load();

    _q[0] = _y0;

    // i is really voltage ..
    // _i0.x = current, _i0.f0 = voltage, _i0.f1 = ohms
    _i0.x  = _y0.x;
    _i0.f0 = differentiate();
    _i0.f1 = tr_c_to_g(_q[0].f1, _i0.f1);

    _m0.x = x;
    _m0.c1 = 1 / ((_i0.c1()==0) ? OPT::shortckt : _i0.c1());
    _m0.c0 = -_i0.c0() * _m0.c1;
  }}
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_load()
{
  {if (subckt().exists()) {
    subckt().tr_load();
  }else{
    tr_load_passive();
  }}
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_review()
{
  {if (subckt().exists()) {
    return subckt().tr_review();
  }else{
    return STORAGE::tr_review();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::tr_unload()
{
  {if (subckt().exists()) {
    untested();
    subckt().tr_unload();
  }else{
    untested();
    tr_unload_passive();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_alloc_matrix()
{
  {if (subckt().exists()) {
    subckt().ac_alloc_matrix();
  }else{
    ac_alloc_matrix_passive();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_begin()
{
  {if (subckt().exists()) {
    subckt().ac_begin();
  }else{
    _ev = _y0.f1;
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::do_ac()
{
  {if (subckt().exists()) {
    subckt().do_ac();
  }else{
    {if (has_ac_eval()) {
      ac_eval();
    }else{
      assert(_ev == _y0.f1);
      assert(has_tr_eval() || _ev == value());
    }}
    {if (_ev * SIM::jomega == 0.) {
      untested();
      _acg = 1. / OPT::shortckt;
    }else{
      _acg = 1. / (_ev * SIM::jomega);
    }}
    ac_load();
  }}
}
/*--------------------------------------------------------------------------*/
void DEV_INDUCTANCE::ac_load()
{
  {if (subckt().exists()) {
    unreachable();
    subckt().ac_load();
  }else{
    ac_load_passive();
  }}
}
/*--------------------------------------------------------------------------*/
double DEV_INDUCTANCE::tr_probe_num(CS& cmd)const
{
  {if (subckt().exists()) {
    return COMPONENT::tr_probe_num(cmd);
  }else{
    {if (cmd.pmatch("PHI") || cmd.pmatch("FLUX")) {
      untested();
      return _q[0].f0;
    }else if (cmd.pmatch("INDuctance")) {
      untested();
      return _q[0].f1;
    }else if (cmd.pmatch("DPDT")) {
      untested();
      return (_q[0].f0 - _q[1].f0) / _dt;
    }else if (cmd.pmatch("DPHI")) {
      untested();
      return (_q[0].f0 - _q[1].f0);
    }else{
      return STORAGE::tr_probe_num(cmd);
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
