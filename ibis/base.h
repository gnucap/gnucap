/*$Id: base.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 */
#ifndef IBIS_BASE_H_INCLUDED
#define IBIS_BASE_H_INCLUDED
#include "l_lib.h"
#include "ap.h"
#include <fstream>
const double NA=.83262832e300;
/*--------------------------------------------------------------------------*/
inline void os_error(const std::string& name)
			{untested(); error(0, name + ':' + strerror(errno));}
inline void skip_tail(CS& file) {file.skipto1('\n');}
bool skip_comment(CS& file);
std::string new_name();
/*--------------------------------------------------------------------------*/
class Float;
class String;
class Vector;
/*--------------------------------------------------------------------------*/
class Base
{
private:
  explicit Base(const Base&) {unreachable();}
  // This private base copy constructor inhibits generation of
  // copy constructors for its derived classes.
protected:
  explicit Base() {}
public:
  virtual void parse(CS&) = 0;
  virtual void dump(std::ostream& o)const = 0;
  virtual ~Base() {}
  virtual std::string val_string()const {unreachable(); return "error";}
  virtual Base* minus()const {unreachable(); return 0;}
  virtual Base* invert()const {unreachable(); return 0;}
  virtual Base* less(const Base&)const {unreachable(); return 0;}
  virtual Base* less_rev(const Float&)const {unreachable(); return 0;}
  virtual Base* greater(const Base&)const {unreachable(); return 0;}
  virtual Base* greater_rev(const Float&)const {unreachable(); return 0;}
  virtual Base* not_equal(const Base&)const {unreachable(); return 0;}
  virtual Base* not_equal(const Float&)const {unreachable(); return 0;}
  virtual Base* not_equal(const String&)const {unreachable(); return 0;}
  virtual Base* equal(const Base&)const {unreachable(); return 0;}
  virtual Base* equal(const Float&)const {unreachable(); return 0;}
  virtual Base* equal(const String&)const {unreachable(); return 0;}
  virtual Base* add(const Base&)const {unreachable(); return 0;}
  virtual Base* add(const Float&)const {unreachable(); return 0;}
  virtual Base* add(const Vector&)const {unreachable(); return 0;}
  virtual Base* multiply(const Base&)const {unreachable(); return 0;}
  virtual Base* multiply(const Float&)const {unreachable(); return 0;}
  Base* subtract(const Base& x)const {return add(*(x.minus()));}
  Base* divide_by(const Base& d)const {return multiply(*(d.invert()));}
};
inline CS&	     operator>>(CS& f, Base& b)	{b.parse(f); return f;}
inline std::ostream& operator<<(std::ostream& out, const Base& d)
					{d.dump(out); return out;}
/*--------------------------------------------------------------------------*/
template <class T>
class List_Base
  :public Base
{
private:
  std::list<T*> _list;
protected:
  virtual void parse(CS& f) = 0;
  virtual void dump(std::ostream& o)const;
  virtual ~List_Base();
  explicit List_Base() {}
  explicit List_Base(const List_Base& p) : Base(), _list(p._list) {}
public:
  typedef typename std::list<T*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}
  const T*	 front()const	 {return _list.front();}
  const T*	 back()const	 {return _list.back();}
  void		 push(T* x)	 {assert(x); _list.push_back(x);}
  void		 push_back(T* x) {assert(x); _list.push_back(x);}
  void  	 pop_back()	 {_list.pop_back();}
  bool		 is_empty()const {return _list.empty();}
  int		 size()const	 {return _list.size();}
};
/*--------------------------------------------------------------------------*/
template <class T>
class List
  :public List_Base<T>
{
protected:
  explicit List() {}
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
template <class T>
class Collection
  :public List_Base<T>
{
protected:
  explicit Collection() {}
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
class String
  :public Base
{
protected:
  std::string _data;
private:
  void parse(CS&) {unreachable(); incomplete();}
  void dump(std::ostream& o)const {o << _data;}
public:
  explicit String(CS& file) {parse(file);}
  explicit String()	    {}
  explicit String(const std::string& s) :_data(s) {}
  operator const std::string&()const {return _data;}
  std::string val_string()const	   {return _data;}
  Base* not_equal(const String& x)const;
  Base* not_equal(const Float&)const   {untested(); return 0;}
  Base* not_equal(const Base& x)const  {return x.not_equal(*this);}
  Base* equal(const String& x)const;
  Base* equal(const Float&)const   {untested(); return 0;}
  Base* equal(const Base& x)const  {return x.equal(*this);}
};
/*--------------------------------------------------------------------------*/
class Name_String	// a string that contains only alnum and _[]
  :public String
{
private:
  void parse(CS&);
public:
  explicit Name_String(CS& file) {parse(file);}
  explicit Name_String()	    {}
};
/*--------------------------------------------------------------------------*/
class Quoted_String	// the first non-blank character is a quote
  :public String	// a repeat of the same character terminates it
{
private:
  void parse(CS&);
public:
  explicit Quoted_String(CS& file) {parse(file);}
  explicit Quoted_String()	    {}
};
/*--------------------------------------------------------------------------*/
class Tail_String	// a string that is parsed to the end of a line
  :public String
{
private:
  void parse(CS&);
public:
  explicit Tail_String(CS& file) {parse(file);}
  explicit Tail_String()	    {}
};
/*--------------------------------------------------------------------------*/
class Text_Block
  :public Base
{
private:
  std::string _data;
  void parse(CS&);
  void dump(std::ostream& o)const  {unreachable(); o << "?error?";}
public:
  explicit Text_Block() {}
  operator const std::string&()const {return _data;}
};
/*--------------------------------------------------------------------------*/
class Float
  :public Base
{
private:
  double _data;
  void dump(std::ostream& o)const {if (_data==NA) {o<<"NA";}else{o<<_data;}}
public:
  /*implicit*/ Float(const Float& p) :Base(), _data(p._data) {}
  explicit Float(CS& file) {parse(file);}
  explicit Float(const std::string& s) {CS cs(s); parse(cs);}
  Float(double x=NA) :_data(x) {}
  void parse(CS&);
  double value()const	 {return _data;}
  operator double()const {return _data;}
  std::string val_string()const	 {return ftos(_data, 0,6,0);}
  Base* minus()const {return new Float(-_data);}
  Base* invert()const {return new Float(1/_data);}
  Base* less_rev(const Float& x)const;
  Base* less(const Base& x)const  {return x.less_rev(*this);}
  Base* greater_rev(const Float& x)const;
  Base* greater(const Base& x)const  {return x.greater_rev(*this);}
  Base* not_equal(const Float& x)const;
  Base* not_equal(const Base& x)const  {return x.not_equal(*this);}
  Base* not_equal(const String&)const  {untested(); return 0;}
  Base* equal(const Float& x)const;
  Base* equal(const Base& x)const  {return x.equal(*this);}
  Base* equal(const String&)const  {untested(); return 0;}
  Base* add(const Vector&)const {incomplete(); return 0;}
  Base* add(const Float& x)const {return new Float(_data + x._data);}
  Base* add(const Base& x)const  {return x.add(*this);}
  Base* multiply(const Float& x)const {return new Float(_data * x._data);}
  Base* multiply(const Base& x)const  {return x.multiply(*this);}
  bool  is_NA()const {return _data == NA;}
};
/*--------------------------------------------------------------------------*/
class Fraction
  :public Base
{
private:
  Float _numerator;
  Float _denominator;

  void parse(CS& file);
  void dump(std::ostream& o)const {o << _numerator << '/' << _denominator;}
public:
  explicit Fraction(const Fraction& p)
    :Base(), _numerator(p._numerator), _denominator(p._denominator) {}
  //explicit Fraction(Float n, Float d=NA)
  //  : _numerator(n), _denominator(d) {untested();}
  Fraction(double n=NA, double d=NA)
    : _numerator(n), _denominator(d) {}
  bool operator==(double x) {
    return (_denominator == NA && _numerator == NA && x == NA)
      || (_denominator == 1 && _numerator == x)
      || (_numerator/_denominator == x);
  }
  double numerator()const   {return _numerator;}
  double denominator()const {return _denominator;}
  bool   is_NA()const {return _numerator.is_NA() || _denominator.is_NA();}
};
/*--------------------------------------------------------------------------*/
enum Typ_Min_Max {rTYP, rMIN, rMAX};
extern Typ_Min_Max dataset;
inline void set_typ() {dataset = rTYP;}
inline void set_min() {dataset = rMIN;}
inline void set_max() {dataset = rMAX;}

template <class T>
class Range
  :public Base
{
private:
  T _values[rMAX+1];
  void dump(std::ostream& o)const 
	{o << _values[rTYP] << ' ' << _values[rMIN] << ' ' << _values[rMAX];}
public:
  explicit Range() {assert(_values[rTYP]==NA); 
		    assert(_values[rMIN]==NA); assert(_values[rMAX]==NA);}
  explicit Range(T typ, T min = NA, T max = NA)
    	{_values[rTYP]=typ; _values[rMIN]=min; _values[rMAX]=max;}
  void parse(CS& file) {for (int i=0; i <= rMAX; ++i) {file >> _values[i];}}
  //bool is_NA()const	{untested(); return _typ==NA && _min==NA && _max==NA;}
  //bool exists()const	{untested(); return !is_NA();}
  const T& value_raw()const	{return _values[dataset];}
  const T& value_num()const 
	{return (!value_raw().is_NA() ? value_raw() : _values[rTYP]);}
};
/*--------------------------------------------------------------------------*/
class Vector_Item
  :public Base
{
private:
  Float  _x;
  Range<Float>  _y;
  //Float  _y;
  
  void parse(CS&);
  void dump(std::ostream& o)const {o << _x << '\t' << _y << '\n';}
public:
  explicit Vector_Item(CS& file) {parse(file);}
  explicit Vector_Item(double x, double y) :_x(x), _y(y) {}
  double x()const {return _x;}
  double y()const {return _y.value_raw();}
  //double y()const {return _y;}
};
/*--------------------------------------------------------------------------*/
class Vector
  :public List<Vector_Item>
{
public:
  std::string val_string()const	 {return new_name();}
  Base* minus()const;
  Base* invert()const;
  Base* add(const Vector&)const {incomplete(); return 0;}
  Base* add(const Float&)const;
  Base* add(const Base& x)const  {return x.add(*this);}
  Base* multiply(const Float&)const;
  Base* multiply(const Base& x)const  {return x.multiply(*this);}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
List_Base<T>::~List_Base()
{
  {for (typename std::list<T*>::iterator
	  i = _list.begin(); i != _list.end(); ++i) {
    assert(*i);
    delete *i;
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
void List_Base<T>::dump(std::ostream& out)const
{
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    out << **i;
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
void List<T>::parse(CS& file)
{
  skip_comment(file);
  int here = file.cursor();
  {for (;;) {
    {if (file.match1('[')) {
      break;
    }else{
      T* x = new T(file);
      {if (!file.stuck(&here)) {
	push(x);
      }else{
	delete x;
	file.warn(0, "what's this? (List)");
	break;
      }}
      skip_comment(file);
    }}
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
void Collection<T>::parse(CS& file)
{
  int here = file.cursor();
  T* m = new T(file);
  {if (!file.stuck(&here)) {
    push(m);
  }else{
    untested();
    delete m;
    file.warn(0, "what's this? (Collection)");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
