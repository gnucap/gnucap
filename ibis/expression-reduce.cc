/*$Id: expression-reduce.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 * Reconstructs an infix expression from the RPN.
 * Incomplete:
 *	Expressions in arg-lists print as ####false####
 *	Not sure what should happen when expression is empty
 */
#include "macro.h"
/*--------------------------------------------------------------------------*/
Token* Token::op(const Token* t1, const Token* t2)const
{
  assert(t1);
  assert(t2);
  assert(type() == BINOP);

  {if (name() == "||") {
    // "or" operator -- take one
    {if (t1->data()) {
      return new Token(*t1);
    }else if (t2->data()) {
      return new Token(*t2);
    }else{
      return new Token("false", CONSTANT, 0, "");
    }}
  }else if (t1->data() && t2->data()) {
    // not "or" operator, both ops exist -- do it
    Base* b = 0;
    {if (name() == "*") {
      b = (t1->data())->multiply(*(t2->data()));
    }else if (name() == "+") {
      b = (t1->data())->add(*(t2->data()));
    }else if (name() == "-") {
      b = (t1->data())->subtract(*(t2->data()));
    }else if (name() == "/") {
      b = (t1->data())->divide_by(*(t2->data()));
    }else if (name() == "=") {
      b = (t1->data())->equal(*(t2->data()));
    }else if (name() == "!") {
      b = (t1->data())->not_equal(*(t2->data()));
    }else if (name() == "<") {
      b = (t1->data())->less(*(t2->data()));
    }else if (name() == ">") {
      b = (t1->data())->greater(*(t2->data()));
    }else{
      unreachable();
      //std::cout << name() << '\n';
      return 0;
    }}
    {if (b) {
      return new Token(b->val_string(),CONSTANT,b,(t1->args()+t2->args()));
    }else{
      return new Token("false", CONSTANT, 0, "");
    }}
  }else{
    // not "or" operator, one op not defined --> result not defined
    return new Token("false", CONSTANT, 0, "");
  }}
}
/*--------------------------------------------------------------------------*/
Token* Token::op(const Token* t1)const
{
  assert(t1);
  assert(type() == UNARY);

  {if (t1->data()) {
    Base* b = 0;
    {if (name() == "-") {
      b = (t1->data())->minus();
    }else if (name() == "!") {
      b = 0;
    }else{
      unreachable();
      //std::cout << name() << '\n';
      return 0;
    }}
    {if (b) {
      return new Token(b->val_string(), CONSTANT, b, (t1->args()));
    }else{
      return new Token("false", CONSTANT, 0, "");
    }}
  }else{
    // op not defined (false)
    {if (name() == "!") {
      return new Token("true", CONSTANT, new String("true"), "");      
    }else{
      return new Token("false", CONSTANT, 0, "");
    }} 
  }}
}
/*--------------------------------------------------------------------------*/
void Expression::reduce_copy(const Expression& proto, const Instance& inst)
{
  // The proto._list is the expression in RPN.
  // Attempt to build a reduced _list here, hopefully with only one item.
  {for (const_iterator i = proto.begin(); i != proto.end(); ++i) {
    switch ((**i).type()) {
    case Token::CONSTANT:
      unreachable();
      break;
    case Token::UNKNOWN:
      unreachable();
      break;
    case Token::STOP: {
      Token* t = new Token(**i);
      push_back(t);
      break;
    }
    case Token::PARLIST: {
      // replace multiple tokens of a PARLIST with a single token
      bool been_here = false;
      std::string tmp(")");
      {for (;;) {
	assert(!is_empty());
	const Token* t = back();
	pop_back();
	{if (t->type() == Token::STOP) {
	  tmp = "(" + tmp;
	  break;
	}else if (t->type() == Token::SYMBOL) {
	  {if (been_here) {
	    tmp = ", " + tmp;
	  }else{
	    been_here = true;
	  }}
	  tmp = t->full_name() + tmp;
	}else{
	  // expression in parlist
	  incomplete();
	  {if (been_here) {
	    tmp = ", " + tmp;
	  }else{
	    been_here = true;
	  }}
	  tmp = "####" + t->full_name() + "####" + tmp;
	}}
	delete t;
      }}
      Token* t = new Token(tmp, Token::PARLIST, NOTHING, "");
      push_back(t);
      break;
    }
    case Token::SYMBOL: {
      // replace single token with its value
      Token* t = 0;
      {if (!is_empty() && (back()->type() == Token::PARLIST)) {
	// has parameters (table or function)
	const Token* t1 = back(); // arglist
	pop_back();

	const Symbol* sym = (*_sym)[(**i).name()];
	{if (sym) {
	  // in symbol table.  see what we can do with it.
	  const Base* value = inst[(**i).name()];
	  {if (value) {
	    // has been assigned a value -- use it
	    assert(dynamic_cast<const Table*>(value));
	    t = new Token((**i).name(),Token::CONSTANT,value,t1->full_name());
	  }else{
	    // symbol in expression has no value -- make it "false"
	    incomplete();
	    //std::cout << (**i).name() << ": missing data\n";
	    assert(!((**i).data()));
	    t = new Token(**i);
	  }}
	}else{
	  // not in symbol table.  error????
	  unreachable();
	  //std::cout << (**i).name() << ": not in symbol table?????\n";
	  t = new Token(**i);
	}}
	delete t1;
      }else{
	// has no parameters (scalar)
	{if (isdigit((**i).name()[0])) {
	  Scalar* s = new Scalar((**i).name(), (**i).name());
	  t = new Token((**i).name(), Token::CONSTANT, s, "");
	}else{
	  const Symbol* sym = (*_sym)[(**i).name()];
	  {if (sym) {
	    // in symbol table.  see what we can do with it.
	    const Base* value = inst[(**i).name()];
	    {if (value) {
	      // has been assigned a value -- use it
	      assert(dynamic_cast<const Scalar*>(value)
		     || dynamic_cast<const String_Var*>(value)
		     || dynamic_cast<const Keyword*>(value));
	      t = new Token(value->val_string(), Token::CONSTANT, value, "");
	    }else{
	      // symbol in expression has no value -- make it "false"
	      //std::cout << (**i).name() << ": missing data\n";
	      assert(!((**i).data()));
	      t = new Token(**i);
	    }}
	  }else{
	    String_Var* s = new String_Var((**i).name(), (**i).name());
	    t = new Token((**i).name(), Token::CONSTANT, s, "");
	  }}
	}}
      }}
      assert(t);
      push_back(t);
      break;
    }
    case Token::BINOP: {
      // replace 2 tokens (binop) with 1 (result)
      assert(!is_empty());
      const Token* t2 = back();
      pop_back();
      assert(!is_empty());
      const Token* t1 = back();
      pop_back();

      //std::cout << ((t1->data()) ? *(t1->data()) : *t1)
      //		<< "::" << (**i).name() << "::" 
      //		<< ((t2->data()) ? *(t2->data()) : *t2)
      //		<< '\n';
      Token* t = (**i).op(t1, t2);
      assert(t);
      push_back(t);
      //const Base* b = t;
      //std::cout << ((t->data()) ? *(t->data()) : *b)
      //		<< '\n';

      delete t1;
      delete t2;
      break;
    }
    case Token::UNARY: {
      // replace 1 token with 1 (result)
      assert(!is_empty());
      const Token* t1 = back();
      pop_back();

      //std::cout << (**i).name() << "::" 
      //		<< ((t1->data()) ? *(t1->data()) : *t1)
      //		<< '\n';
      Token* t = (**i).op(t1);
      assert(t);
      push_back(t);
      //const Base* b = t;
      //std::cout << ((t->data()) ? *(t->data()) : *b)
      //		<< '\n';

      delete t1;
      break;
    }
    }
  }}
  {if (is_empty()) {
    incomplete();
    assert(proto.is_empty());
    //std::cout << "empty\n";
  }else{
    //std::cout << "==========" << back()->name() << '\n';
    assert(size() == 1);
  }}
}
/*--------------------------------------------------------------------------*/
Expression::Expression(const Expression& proto, const Instance& inst)
  :_sym(proto._sym),
   _scope(proto._scope)
{
  reduce_copy(proto, inst);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
