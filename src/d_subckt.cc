/*$Id: d_subckt.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * subcircuit stuff
 * base class for other elements using internal subckts
 * netlist syntax:
 * device: Xxxxx <nodelist> <subckt-name> <args>
 * model:  .subckt <subckt-name> <nodelist>
 *	   (device cards)
 *	   .ends <subckt-name>
 * storage note ...
 * the .subckt always has a comment at the hook point, so a for loop works
 * the expansion (attact to the X) has all comments removed
 *	- need to process the entire ring - for doesn't work
 */
//testing=script 2006.07.17
#include "d_subckt.h"
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT::_count = -1;
int COMMON_SUBCKT::_count = -1;
int MODEL_SUBCKT::_count = -1;
static COMMON_SUBCKT Default_SUBCKT(CC_STATIC);
/*--------------------------------------------------------------------------*/
static DEV_SUBCKT   p1;
static MODEL_SUBCKT p2;
static DISPATCHER<CARD>::INSTALL
  d1(&device_dispatcher, "X|dev_subckt", &p1),
  d2(&device_dispatcher, "subckt|macro", &p2);
/*--------------------------------------------------------------------------*/
bool COMMON_SUBCKT::operator==(const COMMON_COMPONENT& x)const
{
  const COMMON_SUBCKT* p = dynamic_cast<const COMMON_SUBCKT*>(&x);
  bool rv = p 
    && _params == p->_params
    && COMMON_COMPONENT::operator==(x);
  return rv;
}
/*--------------------------------------------------------------------------*/
bool COMMON_SUBCKT::param_is_printable(int i)const
{
  assert(i < COMMON_SUBCKT::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.is_printable(COMMON_SUBCKT::param_count() - 1 - i);
  }else{
    return COMMON_COMPONENT::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_SUBCKT::param_name(int i)const
{
  assert(i < COMMON_SUBCKT::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.name(COMMON_SUBCKT::param_count() - 1 - i);
  }else{
    return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_SUBCKT::param_name(int i, int j)const
{
  assert(i < COMMON_SUBCKT::param_count());
  if (j == 0) {untested();
    return param_name(i);
  }else if (i >= COMMON_COMPONENT::param_count()) {untested();
    return "";
  }else{untested();
    return COMMON_COMPONENT::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string COMMON_SUBCKT::param_value(int i)const
{
  assert(i < COMMON_SUBCKT::param_count());
  if (i >= COMMON_COMPONENT::param_count()) {
    return _params.value(COMMON_SUBCKT::param_count() - 1 - i);
  }else{
    return COMMON_COMPONENT::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::precalc_first(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_first(Scope);

  for (PARAM_LIST::iterator i = _params.begin(); i != _params.end(); ++i) {
    i->second.e_val(NOT_INPUT,Scope);
  }
  _mfactor = _params.deep_lookup("m");
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::precalc_last(const CARD_LIST* Scope)
{
  assert(Scope);
  COMMON_COMPONENT::precalc_last(Scope);
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::MODEL_SUBCKT()
  :COMPONENT()
{
  _n = _nodes;
  new_subckt();
  ++_count;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::MODEL_SUBCKT(const MODEL_SUBCKT& p)
  :COMPONENT(p)
{
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  assert(p.subckt()->is_empty()); // incomplete, but enough for now.
  new_subckt();
  ++_count;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT::~MODEL_SUBCKT()
{
  --_count;
}
/*--------------------------------------------------------------------------*/
CARD* MODEL_SUBCKT::clone_instance()const
{itested();
  DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(p1.clone());
  new_instance->_parent = this;
  return new_instance;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT(),
   _parent(NULL)
{
  attach_common(&Default_SUBCKT);
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _parent(p._parent)
{
  //strcpy(modelname, p.modelname); in common
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  BASE_SUBCKT::expand();
  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);
  const CARD* model = find_looking_out(c->modelname());
  if (!_parent) {
    if(!dynamic_cast<const MODEL_SUBCKT*>(model)) {
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }else{
      _parent = prechecked_cast<const MODEL_SUBCKT*>(model);
    }
  }else{
    assert(model && model == _parent);
  }
  
  //assert(!c->_params._try_again);
  assert(model->subckt());
  assert(model->subckt()->params());
  PARAM_LIST* pl = const_cast<PARAM_LIST*>(model->subckt()->params());
  assert(pl);
  c->_params.set_try_again(pl);
  assert(c->_params._try_again);
  renew_subckt(model, this, scope(), &(c->_params));
  subckt()->expand();
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{
  BASE_SUBCKT::precalc_first();

  if (subckt()) {
    COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
    assert(c);
    subckt()->attach_params(&(c->_params), scope());
    subckt()->precalc_first();
  }else{
  }
  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_last()
{
  BASE_SUBCKT::precalc_last();

  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);
  subckt()->attach_params(&(c->_params), scope());
  subckt()->precalc_last();

  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
double DEV_SUBCKT::tr_probe_num(const std::string& x)const
{itested();
  if (Umatch(x, "p ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (Umatch(x, "pd ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PD");
    }      
    return power;
  }else if (Umatch(x, "ps ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PS");
    }      
    return power;
  }else{itested();
    return COMPONENT::tr_probe_num(x);
  }
  /*NOTREACHED*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
