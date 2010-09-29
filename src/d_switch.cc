/*$Id: d_switch.cc,v 22.9 2002/07/23 20:09:02 al Exp $ -*- C++ -*-
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
 * voltage (and current) controlled switch.
 * netlist syntax:
 * device:  Sxxxx n+ n- vc+ vc- mname <on>|<off> <model-card-args>
 * model:   .model mname SW <args>
 * current controlled switch
 * device:  Wxxxx n+ n- controlelement mname <on>|<off> <model-card-args>
 * model:   .model mname CSW <args>
 */
#include "ap.h"
#include "d_switch.h"
/*--------------------------------------------------------------------------*/
//		MODEL_SWITCH::MODEL_SWITCH();
//	void	MODEL_SWITCH::parse(CS&);
//	void	MODEL_SWITCH::print(int,int)const;
//		SWITCH_BASE::SWITCH_BASE();
//		SWITCH_BASE::SWITCH_BASE(const SWITCH_BASE& p);
//	void	SWITCH_BASE::parse_sb(CS&,int);
//	void	SWITCH_BASE::print(int,int)const;
//	void	SWITCH_BASE::expand_sb();
//	bool	SWITCH_BASE::do_tr();
//	void	SWITCH_BASE::do_ac();
//	void	DEV_CSWITCH::expand();
/*--------------------------------------------------------------------------*/
static COMMON_SWITCH Default_SWITCH(CC_STATIC);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool COMMON_SWITCH::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_SWITCH* p = dynamic_cast<const COMMON_SWITCH*>(&x);
  bool rv = p && is_equal(x);
  if (rv) {
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
MODEL_SWITCH::MODEL_SWITCH()
  :MODEL_CARD()
{
  vt = 0.;
  vh = 0.;
  ron = 1.;
  roff = 1e12;
  type = VOLTAGE;
}
/*--------------------------------------------------------------------------*/
void MODEL_SWITCH::parse(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();		// skip known ".model"
  parse_label(cmd);
  int here = cmd.cursor();
  /**/ set(cmd, "SW",  &type, VOLTAGE)
    || set(cmd, "CSW", &type, CURRENT);
  if (cmd.stuck(&here)){
    untested();
    cmd.warn(bWARNING, "need sw or csw");
  }
  cmd.skiplparen();
  cmd.stuck(&here);
  do{
    get(cmd, "VT",   &vt);
    get(cmd, "VH",   &vh,  mPOSITIVE);
    get(cmd, "IT",   &vt);
    get(cmd, "IH",   &vh,  mPOSITIVE);
    get(cmd, "RON",  &ron);
    get(cmd, "ROFF", &roff);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skiprparen();
  cmd.check(bWARNING, "what's this?");
}
/*--------------------------------------------------------------------------*/
void MODEL_SWITCH::print(OMSTREAM& where, int)const
{
  where.setfloatwidth(7);
  switch (type){
  case VOLTAGE:
    where << ".model " << short_label() << " sw ("
	  <<   "vt=" << vt
	  << "  vh=" << vh;
    break;
  case CURRENT:
    where << ".model " << short_label() << " csw ("
	  <<   "it=" << vt
	  << "  ih=" << vh;
    break;
  }
  where << "  ron="  << ron
	<< "  roff=" << roff
	<< ")\n";
}
/*--------------------------------------------------------------------------*/
SWITCH_BASE::SWITCH_BASE()
  :ELEMENT(),
   _input_label(),
   _input(0),
   _ic(_UNKNOWN),
   _current_state(_UNKNOWN),
   _previous_state(_UNKNOWN)
{
  attach_common(&Default_SWITCH);
}
/*--------------------------------------------------------------------------*/
SWITCH_BASE::SWITCH_BASE(const SWITCH_BASE& p)
  :ELEMENT(p),
   _input_label(p._input_label),
   _input(0),
   _ic(p._ic),
   _current_state(p._current_state),
   _previous_state(p._previous_state)
{
  untested();
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::parse_sb(CS& cmd, int num_nodes)
{
  assert(has_common());
  COMMON_SWITCH* c = prechecked_cast<COMMON_SWITCH*>(common()->clone());
  assert(c);
  assert(!c->has_model());

  parse_Label(cmd);
  parse_nodes(cmd, num_nodes, num_nodes);
  {if (typeid(*this) == typeid(DEV_CSWITCH)){
    _input_label = cmd.ctos(TOKENTERM);
    _input_label[0] = toupper(_input_label[0]);
  }else{
    assert(typeid(*this) == typeid(DEV_VSWITCH));
  }}
  c->parse_modelname(cmd);
  int here = cmd.cursor();
  /**/ ::set(cmd, "OFF",    &_ic, _OFF)
    || ::set(cmd, "ON",	    &_ic, _ON)
    || ::set(cmd, "UNKNOWN",&_ic, _UNKNOWN);
  {if (cmd.stuck(&here)){
    cmd.check(bWARNING, "need off, on, or unknown");
  }else{
    untested();
  }}
  attach_common(c);
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::print(OMSTREAM& where, int)const
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  where << short_label();
  printnodes(where);
  {if (_input){
    where << "  " << _input->short_label();
  }else{
    where << "  " << _input_label;
  }}
  where << ' ' << c->modelname();

  switch (_ic){
    case _OFF:	   where << " off";	break;
    case _ON:	   where << " on";	break;
    case _UNKNOWN: 			break;
  }
  where << '\n';
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::expand_sb()
{
  const MODEL_SWITCH* m = dynamic_cast<const MODEL_SWITCH*>(attach_model());
  if (!m){
    untested();
    assert(has_common());
    error(bERROR, long_label() + ": model " + common()->modelname()
	  + " is not a switch (SW or CSW)\n");
  }
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::precalc()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);

  set_value(m->ron);
  _y0.f0 = LINEAR;
  _y0.f1 = (_ic == _ON) ? m->ron : m->roff;	/* unknown is off */
  _m0.c1 = 1./_y0.f1;
  _m0.c0 = 0.;
  _m1 = _m0;
  _previous_state = _current_state = _ic;
  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  assert(!constant()); /* depends on input */
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::dc_begin()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);

  assert(_loss0 == 0.);
  assert(_loss1 == 0.);
  assert(_y0.f0 == LINEAR);
  _y0.f1 = ((_ic == _ON) ? m->ron : m->roff);  /* unknown is off */
  _m0.c1 = 1./_y0.f1;
  assert(_m0.c0 == 0.);
  _m1 = _m0;
  _current_state = _ic;
}
/*--------------------------------------------------------------------------*/
bool SWITCH_BASE::do_tr()
{
  const COMMON_SWITCH* c = prechecked_cast<const COMMON_SWITCH*>(common());
  assert(c);
  const MODEL_SWITCH* m = prechecked_cast<const MODEL_SWITCH*>(c->model());
  assert(m);

  _y0.x = (_input)			/* _y0.x is controlling value */
    ? CARD::probe(_input,"I")		/* current controlled */
    : _n[IN1].v0() - _n[IN2].v0();	/* voltage controlled */

  state_t new_state;
  {if (_y0.x > m->vt + m->vh){
    new_state = _ON;
  }else if (_y0.x < m->vt - m->vh){
    new_state = _OFF;
  }else{
    new_state = _previous_state;
  }}

  {if (new_state != _current_state){
    _y0.f1 = (new_state == _ON) ? m->ron : m->roff;	/* unknown is off */
    _current_state = new_state;
    _m0.c1 = 1./_y0.f1;
    q_load();
    store_values();
    set_not_converged();
  }else{
    assert(_y0.f1 == ((new_state == _ON) ? m->ron : m->roff));
    set_converged();
  }}
  return converged();
}
/*--------------------------------------------------------------------------*/
void SWITCH_BASE::do_ac()
{
  assert(_ev  == _y0.f1);
  assert(_acg == _m0.c1);
  ac_load();
}
/*--------------------------------------------------------------------------*/
void DEV_CSWITCH::expand()
{
  _input = dynamic_cast<ELEMENT*>(find_in_scope(_input_label));
  if (!_input){
    untested();
    error(bERROR,
	  long_label() + ": " + _input_label + " cannot be used as input\n");
  }
  expand_sb();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
