/*$Id: expression.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
class Symbol_Table;
class Macro_Body;
class Instance;
#define NOTHING 0
/*--------------------------------------------------------------------------*/
class Token
  :public Base
{
public:
  enum Type {UNKNOWN, SYMBOL, BINOP, STOP, PARLIST, UNARY, CONSTANT};
private:
  std::string _name;
  Type	      _type;
  const Base* _data;
  std::string _args;
  void parse(CS&) {unreachable();}
  void dump(std::ostream&)const;
public:
  explicit Token(const std::string& n, Type t, const Base* d,
		 const std::string& a)
    : _name(n), _type(t), _data(d), _args(a) {}
  explicit Token(const Token& p)
    : Base(), _name(p._name), _type(p._type), _data(p._data), _args(p._args) {}
  const std::string& name()const {return _name;}
  Type		     type()const {return _type;}
  const Base*	     data()const {return _data;}
  const std::string& args()const {return _args;}
  const std::string  full_name()const {return name() + args();}
  Token*	     op(const Token* t1, const Token* t2)const;
  Token*	     op(const Token* t1)const;
};
/*--------------------------------------------------------------------------*/
class Expression
  :public List_Base<Token>
{
private:
  Symbol_Table* _sym;
  std::string _scope;
private: // override virtual
  void parse(CS&);
  void dump(std::ostream&)const;
private: // expression-in.cc
  void arglisttail(CS& file);
  void arglist(CS& file);
  void leaf(CS& file);
  void factor(CS& file);
  void term(CS& file);
  void addexp(CS& file);
  void logical(CS& file);
  void expression(CS& file);
public:
  explicit Expression(const Macro_Body* m);
private: // expression-reduce.cc
  void reduce_copy(const Expression&, const Instance&);
public:
  explicit Expression(const Expression&, const Instance&);
public: // other
  bool as_bool()const {return (!is_empty() && back()->data());}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
