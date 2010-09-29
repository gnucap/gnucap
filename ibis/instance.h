/*$Id: instance.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "base.h"
/*--------------------------------------------------------------------------*/
class Symbol_Table; //external
class Macro; //external
class Macro_List; //external
class Instance_List; //forward
/*--------------------------------------------------------------------------*/
class Keyword
  :public Base
{
private:
  std::string _short_name;
  void parse(CS&)  {unreachable();}
  void dump(std::ostream& o)const {o << _short_name;}
  std::string val_string()const	 {return _short_name;}
public:
  Keyword(const std::string& name) :_short_name(name) {}
};
/*--------------------------------------------------------------------------*/
class Scope
  :public List_Base<Base>
{
public:
  mutable std::map<std::string, const Base*> _map;
protected:
  const Symbol_Table* _symbols;
  std::string _base_scope;
  void parse(CS&);
public:
  explicit Scope() {unreachable();}
  explicit Scope(CS& file, const Symbol_Table* symbols,
		 const std::string& base_scope)
    : _symbols(symbols), _base_scope(base_scope) {parse(file);}
  const Base* operator[](const std::string& s)const {return _map[s];}
};
/*--------------------------------------------------------------------------*/
class Foreach_Item
  :public Base
{
private:
  std::vector<std::string> _args;
  
  void parse(CS&);
  void dump(std::ostream&)const;
public:
  explicit Foreach_Item(CS& file) {parse(file);}
  typedef std::vector<std::string>::const_iterator const_iterator;
  const_iterator begin()const	{return _args.begin();}
  const_iterator end()const	{return _args.end();}
};
/*--------------------------------------------------------------------------*/
class Foreach_Data
  :public List<Foreach_Item>
{
private:
  std::string	_short_name;
  void dump(std::ostream&)const;
public:
  explicit Foreach_Data(CS& file, const std::string& short_name)
    : _short_name(short_name) {parse(file);}
  const std::string& short_name()const	{return _short_name;}
};
/*--------------------------------------------------------------------------*/
class String_Var
  :public String
{
private:
  std::string _short_name;
  void parse(CS&);
  void dump(std::ostream&)const;
public:
  explicit String_Var(CS& file, const std::string& short_name)
    : _short_name(short_name) {parse(file);}
  explicit String_Var(const std::string& short_name, const std::string& value)
    : String(value), _short_name(short_name) {}
  const std::string& short_name()const	{return _short_name;}
  const std::string& value()const	{return _data;}
};
/*--------------------------------------------------------------------------*/
class Scalar
  :public Float
{
private:
  std::string _short_name;
  void parse(CS&);
  void dump(std::ostream&)const;
public:
  explicit Scalar() {}
  explicit Scalar(CS& file, const std::string& short_name)
    : _short_name(short_name) {parse(file);}
  explicit Scalar(const std::string& short_name, const std::string& value)
    : Float(value), _short_name(short_name) {}
  const std::string& short_name()const	{return _short_name;}
};
/*--------------------------------------------------------------------------*/
class Table
  :public Vector
{
private:
  Scope _s;
  std::string	_short_name;
  void dump(std::ostream&)const;
public:
  explicit Table(CS& file, const Symbol_Table* symbols,
		  const std::string& long_name, const std::string& short_name)
    : _s(file, symbols, long_name), _short_name(short_name)  {parse(file);}
  const std::string& short_name()const	{return _short_name;}  
};
/*--------------------------------------------------------------------------*/
class Ramp_Data
  :public Base
{
private:
  std::string	  _short_name;
  Range<Fraction> _dv_dt_r;
  Range<Fraction> _dv_dt_f;
  Float		  _r_load;
  void parse(CS&);
  void dump(std::ostream&)const;
public:
  explicit Ramp_Data() {unreachable();}
  explicit Ramp_Data(CS& file, const std::string& short_name)
    : _short_name(short_name) {parse(file);}
  const std::string& short_name()const	{return _short_name;}  
  const Range<Fraction>& dv_dt_r()const {return _dv_dt_r;}
  const Range<Fraction>& dv_dt_f()const {return _dv_dt_f;}
  double		 r_load()const	{return _r_load;}
};
/*--------------------------------------------------------------------------*/
class Instance
  :public Scope
{
private:
  const Macro* _proto;
  const Macro* _reduced;
  std::string _base_type;
  std::string _sub_type;
  std::string _full_name;

  //void parse(CS&); inherited
  void dump(std::ostream&)const;
public:
  explicit Instance(CS& file, const Macro* macro, const std::string& type,
  		    const std::string& name, const std::string& sub_type);
  const Macro*	      proto()const	{return _proto;}
  const Macro*	      reduced()const	{return _reduced;}
  const std::string   base_type()const	{return _base_type;}
  const std::string   sub_type()const	{return _sub_type;}
  const std::string   full_name()const	{return _full_name;}
};
/*--------------------------------------------------------------------------*/
class Instance_List
  :public List_Base<Instance>
{
private:
  const Macro_List* _macros;
  mutable std::map<std::string, const Instance*> _map;
  void parse(CS&);
public:
  Instance_List(const Macro_List* ml=0) : _macros(ml) {assert(ml);}
  //const Instance* operator[](const std::string& name)const
  //	{untested(); return _map[name];}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
