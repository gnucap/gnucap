/*$Id: u_parameter.h  $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for spice compatible .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script 2014.11.25
#ifndef U_PARAMETER_H
#define U_PARAMETER_H
#include "u_opt.h"
#include "io_.h"
#include "m_expression.h"
#include "m_base_vams.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
class LANGUAGE;
/*--------------------------------------------------------------------------*/
class Exception_Clash : public Exception{
public:
  explicit Exception_Clash(std::string const& what) :
    Exception(what) {}
};
/*--------------------------------------------------------------------------*/
class PARA_BASE {
protected:
  std::string _s;
  
public:
  explicit PARA_BASE( ): _s(){}
  explicit PARA_BASE(const PARA_BASE& p): _s(p._s) {}
  explicit PARA_BASE(const std::string s): _s(s){untested();}
  virtual ~PARA_BASE(){}
  virtual PARA_BASE* clone()const = 0;
  virtual PARA_BASE* pclone(void*)const = 0;
  virtual bool operator==(const PARA_BASE&) const = 0;

	  bool	has_hard_value()const {return (_s != "");}
  virtual bool	has_good_value()const = 0;
          bool  is_constant()const    {itested(); return (_s == "#");}
  virtual bool  is_given()const       {untested(); return (_s != "");}
  void	print(OMSTREAM& o)const       { o << string();}

  virtual void	parse(CS& cmd) = 0;
  virtual PARA_BASE& operator=(const std::string& s) = 0;
  virtual PARA_BASE& operator=(Base const*) = 0;
  virtual std::string string()const = 0;

  virtual Base const* value()const = 0;
};
/*--------------------------------------------------------------------------*/
// maps PARAMETER<double> PARAMETER<int>
//   to PARAMETER<Float> PARAMETER<Integer>
template<class T>
struct data_type {
  typedef T type_name;
 // bool has_good_value(type_name const& v)const { return v != NOT_INPUT; }
};
template<>
struct data_type<double> {
  typedef Float type_name;
 // bool has_good_value(type_name const& v)const { return v != NOT_INPUT; }
};
template<>
struct data_type<int> {
  typedef Integer type_name;
 // bool has_good_value(type_name const& v)const { return v != NOT_INPUT; }
};
template<>
struct data_type<bool> {
  typedef Integer type_name;
 // bool has_good_value(type_name const& v)const { return v != NOT_INPUT; }
};
/*--------------------------------------------------------------------------*/
template <class T>
class PARAMETER : public PARA_BASE {
  typedef typename data_type<T>::type_name type_name;
  mutable type_name _v;
public:
  explicit PARAMETER() : PARA_BASE(), _v(type_name().not_input()) {}
	   PARAMETER(const PARAMETER<T>& p) : PARA_BASE(p), _v(p._v) {}
  explicit PARAMETER(T v) :PARA_BASE(), _v(v) {}
  //explicit PARAMETER(T v, const std::string& s) :_v(v), _s(s) {untested();}
  ~PARAMETER() {}
  PARAMETER* clone()const override{ untested();
    return new PARAMETER(*this);
  }
  PARAMETER* pclone(void* p)const override{
    return new(p) PARAMETER(*this);
  }
  
  bool	has_good_value()const override {return _v != not_input();}
  //bool has_soft_value()const {untested(); return (has_good_value() && !has_hard_value());}

  operator T()const {return _v;}
  T e_val(const T& def, const CARD_LIST* scope)const;
  Base const* value()const { return &_v;}
  void	parse(CS& cmd) override;

  std::string string()const override{
    if (_s == "#") {
      return to_string(_v);
    }else if (_s == "") {
      return "NA(" + to_string(_v) + ")";
    }else{
      return _s;
    }
  }
  void	set_default(const T& v)		{_v = v; _s = "";}
  void	set_fixed(const T& v)		{_v = v; _s = "#";}
  PARAMETER& operator=(Base const* v) override {
    type_name* x = _v.assign(v);
    assert(x);
    _v = *x;
    delete x;
    _s = "#";
    return *this;
  }
  void	operator=(const PARAMETER& p)	{_v = p._v; _s = p._s;}
  void	operator=(const T& v)		{_v = v; _s = "#";} // !
  //void	operator=(const std::string& s)	{untested();_s = s;}

  PARAMETER& operator=(const std::string& s)override {
    if (!s.size()) {
      _s = "";
    }else if (s == "NA") {
      _s = "";
    }else if (s.size() && _s.size()) {
      throw Exception_Clash("already set");
    }else if (strchr("'\"{", s[0])) {
      CS cmd(CS::_STRING, s);
      _s = cmd.ctos("", "'\"{", "'\"}");
    }else{
      _s = s;
    }
    return *this;
  }
  bool  operator==(const PARA_BASE& b)const override {
    PARAMETER const* p = dynamic_cast<PARAMETER const*>(&b);
    return (p && _v == p->_v  &&  _s == p->_s);
  }
  bool  operator==(const T& v)const {
    if (v != type_name().not_input()) {
      return _v == v;
    }else{
      return (_v == type_name().not_input() || !has_hard_value());
    }
  }
  //bool	operator!=(const PARAMETER& p)const {untested();
  //  return !(*this == p);
  //}
  //bool	operator!=(const T& v)const {untested();
  //  return !(*this == v);
  //}
protected:
  virtual type_name not_input()const { return type_name().not_input(); }
  Base const* e_val_(const T& def, const CARD_LIST* scope, int recursion=0)const;
  friend class PARAM_INSTANCE;
private:
  T lookup_solve(const T& def, const CARD_LIST* scope)const;
}; // PARAMETER
/*--------------------------------------------------------------------------*/
/* non-class interface, so non-paramaters can have same syntax */
/* It is needed by the model compiler */
#if 0
inline bool operator==(const PARAMETER<int>& p, double v)
{untested();
  if (v != NOT_INPUT) {untested();
    return p.operator==(static_cast<int>(v));
  }else{untested();
    return (!(p.has_value()));
  }
}
#endif

inline bool has_hard_value(const PARA_BASE& p)
{
  return p.has_hard_value();
}

inline bool has_good_value(const PARA_BASE& p)
{
  return p.has_good_value();
}

#if 0
template <class T>
bool has_soft_value(const PARA_BASE& p)
{untested();
  return p.has_soft_value();
}
#endif

template <class T>
bool has_nz_value(const T& p)
{
  return (has_good_value(p) && p != 0);
}

template <class T>
void set_default(PARAMETER<T>* p, const T& v)
{
  assert(p);
  p->set_default(v);
}

template <class T>
void set_default(T* p, const T& v)
{
  assert(p);
  *p = v;
}

template <class T>
void e_val(PARAMETER<T>* p, const PARAMETER<T>& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

template <class T>
void e_val(PARAMETER<T>* p, const T& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

#if 0
template <class T>
void e_val(T* p, const T& def, const CARD_LIST*)
{untested();
  assert(p);
  if (*p == NOT_INPUT) {untested();
    *p = def;
  }else{untested();
  }
}
#endif
/*--------------------------------------------------------------------------*/
// envelope for PARAMETER<T>
class PARAM_INSTANCE {
private:
  // needed. PARA_BASE is abstract.
  class PARA_NONE : public PARA_BASE {
  public:
    explicit PARA_NONE() : PARA_BASE() {}
    explicit PARA_NONE(PARA_NONE const&p) : PARA_BASE(p) {}
    PARA_BASE* clone()const override { untested();unreachable(); return NULL;}
    PARA_BASE* pclone(void* p)const override { return new(p) PARA_NONE(*this);}
    bool operator==(const PARA_BASE&)const override { untested();unreachable(); return false;}
    bool has_good_value()const override { untested();unreachable(); return false;}
    void parse(CS&)override { untested();unreachable();}
    PARA_NONE& operator=(const std::string&)override { untested();unreachable(); return *this;}
    PARA_NONE& operator=(const Base*)override { untested();unreachable(); return *this;}
    std::string string()const override{unreachable(); return "";}
    Base const* value()const override{return nullptr;}
  };
  union{
    char _mem;
    PARAMETER<double> _space; // placeholder: biggest allowed type.
  };
private:
  PARA_BASE const* base()const { return reinterpret_cast<PARA_BASE const*>(&_mem);}
  PARA_BASE* base() { return reinterpret_cast<PARA_BASE*>(&_mem);}
public:
  explicit PARAM_INSTANCE() {
    new(&_mem) PARA_NONE();
  }
  /*explicit*/ PARAM_INSTANCE(PARAM_INSTANCE const& p) {
    p.base()->pclone(&_mem);
  }
  ~PARAM_INSTANCE() {
    base()->~PARA_BASE();
  }

public:
  bool operator==(PARAM_INSTANCE const& p)const {
    return (*base()) == (*p.base());
  }
  PARAM_INSTANCE& operator=(PARAM_INSTANCE const&p) {
    base()->~PARA_BASE();
    p.base()->pclone(&_mem);
    return *this;
  }
 // PARAM_INSTANCE& operator=(Base const&b) {
 //   assert(base());
 //   *base() = &b;
 //   return *this;
 // }
  PARAM_INSTANCE& operator=(std::string const& s) {
#if 0
    // BUG. why double?
    auto p = new(&_mem) PARAMETER<double>();
    *p = s;
#else
    assert(base());
    *base() = s;
#endif
    return *this;
  }
  PARAM_INSTANCE& operator=(PARA_BASE const& p) {
    p.pclone(&_mem);
    return *this;
  }
  void set_fixed(Base const* v) {
    if(auto d = dynamic_cast<PARAMETER<double>*>(base())) {
      if(auto f = dynamic_cast<Float const*>(v)){
	d->set_fixed(f->value());
      }else if(auto i = dynamic_cast<Integer const*>(v)){
	d->set_fixed(double(i->value()));
      }else{
	unreachable();
      }
    }else if(auto pi = dynamic_cast<PARAMETER<int>*>(base())) {
      if(auto f = dynamic_cast<Float const*>(v)){
	pi->set_fixed(int(f->value()));
      }else if(auto i = dynamic_cast<Integer const*>(v)){
	pi->set_fixed(i->value());
      }else{
	unreachable();
      }
    }else if(dynamic_cast<PARA_NONE*>(base())) {
      incomplete();
      *base() = v;
    }else{
      // TODO, get rid of this //
      *base() = v;
    }
  }
#if 1
  PARAM_INSTANCE& operator=(double const& p) = delete;
#else
  // not what it appears to be. call set_fixed instead.
  PARAM_INSTANCE& operator=(double const& p){ untested();
    if(auto d = dynamic_cast<PARAMETER<double>*>(_p)){ untested();
      *d = p;
    }else{ untested();
      delete _p;
      PARAMETER<double>* pp = new PARAMETER<double>();
      *pp = p;
      _p = pp;
    }
    return *this;
  }
#endif
public:
  Base const* value()const {untested(); return base()->value();}
  std::string const string() const{
    assert(base());
    return base()->string();
  }
  bool exists() const{
    assert(base());
    if(dynamic_cast<PARA_NONE const*>(base())){
      return false;
    }else{
      return true;
    }
  }
  bool has_hard_value() const{
    assert(base());
    return base()->has_hard_value();
  }
  Base const* e_val(const double& def, const CARD_LIST* scope)const;
 // Base const* e_val(Base const* def, const CARD_LIST* scope)const;
  operator double() const{ untested();
    assert(0);
    incomplete();
    return NOT_VALID;
  }
  void parse(CS& cmd) {
    base()->parse(cmd);
  }
 //  operator PARAMETER<double> const&()const {
 //    if(auto d = dynamic_cast<PARAMETER<double> const*>(_data.base())){
 //      return *d;
 //    }else{ untested();
 //      throw Exception("not a double");
 //    }
 //  }
  PARA_BASE const* operator->()const {return base();}
  PARA_BASE const* operator*()const {return base();}
}; // PARAM_INSTANCE
/*--------------------------------------------------------------------------*/
class INTERFACE PARAM_LIST {
private:
  typedef std::map<std::string, PARAM_INSTANCE> map;
public:
  typedef map::const_iterator const_iterator;
  typedef map::iterator iterator;
private:
  map _pl;
  PARAM_LIST const* _try_again; // if you don't find it, also look here
  mutable const_iterator _previous;
public:
  explicit PARAM_LIST() :_try_again(nullptr) {}
  explicit PARAM_LIST(const PARAM_LIST& p) :_pl(p._pl), _try_again(p._try_again) {}
  //explicit PARAM_LIST(PARAM_LIST* ta) :_try_again(ta) {untested();}
  ~PARAM_LIST() {}
  PARAM_LIST& operator=(const PARAM_LIST& p) { itested();
    _pl = p._pl;
    return *this;
  }
  void	parse(CS& cmd);
  void	print(OMSTREAM&, LANGUAGE*)const;
  
  size_t size()const {return _pl.size();}
  //bool is_empty()const {untested();return _pl.empty();}
  bool	 is_printable(int)const;
  std::string name(int)const;
  std::string value(int)const;

  void	eval_copy(PARAM_LIST const&, const CARD_LIST*);
  bool  operator==(const PARAM_LIST& p)const{return _pl == p._pl;}
  const PARAM_INSTANCE& deep_lookup(std::string)const;
  const PARAM_INSTANCE& operator[](std::string i)const {return deep_lookup(i);}
  void set(std::string, const double&);
  void set(std::string, const std::string&);
  void set(std::string, const PARAM_INSTANCE&);
  void set_try_again(PARAM_LIST const* t) {_try_again = t;}

  iterator begin() {return _pl.begin();}
  iterator end() {return _pl.end();}
  iterator find(std::string const& k) {itested(); return _pl.find(k); }
  const_iterator begin()const {itested(); return _pl.begin();}
  const_iterator end()const {itested(); return _pl.end();}
  const_iterator find(std::string const& k) const {itested(); return _pl.find(k); }
};
/*--------------------------------------------------------------------------*/
template <>
inline bool PARAMETER<bool>::lookup_solve(const bool&, const CARD_LIST*)const
{
  CS cmd(CS::_STRING, _s);
  return cmd.ctob();
}
/*--------------------------------------------------------------------------*/
template <class T>
T get(Base const* t)
{
  auto f = prechecked_cast<T const*>(t);
  assert(f);
  return *f;
}

// template <>
// inline Float get<Float>(Base const* t)
// {
//   auto f = prechecked_cast<Float const*>(t);
//   assert(f);
//   return *f;
// }

template <>
inline double get<double>(Base const* t)
{
  auto f = prechecked_cast<Float const*>(t);
  assert(f);
  return f->value();
}
// template <>
// inline Integer get<Integer>(Base const* t)
// {
//   auto f = prechecked_cast<Integer const*>(t);
//   assert(f);
//   return *f;
// }
template <>
inline int get<int>(Base const* t)
{
  auto f = prechecked_cast<Integer const*>(t);
  assert(f);
  return f->value();
}
template <>
inline bool get<bool>(Base const* t)
{
  auto f = prechecked_cast<Integer const*>(t);
  assert(f);
  return f->value();
}
/*--------------------------------------------------------------------------*/
template <class T>
inline T PARAMETER<T>::lookup_solve(const T& def, const CARD_LIST* scope)const
{
  CS cmd(CS::_STRING, _s);
  Expression e(cmd);
  Expression reduced(e, scope);
  T v = T(reduced.eval());
  if (v != type_name().not_input()) {
    trace2("los0a", _s, v);
    return v;
  }else{
    const PARAM_LIST* pl = scope->params();
    trace2("los0b", _s, v);
    Base const* b = pl->deep_lookup(_s).e_val(def, scope);
    T ret;
    if(b){
      ret = get<T>(b);
    }else{
      error(bWARNING, "parameter " + _s +  " not specified, using default\n");
      ret = def;
    }
    trace3("los1", _s, v, ret);
    return ret;
  }
}
/*--------------------------------------------------------------------------*/
#if 0
template <class T>
inline T PARAMETER<T>::lookup_solve(const T& def, const CARD_LIST* scope)const
{untested();
  const PARAM_LIST* pl = scope->params();
  return T(pl->deep_lookup(_s).e_val(def, scope));
}
#endif
/*--------------------------------------------------------------------------*/
template <class T>
T PARAMETER<T>::e_val(const T& def, const CARD_LIST* scope)const
{
  trace2("e_val", def, typeid(T).name());
  Base const* t = e_val_(def, scope, 0);
  return get<T>(t);
}
/*--------------------------------------------------------------------------*/
template <class T>
Base const* PARAMETER<T>::e_val_(const T& def, const CARD_LIST* scope, int recurse)const
{
  trace2("e_val", _v, _s);
  assert(scope);

  if (recurse) {
  }else{
  }

  if (_s == "") {
    // blank string means to use default value
    _v = def;
    if (recurse) { untested();
      // reachable?
      error(bWARNING, "parameter " + _s +  " not specified, using default\n");
    }else{
    }
  }else if (_s != "#") {
    // anything else means look up the value
    _v = lookup_solve(def, scope);
    if (_v == NOT_INPUT) {
      //BUG// needs to show scope
      //BUG// T==bool?
      //BUG// it is likely to have a numeric overflow resulting from the bad value
      error(bDANGER, "parameter " + _s + " value is \"NOT_INPUT\"\n");
      // throw Exception(": " + _s + " value is \"NOT_INPUT\"\n");
    }else{
    }
  }else{
    // start with # means we have a final value
  }
  return &_v;
}
/*--------------------------------------------------------------------------*/
template <>
inline void PARAMETER<bool>::parse(CS& cmd) 
{
  bool new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{untested();
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {untested();
      if (name == "NA") {untested();
	_s = "";
      }else{untested();
	_s = name;
      }
    }else{untested();
    }
  }
  if (!cmd) {untested();
    _v = true;
    _s = "#";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline void PARAMETER<T>::parse(CS& cmd) 
{
  T new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {
      if (cmd.match1('(')) {
	_s = name + '(' + cmd.ctos("", "(", ")") + ')';
      }else{
	_s = name;
      }
      if (name == "NA") {untested();
        _s = "";
      }else{
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val);
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val);
/*--------------------------------------------------------------------------*/
template <class T>
inline OMSTREAM& operator<<(OMSTREAM& o, const PARAMETER<T> p)
{
  p.print(o);
  return o;
}
/*--------------------------------------------------------------------------*/
inline OMSTREAM& operator<<(OMSTREAM& o, PARAM_INSTANCE const& p)
{
  trace3("PI << ", p.string(), p.operator->(), &p);
  p->print(o);
  return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
