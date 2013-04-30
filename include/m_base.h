/*$Id: m_base.h,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
class Float;
class String;
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
  virtual std::string val_string()const		{untested(); return "error";}
  virtual bool to_bool()const			{unreachable(); return false;}

  virtual Base* minus()const			{untested(); return NULL;}
  virtual Base* plus()const			{untested(); return NULL;}

  virtual Base* less(const Base*)const		{untested(); return NULL;}
  virtual Base* less(const Float*)const		{untested(); return NULL;}
  virtual Base* less(const String*)const	{untested(); return NULL;}
  virtual Base* greater(const Base*)const	{untested(); return NULL;}
  virtual Base* greater(const Float*)const	{untested(); return NULL;}
  virtual Base* greater(const String*)const	{untested(); return NULL;}
  virtual Base* leq(const Base*)const		{untested(); return NULL;}
  virtual Base* leq(const Float*)const		{untested(); return NULL;}
  virtual Base* leq(const String*)const 	{untested(); return NULL;}
  virtual Base* geq(const Base*)const		{untested(); return NULL;}
  virtual Base* geq(const Float*)const		{untested(); return NULL;}
  virtual Base* geq(const String*)const 	{untested(); return NULL;}
  virtual Base* not_equal(const Base*)const	{untested(); return NULL;}
  virtual Base* not_equal(const Float*)const	{untested(); return NULL;}
  virtual Base* not_equal(const String*)const	{untested(); return NULL;}
  virtual Base* equal(const Base*)const		{untested(); return NULL;}
  virtual Base* equal(const Float*)const	{untested(); return NULL;}
  virtual Base* equal(const String*)const	{untested(); return NULL;}
  virtual Base* add(const Base*)const		{untested(); return NULL;}
  virtual Base* add(const Float*)const		{untested(); return NULL;}
  virtual Base* add(const String*)const		{untested(); return NULL;}
  virtual Base* multiply(const Base*)const	{untested(); return NULL;}
  virtual Base* multiply(const Float*)const	{untested(); return NULL;}
  virtual Base* multiply(const String*)const	{untested(); return NULL;}
  virtual Base* subtract(const Base*)const	{untested(); return NULL;}
  virtual Base* subtract(const Float*)const	{untested(); return NULL;}
  virtual Base* subtract(const String*)const	{untested(); return NULL;}
  virtual Base* r_subtract(const Base*)const	{untested(); return NULL;}
  virtual Base* r_subtract(const Float*)const	{untested(); return NULL;}
  virtual Base* r_subtract(const String*)const	{untested(); return NULL;}
  virtual Base* divide(const Base*)const	{untested(); return NULL;}
  virtual Base* divide(const Float*)const	{untested(); return NULL;}
  virtual Base* divide(const String*)const	{untested(); return NULL;}
  virtual Base* r_divide(const Base*)const	{untested(); return NULL;}
  virtual Base* r_divide(const Float*)const	{untested(); return NULL;}
  virtual Base* r_divide(const String*)const	{untested(); return NULL;}

  Base* logic_not()const;
  Base* logic_or(const Base* X)const;
  Base* logic_and(const Base* X)const;
};
inline CS&	     operator>>(CS& f, Base& b)	{untested();b.parse(f); return f;}
inline std::ostream& operator<<(std::ostream& out, const Base& d)
					{d.dump(out); return out;}
/*--------------------------------------------------------------------------*/
template <class T>
class List_Base
  :public Base
{
private:
  std::list<T*> _list;
public:
  virtual void parse(CS& f) = 0;
protected:
  virtual void dump(std::ostream& o)const;
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
};
/*--------------------------------------------------------------------------*/
template <class T>
class List
  :public List_Base<T>
{
protected:
  explicit List() {untested();}
public:
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
#if 0
template <class T>
class Collection
  :public List_Base<T>
{
protected:
  explicit Collection() {untested();}
public:
  void parse(CS& f);
};
#endif
/*--------------------------------------------------------------------------*/
class Float
  :public Base
{
private:
  double _data;
  void dump(std::ostream& o)const {itested();
    if (_data==NOT_INPUT) {untested();
      o<<"NA";
    }else{itested();
      o<<_data;
    }
  }
public:
  /*implicit*/ Float(const Float& p) :Base(), _data(p._data) {untested();}
  explicit Float(CS& file)		{untested();parse(file);}
  explicit Float(const std::string& s)	{CS cs(CS::_STRING, s); parse(cs);}
  Float(double x=NOT_INPUT) :_data(x) {}
  void parse(CS&);
  double value()const			{return _data;}
  operator double()const		{untested();return _data;}
  std::string val_string()const		{return ftos(_data, 0, 15, ftos_EXP);}
  bool to_bool()const			{return (_data != 0);}

  Base* minus()const			{return new Float(-_data);}
  Base* plus()const			{return new Float(_data);}

  Base* less(const Float* X)const	{assert(X); return new Float((_data < X->_data)?1.:0.);}
  Base* greater(const Float* X)const	{assert(X); return new Float((_data > X->_data)?1.:0.);}
  Base* leq(const Float* X)const	{assert(X); return new Float((_data <= X->_data)?1.:0.);}
  Base* geq(const Float* X)const	{assert(X); return new Float((_data >= X->_data)?1.:0.);}
  Base* not_equal(const Float* X)const	{assert(X); return new Float((_data != X->_data)?1.:0.);}
  Base* equal(const Float* X)const	{assert(X); return new Float((_data == X->_data)?1.:0.);}
  Base* add(const Float* X)const	{assert(X); return new Float(_data + X->_data);}
  Base* multiply(const Float* X)const	{assert(X); return new Float(_data * X->_data);}
  Base* subtract(const Float* X)const	{untested();assert(X); return new Float(_data - X->_data);}
  Base* r_subtract(const Float* X)const	{assert(X); return new Float(X->_data - _data);}
  Base* divide(const Float* X)const	{untested();assert(X); return new Float(_data / X->_data);}
  Base* r_divide(const Float* X)const	{assert(X); return new Float(X->_data / _data);}

  Base* less(const Base* X)const	{return ((X) ? (X->greater(this))   : (NULL));}
  Base* greater(const Base* X)const	{return ((X) ? (X->less(this))      : (NULL));}
  Base* leq(const Base* X)const		{return ((X) ? (X->geq(this))       : (NULL));}
  Base* geq(const Base* X)const		{return ((X) ? (X->leq(this))       : (NULL));}
  Base* not_equal(const Base* X)const	{return ((X) ? (X->not_equal(this)) : (NULL));}
  Base* equal(const Base* X)const	{return ((X) ? (X->equal(this))	    : (NULL));}
  Base* add(const Base* X)const 	{return ((X) ? (X->add(this))       : (NULL));}
  Base* multiply(const Base* X)const	{return ((X) ? (X->multiply(this))  : (NULL));}
  Base* subtract(const Base* X)const	{return ((X) ? (X->r_subtract(this)): (NULL));}
  Base* r_subtract(const Base* X)const	{untested();return ((X) ? (X->subtract(this))  : (NULL));}
  Base* divide(const Base* X)const	{return ((X) ? (X->r_divide(this))  : (NULL));}
  Base* r_divide(const Base* X)const	{untested();return ((X) ? (X->divide(this))    : (NULL));}

  Base* less(const String*)const	{untested();return NULL;}
  Base* greater(const String*)const	{untested();return NULL;}
  Base* leq(const String*)const 	{untested();return NULL;}
  Base* geq(const String*)const 	{untested();return NULL;}
  Base* not_equal(const String*)const	{untested();return NULL;}
  Base* equal(const String*)const	{untested();return NULL;}
  Base* add(const String*)const 	{untested();return NULL;}
  Base* multiply(const String*)const	{untested();return NULL;}
  Base* subtract(const String*)const	{untested();return NULL;}
  Base* r_subtract(const String*)const	{untested();return NULL;}
  Base* divide(const String*)const	{untested();return NULL;}
  Base* r_divide(const String*)const	{	    return NULL;}

  bool  is_NA()const			{untested();return _data == NOT_INPUT;}
};
/*--------------------------------------------------------------------------*/
class String
  :public Base
{
protected:
  std::string _data;
public:
  void parse(CS&) {unreachable(); incomplete();}
private:
  void dump(std::ostream& o)const {untested();o << _data;}
public:
  explicit String(CS& file) {untested();parse(file);}
  explicit String()	    {}
  explicit String(const std::string& s) :_data(s) {}
  operator const std::string&()const	{return _data;}
  std::string val_string()const		{untested();return _data;}
  bool to_bool()const			{untested();return (_data != "");}

  Base* minus()const			{untested(); return NULL;}
  Base* plus()const			{untested(); return NULL;}

  Base* less(const String* X)const	{untested();assert(X); return new Float((_data < X->_data)?1.:0.);}
  Base* greater(const String* X)const	{untested();assert(X); return new Float((_data > X->_data)?1.:0.);}
  Base* leq(const String* X)const	{untested();assert(X); return new Float((_data <= X->_data)?1.:0.);}
  Base* geq(const String* X)const	{untested();assert(X); return new Float((_data >= X->_data)?1.:0.);}
  Base* not_equal(const String* X)const	{untested();assert(X); return new Float((_data != X->_data)?1.:0.);}
  Base* equal(const String* X)const	{untested();assert(X); return new Float((_data == X->_data)?1.:0.);}
  Base* add(const String* X)const	{untested();assert(X); return new String(_data + X->_data);}
  Base* multiply(const String*)const	{untested(); return NULL;}
  Base* subtract(const String*)const	{untested(); return NULL;}
  Base* r_subtract(const String*)const	{untested(); return NULL;}
  Base* divide(const String*)const	{untested(); return NULL;}
  Base* r_divide(const String*)const	{untested(); return NULL;}

  Base* less(const Base* X)const	{untested();return ((X) ? (X->greater(this))   : (NULL));}
  Base* greater(const Base* X)const	{untested();return ((X) ? (X->less(this))      : (NULL));}
  Base* leq(const Base* X)const		{untested();return ((X) ? (X->geq(this))       : (NULL));}
  Base* geq(const Base* X)const		{untested();return ((X) ? (X->leq(this))       : (NULL));}
  Base* not_equal(const Base* X)const	{untested();return ((X) ? (X->not_equal(this)) : (NULL));}
  Base* equal(const Base* X)const	{untested();return ((X) ? (X->equal(this))	    : (NULL));}
  Base* add(const Base* X)const 	{untested();return ((X) ? (X->add(this))       : (NULL));}
  Base* multiply(const Base* X)const	{untested();return ((X) ? (X->multiply(this))  : (NULL));}
  Base* subtract(const Base* X)const	{untested();return ((X) ? (X->r_subtract(this)): (NULL));}
  Base* r_subtract(const Base* X)const	{untested();return ((X) ? (X->subtract(this))  : (NULL));}
  Base* divide(const Base* X)const	{	    return ((X) ? (X->r_divide(this))  : (NULL));}
  Base* r_divide(const Base* X)const	{untested();return ((X) ? (X->divide(this))    : (NULL));}

  Base* less(const Float*)const 	{untested();return NULL;}
  Base* greater(const Float*)const	{untested();return NULL;}
  Base* leq(const Float*)const  	{untested();return NULL;}
  Base* geq(const Float*)const  	{untested();return NULL;}
  Base* not_equal(const Float*)const	{untested();return NULL;}
  Base* equal(const Float*)const	{untested();return NULL;}
  Base* add(const Float*)const  	{untested();return NULL;}
  Base* multiply(const Float*)const	{untested();return NULL;}
  Base* subtract(const Float*)const	{untested();return NULL;}
  Base* r_subtract(const Float*)const	{untested();return NULL;}
  Base* divide(const Float*)const	{untested();return NULL;}
  Base* r_divide(const Float*)const	{untested();return NULL;}
};
/*--------------------------------------------------------------------------*/
class Name_String	// a string that contains only alnum and _[]
  :public String
{
public:
  void parse(CS&);
public:
  explicit Name_String(CS& file)	{parse(file);}
  explicit Name_String()		{untested();}
};
/*--------------------------------------------------------------------------*/
class Quoted_String	// the first non-blank character is a quote
  :public String	// a repeat of the same character terminates it
{
public:
  void parse(CS&);
public:
  explicit Quoted_String(CS& file)	{untested();parse(file);}
  explicit Quoted_String()		{untested();}
};
/*--------------------------------------------------------------------------*/
class Tail_String	// a string that is parsed to the end of a line
  :public String
{
public:
  void parse(CS&);
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
{untested();
  for (const_iterator i = begin(); i != end(); ++i) {untested();
    assert(*i);
    Out << **i;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void List<T>::parse(CS& File)
{untested();
  //skip_comment(File);
  unsigned here = File.cursor();
  for (;;) {untested();
    if (File.match1('[')) {untested();
      break;
    }else{untested();
      T* x = new T(File);
      if (!File.stuck(&here)) {untested();
	push(x);
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
  unsigned here = File.cursor();
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
