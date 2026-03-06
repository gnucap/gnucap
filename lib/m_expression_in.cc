/*$Id: m_expression_in.cc,v 26.115 2009/08/17 22:49:30 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
 *               2024-2026 Felix Salfelder
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
 *
 * addexptail	: "+" term addexptail
 *		| "-" term addexptail
 *		| nothing
 * addexp	: term addexptail
 *
 * shifttail	: "<<<" addexp shifttail
 *		| ">>>" addexp shifttail
 *		| "<<" addexp shifttail
 *		| ">>" addexp shifttail
 *		| nothing
 * shift	: addexp shifttail
 *
 * ordertail	: "<" shift ordertail
 *		| ">" shift ordertail
 *		| "<=" shift ordertail
 *		| ">=" shift ordertail
 *		| nothing
 * order	: shift ordertail
 *
 * comparetail	: "==" order comparetail
 *		| "!=" order comparetail
 *		| nothing
 * compare	: order comparetail
 *
 * bitandtail	: "&" compare bitandtail
 *		| nothing
 * bitandarg	: compare bitandtail
 *
 * bitxortail	: "^" bitandarg bitxortail
 *		| "^~" bitandarg bitxortail
 *		| nothing
 * bitxorarg	: bitandarg bitxortail
 *
 * bitortail	: "|" bitxorarg bitortail
 *		| nothing
 * bitorarg	: bitxorarg bitortail
 *
 * andtail	: "&&" bitorarg andtail
 *		| nothing
 * andarg	: bitorarg andtail
 *
 * ortail	: "||" andarg ortail
 *		| nothing
 * orarg	: andarg ortail
 *
 * exptail	| "?" expression ":" expression
 *		| nothing
 * ... {} {{}}...
 * expression	: orarg exptail
 */
//testing=
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
    push_back(new Token_STOP('('));
    if (!File.skip1b(")")) {
      expression(File);
      arglisttail(File);
      if (!File.skip1b(")")) {itested();
	throw Exception_CS("unbalanced parentheses (arglist)", File);
      }else{
      }
    }else{
    }
    push_back(new Token_PARLIST(')'));
  }else{
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
CS& Expression::array(CS& File)
{
  trace1("array", File.tail().substr(0,20));
  size_t here = File.cursor();
  if (File.skip1b("'") && File.peek() == '{') {
    File.skip();
    trace1("array2", File.tail().substr(0,20));
    push_back(new Token_STOP("'{"));
    if (!File.skip1b("}")) {
      expression(File);
      arglisttail(File);
      if (!File.skip1b("}")) { untested();
	throw Exception_CS("unbalanced parentheses (array)", File);
      }else{
      }
    }else{itested();
    }

    push_back(new Token_ARRAY("}"));
  }else{
    File.reset_fail(here);
  }
  return File;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::leaf(CS& File)
{
  trace1("leaf?", File.tail().substr(0,20));
  size_t here = File.cursor();
  char pk = File.peek();
  if (pk == '.'){
    Name_String name(File);
    if (!File.stuck(&here)) {
      arglist(File);
      if (strchr("0123456789", std::string(name)[1])) {
        push_back(new Token_LITERAL(name));
      }else{itested();
	// hack: paramset output variable reference
        push_back(new Token_SYMBOL(name));
      }
    }else{
      throw Exception_CS("what's this?", File);
    }
  }else if (strchr("0123456789", pk)) {
    Name_String name(File);
    if (!File.stuck(&here)) {
      arglist(File);
      push_back(new Token_LITERAL(name));
    }else{
      throw Exception_CS("what's this?", File);
    }
  }else if (File.peek() == '"') {
    vString* s = new vString(File);
    if (File.stuck(&here)) {
      delete s;
      throw Exception_CS("what's this?", File);
    }else{
      push_back(new Token_CONSTANT(s));
    }
  }else if (File.peek() == '<') {
    std::string s = File.ctos("", "<", ">");
    push_back(new Token_SYMBOL("<" + s + ">"));
  }else if (array(File)) {
  }else{
    Name_String name(File);
    if (!File.stuck(&here)) {
      arglist(File);
      push_back(new Token_SYMBOL(name));
    }else{itested();
      trace1("leafstuck", File.tail().substr(0,20));
      throw Exception_CS("what's this?", File);
    }
  }
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void Expression::factor(CS& File)
{
  Token* t = nullptr;
  if (File >> "-|+|!|~") {
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
  trace1("factor1", File.tail().substr(0,20));
}
/*--------------------------------------------------------------------------*/
void Expression::ternary(CS& File)
{
  std::string name(File.last_match());
  Expression* true_part = nullptr;
  Expression* false_part = nullptr;

  true_part = new Expression(File);

  if (!File.skip1b(":")) {
    delete true_part;
    throw Exception_CS("missing colon (ternary)", File);
  }else{
    // push_back(new Token_STOP(":"));
  }
  false_part = new Expression(File);

 // andarg(File);

  assert(name == "?");
  push_back(new Token_TERNARY(true_part, false_part));
}
/*--------------------------------------------------------------------------*/
void Expression::termtail(CS& File)
{
  if (File >> "*|/|%") {
    std::string name(File.last_match());
    assert(name.size()==1);
    factor(File);
    push_back(new Token_BINOP(name[0]));
    termtail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::term(CS& File)
{
  trace1("term0", File.tail().substr(0,20));
  factor(File);
  trace1("term1", File.tail().substr(0,20));
  termtail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::addexptail(CS& File)
{
  if (File >> "+|-") {
    std::string name(File.last_match());
    assert(name.size()==1);
    term(File);
    push_back(new Token_BINOP(name[0]));
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
void Expression::shifttail(CS& File)
{
  if (File >> "<<<|<<|>>>|>>") {
    std::string name(File.last_match());
    addexp(File);
    push_back(new Token_BINOP(name));
    shifttail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::shift(CS& File)
{
  addexp(File);
  shifttail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::ordertail(CS& File)
{
  if (File >> "<=|>=|<|>") {
    std::string name(File.last_match());
    shift(File);
    push_back(new Token_BINOP(name));
    ordertail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::order(CS& File)
{
  shift(File);
  ordertail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::comparetail(CS& File)
{
  if (File >> "==|!=") {
    std::string name(File.last_match());
    order(File);
    push_back(new Token_BINOP(name));
    comparetail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::compare(CS& File)
{
  order(File);
  comparetail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::bitandtail(CS& File)
{
  if (File >> "& ") {
    std::string name(File.last_match());
    compare(File);
    push_back(new Token_BINOP("&"));
    bitandtail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::bitandarg(CS& File)
{
  compare(File);
  bitandtail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::bitxortail(CS& File)
{
  if (File >> "^|~^") {
    std::string name(File.last_match());
    bitandarg(File);
    push_back(new Token_BINOP(name));
    bitxortail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::bitxorarg(CS& File)
{
  bitandarg(File);
  bitxortail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::bitortail(CS& File)
{
  if (File >> "\\| ") {
    std::string name(File.last_match());
    bitxorarg(File);
    push_back(new Token_BINOP('|'));
    bitortail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::bitorarg(CS& File)
{
  bitxorarg(File);
  bitortail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::andtail(CS& File)
{
  if (File >> "&&") {
    std::string name(File.last_match());
    bitorarg(File);
    push_back(new Token_BINOP(name));
    andtail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::andarg(CS& File)
{
  bitorarg(File);
  andtail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::ortail(CS& File)
{
  if (File >> "\\|\\|") { // "||"
    std::string name(File.last_match());
    andarg(File);
    push_back(new Token_BINOP(name));
    ortail(File);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Expression::orarg(CS& File)
{
  andarg(File);
  ortail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::exptail(CS& File)
{
  if (File >> "?") { untested();
    assert(size());
    ternary(File);
    exptail(File);
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
void Expression::expression(CS& File)
{
  orarg(File);
  exptail(File);
}
/*--------------------------------------------------------------------------*/
void Expression::parse(CS& File)
{
  expression(File);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
