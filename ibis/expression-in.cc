/*$Id: expression-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 * Classic recursive descent parser.
 * Builds a RPN representation in _list.
 * BNF:
 * arglisttail	: expression arglisttail
 *		| ")"
 * arglist	: "(" arglisttail;
 *		| nothing
 * leaf		: name arglist
 * factor	: "(" expression ")"
 *		| leaf
 * termtail	: "*" term
 *		| "/" term
 *		| nothing
 * term		: factor termtail
 * addexptail	: "+" addexp
 *		| "-" addexp
 *		| nothing
 * addexp	: term addexptail
 * logicaltail	: "<" logical
 *		| ">" logical
 *		| "==" logical
 *		| "!=" logical
 *		| nothing
 * logical	: addexp logicaltail
 * exptail	: "||" expression
 *		| "for" expression
 *		| nothing
 * expression	: logical exptail
 * assign	: lvalue "=" expression
 */
//#include "expression.h"
#include "macro.h"
/*--------------------------------------------------------------------------*/
void Expression::arglisttail(CS& file)
{
  {if (file.skip1b(")")) {
    /* done */
  }else{ // if (file.skip1b(",")) { // implied
    expression(file);
    arglisttail(file);
  }}
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::arglist(CS& file)
{
  if (file.skip1b("(")) {
    push(new Token("(", Token::STOP, NOTHING, ""));
    arglisttail(file);
    push(new Token(")", Token::PARLIST, NOTHING, ""));
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::leaf(CS& file)
{
  {if (file.peek() == '"') {
    Quoted_String name(file);
    push(new Token(name, Token::SYMBOL, NOTHING, ""));
    // do not put constants in symbol table
  }else if (file.is_pfloat()) {
    Float x(file);
    char name[20];
    sprintf(name, "%g", double(x));
    push(new Token(name, Token::SYMBOL, NOTHING, ""));
    // do not put constants in symbol table
  }else{
    file.skip1("-!$"); // BUG:: skip unary operators
    int name_here = file.cursor();
    Name_String name(file);
    int here = file.cursor();
    {if (name_here != here) {
      arglist(file);
      push(new Token(name, Token::SYMBOL, NOTHING, ""));
      Symbol_Type type = ((file.stuck(&here)) ? tSCALAR : tVECTOR);
      _sym->check_add_use(file, name_here, type, name);
    }else{
      file.warn(0, "bad name?");
    }}
  }}
  file.skipcom();
  // scope BUG
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::factor(CS& file)
{
  Token* t = 0;
  {if (file.dmatch("-") || file.dmatch("!")) {
    std::string name(file.last_match());
    t = new Token(name, Token::UNARY, NOTHING, "");
  }}
  {if (file.skip1b("(")) {
    expression(file);
    if (!file.skip1b(")")) {
      untested();
      file.warn(0, "unbalanced parentheses");
    }
  }else{
    leaf(file);
  }}
  {if (t) {
    push(t);
  }}
}
/*--------------------------------------------------------------------------*/
void Expression::term(CS& file)
{
  factor(file);
  if (file.dmatch("*") || file.dmatch("/")) {
    std::string name(file.last_match());
    term(file);
    push(new Token(name, Token::BINOP, NOTHING, ""));
  }
}
/*--------------------------------------------------------------------------*/
void Expression::addexp(CS& file)
{
  term(file);
  if (file.dmatch("+") || file.dmatch("-")) {
    std::string name(file.last_match());
    addexp(file);
    push(new Token(name, Token::BINOP, NOTHING, ""));
  }
}
/*--------------------------------------------------------------------------*/
void Expression::logical(CS& file)
{
  //BUG:: single "=" here hides parser interpretation of "=" as comma
  addexp(file);
  if (file.dmatch("<") || file.dmatch(">") 
      || file.dmatch("=") || file.dmatch("!")) {
    std::string name(file.last_match());
    logical(file);
    push(new Token(name, Token::BINOP, NOTHING, ""));
  }
}
/*--------------------------------------------------------------------------*/
void Expression::expression(CS& file)
{
  logical(file);
  if (file.dmatch("||") || file.dmatch("for")) {
    std::string name(file.last_match());
    expression(file);
    push(new Token(name, Token::BINOP, NOTHING, ""));
  }
}
/*--------------------------------------------------------------------------*/
void Expression::parse(CS& file)
{
  assert(_sym);
  expression(file);
}
/*--------------------------------------------------------------------------*/
// Consider this to be a default constructor.
// It makes a blank expression, but has the macro attached for its 
// symbol table and scope info.
// It is normally immediately followed by a parse.
Expression::Expression(const Macro_Body* m)
  :_sym(m->sym()),
   _scope(m->scope())
{
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
