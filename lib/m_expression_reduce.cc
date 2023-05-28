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
  }else{
    // op (name()) not one of those listed
    unreachable();
    return NULL;
  }
  if (b) {
    if (T1->aRgs() == "") {
    }else{untested();
    }
    if (T2->aRgs() == "") {
    }else{untested();
    }
    return new Token_CONSTANT(b->val_string(), b, (T1->aRgs()+T2->aRgs()));
  }else{
    // can get here if either T1 or T2 has no data
    return new Token_CONSTANT("false", NULL, "");
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
  }else{
    // op (name()) not one of those listed
    unreachable();
    return NULL;
  }
  if (b) {
    if (T1->aRgs() == "") {
    }else{untested();
    }
    return new Token_CONSTANT(b->val_string(), b, (T1->aRgs()));
  }else{untested();
    // can get here if T1 has no data
    return new Token_CONSTANT("false", NULL, "");
  }
}
/*--------------------------------------------------------------------------*/
void Token_SYMBOL::stack_op(Expression* E)const
{
  assert(E);
  // replace single token with its value
  if (!E->is_empty() && dynamic_cast<const Token_PARLIST*>(E->back())) {
    // has parameters (table or function)
    if (FUNCTION* f = function_dispatcher[name()]) {
      const Token* T1 = E->back(); // Token_PARLIST
      assert(T1->data()); // expression
      Base const* d = T1->data();
      auto ee = prechecked_cast<Expression const*>(d);
      assert(ee);

      // Expression fee = f->ev(ee); ?

      bool all_float = true;
      for (Expression::const_iterator i = ee->begin(); i != ee->end(); ++i) {
	trace1("float?", (**i).name());
	all_float = dynamic_cast<Float const*>((**i).data());
	if(!all_float){
	  break;
	}else{
	}
      }

      if(all_float){
	// function call as usual
	CS cmd(CS::_STRING, T1->name());
	std::string value = f->eval(cmd, E->_scope);
	E->pop_back();
	const Float* v = new Float(value);
	E->push_back(new Token_CONSTANT(value, v, ""));
      }else{
	// restore argument.
	E->pop_back();
	E->push_back(new Token_STOP("stopname"));
	for (Expression::const_iterator i = ee->begin(); i != ee->end(); ++i) {
	  trace1("stackop restore arg", (**i).name());
	  (**i).stack_op(E);
	}
	E->push_back(new Token_PARLIST(".."));
	E->push_back(clone());
      }
      delete T1;
    }else{
      throw Exception_No_Match(name()); //BUG// memory leak
      unreachable();
      E->push_back(clone());
    }
  }else{
    // has no parameters (scalar)
    if (strchr("0123456789.", name()[0])) {
      // a number
      Float* n = new Float(name());
      trace1("found number", name());
      E->push_back(new Token_CONSTANT(name(), n, ""));
    }else{
      // a name
      PARAMETER<double> p = (*(E->_scope->params()))[name()];
      if (p.has_hard_value()) {
	CS cmd(CS::_STRING, p.string());
	Expression pp(cmd);
	Expression e(pp, E->_scope);
	double v = e.eval();

	if(v!=NOT_INPUT){
	  // it's a float constant.
	  Float* n = new Float(v);
	  E->push_back(new Token_CONSTANT(n->val_string(), n, ""));
	}else{
	  // not a float. keep expression
	  for (Expression::const_iterator i = e.begin(); i != e.end(); ++i) {
	    E->push_back(*i);
	  }
	  // disown
	  while (e.size()){
	    e.pop_back();
	  }
	}
      }else{
	// no value - push name (and accept incomplete solution later)
#if 1
	String* s = new String(name());
	E->push_back(new Token_CONSTANT(name(), s, ""));
#else
	E->push_back(new Token_SYMBOL(name(), ""));	
#endif
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void Token_TERNARY::stack_op(Expression* E)const
{
  assert(E);
  Token const* t = E->back();
  auto constant = dynamic_cast<Token_CONSTANT const*>(t);

  bool is_float = false;
  if(constant){
    is_float = dynamic_cast<Float const*>(constant->data());
  }else{
  }

  assert(true_part());
  assert(false_part());
  if (is_float) {
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
    Expression* t = new Expression(*true_part(), E->_scope);
    Expression* f = new Expression(*false_part(), E->_scope);
    E->push_back(new Token_TERNARY(name(), t, f));
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
	if (strchr("+*", name()[0]) && !dynamic_cast<const Float*>(t1->data())) {
	  // change order to enable later optimization
	  E->push_back(t1);
	  E->push_back(t2);
	}else{
	  E->push_back(t2);
	  E->push_back(t1);
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
void Token_PARLIST::stack_op(Expression* E)const
{
  assert(E);
  std::stack<Token*> stack;
  auto arg_exp = new Expression();
  // replace multiple tokens of a PARLIST with a single token
  bool been_here = false;
  std::string tmp;//(")");
  for (;;) {
    Token* t = E->back();
    E->pop_back();
    if (dynamic_cast<const Token_STOP*>(t)) {
      // tmp = "(" + tmp;
      delete t;
      break;
    }else{
      if (been_here) {
	tmp = ", " + tmp;
      }else{
	been_here = true;
      }
      tmp = t->name() + tmp;
      stack.push(t);
      // arg_exp->push_back(t);
    }
  }
  // turn over (there is no push_front, maybe on purpose)
  while(!stack.empty()){
    trace1("pushing", stack.top()->name());
    arg_exp->push_back(stack.top());
    stack.pop();
  }
  auto parlist = new Token_PARLIST(tmp, arg_exp);
  E->push_back(parlist);
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
  trace3("stackop constant", name(), aRgs(), dynamic_cast<Float const*>(data()));

  assert(E);
  if(auto f = dynamic_cast<Float const*>(data())){
    E->push_back(new Token_CONSTANT(name(), new Float(*f), aRgs()));
  }else if(auto s = dynamic_cast<String const*>(data())){
    E->push_back(new Token_CONSTANT(name(), new String(*s), aRgs()));
  }else{
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
    trace2("reducecopy", (**i).name(), (**i).data() );
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
