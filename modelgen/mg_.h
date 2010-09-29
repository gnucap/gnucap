/*$Id: mg_.h,v 24.12 2003/12/14 01:58:28 al Exp $ -*- C++ -*-
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
 */
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
inline std::string to_lower(std::string s)
{
  {for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
    *i = tolower(*i);
  }}
  return s;
}
/*--------------------------------------------------------------------------*/
inline std::string to_upper(std::string s)
{
  {for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
    *i = toupper(*i);
  }}
  return s;
}
/*--------------------------------------------------------------------------*/
inline void error(const std::string& message)
{
  untested();
  std::cerr << message << '\n';
  exit(1);
}
/*--------------------------------------------------------------------------*/
inline void os_error(const std::string& name)
{
  untested();
  error(name + ':' + strerror(errno));
}
/*--------------------------------------------------------------------------*/
class Base
{
public:
  virtual void parse(CS&) = 0;
  virtual void print(std::ostream&)const {}
  virtual ~Base() {}
};
inline CS&	     operator>>(CS& f, Base& b)	{b.parse(f); return f;}
inline std::ostream& operator<<(std::ostream& f, const Base& d)
						{d.print(f); return f;}
/*--------------------------------------------------------------------------*/
template <class T>
class List_Base
  :public Base
{
protected:
  std::list<T*> _list;
  virtual void parse(CS& f) = 0;
  virtual ~List_Base() {
    for (typename std::list<T*>::iterator
	   i = _list.begin(); i != _list.end(); ++i) {
      delete *i;
    }
  }
public:
  typedef typename std::list<T*>::const_iterator const_iterator;
  const_iterator begin()const	 {return _list.begin();}
  const_iterator end()const	 {return _list.end();}
  bool		 is_empty()const {return _list.empty();}
  int		 size()const	 {return _list.size();}
};
/*--------------------------------------------------------------------------*/
template <class T, char BEGIN, char END>
class List
  :public List_Base<T>
{
  void parse(CS& file) {
    C_Comment   dummy_c_comment;
    Cxx_Comment dummy_cxx_comment;
    int paren = file.skip1b(BEGIN);
    int here = file.cursor();
    {for (;;) {
      get(file, "/*$$", &dummy_c_comment);
      get(file, "//$$", &dummy_cxx_comment);
      if (file.stuck(&here)) {
	paren -= file.skip1b(END);
	if (paren == 0) {
	  break;
	}
	T* p = new T(file);
	{if (!file.stuck(&here)) {
	  _list.push_back(p);
	}else {
	  delete p;
	  file.warn(0, "not valid here");
	  break;
	}}
      }
    }}
  }
};
/*--------------------------------------------------------------------------*/
template <class T>
class Collection
  :public List_Base<T>
{
  void parse(CS& file) {
    int here = file.cursor();
    T* m = new T(file);
    {if (!file.stuck(&here)) {
      _list.push_back(m);
    }else{
      delete m;
      file.warn(0, "what's this??");
    }}
  }
};
/*--------------------------------------------------------------------------*/
class C_Comment
  :public Base
{
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
class Cxx_Comment
  :public Base
{
  void parse(CS& f);
};
/*--------------------------------------------------------------------------*/
class Key
{
  std::string _name;
  std::string _var;
  std::string _value;
  void parse(CS& f) {f >> _name >> _var >> _value; f.skip1b(";");}
public:
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
  void parse(CS& f)		{f >> _s; f.skip1b(";");}
  void print(std::ostream& f)const	{f << _s;}
public:
  void operator=(const std::string& s)	{_s = s;}
  void operator+=(const std::string& s)	{_s += s;}
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
  void parse(CS& f)	{_s = true; f.skip1b(";");}
  void print(std::ostream& f)const {f << _s;}
public:
  Bool_Arg() :_s(false) {}
  operator bool()const {return _s;}
};
/*--------------------------------------------------------------------------*/
class Parameter
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
  void parse(CS& f);
public:
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
};
typedef List<Parameter, '{', '}'> Parameter_List;
/*--------------------------------------------------------------------------*/
class Code_Block
  :public Base
{
  const char* _begin;
  const char* _end;
  void parse(CS& f);
  void print(std::ostream& f)const
			{if (!is_empty()) f.write(_begin, _end-_begin);}
public:
  Code_Block() :_begin(0), _end(0) {}
  bool is_empty()const {return _end - _begin < 2;}
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
  void parse(CS& f);
public:
  const String_Arg&	unnamed_value()const	{return _unnamed_value;}
  const Parameter_List& override()const		{return _override;}
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
};
/*--------------------------------------------------------------------------*/
class Eval
  :public Base
{
protected:
  String_Arg _name;
  Code_Block _code;
  void parse(CS& f);
  Eval() :_name(), _code() {}
public:
  Eval(CS& f) :_name(), _code() {parse(f);}
  const String_Arg&	name()const	{return _name;}
  const Code_Block&	code()const	{return _code;}
};
typedef Collection<Eval> Eval_List;
/*--------------------------------------------------------------------------*/
class Function
  :public Eval
{
  void parse(CS& f);
public:
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
  void parse(CS& f);
public:
  Port() {}
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
  std::string _class_name;
  std::string _name;
  Port_List   _port_list;
  std::string _eval;
  std::string _value;
  std::string _args;
  std::string _omit;
  std::string _reverse;
  std::string _state;
  void parse(CS&);
public:
  Element() {}
  Element(CS& f) {parse(f);}
  const std::string& dev_type()const	{return _dev_type;}
  const std::string& class_name()const	{return _class_name;}
  const Port_List&   ports()const	{return _port_list;}
  const std::string& name()const 	{return _name;}
  const std::string& eval()const 	{return _eval;}
  const std::string& value()const 	{return _value;}
  const std::string& args()const 	{return _args;}
  const std::string& omit()const 	{return _omit;}
  const std::string& reverse()const 	{return _reverse;}
  const std::string& state()const	{return _state;}
	int	     num_nodes()const	{return ports().size();}
};
typedef Collection<Element> Element_List;
/*--------------------------------------------------------------------------*/
class Arg
  :public Base
{
  std::string _arg;
  void parse(CS& f);
public:
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
  void parse(CS& f) {f >> _name >> _type >> _arg_list;}
public:
  Args(CS& f) {parse(f);}
  const String_Arg& name()const {return _name;}
  const String_Arg& type()const {return _type;}
  typedef Arg_List::const_iterator const_iterator;
  const_iterator begin()const	 {return _arg_list.begin();}
  const_iterator end()const	 {return _arg_list.end();}
};
typedef Collection<Args> Args_List;
/*--------------------------------------------------------------------------*/
class Circuit
  :public Base
{
  bool		_sync;
  Port_List	_required_nodes;
  Port_List	_optional_nodes;
  Port_List	_local_nodes;
  Element_List	_element_list;
  Args_List	_args_list;
  void parse(CS&);
public:
  Circuit() : _sync(false) {}
  bool		      sync()const	 {return _sync;}
  const Port_List&    req_nodes()const	 {return _required_nodes;}
  const Port_List&    opt_nodes()const	 {return _optional_nodes;}
  const Port_List&    local_nodes()const {return _local_nodes;}
  const Element_List& elements()const	 {return _element_list;}
  const Args_List&    args_list()const	 {return _args_list;}
	int	      min_nodes()const	 {return req_nodes().size();}
        int	      max_nodes()const {return opt_nodes().size()+min_nodes();}
	int	      net_nodes()const	 {return max_nodes();}
};
/*--------------------------------------------------------------------------*/
class Probe
  :public Base
{
  std::string _name;
  std::string _expression;
  void parse(CS& f) {f >> _name >> _expression; f.skip1b(";");}
public:
  Probe() {}
  Probe(CS& f) {parse(f);}
  const std::string& name()const	{return _name;}
  const std::string& expression()const	{return _expression;}
};
typedef List<Probe, '{', '}'> Probe_List;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model
{
  String_Arg		_name;
  String_Arg		_level;
  String_Arg		_dev_type;
  String_Arg		_inherit;
  Key_List		_key_list;
  Parameter_Block	_independent;
  Parameter_Block	_size_dependent;
  Parameter_Block	_temperature;
  Code_Block		_tr_eval;
  Code_Block		_validate;
  Bool_Arg		_is_base;
  void parse(CS& f);
public:
  Model(CS& f) {parse(f);}
  bool			 is_base()const		{return _is_base;}
  const String_Arg&	 name()const		{return _name;}
  const String_Arg&	 level()const		{return _level;}
  const String_Arg&	 dev_type()const	{return _dev_type;}
  const String_Arg&	 inherit()const		{return _inherit;}
  const Key_List&	 key_list()const	{return _key_list;}
  const Parameter_Block& independent()const	{return _independent;}
  const Parameter_Block& size_dependent()const	{return _size_dependent;}
  const Parameter_Block& temperature()const	{return _temperature;}
  const Code_Block&	 tr_eval()const		{return _tr_eval;}
  const Code_Block&	 validate()const	{return _validate;}
};
typedef Collection<Model> Model_List;
/*--------------------------------------------------------------------------*/
class Device
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
  void parse(CS& f);
public:
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
    	int		 min_nodes()const	{return circuit().min_nodes();}
    	int		 max_nodes()const	{return circuit().max_nodes();}
    	int		 net_nodes()const	{return circuit().net_nodes();}
};
typedef Collection<Device> Device_List;
/*--------------------------------------------------------------------------*/
class Head
  :public Base
{
  const char* _begin;
  const char* _end;
  void parse(CS& f);
  void print(std::ostream& f)const
			{if (_begin!=_end) f.write(_begin, _end-_begin);}
public:
  Head() :_begin(0), _end(0) {}
};
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
  const char*        c_str()const	{return _file.fullstring();}
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
