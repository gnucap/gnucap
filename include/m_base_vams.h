/*                           -*- C++ -*-
 * Copyright (C) 2034 Felix Salfelder
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
 */
#ifndef BASE_VAMS_H_INCLUDED
#define BASE_VAMS_H_INCLUDED
#include "m_base.h"
/*--------------------------------------------------------------------------*/
// Verilog-like integer. C style arithmetics
// e.g. 1/2 is vInteger(0)
class vInteger : public Integer {
  using Integer::less;
  using Integer::greater;
  using Integer::leq;
  using Integer::geq;
  using Integer::not_equal;
  using Integer::equal;
  using Integer::add;
  using Integer::multiply;
  using Integer::subtract;
  using Integer::r_subtract;
  using Integer::divide;
  using Integer::r_divide;
  using Integer::modulo;
  using Integer::r_modulo;
public:
  /*implicit*/ vInteger(const vInteger& p) : Integer(p) {}
  explicit vInteger(CS& file)		{untested();parse(file);}
  explicit vInteger(const std::string& s)	{CS cs(CS::_STRING, s); parse(cs);}
  vInteger(int32_t x) : Integer(x) {}
  vInteger() : Integer() {}

  vInteger& operator=(vInteger const& o) { _data = o._data; return *this; }

  vInteger* assign(const Base*X)const override;
  vInteger* assign(const Integer*X)const override;
  vInteger* assign(const Float*X)const override;
  vInteger* assign(const String*X)const override;

  vInteger* less(const Integer* X)const override	 { untested(); assert(X); return new vInteger((_data < X->_data)?1:0);}
  vInteger* greater(const Integer* X)const override	 { untested(); assert(X); return new vInteger((_data > X->_data)?1:0);}
  vInteger* leq(const Integer* X)const override	 { untested(); assert(X); return new vInteger((_data <= X->_data)?1:0);}
  vInteger* geq(const Integer* X)const override	 {itested(); assert(X); return new vInteger((_data >= X->_data)?1:0);}
  vInteger* not_equal(const Integer* X)const override { untested(); assert(X); return new vInteger((_data != X->_data)?1:0);}
  vInteger* equal(const Integer* X)const override	 { untested(); assert(X); return new vInteger((_data == X->_data)?1:0);}
  vInteger* add(const Integer* X)const override	 { untested(); assert(X); return new vInteger(_data + X->_data);}
  vInteger* multiply(const Integer* X)const override  { untested(); assert(X); return new vInteger(_data * X->_data);}
  vInteger* subtract(const Integer* X)const override  {untested(); assert(X); return new vInteger(_data - X->_data);}
  vInteger* r_subtract(const Integer* X)const override{itested();assert(X); return new vInteger(X->_data - _data);}
  vInteger* divide(const Integer* X)const override	 {untested(); assert(X); return new vInteger(_data / X->_data);}
  vInteger* r_divide(const Integer* X)const override  { assert(X); return new vInteger(X->_data / _data);}
  vInteger* modulo(const Integer* X)const override	 {untested(); assert(X); return new vInteger(_data % X->_data);}
  vInteger* r_modulo(const Integer* X)const override  { untested(); assert(X); return new vInteger(X->_data % _data);}

};
/*--------------------------------------------------------------------------*/
// Verilog "real"
// e.g. log is base 10
class vReal : public Float {
  using Float::less;
  using Float::greater;
  using Float::leq;
  using Float::geq;
  using Float::not_equal;
  using Float::equal;
  using Float::add;
  using Float::multiply;
  using Float::subtract;
  using Float::r_subtract;
  using Float::divide;
  using Float::r_divide;
  using Float::modulo;
  using Float::r_modulo;

public:
  vReal(double x=::NOT_INPUT) : Float(x) {}
  /*implicit*/ vReal(const vReal& p) : Float(p) {}
  explicit vReal(CS& file)		{untested();parse(file);}
  explicit vReal(const std::string& s)	{CS cs(CS::_STRING, s); parse(cs);}

  void dump(std::ostream& o)const override {itested();
    if (_data >= BIGBIG) { untested();
      o<<"inf";
    }else if (_data <= -BIGBIG) { untested();
      o<<"-inf";
    // }else if (_data==NOT_INPUT) { untested();
    //   o<<"NA";
    }else{ untested();
      o<<_data;
    }
  }

  vReal& operator=(vReal const& o) { Float::operator=(o); return *this; }

  vReal* assign(const Base*X)   const override { return new vReal(X?X->to_Float():Float());}
  vReal* assign(const Integer*X)const override {untested(); return new vReal(X?X->to_Float():Float());}
  vReal* assign(const Float*X)  const override {untested(); return new vReal(X?(*X):Float());}
  vReal* assign(const String*)  const override {untested(); return nullptr;}

  Integer* less(const Float* X)const override	{ untested();assert(X); return new Integer((_data < X->_data)?1:0);}
  Integer* greater(const Float* X)const override{ untested();assert(X); return new Integer((_data > X->_data)?1:0);}
  Integer* leq(const Float* X)const override	{ untested();assert(X); return new Integer((_data <= X->_data)?1:0);}
  Integer* geq(const Float* X)const override	{itested();assert(X); return new Integer((_data >= X->_data)?1:0);}
  Integer* not_equal(const Float* X)const override { untested();assert(X); return new Integer((_data != X->_data)?1:0);}
  Integer* equal(const Float* X)const override	{ untested();assert(X); return new Integer((_data == X->_data)?1:0);}
  Float* add(const Float* X)const override	{itested();assert(X); return new Float(_data + X->_data);}
  Float* multiply(const Float* X)const override	{itested();assert(X); return new Float(_data * X->_data);}
  Float* subtract(const Float* X)const override	{untested();assert(X); return new Float(_data - X->_data);}
  Float* r_subtract(const Float* X)const override{assert(X); return new Float(X->_data - _data);}
  Float* divide(const Float* X)const override	{untested();assert(X); return new Float(_data / X->_data);}
  Float* r_divide(const Float* X)const override	{assert(X); return new Float(X->_data / _data);}
  Float* modulo(const Float* X)const override	{untested();assert(X); return new Float(fmod(_data, X->_data));}
  Float* r_modulo(const Float* X)const override	{ untested();           assert(X); return new Float(fmod(X->_data, _data));}
};
/*--------------------------------------------------------------------------*/
inline vInteger* vInteger::assign(const Base*X)	 const { return new vInteger(X?Integer(X->to_Integer()):Integer());}
inline vInteger* vInteger::assign(const Integer*X)const {untested(); return new vInteger(X?*X:Integer());}
inline vInteger* vInteger::assign(const Float*X)  const {untested(); return new vInteger(X?Integer(X->to_Integer()):Integer());}
inline vInteger* vInteger::assign(const String*)  const {untested(); return nullptr;}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
