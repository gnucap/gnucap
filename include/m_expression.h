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
class Token
  :public Base
{
private:
  std::string _name;
  const Base* _data;
  std::string _aRgs;
public:
  void parse(CS&) override {unreachable();}
public:
  void dump(std::ostream&)const override;
protected:
  explicit Token(const std::string Name, const Base* Data, const std::string Args)
    : _name(Name), _data(Data), _aRgs(Args) {}
  explicit Token(const Token& P)
    : Base(), _name(P._name), _data(P._data), _aRgs(P._aRgs) {assert(!_data);}
public:
  virtual ~Token()   { delete _data; _data=NULL; }
  virtual Token*     clone()const = 0;
  const std::string& name()const {return _name;}
  const Base*	     data()const {return _data;}
  const std::string& aRgs()const {return _aRgs;}
  const std::string  full_name()const {return name() + aRgs();}
  virtual void	     stack_op(Expression*)const {unreachable();}
  bool operator==(const Token& P) {return (typeid(*this)==typeid(P))
      && (data()==P.data()) && (name()==P.name()) && (aRgs()==P.aRgs());}
};
/*--------------------------------------------------------------------------*/
class Token_SYMBOL : public Token
{
public:
  explicit Token_SYMBOL(const std::string Name, const std::string Args)
    : Token(Name, NULL, Args) {}
  explicit Token_SYMBOL(const Token_SYMBOL& P) : Token(P) {}
  Token* clone()const  override{return new Token_SYMBOL(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_BINOP : public Token
{
public:
  explicit Token_BINOP(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_BINOP(const Token_BINOP& P) : Token(P) {}
  Token* clone()const override{return new Token_BINOP(*this);}
  Token* op(const Token* t1, const Token* t2)const;
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_TERNARY : public Token {
  Expression const* _true{NULL};
  Expression const* _false{NULL};
public:
  explicit Token_TERNARY(const std::string Name, Expression const* t, Expression const* f)
    : Token(Name, NULL, ""), _true(t), _false(f) {}
  explicit Token_TERNARY(const Token_TERNARY& P) : Token(P) {}
  ~Token_TERNARY() { }
  Token* clone()const override{return new Token_TERNARY(*this);}
  Token* op(const Token* t1, const Token* t2, const Token* t3)const;
  void stack_op(Expression*)const override;
public:
  Expression const* true_part() const{ return _true; }
  Expression const* false_part() const{ return _false; }
};
/*--------------------------------------------------------------------------*/
class Token_STOP : public Token
{
public:
  explicit Token_STOP(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_STOP(const Token_STOP& P) : Token(P) {}
  Token* clone()const override{return new Token_STOP(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_PARLIST : public Token
{
public:
  explicit Token_PARLIST(const std::string Name, Base* L=NULL)
    : Token(Name, L, "") {}
  explicit Token_PARLIST(const Token_PARLIST& P) : Token(P) {itested();}
  Token* clone()const override{itested();return new Token_PARLIST(*this);}
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_UNARY : public Token
{
public:
  explicit Token_UNARY(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_UNARY(const Token_UNARY& P) : Token(P) {itested();}
  Token* clone()const override {itested();return new Token_UNARY(*this);}
  Token* op(const Token* t1)const;
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class Token_CONSTANT : public Token
{
public:
  explicit Token_CONSTANT(const std::string Name, const Base* Data, const std::string Args)
    : Token(Name, Data, Args) {}
  explicit Token_CONSTANT(const Token_CONSTANT& P) : Token(P) {untested();}
  Token* clone()const override {
    if (auto s = dynamic_cast<String const*>(data())) {itested();
      return new Token_CONSTANT(name(), new String(*s), aRgs());
    }else{ untested();
      return new Token_CONSTANT(*this);
    }
  }
  void stack_op(Expression*)const override;
};
/*--------------------------------------------------------------------------*/
class INTERFACE Expression
  :public List_Base<Token>
{
public:
  const CARD_LIST* _scope;
public:
  void parse(CS&) override;
  void dump(std::ostream&)const override;
private: // expression-in.cc
  void arglisttail(CS& File);
  void arglist(CS& File);
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
  explicit Expression() : _scope(NULL) {}
  explicit Expression(CS& File) : _scope(NULL) {parse(File);}
private: // expression-reduce.cc
  void reduce_copy(const Expression&);
public:
  explicit Expression(const Expression&, const CARD_LIST*);
public: // other
  bool as_bool()const {untested();return (!is_empty() && back()->data());}
  double eval()const {
    const Float* f = dynamic_cast<const Float*>(back()->data());
    return ((f && size()==1) ? (f->value()) : (NOT_INPUT));
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
