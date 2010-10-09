/*$Id: m_expression.h,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
//testing=script 2009.08.12
#include "m_base.h"
/*--------------------------------------------------------------------------*/
class Symbol_Table;
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
  void parse(CS&) {unreachable();}
public:
  void dump(std::ostream&)const;
protected:
  explicit Token(const std::string Name, const Base* Data, const std::string Args)
    : _name(Name), _data(Data), _aRgs(Args) {}
  explicit Token(const Token& P)
    : Base(), _name(P._name), _data(P._data), _aRgs(P._aRgs) {assert(!_data);}
public:
  virtual ~Token()   {if (_data) {delete _data;}else{}}
  virtual Token*     clone()const = 0;
  const std::string& name()const {return _name;}
  const Base*	     data()const {return _data;}
  const std::string& aRgs()const {return _aRgs;}
  const std::string  full_name()const {return name() + aRgs();}
  virtual void	     stack_op(Expression*)const {unreachable();}
  bool operator==(const Token& P) {untested();return (typeid(*this)==typeid(P))
      && (data()==P.data()) && (name()==P.name()) && (aRgs()==P.aRgs());}
};
/*--------------------------------------------------------------------------*/
class Token_SYMBOL : public Token
{
public:
  explicit Token_SYMBOL(const std::string Name, const std::string Args)
    : Token(Name, NULL, Args) {}
  explicit Token_SYMBOL(const Token_SYMBOL& P) : Token(P) {untested();}
  Token* clone()const {untested();return new Token_SYMBOL(*this);}
  void stack_op(Expression*)const;
};
class Token_BINOP : public Token
{
public:
  explicit Token_BINOP(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_BINOP(const Token_BINOP& P) : Token(P) {}
  Token* clone()const {return new Token_BINOP(*this);}
  Token* op(const Token* t1, const Token* t2)const;
  void stack_op(Expression*)const;
};
class Token_STOP : public Token
{
public:
  explicit Token_STOP(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_STOP(const Token_STOP& P) : Token(P) {}
  Token* clone()const {return new Token_STOP(*this);}
  void stack_op(Expression*)const;
};
class Token_PARLIST : public Token
{
public:
  explicit Token_PARLIST(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_PARLIST(const Token_PARLIST& P) : Token(P) {untested();}
  Token* clone()const {untested();return new Token_PARLIST(*this);}
  void stack_op(Expression*)const;
};
class Token_UNARY : public Token
{
public:
  explicit Token_UNARY(const std::string Name)
    : Token(Name, NULL, "") {}
  explicit Token_UNARY(const Token_UNARY& P) : Token(P) {untested();}
  Token* clone()const {untested();return new Token_UNARY(*this);}
  Token* op(const Token* t1)const;
  void stack_op(Expression*)const;
};
class Token_CONSTANT : public Token
{
public:
  explicit Token_CONSTANT(const std::string Name, const Base* Data, const std::string Args)
    : Token(Name, Data, Args) {}
  explicit Token_CONSTANT(const Token_CONSTANT& P) : Token(P) {untested();}
  Token* clone()const {untested();return new Token_CONSTANT(*this);}
  void stack_op(Expression*)const;
};
/*--------------------------------------------------------------------------*/
class INTERFACE Expression
  :public List_Base<Token>
{
public:
  const CARD_LIST* _scope;
public:
  void parse(CS&);
  void dump(std::ostream&)const;
private: // expression-in.cc
  void arglisttail(CS& File);
  void arglist(CS& File);
  void leaf(CS& File);
  void factor(CS& File);
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
  explicit Expression() : _scope(NULL) {untested();}
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
