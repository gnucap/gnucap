/*$Id: mg_.h,v 26.81 2008/05/27 05:33:43 al Exp $ -*- C++ -*-
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
 */
//testing=script 2006.10.31
#include <stdexcept>
#include "md.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
// defined here
class Base;
class C_Comment;
class Cxx_Comment;
class Parameter;
class Code_Block;
class Parameter_Block;
class String_Arg;
class Model;
class Head;
class File;
/*--------------------------------------------------------------------------*/
// external
class CS;
/*--------------------------------------------------------------------------*/
#ifdef PASS_TRACE_TAGS
#define make_tag() (out << "//" << __func__ << ":" << __LINE__ << "\n")
#else
#define make_tag()
#endif
/*--------------------------------------------------------------------------*/
inline std::string to_lower(std::string s)
{
  for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
    *i = static_cast<char>(tolower(*i));
  }
  return s;
}
/*--------------------------------------------------------------------------*/
inline std::string to_upper(std::string s)
{
  for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
    *i = static_cast<char>(toupper(*i));
  }
  return s;
}
/*--------------------------------------------------------------------------*/
inline void error(const std::string& message)
{untested();
  std::cerr << message << '\n';
  exit(1);
}
/*--------------------------------------------------------------------------*/
inline void os_error(const std::string& name)
{untested();
  error(name + ':' + strerror(errno));
}
/*--------------------------------------------------------------------------*/
class Base
{
public:
  virtual void parse(CS&) = 0;
  virtual void print(std::ostream& f)const {unreachable(); f << "Base::print";}
  virtual ~Base() {}
};
inline CS&	     operator>>(CS& f, Base& b)
				{untested();b.parse(f); return f;}
inline std::ostream& operator<<(std::ostream& f, const Base& d)
				{d.print(f); return f;}
/*--------------------------------------------------------------------------*/
template <class T>
class List_Base
  :public Base
{
protected:
  typedef typename std::list<T*> _Std_List_T;
  _Std_List_T _list;
  virtual ~List_Base() {
    for (typename std::list<T*>::iterator
	 i = _list.begin();
	 i != _list.end();
	 ++i) {
      delete *i;
    }
  }
public:
  virtual void parse(CS& f) = 0;
  typedef typename std::list<T*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}
  bool		 is_empty()const {return _list.empty();}
  size_t	 size()const	 {return _list.size();}
};
/*--------------------------------------------------------------------------*/
/* A "Collection" differs from a "List" in how it is parsed.
 * Each parse of a "Collection" created one more object and stores
 * it in the Collection.  The size of the Collection therefore grows by 1.
 * A "Collection" is often parsed many times.
 * Each parse of a "List" creates a bunch of objects, and storing them.
 * A list has opening and closing delimeters, usually {}.
 * A "List" is usually parsed once.
 */
template <class T, char BEGIN, char END>
class List
  :public List_Base<T>
{
  using List_Base<T>::_list;
public:
  //BUG//  why not inherited?
  typedef typename std::list<T*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}

  void parse(CS& file) {
    int paren = !BEGIN || file.skip1b(BEGIN);
    unsigned here = file.cursor();
    for (;;) {
      C_Comment   c_comment;
      Cxx_Comment cxx_comment;
      (file >> "/*") && (file >> c_comment);
      (file >> "//") && (file >> cxx_comment);
      if (file.stuck(&here)) {
	paren -= file.skip1b(END);
	if (paren == 0) {
	  //file.warn(0, "list exit");
	  break;
	}else{
	  //file.warn(0, "list");
	}
	T* p = new T(file);
	if (!file.stuck(&here)) {
	  _list.push_back(p);
	}else{itested();
	  delete p;
	  file.warn(0, "not valid here");
	  break;
	}
      }else{
      }
    }
  }
  void print(std::ostream& f)const {
    f << BEGIN;
    for (const_iterator i = begin(); i != end(); ++i) {
      f << (**i);
    }
    f << END;
  }
};
/*--------------------------------------------------------------------------*/
/* A "Collection" differs from a "List" in how it is parsed.
 * Each parse of a "Collection" created one more object and stores
 * it in the Collection.  The size of the Collection therefore grows by 1.
 * A "Collection" is often parsed many times.
 * Each parse of a "List" creates a bunch of objects, and storing them.
 * A list has opening and closing delimeters, usually {}.
 * A "List" is usually parsed once.
 */
template <class T>
class Collection
  :public List_Base<T>
{
  using List_Base<T>::_list;
public:
  //BUG//  why not inherited?
  typedef typename std::list<T*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}

  void parse(CS& file) {
    unsigned here = file.cursor();
    T* m = new T(file);
    if (!file.stuck(&here)) {
      _list.push_back(m);
    }else{untested();
      delete m;
      file.warn(0, "what's this??");
    }
  }
  void print(std::ostream& f)const {
    for (const_iterator i = begin(); i != end(); ++i) {
      f << (**i);
    }
  }
};
/*--------------------------------------------------------------------------*/
class C_Comment
  :public Base
{
public:
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
class Cxx_Comment
  :public Base
{
public:
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
class Key
  :public Base
{
  std::string _name;
  std::string _var;
  std::string _value;
public:
  void parse(CS& f) {f >> _name >> _var >> '=' >> _value >> ';';}
  void print(std::ostream& f)const
  {f << name() << " " << var() << "=" << value() << "; ";}
  Key(CS& f) {parse(f);}
  const std::string& name()const	{return _name;}
  const std::string& var()const 	{return _var;}
  const std::string& value()const	{return _value;}
};
typedef List<Key, '{', '}'> Key_List;
/*--------------------------------------------------------------------------*/
class String_Arg
  :public Base
{
  std::string	_s;
public:
  void parse(CS& f)			 {f >> _s >> ';';}
  void print(std::ostream& f)const	 {f << _s;}
  void operator=(const std::string& s)	 {untested();_s = s;}
  void operator+=(const std::string& s)	 {_s += s;}
  bool operator!=(const std::string& s)const {return _s != s;}
  bool			is_empty()const	 {return _s.empty();}
  std::string		lower()const	 {return to_lower(_s);}
  const std::string&	to_string()const {return _s;}
};
/*--------------------------------------------------------------------------*/
class Bool_Arg
  :public Base
{
  bool _s;
public:
  void parse(CS& f)	{_s = true; f.skip1b(";");}
  void print(std::ostream& f)const {untested();f << _s;}
  Bool_Arg() :_s(false) {}
  operator bool()const {return _s;}
};
/*--------------------------------------------------------------------------*/
class Parameter
  :public Base
{
  std::string _type;
  std::string _code_name;
  std::string _user_name;
  std::string _alt_name;
  std::string _default_val;
  std::string _comment;
  std::string _print_test;
  std::string _calc_print_test;
  std::string _scale;
  std::string _offset;
  std::string _calculate;
  std::string _quiet_min;
  std::string _quiet_max;
  std::string _final_default;
  bool	      _positive;
  bool	      _octal;
public:
  void parse(CS& f);
  void print(std::ostream& f)const;
  Parameter(CS& f) :_positive(false), _octal(false) {parse(f);}
  const std::string& type()const		{return _type;}
  const std::string& code_name()const		{return _code_name;}
  const std::string& user_name()const		{return _user_name;}
  const std::string& alt_name()const		{return _alt_name;}
  const std::string& comment()const		{return _comment;}
  const std::string& default_val()const 	{return _default_val;}
  const std::string& print_test()const		{return _print_test;}
  const std::string& calc_print_test()const	{return _calc_print_test;}
  const std::string& scale()const		{return _scale;}
  const std::string& offset()const		{return _offset;}
  const std::string& calculate()const		{return _calculate;}
  const std::string& quiet_min()const		{return _quiet_min;}
  const std::string& quiet_max()const		{return _quiet_max;}
  const std::string& final_default()const	{return _final_default;}
  bool		positive()const			{return _positive;}
  bool		octal()const			{return _octal;}

  void fill_in_default_name() {
    if (_user_name.empty()) {
      _user_name = to_upper(_code_name);
    }else{
    }
  }
};
typedef List<Parameter, '{', '}'> Parameter_List;
/*--------------------------------------------------------------------------*/
class Code_Block
  :public Base
{
  std::string s;
public:
  void parse(CS& f);
  void print(std::ostream& f)const {f << s;}
  Code_Block() {}
  bool is_empty()const {return s.length() < 2;}
};
/*--------------------------------------------------------------------------*/
class Parameter_Block
  :public Base
{
  String_Arg	 _unnamed_value;
  Parameter_List _override;
  Parameter_List _raw;
  Parameter_List _calculated;
  Code_Block	 _code_pre;
  Code_Block	 _code_mid;
  Code_Block     _code_post;
public:
  void parse(CS& f);
  void print(std::ostream& f)const;
  const String_Arg&	unnamed_value()const	{return _unnamed_value;}
  const Parameter_List& override()const 	{return _override;}
  const Parameter_List& raw()const		{return _raw;}
  const Parameter_List& calculated()const	{return _calculated;}
  const Code_Block&	code_pre()const		{return _code_pre;}
  const Code_Block&	code_mid()const		{return _code_mid;}
  const Code_Block&	code_post()const	{return _code_post;}
  bool is_empty()const {return (calculated().is_empty() 
				&& code_post().is_empty()
				&& code_mid().is_empty()
				&& override().is_empty()
				&& raw().is_empty()
				&& code_pre().is_empty());}
  void fill_in_default_values();
};
/*--------------------------------------------------------------------------*/
class Eval
  :public Base
{
protected:
  String_Arg _name;
  Code_Block _code;
  Eval() :_name(), _code() {}
public:
  void parse(CS& f);
  void print(std::ostream& f)const;  
  Eval(CS& f) :_name(), _code() {parse(f);}
  const String_Arg&	name()const	{return _name;}
  const Code_Block&	code()const	{return _code;}
};
typedef Collection<Eval> Eval_List;
/*--------------------------------------------------------------------------*/
class Function
  :public Eval
{
public:
  void parse(CS& f);
  void print(std::ostream& f)const;  
  Function(CS& f) :Eval() {parse(f);}
};
typedef Collection<Function> Function_List;
/*--------------------------------------------------------------------------*/
class Port
  :public Base
{
  std::string _name;
  std::string _short_to;
  std::string _short_if;
public:
  void parse(CS& f);
  void print(std::ostream& f)const;
  Port() {untested();}
  Port(CS& f) {parse(f);}
  const std::string& name()const	{return _name;}
  const std::string& short_to()const 	{return _short_to;}
  const std::string& short_if()const 	{return _short_if;}
};
typedef List<Port, '{', '}'> Port_List;
/*--------------------------------------------------------------------------*/
class Element
  :public Base
{
  std::string _dev_type;
  std::string _name;
  Port_List _port_list;
  std::string _eval;
  std::string _value;
  std::string _args;
  std::string _omit;
  std::string _reverse;
  std::string _state;
public:
  void parse(CS&);
  void print(std::ostream& f)const;
  Element() {untested();}
  Element(CS& f) {parse(f);}
  const std::string& dev_type()const	{return _dev_type;}
  const Port_List& ports()const 	{return _port_list;}
  const std::string& name()const 	{return _name;}
  const std::string& eval()const 	{return _eval;}
  const std::string& value()const 	{return _value;}
  const std::string& args()const 	{return _args;}
  const std::string& omit()const 	{return _omit;}
  const std::string& reverse()const 	{return _reverse;}
  const std::string& state()const	{return _state;}
	size_t	     num_nodes()const	{return ports().size();}
};
typedef Collection<Element> Element_List;
/*--------------------------------------------------------------------------*/
class Arg
  :public Base
{
  std::string _arg;
public:
  void parse(CS& f);
  void print(std::ostream& f)const {f << "      " << arg() << ";\n";}
  Arg(CS& f) {parse(f);}
  const std::string& arg()const {return _arg;}
};
typedef List<Arg, '{', '}'> Arg_List;
/*--------------------------------------------------------------------------*/
class Args
  :public Base
{
  String_Arg _name;
  String_Arg _type;
  Arg_List   _arg_list;
public:
  void parse(CS& f) {f >> _name >> _type >> _arg_list;}
  void print(std::ostream& f)const
  {f << "    args " << name() << " " << type() << "\n"
     << arg_list() << "\n";}
  Args(CS& f) {parse(f);}
  const String_Arg& name()const {return _name;}
  const String_Arg& type()const {return _type;}
  const Arg_List&   arg_list()const {return _arg_list;}
  typedef Arg_List::const_iterator const_iterator;
  const_iterator begin()const	{return _arg_list.begin();}
  const_iterator end()const	{return _arg_list.end();}
};
typedef Collection<Args> Args_List;
/*--------------------------------------------------------------------------*/
class Circuit
  :public Base
{
  Port_List	_required_nodes;
  Port_List	_optional_nodes;
  Port_List	_local_nodes;
  Element_List _element_list;
  Args_List	_args_list;
  bool		_sync;
public:
  void parse(CS&);
  void print(std::ostream& f)const;
  Circuit() : _sync(false) {}
  bool		      sync()const	 {return _sync;}
  const Port_List&    req_nodes()const   {return _required_nodes;}
  const Port_List&    opt_nodes()const   {return _optional_nodes;}
  const Port_List&    local_nodes()const {return _local_nodes;}
  const Element_List& elements()const	 {return _element_list;}
  const Args_List&    args_list()const	 {return _args_list;}
	size_t	      min_nodes()const	 {return req_nodes().size();}
        size_t	      max_nodes()const {return opt_nodes().size()+min_nodes();}
	size_t	      net_nodes()const	 {untested();return max_nodes();}
};
/*--------------------------------------------------------------------------*/
class Probe
  :public Base
{
  std::string _name;
  std::string _expression;
public:
  void parse(CS& f) {f >> _name >> '=' >> _expression >> ';';}
  void print(std::ostream& f)const
	{f << "    " << name() << " = \"" << expression() << "\";\n";}
  Probe() {untested();}
  Probe(CS& f) {parse(f);}
  const std::string& name()const	{return _name;}
  const std::string& expression()const	{return _expression;}
};
typedef List<Probe, '{', '}'> Probe_List;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model
  :public Base
{
  String_Arg		_name;
  String_Arg		_level;
  String_Arg		_dev_type;
  String_Arg		_inherit;
  Key_List		_public_key_list;
  Key_List		_private_key_list;
  Parameter_Block	_independent;
  Parameter_Block	_size_dependent;
  Parameter_Block	_temperature;
  Code_Block		_tr_eval;
  Code_Block		_validate;
  Bool_Arg		_hide_base;
public:
  void parse(CS& f);
  void print(std::ostream& f)const;
  Model(CS& f) {parse(f);}
  bool			 hide_base()const	{return _hide_base;}
  const String_Arg&	 name()const		{return _name;}
  const String_Arg&	 level()const		{return _level;}
  const String_Arg&	 dev_type()const	{return _dev_type;}
  const String_Arg&	 inherit()const		{return _inherit;}
  const Key_List&	 public_key_list()const	{return _public_key_list;}
  const Key_List&	 private_key_list()const{return _private_key_list;}
  const Parameter_Block& independent()const	{return _independent;}
  const Parameter_Block& size_dependent()const	{return _size_dependent;}
  const Parameter_Block& temperature()const	{return _temperature;}
  const Code_Block&	 tr_eval()const		{return _tr_eval;}
  const Code_Block&	 validate()const	{return _validate;}
};
typedef Collection<Model> Model_List;
/*--------------------------------------------------------------------------*/
class Device
  :public Base
{
  String_Arg		_name;
  String_Arg		_parse_name;
  String_Arg		_id_letter;
  String_Arg		_model_type;
  Circuit		_circuit;
  Probe_List		_probes;
  Parameter_Block	_device;
  Parameter_Block	_common;
  Code_Block		_tr_eval;
  Eval_List		_eval_list;
  Function_List		_function_list;
public:
  void parse(CS& f);
  void print(std::ostream& f)const;
  Device(CS& f) {parse(f);}
  const String_Arg&	 name()const		{return _name;}
  const String_Arg&	 parse_name()const	{return _parse_name;}
  const String_Arg&	 id_letter()const	{return _id_letter;}
  const String_Arg&	 model_type()const	{return _model_type;}
  const Circuit&	 circuit()const		{return _circuit;}
  const Probe_List&	 probes()const		{return _probes;}
  const Parameter_Block& device()const		{return _device;}
  const Parameter_Block& common()const		{return _common;}
  const Code_Block&	 tr_eval()const		{return _tr_eval;}
  const Eval_List&	 eval_list()const	{return _eval_list;}
  const Function_List&	 function_list()const	{return _function_list;}
    	size_t		 min_nodes()const	{return circuit().min_nodes();}
    	size_t		 max_nodes()const	{return circuit().max_nodes();}
    	size_t	    net_nodes()const {untested();return circuit().net_nodes();}
};
typedef Collection<Device> Device_List;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Head
  :public Base
{
  std::string s;
public:
  void parse(CS& f);
  void print(std::ostream& f)const {f << s;}
  Head() {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Attribute
  :public Base
{
  String_Arg _name;
  String_Arg _value;
public:
  void parse(CS& f);
  Attribute(CS& f) {untested();parse(f);}
  const String_Arg& name()const		{untested();return _name;}
  const String_Arg& value()const	{untested();return _value;}
};
typedef Collection<Attribute> Attribute_List;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class File
{
  std::string	_name;
  CS		_file;
  Head		_head;
  Code_Block	_h_headers;
  Code_Block	_cc_headers;
  Model_List	_model_list;
  Device_List	_device_list;
  Code_Block	_h_direct;
  Code_Block	_cc_direct;
public:
  File(const std::string& file_name);
  const std::string& name()const	{return _name;}
  const Head&	     head()const	{return _head;}
  const Code_Block&  h_headers()const	{return _h_headers;}
  const Code_Block&  cc_headers()const	{return _cc_headers;}
  const Model_List&  models()const	{return _model_list;}
  const Device_List& devices()const	{return _device_list;}
  const Code_Block&  h_direct()const	{return _h_direct;}
  const Code_Block&  cc_direct()const	{return _cc_direct;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
