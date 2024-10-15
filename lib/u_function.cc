/*                               -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * FUNCTION
 */
#include "u_function.h"
#include "m_expression.h"
/*--------------------------------------------------------------------------*/
static std::string call_function_eval(FUNCTION const* F, Expression const* E)
{
  assert(!E->is_empty());
  Expression::const_iterator input = E->end();
  --input;
  assert(dynamic_cast<const Token_PARLIST*>(*input));
  --input;

  std::string arg;
  std::string comma = "";
  bool all_num = true;
  while (!dynamic_cast<const Token_STOP*>(*input)) {
    all_num = dynamic_cast<Float const*>((*input)->data())
            ||dynamic_cast<Integer const*>((*input)->data());
    if(!all_num){
      trace1("not numerical", (*input)->name());
      break;
    }else{
      assert(dynamic_cast<Token_CONSTANT const*>(*input));
    }

    arg = (*input)->name() + comma + arg;
    comma = ", ";
    assert(input != E->begin());
    --input;
  }

  if(all_num){
    // function call as usual
    CS cmd(CS::_STRING, arg);
    return F->eval(cmd, E->_scope);
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void FUNCTION::stack_op(Expression* E) const
{
  assert(--_which && "need stack_op or eval");
  assert(E);

  std::string ret = call_function_eval(this, E);

  if(ret==""){
    throw Exception("didnt work");
  }else{
    while (!dynamic_cast<const Token_STOP*>(E->back())) {
      delete(E->back());
      E->pop_back();
      assert(!E->is_empty());
    }
    delete(E->back());
    E->pop_back();
    const Float* v = new Float(ret); // BUG. what if integer?
    E->push_back(new Token_CONSTANT(v));
  }

}
/*--------------------------------------------------------------------------*/
std::string FUNCTION::eval(CS& cmd, const CARD_LIST* scope) const
{ untested();
  assert(++_which && "need stack_op or eval");

  Expression e0(cmd);
  Expression e(e0, scope);
  stack_op(&e);

  std::stringstream s;
  e.dump(s);
  trace2("eval wrap", cmd.fullstring(), s.str());
  return s.str();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
