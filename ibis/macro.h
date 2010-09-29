/*$Id: macro.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "instance.h"
#include "expression.h"
/*--------------------------------------------------------------------------*/
class Macro; // forward
class Macro_List;
/*--------------------------------------------------------------------------*/
enum Symbol_Type {tSCOPE=1, tSCALAR, tNODE, tKEYWORD, tVECTOR, tRAMP,
		  tSTRINGVAR, tFOREACH, tNUMBER_CONST, tSTRING_CONST};
std::ostream& operator<<(std::ostream& out, Symbol_Type t);
const char* c_str(Symbol_Type);

enum Symbol_Mode {mDECLARE=1, mUSE};

class Symbol
  :public Base
{
  mutable Symbol_Type  _type;
  std::string  _name;
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
public:
  Symbol(Symbol_Type type, const std::string& name);
  Symbol_Type		type()const {return _type;}
  const std::string&	name()const {return _name;}
  void			change_type(Symbol_Type t)const {_type = t;}
};

class Symbol_Table
  :public Base
{
  mutable std::map<std::string, const Symbol*> _map;
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
public:
  Symbol_Table();
  ~Symbol_Table() {for (std::map<std::string, const Symbol*>::iterator
	  i = _map.begin(); i != _map.end(); ++i) {/*delete i->second;*/}}
  // BUG:: memory leak??
  typedef std::map<std::string, const Symbol*>::const_iterator const_iterator;
  const_iterator begin()const	{return _map.begin();}
  const_iterator end()const	{return _map.end();}
  void check_add_declare(CS& file, int here, Symbol_Type type,
		 const std::string& name);
  void check_add_use(CS& file, int here, Symbol_Type type,
		 const std::string& name);
  void check_add(CS& file, int here, Symbol_Mode mode, Symbol_Type type,
		 const std::string& name);
  void add_symbols(const Symbol_Table&, const std::string& scope);
  const Symbol* operator[](const std::string& key)const {return _map[key];}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Ramp_Name
  :public Base
{
  Symbol_Table* _sym;
  std::string _name;
  void parse(CS& file);
  void dump(std::ostream& o)const;
public:
  Ramp_Name(const Ramp_Name& p, const Instance&)
    :Base(), _sym(p._sym), _name("foopoop") {}
  Ramp_Name(Symbol_Table* s) : _sym(s) {}
};
/*--------------------------------------------------------------------------*/
class Test
  :public Base
{
  Expression _expr;
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
public:
  Test(CS&, const Macro_Body*);
  bool eval(const Instance& i)const {return(Expression(_expr, i).as_bool());}
};
/*--------------------------------------------------------------------------*/
class Item_Base
  :public Base
{
public:
  virtual const char*	   type_name()const   = 0;
  virtual void reduce_push(Macro_Body*, const Instance&)const = 0;
};
/*--------------------------------------------------------------------------*/
class Macro_Body
  :public List_Base<Item_Base>
{
  Symbol_Table*	    _sym;
  const Macro_List* _ml;
  std::string	    _scope;
public:
  void parse(CS&);
  void if_body(CS&);
  // real default constructor -- error if it ever gets called
  explicit Macro_Body() {unreachable();}
  // "default" constructor -- as close as it gets, init scope data
  explicit Macro_Body(Symbol_Table* s, const Macro_List* ml,
		      const std::string& scope)
    : _sym(s), _ml(ml), _scope(scope) {}
  // alternate "default" constructor -- just inherits scope data
  explicit Macro_Body(int, const Macro_Body* m)
    : _sym(m->_sym), _ml(m->_ml), _scope(m->_scope) {}
  // parsing constructor -- builds from "file", inherits scope data
  explicit Macro_Body(CS& file, const Macro_Body* m)
    : _sym(m->_sym), _ml(m->_ml), _scope(m->_scope) {parse(file);}
  // reducing copy constructor -- makes a reduced copy, defined by Instance
  explicit Macro_Body(const Macro_Body& m, const Instance&);
  void reduce_push(Macro_Body*, const Instance&)const;

  Symbol_Table*		sym()const	{return _sym;}
  const Macro_List*	ml()const	{return _ml;}
  const std::string&	scope()const	{return _scope;}
};
/*--------------------------------------------------------------------------*/
class Port_List
  :public Base
{
  Symbol_Table*			_sym;
  Symbol_Mode   		_mode;
  int				_nominal_size;
  std::list<const std::string*>	_list;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  enum {SIZE_UNKNOWN = -1};
  Port_List() {unreachable();}
  Port_List(const Port_List& p)
    :Base(), _sym(p._sym), _mode(p._mode), _nominal_size(p._nominal_size),
     _list(p._list) {}
  Port_List(Symbol_Table* s, const Macro_List*, Symbol_Mode mode,
	    int nom_size=SIZE_UNKNOWN)
    : _sym(s), _mode(mode), _nominal_size(nom_size) {}
  ~Port_List() {for (std::list<const std::string*>::iterator
	       i = _list.begin(); i != _list.end(); ++i) {/*delete *i;*/}}
  // BUG:: memory leak??
  typedef std::list<const std::string*>::const_iterator const_iterator;
  const_iterator begin()const	{return _list.begin();}
  const_iterator end()const	{return _list.end();}
  int		 length()const	{return _list.size();}
  bool		 is_empty()const{return length() == 0;}
};
/*--------------------------------------------------------------------------*/
class Ckt_Block
  :public Item_Base
{
protected:
  std::string _name;
  Port_List   _ports;
  std::string _lvalue;
  Expression  _value;
  void parse(CS&);
private:
  void dump(std::ostream& o)const;
protected:
  explicit Ckt_Block(CS& file, const Macro_Body* m, int nodes);
  explicit Ckt_Block(const Ckt_Block& p, const Instance& i)
    :_name(p._name),_ports(p._ports),_lvalue(p._lvalue),_value(p._value,i) {}
public:
  const std::string&	name()const	{return _name;}
  const Port_List&	ports()const	{return _ports;}
  const std::string&	lvalue()const	{return _lvalue;}
  const Expression&	value()const	{return _value;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Alarm
  :public Ckt_Block
{
  const char* type_name()const {return "alarm";}
  explicit Alarm(const Alarm& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Alarm(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,0) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Capacitor
  :public Ckt_Block
{
  const char* type_name()const {return "capacitor";}
  explicit Capacitor(const Capacitor& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Capacitor(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Driver
  :public Ckt_Block
{
  Expression _s0;
  Expression _s1;
  Expression _w01;
  Expression _w10;
  Ramp_Name  _ramp;
  const char* type_name()const {return "driver";}
  void parse(CS&);
  void dump(std::ostream& o)const;
  explicit Driver(const Driver& p, const Instance& i)
    :Ckt_Block(p, i), _s0(p._s0, i), _s1(p._s1, i), _w01(p._w01, i),
     _w10(p._w10, i), _ramp(p._ramp, i) {}
public:
  explicit Driver(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,7), _s0(m), _s1(m), _w01(m), _w10(m),
      _ramp(m->sym()) {parse(file);}
  const Expression& s0()const	{return _s0;}
  const Expression& s1()const	{return _s1;}
  const Expression& w01()const	{return _w01;}
  const Expression& w10()const	{return _w10;}
  const Ramp_Name&  ramp()const	{return _ramp;}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Dsource
  :public Ckt_Block
{
  const char* type_name()const {return "dsource";}
public:
  explicit Dsource(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Inductor
  :public Ckt_Block
{
  const char* type_name()const {return "inductor";}
  explicit Inductor(const Inductor& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Inductor(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Inverter
  :public Ckt_Block
{
  const char* type_name()const {return "inverter";}
  explicit Inverter(const Inverter& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Inverter(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,3) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Reshape
  :public Ckt_Block
{
  std::string _rise_on;
  std::string _rise_off;
  std::string _fall_on;
  std::string _fall_off;
  const char* type_name()const {return "reshape";}
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Reshape(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,4) {parse(file);}
  const std::string&	rise_on()const	{return _rise_on;}
  const std::string&	rise_off()const	{return _rise_off;}
  const std::string&	fall_on()const	{return _fall_on;}
  const std::string&	fall_off()const	{return _fall_off;}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class T_Line
  :public Ckt_Block
{
  Expression _z0;
  Expression _delay;
  const char* type_name()const {return "tline";}
  void parse(CS&);
  void dump(std::ostream& o)const;
  explicit T_Line(const T_Line& p, const Instance& i)
    :Ckt_Block(p, i), _z0(p._z0, i), _delay(p._delay, i) {}
public:
  explicit T_Line(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,4), _z0(m), _delay(m) {parse(file);}
  const Expression&	z0()const	{return _z0;}
  const Expression&	delay()const	{return _delay;}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Resistor
  :public Ckt_Block
{
  const char* type_name()const {return "resistor";}
  explicit Resistor(const Resistor& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Resistor(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Subckt
  :public Ckt_Block
{
  Symbol_Table* _sym;
  std::string _subckt;
  const char* type_name()const {return "subckt";}
  void parse(CS&);
  void dump(std::ostream& o)const;
  explicit Subckt(const Subckt& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Subckt(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,Port_List::SIZE_UNKNOWN), _sym(m->sym()) {parse(file);}
  const std::string&	subckt()const	{return _subckt;}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Trigger
  :public Ckt_Block
{
  Symbol_Table* _sym;
  const char* type_name()const {return "trigger";}
  void parse(CS&);
  //void dump(std::ostream& o)const;
  explicit Trigger(const Trigger& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Trigger(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,0), _sym(m->sym()) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class VCG
  :public Ckt_Block
{
  const char* type_name()const {return "vcg";}
  explicit VCG(const VCG& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit VCG(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,4) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class VCVS
  :public Ckt_Block
{
  const char* type_name()const {return "vcvs";}
  explicit VCVS(const VCVS& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit VCVS(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,4) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class VCCS
  :public Ckt_Block
{
  const char* type_name()const {return "vccs";}
  explicit VCCS(const VCCS& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit VCCS(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,4) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Vsource
  :public Ckt_Block
{
  const char* type_name()const {return "vsource";}
  explicit Vsource(const Vsource& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Vsource(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Isource
  :public Ckt_Block
{
  const char* type_name()const {return "isource";}
  explicit Isource(const Isource& p, const Instance& i) :Ckt_Block(p, i) {}
public:
  explicit Isource(CS& file, const Macro_Body* m)
    : Ckt_Block(file,m,2) {parse(file);}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Assert
  :public Item_Base
{
  Test _test;
  const char* type_name()const {return "assert";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Assert(CS& file, const Macro_Body* m) : _test(file, m) {}
  const Test& test()const		   {return _test;}
  bool	      test(const Instance& i)const {return test().eval(i);}
};
/*--------------------------------------------------------------------------*/
class Define
  :public Item_Base
{
  std::string _name;
  Expression  _value;
  const char* type_name()const {return "define";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
  explicit Define(const Define& p, const Instance& i)
    :_name(p._name), _value(p._value, i) {}
public:
  explicit Define(CS& file, const Macro_Body* m);
  const std::string& name()const	{return _name;}
  const Expression&  value()const	{return _value;}
};
/*--------------------------------------------------------------------------*/
class Export
  :public Item_Base
{
  Tail_String _string;
  const char* type_name()const {return "export";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Export(CS& file, const Macro_Body* m);
};
/*--------------------------------------------------------------------------*/
class Foreach
  :public Item_Base
{
  std::string _name;
  std::string _iterator;
  Macro_Body  _body;
  Macro_Body  _false_part;
  const char* type_name()const {return "foreach";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Foreach(CS& file, const Macro_Body* m);
  const std::string& name()const	{return _name;}
  const std::string& iterator()const	{return _iterator;}
  const Macro_Body&  body()const	{return _body;}
  const Macro_Body&  false_part()const	{return _false_part;}
};
/*--------------------------------------------------------------------------*/
class If
  :public Item_Base
{
  Test	     _test;
  Macro_Body _true_part;
  Macro_Body _false_part;
  const char* type_name()const {return "if";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit If(CS& file, const Macro_Body* m);
  const Test&	    test()const			 {return _test;}
  bool		    test(const Instance& i)const {return test().eval(i);}
  const Macro_Body& true_part()const		 {return _true_part;}
  const Macro_Body& false_part()const		 {return _false_part;}
};
/*--------------------------------------------------------------------------*/
class Inherit
  :public Item_Base
{
  const Macro* _macro;
  Tail_String _string;
  const char* type_name()const {return "inherit";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Inherit(CS& file, const Macro_Body* m);
  const Macro*	   macro()const {assert(_macro); return _macro;}
};
/*--------------------------------------------------------------------------*/
class Declare
  :public Item_Base
{
  Symbol_Type  _type;
  Tail_String  _string;
  const char* type_name()const {untested(); return c_str(_type);}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Declare(CS& file, const Macro_Body* m, Symbol_Type t);
};
/*--------------------------------------------------------------------------*/
class Case
  :public Item_Base
{
  int		_here;
  Tail_String	_name;
  Macro_Body	_body;
  const char* type_name()const {return "case";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
public:
  explicit Case(CS& file, const Macro_Body* m);
  const std::string& name()const {return _name;}
  const Macro_Body&  body()const {return _body;}
  void reduce_push(Macro_Body*, const Instance&)const;
};
/*--------------------------------------------------------------------------*/
class Select
  :public Item_Base, public List_Base<Case>
{
  std::string _control;
  const char* type_name()const {return "select";}
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const;
  void reduce_push(Macro_Body*, const Instance&)const;
public:
  explicit Select(CS& file, const Macro_Body* m);
  const std::string& control()const {return _control;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Macro
  :public Base
{
  std::string	_type;
  std::string	_name;
  Symbol_Table	_sym;
  Port_List	_ports;
  Macro_Body	_body;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Macro(CS& file, const Macro_List* ml, const std::string& scope)
    :_ports(&_sym, ml, mDECLARE), _body(&_sym, ml, scope) {parse(file);}
  explicit Macro(const Macro& m, const Instance& i)  // reducing constructor
    :_type(m._type), _name(m._name), _sym(), _ports(m._ports),
     _body(m._body, i) {}
  void reduce_push(Macro_Body*, const Instance&)const;
  const std::string&	type()const	{return _type;}
  const std::string&	name()const	{return _name;}
  const Symbol_Table&	symbols()const	{return _sym;}
  const Port_List&	ports()const	{return _ports;}
  const Macro_Body&	body()const	{return _body;}
};
/*--------------------------------------------------------------------------*/
class Macro_List
  :public List_Base<Macro>
{
  mutable std::map<std::string, const Macro*> _map;
  void parse(CS&);
  //void dump(std::ostream& o)const; inherited
public:
  explicit Macro_List() {};
  const Macro* operator[](const std::string& name)const {return _map[name];}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
