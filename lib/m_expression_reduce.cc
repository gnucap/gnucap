/*                               -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
 *               2023 Felix Salfelder
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
 * Constructs a simplified (reduced) version of an expression.
 * Incomplete:
 *	Expressions in arg-lists print as ####false####
 *	Not sure what should happen when expression is empty
 */
//testing=script,sparse 2009.08.12
#include "globals.h"
#include "u_function.h"
#include "u_parameter.h"
#include <stack>
/*--------------------------------------------------------------------------*/
Token* Token_BINOP::op(const Token* T1, const Token* T2)const
{
  assert(T1);
  assert(T2);
  assert(dynamic_cast<const Token_CONSTANT*>(T1));
  assert(dynamic_cast<const Token_CONSTANT*>(T2));
  assert(T1->data());
  assert(T2->data());

  // not "or" operator, both ops exist -- do it
  Base* b = 0;
  if (name() == "*") {
    b = (T1->data())->multiply(T2->data());
  }else if (name() == "+") {
    b = (T1->data())->add(T2->data());
  }else if (name() == "-") {
    b = (T1->data())->subtract(T2->data());
  }else if (name() == "/") {
    b = (T1->data())->divide(T2->data());
  }else if (name() == "%") {
    b = (T1->data())->modulo(T2->data());
  }else if (name() == "==") {
    b = (T1->data())->equal(T2->data());
  }else if (name() == "!=") {
    b = (T1->data())->not_equal(T2->data());
  }else if (name() == "<") {
    b = (T1->data())->less(T2->data());
  }else if (name() == ">") {
    b = (T1->data())->greater(T2->data());
  }else if (name() == "<=") {
    b = (T1->data())->leq(T2->data());
  }else if (name() == ">=") {
    b = (T1->data())->geq(T2->data());
  }else if (name() == "||") {
    b = (T1->data())->logic_or(T2->data());
  }else if (name() == "&&") {
    b = (T1->data())->logic_and(T2->data());
  }else{ untested();
    // op (name()) not one of those listed
    unreachable();
    return nullptr;
  }
  if (b) {
    return new Token_CONSTANT(b);
  }else{
    // can get here if either T1 or T2 has no data
    return new Token_CONSTANT(nullptr);
  }
}
/*--------------------------------------------------------------------------*/
Token* Token_UNARY::op(const Token* T1)const
{
  assert(T1);
  assert(dynamic_cast<const Token_CONSTANT*>(T1));
  assert(T1->data());
  
  const Base* b = 0;
  if (name() == "-") {
    b = (T1->data())->minus();
  }else if (name() == "+") {
    b = (T1->data())->plus();
  }else if (name() == "!") {
    b = (T1->data())->logic_not();
  }else{ untested();
    // op (name()) not one of those listed
    unreachable();
    return nullptr;
  }
  if (b) {
  }else{untested();
    // can get here if T1 has no data
  }
  return new Token_CONSTANT(b);
}
/*--------------------------------------------------------------------------*/
static Base* eval_base(PARAM_INSTANCE const& p, Expression const& e)
{ untested();
  if(dynamic_cast<PARAMETER<double> const*>(*p)){ untested();
    Base const* v = e.value();
    if(auto f = dynamic_cast<Float const*>(v)){ untested();
      return new Float(f->value());
    }else if(auto i = dynamic_cast<Integer const*>(v)){ untested();
      return new Float(i->value());
    }else{ untested();
      trace1("name lost?", p.string());
    }
  }else if(dynamic_cast<PARAMETER<int> const*>(*p)) {
    Base const* v = e.value();
    if(auto f = dynamic_cast<Float const*>(v)){ untested();
      return new Integer(int32_t(f->value()));
    }else if(auto i = dynamic_cast<Integer const*>(v)){
      return new Integer(i->value());
    }else{ untested();
    }
  }else if(dynamic_cast<PARAMETER<bool> const*>(*p)) { untested();
    incomplete();
  }
  return nullptr;
}
/*--------------------------------------------------------------------------*/
void Token_SYMBOL::stack_op(Expression* E)const
{ untested();
  assert(E);
  // replace single token with its value
  if (!E->is_empty() && dynamic_cast<const Token_PARLIST*>(E->back())) {
    trace1("SYM stackop", name());
    // has parameters (table or function)
    if (FUNCTION* f = function_dispatcher[name()]) {
      try{
	f->stack_op(E);
      }catch (Exception const& e) {
	// didnt work. put back function token
	E->push_back(clone());
      }
    }else{
      throw Exception_No_Match(name()); //BUG// memory leak
      unreachable();
      E->push_back(clone());
    }
  }else{
    // has no parameters (scalar)
    if (strchr("0123456789.", name()[0])) {
      // a number
      bool is_int = true;
      trace2("type", name(), name().size());
      std::string sn = name();
      for(std::string::const_iterator c = sn.begin();
	  is_int && c != sn.end(); ++c){
	trace3("type", name(), *c, isdigit(*c));
	is_int = isdigit(*c);
      }

      Base* n;
      if(is_int) {
	n = new Integer(name());
	trace1("found Integer", name());
      }else{
	n = new Float(name());
	trace1("found Float", name());
      }
      E->push_back(new Token_CONSTANT(n));
    }else{ untested();
      // a name
      PARAM_INSTANCE p = (*(E->_scope->params()))[name()];
      assert(name().size());
      // TODO? Token based PARAMETER
      // p.eval(E->_scope) ... ?
      if (p.has_hard_value()) { untested();
	trace1("hard value", name());
	assert((*(E->_scope->params()))[name()].has_hard_value());
	CS cmd(CS::_STRING, p.string());
	Expression pp(cmd);
	Expression e(pp, E->_scope);

	trace1("eval_base", name());
	Base* n = eval_base(p, e);
	trace1("/eval_base", name());

	if(n){ untested();
	  E->push_back(new Token_CONSTANT(n));
	}else{ untested();
	  // keep expression
	  for (Expression::const_iterator i = e.begin(); i != e.end(); ++i) {
	    E->push_back(*i);
	  }
	  // disown
	  while (e.size()){
	    e.pop_back();
	  }
	}

      }else{ untested();
	// no value - keep name (and accept incomplete solution later)
	trace1("no value", name());
	E->push_back(clone());
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
Token_TERNARY::~Token_TERNARY()
{
  delete _true;
  _true = nullptr;

  delete _false;
  _false = nullptr;
}
/*--------------------------------------------------------------------------*/
void Token_TERNARY::stack_op(Expression* E)const
{
  assert(E);
  Token const* t = E->back();
  auto constant = dynamic_cast<Token_CONSTANT const*>(t);

  bool is_num = false;
  if(constant){
    is_num = dynamic_cast<Float const*>(constant->data())
           ||dynamic_cast<Integer const*>(constant->data());
  }else{
  }

  assert(true_part());
  assert(false_part());
  if (is_num) {
    assert(constant->data());
    bool select = constant->data()->to_bool();
    delete t;
    E->pop_back();
    Expression const* sel;

    if(select){
      sel = true_part();
    }else{
      sel = false_part();
    }
    // E->reduce_copy(*sel);
    for (Expression::const_iterator i = sel->begin(); i != sel->end(); ++i) {
      (**i).stack_op(E);
    }

  }else{
    Expression* te = new Expression(*true_part(), E->_scope);
    Expression* fe = new Expression(*false_part(), E->_scope);
    E->push_back(new Token_TERNARY(te, fe));
  }
}
/*--------------------------------------------------------------------------*/
void Token_BINOP::stack_op(Expression* E)const
{
  assert(E);
  // replace 2 tokens (binop) with 1 (result)
  Token* t1 = E->back();
  E->pop_back();
  Token* t2 = E->back();
  E->pop_back();
  if (dynamic_cast<Token_CONSTANT*>(t1)) {
    if (dynamic_cast<Token_CONSTANT*>(t2)) {
      // have # # + .. becomes result (the usual)
      Token* t = op(t2, t1);
      assert(t);
      if (t->data()) {
	// success
	E->push_back(t);
	delete t2;
	delete t1;
      }else{
	// fail - one arg is unknown, push back args
	if (!strchr("+*", name()[0])) {
	  trace3("order unchanged0", t2->name(), name(), t1->name());
	  E->push_back(t2);
	  E->push_back(t1);
	}else if (dynamic_cast<const Integer*>(t1->data())) {
	  trace3("order unchanged1", t2->name(), name(), t1->name());
	  E->push_back(t2);
	  E->push_back(t1);
	}else if (dynamic_cast<const Float*>(t1->data())) {
	  trace3("order unchanged2", t2->name(), name(), t1->name());
	  E->push_back(t2);
	  E->push_back(t1);
	}else{
	  trace3("order changed", t2->name(), name(), t1->name());
	  // change order to enable later optimization
	  E->push_back(t1);
	  E->push_back(t2);
	}
	E->push_back(clone()); //op
	delete t;
      }
    }else if (((*t2) == (*this)) && strchr("+*", name()[0])
	      && dynamic_cast<Token_CONSTANT*>(E->back())) {
      // have # + # + .. becomes result + (previous unknown, try to optimize)
      Token* t3 = E->back();
      E->pop_back();
      Token* t = op(t3, t1);
      assert(t);
      if (t->data()) {
	// success
	E->push_back(t);
	E->push_back(t2);
	delete t3;
	delete t1;
      }else{
	// fail - push all
	E->push_back(t3);
	E->push_back(t2);
	E->push_back(t1);
	E->push_back(clone());
	delete t;
      }
    }else{
      // # - # - or something like that
      E->push_back(t2);
      E->push_back(t1);
      E->push_back(clone());
    }
  }else{
    // # - # - or something like that
    E->push_back(t2);
    E->push_back(t1);
    E->push_back(clone());
  }
}
/*--------------------------------------------------------------------------*/
void Token_STOP::stack_op(Expression* E)const
{
  assert(E);
  E->push_back(clone());
}
/*--------------------------------------------------------------------------*/
void Token_ARRAY::stack_op(Expression* E)const
{
  assert(E);
  E->push_back(clone());
  return;
}
/*--------------------------------------------------------------------------*/
void Token_PARLIST::stack_op(Expression* E)const
{
  assert(E);
  E->push_back(clone());
  return;
}
/*--------------------------------------------------------------------------*/
void Token_UNARY::stack_op(Expression* E)const
{
  assert(E);
  // replace 1 token with 1 (result)
  Token* t1 = E->back();
  E->pop_back();
  if (dynamic_cast<Token_CONSTANT*>(t1)) {
    Token* t = op(t1);
    assert(t);
    if (t->data()) {
      E->push_back(t);
      delete t1;
    }else{untested();
      E->push_back(t1);
      E->push_back(clone());
      delete t;
    }
  }else{itested();
    E->push_back(t1);
    E->push_back(clone());
  }
}
/*--------------------------------------------------------------------------*/
void Token_CONSTANT::stack_op(Expression* E)const
{
  // unreachable(); no. restoring arg expression??
  trace2("stackop constant", name(), dynamic_cast<Float const*>(data()));

  assert(E);
  if(auto f = dynamic_cast<Float const*>(data())){untested();
    E->push_back(new Token_CONSTANT(new Float(*f)));
  }else if(auto i = dynamic_cast<Integer const*>(data())){untested();
    E->push_back(new Token_CONSTANT(new Integer(*i)));
  }else if(auto s = dynamic_cast<String const*>(data())){
    E->push_back(new Token_CONSTANT(new String(*s)));
  }else{ untested();
    assert(false);
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Expression::reduce_copy(const Expression& Proto)
{
  // The Proto._list is the expression in RPN.
  // Attempt to build a reduced _list here, hopefully with only one item.
  for (const_iterator i = Proto.begin(); i != Proto.end(); ++i) {
    trace3("reducecopy", (**i).name(), (**i).name().size(), (**i).data() );
    trace1("reducecopy", dynamic_cast<const Token_CONSTANT*>(*i));
    trace1("reducecopy", dynamic_cast<const Token_SYMBOL*>(*i));
    (**i).stack_op(this);
  }
  if (is_empty()) {untested();
    assert(Proto.is_empty());
  }else{
  }
}
/*--------------------------------------------------------------------------*/
Expression::Expression(const Expression& Proto, const CARD_LIST* Scope)
  :_scope(Scope)
{
  //BUG// is this thread-safe?
  static int recursion = 0;
  static Expression const* first_name;

  if(recursion==0){
    first_name = &Proto;
  }else{
  }

  ++recursion;
  if (recursion <= OPT::recursion) {
    try{
      reduce_copy(Proto);
    }catch(Exception const& e){
      recursion = 0;
      throw e;
    }
    // first_name->dump(std::cerr);
  }else{
    std::stringstream s;
    first_name->dump(s);
    recursion = 0;
    //BUG// needs to show scope
    throw Exception("parameter " + s.str() + ": recursion too deep");
  }

  --recursion;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
