/*$Id: m_expression_dump.cc,v 26.115 2009/08/17 22:49:30 al Exp $ -*- C++ -*-
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
 * Reconstructs an infix expression from the RPN.
 */
//testing=script,sparse 2009.08.12
#include "m_expression.h"
/*--------------------------------------------------------------------------*/
bool mangle_identifier(std::string& name)
{
  bool plain = true;

  if(isalpha(name[0])){
  }else if(name[0] == '$'){
  }else if(name[0] == '_'){
  }else if(name[0] == '.'){ untested();
    // paramset hack. references to output variables in parent
  }else if(name[0] == '<'){
    // HACK: treat port branches.
    // problem in the standard:
    // <p> could as well be expressed as $port(p), without this mess.
    return false;
  }else{
    plain = false;
  }

  for(size_t i=1; plain && i<name.size(); ++i){
    if(isalnum(name[i])){
    }else if(name[i] == '_'){
    }else if(name[i] == '$'){
    }else{
      plain = false;
    }
  }

  if(plain){
    return false;
  }else{
    std::string ret("\\");
    for(size_t i=0; i<name.size(); ++i){
      if(name[i] == '\\'){ untested();
	ret += "\\";
      }else{
      }
      ret += name[i];
    }
    ret += " ";
    name = ret;
    return true;
  }
}
/*--------------------------------------------------------------------------*/

void Token::dump(std::ostream& out)const
{itested();
  out << val_string() << ' ';
}
/*--------------------------------------------------------------------------*/
void Token_CONSTANT::dump(std::ostream& out)const
{itested();
  out << val_string();
}
/*--------------------------------------------------------------------------*/
void Expression::dump(std::ostream& out)const
{
  class stt : public Token{
    std::string _name;
  public:
    explicit stt(std::string Name) : Token(nullptr), _name(Name) {}
  private:
    Token* clone()const override {unreachable(); return nullptr;}
    std::string val_string()const override { return _name; }
  };
  std::vector<const Token*> locals; // a way of faking garbage collection.
  std::vector<const Token*> stack;  // actually use this
  // The _list is the expression in RPN.
  // Un-parse it -- back to infix.
  for (const_iterator i = begin(); i != end(); ++i) {
    if (dynamic_cast<const Token_STOP*>(*i)) {
      stack.push_back(*i);
    }else if (dynamic_cast<const Token_PARLIST*>(*i)
           || dynamic_cast<const Token_ARRAY*>(*i)) {
      if((*i)->data()){ untested();
      }else{
      }
      // pop*n  push
      bool been_here = false;
      std::string tmp((*i)->name());
      for (;;) {
	if (stack.empty()) {untested();
	  throw Exception("bad expression");
	}else{
	}
	const Token* t = stack.back();
	stack.pop_back();
	if (dynamic_cast<const Token_STOP*>(t)) {
	  tmp = t->name() + tmp;
	  break;
	}else if (dynamic_cast<const Token_SYMBOL*>(t)
	      ||  dynamic_cast<const Token_CONSTANT*>(t)
	      ||  dynamic_cast<const Token_ARRAY*>(t)
	      ||  dynamic_cast<const stt*>(t)) {
	  if (been_here) {
	    tmp = ", " + tmp;
	  }else{
	    been_here = true;
	  }
	  tmp = t->full_name() + tmp;
	}else{
	  unreachable();
	}
      }
      Token* t;
      if (dynamic_cast<const Token_PARLIST*>(*i)){
	t = new Token_PARLIST(tmp);
      }else{
	t = new stt(tmp);
      }
      locals.push_back(t);
      stack.push_back(t);
    }else if (dynamic_cast<const Token_CONSTANT*>(*i)) {
      stack.push_back(*i);
    }else if (dynamic_cast<const Token_CONSTANT*>(*i)|| dynamic_cast<const Token_SYMBOL*>(*i)) {
      if (!stack.empty() && (dynamic_cast<const Token_PARLIST*>(stack.back()))) {
	// has parameters (table or function)
	// pop op push
	const Token* t1 = stack.back();
	stack.pop_back();
	std::string mangled = (*i)->name();
	mangle_identifier(mangled);
	Token* t = new stt(mangled + t1->full_name());
	locals.push_back(t);
	stack.push_back(t);
      }else{
	// has no parameters (scalar)
	if (dynamic_cast<const Token_SYMBOL*>(*i)) {
	  // mangle if needed.
	  std::string name_ = (*i)->name();
	  if(mangle_identifier(name_)){
	    Token* t = new stt(name_);
	    stack.push_back(t);
	    locals.push_back(t);
	  }else{
	    stack.push_back(*i);
	  }
	}else{
	  stack.push_back(*i);
	}
      }
    }else if (dynamic_cast<const Token_BINOP*>(*i)) {
      // pop pop op push
      assert(!stack.empty());
      const Token* t2 = stack.back();
      stack.pop_back();
      assert(!stack.empty());
      const Token* t1 = stack.back();
      stack.pop_back();
      std::string tmp('(' + t1->full_name() + ' ' + (**i).name() + ' ' + t2->full_name() + ')');
      Token* t = new stt(tmp);
      locals.push_back(t);
      stack.push_back(t);
    }else if (dynamic_cast<const Token_UNARY*>(*i)) {
      // pop op push
      assert(!stack.empty());
      const Token* t1 = stack.back();
      stack.pop_back();
      std::string tmp('(' + (**i).name() + ' ' + t1->full_name() + ')');
      Token* t = new stt(tmp);
      locals.push_back(t);
      stack.push_back(t);
    }else if (auto t = dynamic_cast<const Token_TERNARY*>(*i)) {
      assert(!stack.empty());
      const Token* cond = stack.back();
      stack.pop_back();
      std::stringstream tmp;

      tmp << '(' << cond->full_name() << ")? (";
      t->true_part()->dump(tmp);
      tmp << "):(";
      t->false_part()->dump(tmp);
      tmp << ')';

      Token* n = new stt(tmp.str());
      locals.push_back(n);
      stack.push_back(n);
    }else{ untested();
      unreachable();
    }
  }
  if (stack.empty()) {untested();
    out << "empty";
  }else{
    out << stack.back()->full_name();
    if(stack.size() == 1){
    }else{ untested();
      incomplete();
    }
  }
  while (!locals.empty()) {
    delete locals.back();
    locals.pop_back();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
