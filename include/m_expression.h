/*$Id: m_expression.h 2014/07/04 al $ -*- C++ -*-
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
//testing=script 2014.07.04
#ifndef M_EXPRESSION_H
#define M_EXPRESSION_H
#include "m_base.h"
/*--------------------------------------------------------------------------*/
//class Symbol_Table;
class CARD_LIST;
class Expression;
/*--------------------------------------------------------------------------*/
class Token : public Base {
private:
  std::string _name;
  const Base* _data;
public:
  void parse(CS&) override {unreachable();}
public:
  void dump(std::ostream&)const override;
protected:
  explicit Token(std::string Name, const Base* Data, std::string Args="")
    : _name(Name), _data(Data) {assert(Args==""); }
  explicit Token(const Token& P)
    : Base(), _name(P._name), _data(P._data) {assert(!_data);}
public:
  virtual ~Token()   { delete _data; _data=nullptr; }
  virtual Token*     clone()const = 0;
  const std::string& name()const {return _name;}
  const Base*	     data()const {return _data;}
  const std::string& full_name()const {return name();}
  virtual void	     stack_op(Expression*)const {unreachable();}
  bool operator==(const Token& P) {return (typeid(*this)==typeid(P))
      && (data()==P.data()) && (name()==P.name());}
};
/*--------------------------------------------------------------------------*/
class Token_SYMBOL : public Token {
protected:
  explicit Token_SYMBOL(std::string Name, Base const* Data)
    : Token(Name, Data) {}
public:
  explicit Token_SYMBOL(std::string Name, std::string Args="")
    : Token(Name, nullptr) {assert(Args=="");}
  explicit Token_SYMBOL(const Token_SYMBOL& P) : Token(P) {}
  Token* clone()const  override{return new Token_SYMBOL(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_BINOP : public Token {
protected:
  explicit Token_BINOP(std::string Name, Base const* Data)
    : Token(Name, Data) {}
public:
  explicit Token_BINOP(std::string Name)
    : Token(Name, nullptr) {}
  explicit Token_BINOP(const Token_BINOP& P) : Token(P) {}
  Token* clone()const override{return new Token_BINOP(*this);}
  Token* op(const Token* t1, const Token* t2)const;
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_TERNARY : public Token {
  Expression const* _true{nullptr};
  Expression const* _false{nullptr};
protected:
  explicit Token_TERNARY(std::string Name, Base const* Data)
    : Token(Name, Data) {}
public:
  explicit Token_TERNARY(std::string Name, Expression const* t, Expression const* f)
    : Token(Name, nullptr), _true(t), _false(f) {}
  explicit Token_TERNARY(const Token_TERNARY& P) : Token(P) {}
  ~Token_TERNARY();
  Token* clone()const override{return new Token_TERNARY(*this);}
  Token* op(const Token* t1, const Token* t2, const Token* t3)const;
  void stack_op(Expression*)const override;
public:
  Expression const* true_part() const{ return _true; }
  Expression const* false_part() const{ return _false; }
};
/*--------------------------------------------------------------------------*/
class Token_STOP : public Token {
protected:
  explicit Token_STOP(std::string Name, Base const* Data)
    : Token(Name, Data) {untested();}
public:
  explicit Token_STOP(std::string Name)
    : Token(Name, nullptr) {}
  explicit Token_STOP(const Token_STOP& P) : Token(P) {}
  Token* clone()const override{return new Token_STOP(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_ARRAY : public Token {
protected:
  explicit Token_ARRAY(std::string Name, Base const* Data)
    : Token(Name, Data) {untested();}
public:
  explicit Token_ARRAY(std::string Name, Base* L=nullptr)
    : Token(Name, L) {}
  explicit Token_ARRAY(const Token_ARRAY& P) : Token(P) {}
  Token* clone()const override{return new Token_ARRAY(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_PARLIST : public Token {
protected:
  explicit Token_PARLIST(std::string Name, Base const* Data)
    : Token(Name, Data) {untested();}
public:
  explicit Token_PARLIST(std::string Name, Base* L=nullptr)
    : Token(Name, L) {}
  explicit Token_PARLIST(const Token_PARLIST& P) : Token(P) {}
  Token* clone()const override{return new Token_PARLIST(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_UNARY : public Token {
protected:
  explicit Token_UNARY(std::string Name, Base const* Data)
    : Token(Name, Data) {itested();}
public:
  explicit Token_UNARY(std::string Name)
    : Token(Name, nullptr) {}
  explicit Token_UNARY(const Token_UNARY& P) : Token(P) {itested();}
  Token* clone()const override {itested();return new Token_UNARY(*this);}
  Token* op(const Token* t1)const;
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_CONSTANT : public Token {
public:
  explicit Token_CONSTANT(std::string Name, Base const* Data, std::string Args="")
    : Token(Name, Data) {assert(Args=="");}
  explicit Token_CONSTANT(const Token_CONSTANT& P) : Token(P) {untested();}
  Token* clone()const override { untested();
    if (auto s = dynamic_cast<String const*>(data())) {itested();
      return new Token_CONSTANT(name(), new String(*s)); // BUG?
    }else{ untested();
      return new Token_CONSTANT(*this);
    }
  }
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class INTERFACE Expression : public List_Base<Token> {
public:
  const CARD_LIST* _scope;
public:
  void parse(CS&) override;
  void dump(std::ostream&)const override;
private: // expression-in.cc
  void arglisttail(CS& File);
  void arglist(CS& File);
  CS& array(CS& File);
  void leaf(CS& File);
  void factor(CS& File);
  void ternary(CS& File);
  void termtail(CS& File);
  void term(CS& File);
  void addexptail(CS& File);
  void addexp(CS& File);
  void logicaltail(CS& File);
  void logical(CS& File);
  void andtail(CS& File);
  void andarg(CS& File);
  void exptail(CS& File);
  void expression(CS& File);
public:
  explicit Expression() : _scope(nullptr) {}
  explicit Expression(CS& File) : _scope(nullptr) {parse(File);}
private: // expression-reduce.cc
  void reduce_copy(const Expression&);
public:
  explicit Expression(const Expression&, const CARD_LIST*);
public: // other
  bool as_bool()const {untested();return (!is_empty() && back()->data());}
  double eval()const {
    if(auto f = dynamic_cast<const Float*>(back()->data())) {
      return (size()==1) ? (f->value()) : NOT_INPUT;
    }else if(auto i = dynamic_cast<const Integer*>(back()->data())) {
      return (size()==1) ? (i->value()) : NOT_INPUT;
    }else{
      return NOT_INPUT; // bug, why double?
    }
  }
  void clear() {
    while(size()){
      delete back();
      pop_back();
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
