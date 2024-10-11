/*$Id: m_base.h,v 26.138 2013/04/24 02:32:27 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 */
//testing=script,sparse 2009.08.13
#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED
#include "l_lib.h"
#include "ap.h"
#include "constant.h"
/*--------------------------------------------------------------------------*/
class Integer;
class Float;
class String;
/*--------------------------------------------------------------------------*/
class Base {
private:
  explicit Base(const Base&) { untested();unreachable();}
  // This private base copy constructor inhibits generation of
  // copy constructors for its derived classes.
protected:
  explicit Base() {}
public:
  virtual void parse(CS&) = 0;
  virtual void dump(std::ostream& o)const = 0;
  virtual ~Base() {}
  virtual std::string val_string()const		{untested(); return "error";}
  virtual bool to_bool()const			{ untested();unreachable(); return false;}

  virtual Base* minus()const			{untested(); return nullptr;}
  virtual Base* plus()const			{untested(); return nullptr;}

  virtual Base* less(const Base*)const		{untested(); return nullptr;}
  virtual Base* less(const Integer*)const	{untested(); return nullptr;}
  virtual Base* less(const Float*)const		{untested(); return nullptr;}
  virtual Base* less(const String*)const	{untested(); return nullptr;}
  virtual Base* greater(const Base*)const	{untested(); return nullptr;}
  virtual Base* greater(const Integer*)const	{untested(); return nullptr;}
  virtual Base* greater(const Float*)const	{untested(); return nullptr;}
  virtual Base* greater(const String*)const	{untested(); return nullptr;}
  virtual Base* leq(const Base*)const		{untested(); return nullptr;}
  virtual Base* leq(const Integer*)const	{untested(); return nullptr;}
  virtual Base* leq(const Float*)const		{untested(); return nullptr;}
  virtual Base* leq(const String*)const 	{untested(); return nullptr;}
  virtual Base* geq(const Base*)const		{untested(); return nullptr;}
  virtual Base* geq(const Integer*)const	{untested(); return nullptr;}
  virtual Base* geq(const Float*)const		{untested(); return nullptr;}
  virtual Base* geq(const String*)const 	{untested(); return nullptr;}
  virtual Base* not_equal(const Base*)const	{untested(); return nullptr;}
  virtual Base* not_equal(const Integer*)const	{untested(); return nullptr;}
  virtual Base* not_equal(const Float*)const	{untested(); return nullptr;}
  virtual Base* not_equal(const String*)const	{untested(); return nullptr;}
  virtual Base* equal(const Base*)const		{untested(); return nullptr;}
  virtual Base* equal(const Integer*)const	{untested(); return nullptr;}
  virtual Base* equal(const Float*)const	{untested(); return nullptr;}
  virtual Base* equal(const String*)const	{untested(); return nullptr;}
  virtual Base* add(const Base*)const		{untested(); return nullptr;}
  virtual Base* add(const Integer*)const	{untested(); return nullptr;}
  virtual Base* add(const Float*)const		{untested(); return nullptr;}
  virtual Base* add(const String*)const		{untested(); return nullptr;}
  virtual Base* multiply(const Base*)const	{untested(); return nullptr;}
  virtual Base* multiply(const Integer*)const	{untested(); return nullptr;}
  virtual Base* multiply(const Float*)const	{untested(); return nullptr;}
  virtual Base* multiply(const String*)const	{untested(); return nullptr;}
  virtual Base* subtract(const Base*)const	{untested(); return nullptr;}
  virtual Base* subtract(const Integer*)const	{untested(); return nullptr;}
  virtual Base* subtract(const Float*)const	{untested(); return nullptr;}
  virtual Base* subtract(const String*)const	{untested(); return nullptr;}
  virtual Base* r_subtract(const Base*)const	{untested(); return nullptr;}
  virtual Base* r_subtract(const Integer*)const	{untested(); return nullptr;}
  virtual Base* r_subtract(const Float*)const	{untested(); return nullptr;}
  virtual Base* r_subtract(const String*)const	{untested(); return nullptr;}
  virtual Base* divide(const Base*)const	{untested(); return nullptr;}
  virtual Base* divide(const Integer*)const	{untested(); return nullptr;}
  virtual Base* divide(const Float*)const	{untested(); return nullptr;}
  virtual Base* divide(const String*)const	{untested(); return nullptr;}
  virtual Base* r_divide(const Base*)const	{untested(); return nullptr;}
  virtual Base* r_divide(const Integer*)const	{untested(); return nullptr;}
  virtual Base* r_divide(const Float*)const	{untested(); return nullptr;}
  virtual Base* r_divide(const String*)const	{untested(); return nullptr;}
  virtual Base* modulo(const Base*)const	{untested(); return nullptr;}
  virtual Base* modulo(const Integer*)const	{untested(); return nullptr;}
  virtual Base* modulo(const Float*)const	{untested(); return nullptr;}
  virtual Base* modulo(const String*)const	{untested(); return nullptr;}
  virtual Base* r_modulo(const Base*)const	{untested(); return nullptr;}
  virtual Base* r_modulo(const Integer*)const	{untested(); return nullptr;}
  virtual Base* r_modulo(const Float*)const	{untested(); return nullptr;}
  virtual Base* r_modulo(const String*)const	{untested(); return nullptr;}

  Base* logic_not()const;
  Base* logic_or(const Base* X)const;
  Base* logic_and(const Base* X)const;
};
inline CS&	     operator>>(CS& f, Base& b)	{itested();b.parse(f); return f;}
inline std::ostream& operator<<(std::ostream& out, const Base& d)
					{d.dump(out); return out;}
/*--------------------------------------------------------------------------*/
template <class T>
class List_Base :public Base {
private:
  std::list<T*> _list;
public:
  virtual void parse(CS& f) override = 0;
protected:
  virtual void dump(std::ostream& o)const override;
  virtual ~List_Base();
  explicit List_Base() {}
  explicit List_Base(const List_Base& p) : Base(), _list(p._list) {untested();}
public:
  typedef typename std::list<T*>::const_iterator const_iterator;
  bool		 is_empty()const {return _list.empty();}
  size_t	 size()const	 {return _list.size();}
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}
  const T*	 front()const	 {untested();assert(!is_empty()); return _list.front();}
  const T*	 back()const	 {assert(!is_empty()); return _list.back();}
  T*		 back()		 {assert(!is_empty()); return _list.back();}
  void		 push_back(T* x) {assert(x);	       _list.push_back(x);}
  void  	 pop_back()	 {assert(!is_empty()); _list.pop_back();}
  const_iterator erase(const_iterator i) { itested();
    delete *i;
    return _list.erase(i);
  }
};
/*--------------------------------------------------------------------------*/
template <class T>
class List :public List_Base<T> {
protected:
  explicit List() {untested();}
public:
  void parse(CS& f) override;
};
/*--------------------------------------------------------------------------*/
#if 0
template <class T>
class Collection
  :public List_Base<T>
{ untested();
protected:
  explicit Collection() {untested();}
public:
  void parse(CS& f);
};
#endif
/*--------------------------------------------------------------------------*/
class Integer : public Base {
private:
  friend class Float;
  int32_t _data;
  bool _input;
  void dump(std::ostream& o)const override {itested();
    if (!_input) { untested();
      o << "NA";
    }else{ untested();
      o << _data;
    }
  }
public:
  /*implicit*/ Integer(const Integer& p) :Base(), _data(p._data), _input(p._input) { untested();}
  explicit Integer(CS& file)		{untested();parse(file);}
  explicit Integer(const std::string& s)	{CS cs(CS::_STRING, s); parse(cs);}
  Integer(int32_t x) :_data(x), _input(true) {}
  Integer() : _input(false) { untested();}
  void parse(CS&) override;
  int32_t value()const			{return _data;}
  operator int32_t()const		{untested();return _data;}
  std::string val_string()const override{return " "+std::to_string(_data);}
  bool to_bool()const override		{return (_input && _data != 0);}

  Base* minus()const override		{return new Integer(-_data);}
  Base* plus()const override		{return new Integer(_data);}

  Integer* less(const Integer* X)const override	 { assert(X); return new Integer((_data < X->_data)?1:0);}
  Integer* greater(const Integer* X)const override	 { assert(X); return new Integer((_data > X->_data)?1:0);}
  Integer* leq(const Integer* X)const override	 { assert(X); return new Integer((_data <= X->_data)?1:0);}
  Integer* geq(const Integer* X)const override	 { assert(X); return new Integer((_data >= X->_data)?1:0);}
  Integer* not_equal(const Integer* X)const override{ assert(X); return new Integer((_data != X->_data)?1:0);}
  Integer* equal(const Integer* X)const override	 { assert(X); return new Integer((_data == X->_data)?1:0);}
  Integer* add(const Integer* X)const override	 { assert(X); return new Integer(_data + X->_data);}
  Integer* multiply(const Integer* X)const override { assert(X); return new Integer(_data * X->_data);}
  Integer* subtract(const Integer* X)const override {untested(); assert(X); return new Integer(_data - X->_data);}
  Integer* r_subtract(const Integer* X)const override{assert(X); return new Integer(X->_data - _data);}
  Integer* divide(const Integer* X)const override	 {untested(); assert(X); return new Integer(_data / X->_data);}
  Integer* r_divide(const Integer* X)const override { assert(X); return new Integer(X->_data / _data);}
  Integer* modulo(const Integer* X)const override	 {untested(); assert(X); return new Integer(_data % X->_data);}
  Integer* r_modulo(const Integer* X)const override { assert(X); return new Integer(X->_data % _data);}

  Base* less(const Float* X)const override;
  Base* greater(const Float* X)const override;
  Base* leq(const Float* X)const override;
  Base* geq(const Float* X)const override;
  Base* not_equal(const Float* X)const override;
  Base* equal(const Float* X)const override;
  Base* add(const Float* X)const override;
  Base* multiply(const Float* X)const override;
  Base* subtract(const Float* X)const override;
  Base* r_subtract(const Float* X)const override;
  Base* divide(const Float* X)const override;
  Base* r_divide(const Float* X)const override;
  Base* modulo(const Float* X)const override;
  Base* r_modulo(const Float* X)const override;
														 //
  Base* less(const Base* X)const override	{return ((X) ? (X->greater(this))   : (nullptr));}
  Base* greater(const Base* X)const override	{return ((X) ? (X->less(this))      : (nullptr));}
  Base* leq(const Base* X)const override	{return ((X) ? (X->geq(this))       : (nullptr));}
  Base* geq(const Base* X)const override	{return ((X) ? (X->leq(this))       : (nullptr));}
  Base* not_equal(const Base* X)const override	{return ((X) ? (X->not_equal(this)) : (nullptr));}
  Base* equal(const Base* X)const override	{return ((X) ? (X->equal(this))     : (nullptr));}
  Base* add(const Base* X)const override	{return ((X) ? (X->add(this))       : (nullptr));}
  Base* multiply(const Base* X)const override	{return ((X) ? (X->multiply(this))  : (nullptr));}
  Base* subtract(const Base* X)const override	{return ((X) ? (X->r_subtract(this)): (nullptr));}
  Base* r_subtract(const Base* X)const override	{untested();return ((X) ? (X->subtract(this))  : (nullptr));}
  Base* divide(const Base* X)const override	{return ((X) ? (X->r_divide(this))  : (nullptr));}
  Base* r_divide(const Base* X)const override	{untested();return ((X) ? (X->divide(this))    : (nullptr));}
  Base* modulo(const Base* X)const override	{return ((X) ? (X->r_modulo(this))  : (nullptr));}
  Base* r_modulo(const Base* X)const override	{untested();return ((X) ? (X->modulo(this))    : (nullptr));}

  Base* less(const String*)const override	{untested();return nullptr;}
  Base* greater(const String*)const override	{untested();return nullptr;}
  Base* leq(const String*)const override 	{untested();return nullptr;}
  Base* geq(const String*)const override 	{untested();return nullptr;}
  Base* not_equal(const String*)const override	{return nullptr;}
  Base* equal(const String*)const override	{return nullptr;}
  Base* add(const String*)const override 	{return nullptr;}
  Base* multiply(const String*)const override	{untested();return nullptr;}
  Base* subtract(const String*)const override	{untested();return nullptr;}
  Base* r_subtract(const String*)const override	{return nullptr;}
  Base* divide(const String*)const override	{untested();return nullptr;}
  Base* r_divide(const String*)const override	{untested();return nullptr;}
  Base* modulo(const String*)const override	{untested();return nullptr;}
  Base* r_modulo(const String*)const override	{untested();return nullptr;}

  bool  is_NA()const			{untested();return !_input;}
};
/*--------------------------------------------------------------------------*/
class Float : public Base {
private:
  friend class Integer;
  double _data;
  void dump(std::ostream& o)const override {itested();
    if (_data==NOT_INPUT) {untested();
      o<<"NA";
    }else{untested();
      o<<_data;
    }
  }
public:
  /*implicit*/ Float(const Float& p) :Base(), _data(p._data) { untested();}
  explicit Float(CS& file)		{untested();parse(file);}
  explicit Float(const std::string& s)	{CS cs(CS::_STRING, s); parse(cs);}
  Float(double x=NOT_INPUT) :_data(x) {}
  void parse(CS&) override;
  double value()const			{return _data;}
  operator double()const		{untested();return _data;}
  std::string val_string()const override{return ftos(_data, 0, 15, ftos_EXP);}
  bool to_bool()const override		{return (_data != 0.);}

  Base* minus()const override		{return new Float(-_data);}
  Base* plus()const override		{ untested();return new Float(_data);}

  Integer* less(const Float* X)const override	{ untested();assert(X); return new Integer((_data < X->_data)?1:0);}
  Integer* greater(const Float* X)const override	{ untested();assert(X); return new Integer((_data > X->_data)?1:0);}
  Integer* leq(const Float* X)const override	{ untested();assert(X); return new Integer((_data <= X->_data)?1:0);}
  Integer* geq(const Float* X)const override	{ untested();assert(X); return new Integer((_data >= X->_data)?1:0);}
  Integer* not_equal(const Float* X)const override	{assert(X); return new Integer((_data != X->_data)?1:0);}
  Integer* equal(const Float* X)const override	{ untested();assert(X); return new Integer((_data == X->_data)?1:0);}
  Float* add(const Float* X)const override	{assert(X); return new Float(_data + X->_data);}
  Float* multiply(const Float* X)const override	{assert(X); return new Float(_data * X->_data);}
  Float* subtract(const Float* X)const override	{untested();assert(X); return new Float(_data - X->_data);}
  Float* r_subtract(const Float* X)const override{assert(X); return new Float(X->_data - _data);}
  Float* divide(const Float* X)const override	{untested();assert(X); return new Float(_data / X->_data);}
  Float* r_divide(const Float* X)const override	{assert(X); return new Float(X->_data / _data);}
  Float* modulo(const Float* X)const override	{untested();assert(X); return new Float(fmod(_data, X->_data));}
  Float* r_modulo(const Float* X)const override	{ untested();           assert(X); return new Float(fmod(X->_data, _data));}

  Base* less(const Integer* X)const override	 {untested(); assert(X); return X->greater(this);}
  Base* greater(const Integer* X)const override	 {untested(); assert(X); return X->less(this);}
  Base* leq(const Integer* X)const override	 {untested(); assert(X); return X->geq(this);}
  Base* geq(const Integer* X)const override	 {untested(); assert(X); return X->leq(this);}
  Base* not_equal(const Integer* X)const override{untested(); assert(X); return new Integer(X->_data != _data);}
  Base* equal(const Integer* X)const override	 {untested(); assert(X); return new Integer(X->_data == _data);}
  Base* add(const Integer* X)const override	 { assert(X); return new Float(X->_data + _data);}
  Base* multiply(const Integer* X)const override { assert(X); return new Float(X->_data * _data);}
  Base* subtract(const Integer* X)const override { assert(X); return new Float(_data - X->_data);}
  Base* r_subtract(const Integer* X)const override{assert(X); return new Float(X->_data - _data);}
  Base* divide(const Integer* X)const override	 { assert(X); return new Float(_data / X->_data);} // compliant?
  Base* r_divide(const Integer* X)const override { assert(X); return new Float(X->_data / _data);} // compliant?
  Base* modulo(const Integer* X)const override	 {untested(); assert(X); return new Float(fmod(_data, X->_data));} // compliant?
  Base* r_modulo(const Integer* X)const override {untested(); assert(X); return new Float(fmod(_data, X->_data));} // compliant?

  Base* less(const Base* X)const override	{ untested();return ((X) ? (X->greater(this))   : (nullptr));}
  Base* greater(const Base* X)const override	{return ((X) ? (X->less(this))      : (nullptr));}
  Base* leq(const Base* X)const override	{ untested();return ((X) ? (X->geq(this))       : (nullptr));}
  Base* geq(const Base* X)const override	{return ((X) ? (X->leq(this))       : (nullptr));}
  Base* not_equal(const Base* X)const override	{return ((X) ? (X->not_equal(this)) : (nullptr));}
  Base* equal(const Base* X)const override	{return ((X) ? (X->equal(this))	    : (nullptr));}
  Base* add(const Base* X)const override	{return ((X) ? (X->add(this))       : (nullptr));}
  Base* multiply(const Base* X)const override	{return ((X) ? (X->multiply(this))  : (nullptr));}
  Base* subtract(const Base* X)const override	{return ((X) ? (X->r_subtract(this)): (nullptr));}
  Base* r_subtract(const Base* X)const override	{untested();return ((X) ? (X->subtract(this))  : (nullptr));}
  Base* divide(const Base* X)const override	{return ((X) ? (X->r_divide(this))  : (nullptr));}
  Base* r_divide(const Base* X)const override	{untested();return ((X) ? (X->divide(this))    : (nullptr));}
  Base* modulo(const Base* X)const override	{ untested();return ((X) ? (X->r_modulo(this))  : (nullptr));}
  Base* r_modulo(const Base* X)const override	{untested();return ((X) ? (X->modulo(this))    : (nullptr));}

  Base* less(const String*)const override	{untested();return nullptr;}
  Base* greater(const String*)const override	{untested();return nullptr;}
  Base* leq(const String*)const override 	{untested();return nullptr;}
  Base* geq(const String*)const override 	{untested();return nullptr;}
  Base* not_equal(const String*)const override	{return nullptr;} //?
  Base* equal(const String*)const override	{return nullptr;} //?
  Base* add(const String*)const override 	{           return nullptr;}
  Base* multiply(const String*)const override	{untested();return nullptr;}
  Base* subtract(const String*)const override	{untested();return nullptr;}
  Base* r_subtract(const String*)const override	{ untested();           return nullptr;}
  Base* divide(const String*)const override	{untested();return nullptr;}
  Base* r_divide(const String*)const override	{ untested();	    return nullptr;}
  Base* modulo(const String*)const override	{untested();return nullptr;}
  Base* r_modulo(const String*)const override	{untested();return nullptr;}

  bool  is_NA()const			{untested();return _data == NOT_INPUT;}
}; // Float
/*--------------------------------------------------------------------------*/
inline Base* Integer::less      (const Float* X) const { assert(X); return new Integer((_data < X->_data)?1:0);}
inline Base* Integer::greater   (const Float* X) const {untested(); assert(X); return new Integer((_data > X->_data)?1:0);}
inline Base* Integer::leq       (const Float* X) const { assert(X); return new Integer((_data <= X->_data)?1:0);}
inline Base* Integer::geq       (const Float* X) const {untested(); assert(X); return new Integer((_data >= X->_data)?1:0);}
inline Base* Integer::not_equal (const Float* X) const {untested(); assert(X); return new Integer((_data != X->_data)?1:0);}
inline Base* Integer::equal     (const Float* X) const {untested(); assert(X); return new Integer((_data == X->_data)?1:0);}
inline Base* Integer::add       (const Float* X) const { assert(X); return X? X->add(this) : nullptr;}
inline Base* Integer::multiply  (const Float* X) const { assert(X); return X? X->multiply(this) : nullptr;}
inline Base* Integer::subtract  (const Float* X) const {untested(); assert(X); return X? X->r_subtract(this) : nullptr;}
inline Base* Integer::r_subtract(const Float* X) const { assert(X); return X? X->subtract(this) : nullptr;}
inline Base* Integer::divide    (const Float* X) const {untested(); assert(X); return X? X->r_divide(this) : nullptr;}
inline Base* Integer::r_divide  (const Float* X) const { assert(X); return X? X->divide(this) : nullptr;}
inline Base* Integer::modulo    (const Float* X) const {untested(); assert(X); return X? X->r_modulo(this) : nullptr;}
inline Base* Integer::r_modulo  (const Float* X) const {untested(); assert(X); return X? X->modulo(this) : nullptr;}
/*--------------------------------------------------------------------------*/
class String : public Base {
protected:
  std::string _data;
public:
  void parse(CS&) override { untested();unreachable(); incomplete();}
private:
  void dump(std::ostream& o)const override { untested();o << _data;}
public:
  explicit String(CS& file) {untested();parse(file);}
  explicit String()	    {}
  explicit String(const std::string& s) :_data(s) {}
  explicit String(const String& s) : Base(), _data(s._data) {}
  operator const std::string&()const	{return _data;}
  std::string val_string()const override		{return _data;}
  bool to_bool()const override			{untested();return (_data != "");}

  Base* minus()const override			{untested(); return nullptr;}
  Base* plus()const override			{untested(); return nullptr;}

  Base* less(const String* X)const override	{untested();assert(X); return new Float((_data < X->_data)?1.:0.);}
  Base* greater(const String* X)const override	{untested();assert(X); return new Float((_data > X->_data)?1.:0.);}
  Base* leq(const String* X)const override	{untested();assert(X); return new Float((_data <= X->_data)?1.:0.);}
  Base* geq(const String* X)const override	{untested();assert(X); return new Float((_data >= X->_data)?1.:0.);}
  Base* not_equal(const String* X)const override{untested();assert(X); return new Float((_data != X->_data)?1.:0.);}
  Base* equal(const String* X)const override	{assert(X); return new Float((_data == X->_data)?1.:0.);}
  Base* add(const String*)const override	{	    return nullptr;}
  Base* multiply(const String*)const override	{           return nullptr;}
  Base* subtract(const String*)const override	{untested(); return nullptr;}
  Base* r_subtract(const String*)const override	{itested(); return nullptr;}
  Base* divide(const String*)const override	{untested(); return nullptr;}
  Base* r_divide(const String*)const override	{itested(); return nullptr;}
  Base* modulo(const String*)const override	{untested(); return nullptr;}
  Base* r_modulo(const String*)const override	{untested(); return nullptr;}

  Base* less(const Base* X)const override	{untested();return ((X) ? (X->greater(this))   : (nullptr));}
  Base* greater(const Base* X)const override	{untested();return ((X) ? (X->less(this))      : (nullptr));}
  Base* leq(const Base* X)const override	{untested();return ((X) ? (X->geq(this))       : (nullptr));}
  Base* geq(const Base* X)const override	{untested();return ((X) ? (X->leq(this))       : (nullptr));}
  Base* not_equal(const Base* X)const override	{           return ((X) ? (X->not_equal(this)) : (nullptr));}
  Base* equal(const Base* X)const override	{           return ((X) ? (X->equal(this))     : (nullptr));}
  Base* add(const Base* X)const override 	{	    return ((X) ? (X->add(this))       : (nullptr));}
  Base* multiply(const Base* X)const override	{           return ((X) ? (X->multiply(this))  : (nullptr));}
  Base* subtract(const Base* X)const override	{           return ((X) ? (X->r_subtract(this)): (nullptr));}
  Base* r_subtract(const Base* X)const override	{untested();return ((X) ? (X->subtract(this))  : (nullptr));}
  Base* divide(const Base* X)const override	{ untested();	    return ((X) ? (X->r_divide(this))  : (nullptr));}
  Base* r_divide(const Base* X)const override	{untested();return ((X) ? (X->divide(this))    : (nullptr));}
  Base* modulo(const Base* X)const override	{ untested();	    return ((X) ? (X->r_modulo(this))  : (nullptr));}
  Base* r_modulo(const Base* X)const override	{untested();return ((X) ? (X->modulo(this))    : (nullptr));}

  Base* less(const Float*)const override 	{untested();return nullptr;}
  Base* greater(const Float*)const override	{untested();return nullptr;}
  Base* leq(const Float*)const override 	{untested();return nullptr;}
  Base* geq(const Float*)const override		{untested();return nullptr;}
  Base* not_equal(const Float*)const override	{untested();return nullptr;}
  Base* equal(const Float*)const override	{return nullptr;}
  Base* add(const Float*)const override 	{           return nullptr;}
  Base* multiply(const Float*)const override	{untested();return nullptr;}
  Base* subtract(const Float*)const override	{untested();return nullptr;}
  Base* r_subtract(const Float*)const override	{untested();return nullptr;}
  Base* divide(const Float*)const override	{untested();return nullptr;}
  Base* r_divide(const Float*)const override	{ itested();return nullptr;}
  Base* modulo(const Float*)const override	{untested();return nullptr;}
  Base* r_modulo(const Float*)const override	{untested();return nullptr;}

  Base* less      (const Integer*)const override {untested();return nullptr;}
  Base* greater   (const Integer*)const override {untested();return nullptr;}
  Base* leq       (const Integer*)const override {untested();return nullptr;}
  Base* geq       (const Integer*)const override {untested();return nullptr;}
  Base* not_equal (const Integer*)const override {untested();return nullptr;} //?
  Base* equal     (const Integer*)const override {untested();return nullptr;} //?
  Base* add       (const Integer*)const override {return nullptr;}
  Base* multiply  (const Integer*)const override {untested(); incomplete(); return nullptr;}
  Base* subtract  (const Integer*)const override {untested();return nullptr;}
  Base* r_subtract(const Integer*)const override {untested();return nullptr;}
  Base* divide    (const Integer*)const override {untested();return nullptr;}
  Base* r_divide  (const Integer*)const override {return nullptr;}
  Base* modulo    (const Integer*)const override {untested();return nullptr;}
  Base* r_modulo  (const Integer*)const override {untested();return nullptr;}
}; // String
/*--------------------------------------------------------------------------*/
// a string that contains only alnum and _[]
class Name_String : public String {
public:
  void parse(CS&) override;
public:
  explicit Name_String(CS& file)	{parse(file);}
  explicit Name_String()		{untested();}
};
/*--------------------------------------------------------------------------*/
// <alphanumeric> string
class Angled_String : public String {
public:
  void parse(CS&) override;
public:
  explicit Angled_String(CS& file)	{ untested();parse(file);}
  explicit Angled_String()		{ untested();}
};
/*--------------------------------------------------------------------------*/
// the first non-blank character is a quote
// a repeat of the same character terminates it
class Quoted_String : public String {
public:
  void parse(CS&) override;
public:
  explicit Quoted_String(CS& file)	{parse(file);}
  explicit Quoted_String()		{ untested();}
};
/*--------------------------------------------------------------------------*/
// a string that is parsed to the end of a line
class Tail_String : public String {
public:
  void parse(CS&) override;
  explicit Tail_String(CS& file)	{untested();parse(file);}
  explicit Tail_String()		{untested();}
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
List_Base<T>::~List_Base()
{
  for (typename std::list<T*>::iterator
	  i = _list.begin(); i != _list.end(); ++i) {
    assert(*i);
    delete *i;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void List_Base<T>::dump(std::ostream& Out)const
{itested();
  for (const_iterator i = begin(); i != end(); ++i) {itested();
    assert(*i);
    Out << **i;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void List<T>::parse(CS& File)
{untested();
  //skip_comment(File);
  size_t here = File.cursor();
  for (;;) {untested();
    if (File.match1('[')) {untested();
      break;
    }else{untested();
      T* x = new T(File);
      if (!File.stuck(&here)) {untested();
	List_Base<T>::push_back(x);
      }else{untested();
	delete x;
	File.warn(0, "what's this? (List)");
	break;
      }
      //skip_comment(File);
    }
  }
}
/*--------------------------------------------------------------------------*/
#if 0
template <class T>
void Collection<T>::parse(CS& File)
{untested();
  size_t here = File.cursor();
  T* m = new T(File);
  if (!File.stuck(&here)) {untested();
    push(m);
  }else{untested();
    delete m;
    File.warn(0, "what's this? (Collection)");
  }
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
