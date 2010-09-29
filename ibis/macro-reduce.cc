/*$Id: macro-reduce.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 * Make a "reduced" circuit based on the original.
 * Part of "reduction" is to EXPAND includes.
 * Incomplete:
 *	dsource, reshape, select, foreach
 */
#include "macro.h"
/*--------------------------------------------------------------------------*/
void Alarm::reduce_push(Macro_Body* m, const Instance& i)const
{
  untested();
  Alarm* x = new Alarm(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Capacitor::reduce_push(Macro_Body* m, const Instance& i)const
{
  Capacitor* x = new Capacitor(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Dsource::reduce_push(Macro_Body*, const Instance&)const
{
  untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
void Inverter::reduce_push(Macro_Body* m, const Instance& i)const
{
  Inverter* x = new Inverter(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Inductor::reduce_push(Macro_Body* m, const Instance& i)const
{
  Inductor* x = new Inductor(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Resistor::reduce_push(Macro_Body* m, const Instance& i)const
{
  Resistor* x = new Resistor(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void VCG::reduce_push(Macro_Body* m, const Instance& i)const
{
  VCG* x = new VCG(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void VCVS::reduce_push(Macro_Body* m, const Instance& i)const
{
  VCVS* x = new VCVS(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void VCCS::reduce_push(Macro_Body* m, const Instance& i)const
{
  VCCS* x = new VCCS(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void T_Line::reduce_push(Macro_Body* m, const Instance& i)const
{
  T_Line* x = new T_Line(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Trigger::reduce_push(Macro_Body* m, const Instance& i)const
{
  untested();
  Trigger* x = new Trigger(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Subckt::reduce_push(Macro_Body* m, const Instance& i)const
{
  Subckt* x = new Subckt(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Reshape::reduce_push(Macro_Body*, const Instance&)const
{
  untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
void Driver::reduce_push(Macro_Body* m, const Instance& i)const
{
  // pass on a driver element, for now
  // eventually will decompose into simpler elements
  Driver* x = new Driver(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Vsource::reduce_push(Macro_Body* m, const Instance& i)const
{
  Vsource* x = new Vsource(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Isource::reduce_push(Macro_Body* m, const Instance& i)const
{
  Isource* x = new Isource(*this, i);
  m->push(x);
}
/*--------------------------------------------------------------------------*/
void Case::reduce_push(Macro_Body* m, const Instance& i)const
{
  untested();
  body().reduce_push(m, i);
}
/*--------------------------------------------------------------------------*/
void Select::reduce_push(Macro_Body* m, const Instance& i)const
{
  untested();
  // for now, just pick the first one
  {if (begin() != end()) {
    (**(begin())).reduce_push(m, i);
  }else{
    untested();
    incomplete();
  }}
}
/*--------------------------------------------------------------------------*/
void Declare::reduce_push(Macro_Body*, const Instance&)const
{
  // nothing
}
/*--------------------------------------------------------------------------*/
void Inherit::reduce_push(Macro_Body* m, const Instance& i)const
{
  // Replace with its expansion.  (Opposite of reduce??)
  macro()->reduce_push(m, i);
}
/*--------------------------------------------------------------------------*/
void If::reduce_push(Macro_Body* m, const Instance& i)const
{
  {if (test(i)) {
    true_part().reduce_push(m, i);
  }else{
    false_part().reduce_push(m, i);
  }}
}
/*--------------------------------------------------------------------------*/
void Foreach::reduce_push(Macro_Body* m, const Instance& i)const
{
  incomplete();
  // true part is missing
  false_part().reduce_push(m, i);
}
/*--------------------------------------------------------------------------*/
void Export::reduce_push(Macro_Body*, const Instance&)const
{
  // nothing
}
/*--------------------------------------------------------------------------*/
void Define::reduce_push(Macro_Body*, const Instance& i)const
{
  // don't bother making a copy of this one
  //Define* x = new Define(*this, i);
  //m->push(x);

  // instead, reduce the expression ...
  Expression exp(value(), i);

  // then save as if it was data.
  //Scalar* val = new Scalar(name(), x->value().back()->name());
  Scalar* val = new Scalar(name(), exp.back()->name());
  Instance& inst(const_cast<Instance&>(i));
  inst.push(val);
  inst._map[name()] = val;
}
/*--------------------------------------------------------------------------*/
void Assert::reduce_push(Macro_Body*, const Instance& i)const
{
  if (!test(i)) {
    error(0, "macro assertion failed\n"
	  "Base type:" + i.base_type() + "\n"
	  "Sub type:" + i.sub_type() + "\n"
	  "Full name:" + i.full_name() + "\n");
	  //"Expression:" + test() + "\n");
  }
  //BUG:: This does not give a clear error message
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Macro_Body::reduce_push(Macro_Body* m, const Instance& instance)const
{
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    assert(dynamic_cast<const Item_Base*>(*i));
    (**i).reduce_push(m, instance);
  }}
}
/*--------------------------------------------------------------------------*/
void Macro::reduce_push(Macro_Body* m, const Instance& i)const
{
  body().reduce_push(m, i);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Macro_Body::Macro_Body(const Macro_Body& m, const Instance& instance)
  :_sym(m._sym),
   _ml(m._ml),
   _scope(m._scope)
{
  m.reduce_push(this, instance);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
