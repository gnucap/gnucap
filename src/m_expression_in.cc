/*$Id: m_expression_in.cc,v 26.115 2009/08/17 22:49:30 al Exp $ -*- C++ -*-
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
 * Classic recursive descent parser.
 * Builds a RPN representation in _list.
 * Numbers stay as strings.
 *
 * BNF:
 * arglisttail	: "," expression arglisttail
 *		| nothing
 * arglist	: "(" expression arglisttail ")";
 *		| "(" ")"
 *		| nothing
 * leaf		: name arglist
 * factor	: unary "(" expression ")"
 *		| unary leaf
 * termtail	: "*" factor termtail
 *		| "/" factor termtail
 *		| nothing
 * term		: factor termtail
 * addexptail	: "+" term addexptail
 *		| "-" term addexptail
 *		| nothing
 * addexp	: term addexptail
 * logicaltail	: "<" addexp logicaltail
 *		| ">" addexp logicaltail
 *		| "<=" addexp logicaltail
 *		| ">=" addexp logicaltail
 *		| "==" addexp logicaltail
 *		| "!=" addexp logicaltail
 *		| nothing
 * logical	: addexp logicaltail
 * andtail	: "&&" logical andtail
 *		| nothing
 * andarg	: logical andtail
 * exptail	: "||" andarg exptail
 *		| nothing
 * expression	: andarg exptail
 */
//testing=script 2009.08.12
#include "m_expression.h"
/*--------------------------------------------------------------------------*/
void Expression::arglisttail(CS& File)
{
  if (File.skip1b(",")) {
    expression(File);
    arglisttail(File);
  }else{
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::arglist(CS& File)
{
  if (File.skip1b("(")) {
    push_back(new Token_STOP("("));
    if (!File.skip1b(")")) {
      expression(File);
      arglisttail(File);
      if (!File.skip1b(")")) {itested();
	throw Exception_CS("unbalanced parentheses (arglist)", File);
      }else{
      }
    }else{
    }
    push_back(new Token_PARLIST(")"));
  }else{
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::leaf(CS& File)
{
#if 0
  if (File.peek() == '"') {untested();
    Quoted_String name(File);
    push_back(new Token_SYMBOL(name, ""));
    // do not put constants in symbol table
  } // else 
#endif
  unsigned here = File.cursor();
  Name_String name(File);
  if (!File.stuck(&here)) {
    arglist(File);
    push_back(new Token_SYMBOL(name, ""));
  }else{itested();
    throw Exception_CS("what's this?", File);
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::factor(CS& File)
{
  Token* t = 0;
  if (File >> "-|+|!") {
    std::string name(File.last_match());
    t = new Token_UNARY(name);
  }else{
  }
  if (File.skip1b("(")) {
    expression(File);
    if (!File.skip1b(")")) {untested();
      throw Exception_CS("unbalanced parentheses (factor)", File);
    }else{
    }
  }else{
    leaf(File);
  }
  if (t) {
    push_back(t);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::termtail(CS& File)
{
  if (File >> "*|/") {
    std::string name(File.last_match());
    factor(File);
    push_back(new Token_BINOP(name));
    termtail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::term(CS& File)
{
  factor(File);
  termtail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::addexptail(CS& File)
{
  if (File >> "+|-") {
    std::string name(File.last_match());
    term(File);
    push_back(new Token_BINOP(name));
    addexptail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::addexp(CS& File)
{
  term(File);
  addexptail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::logicaltail(CS& File)
{
  if (File >> "<=|<|>=|>|==|!=") {
    std::string name(File.last_match());
    addexp(File);
    push_back(new Token_BINOP(name));
    logicaltail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::logical(CS& File)
{
  addexp(File);
  logicaltail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::andtail(CS& File)
{
  if (File >> "&&") {
    std::string name(File.last_match());
    logical(File);
    push_back(new Token_BINOP(name));
    andtail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::andarg(CS& File)
{
  logical(File);
  andtail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::exptail(CS& File)
{
  if (File >> "\\|\\|") { // "||"
    std::string name(File.last_match());
    andarg(File);
    push_back(new Token_BINOP(name));
    exptail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::expression(CS& File)
{
  andarg(File);
  exptail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::parse(CS& File)
{
  expression(File);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
