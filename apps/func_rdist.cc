/*                           -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
 * Author: Felix Salfelder <felix@salfelder.org>
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
 * You should have received a copy of the GNU General Public License along with
 * Foobar. If not, see <https://www.gnu.org/licenses/>.
 *------------------------------------------------------------------
 */
#include "u_function.h"
#include "m_random.h"
#include "m_expression.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
void get_args(Expression* E, Base const** a, int howmany)
{
  assert(E);
  std::pair<Base const*, Base const*> ret;
  auto it = E->end();
  --it;

  Token* pl = E->back();
  if(dynamic_cast<Token_PARLIST*>(pl)) {
    assert(it!=E->begin());
    --it;
  }else{ untested();
    throw Exception("need args");
  }

  for(int i = 0; i < howmany; ++i) {
    if(auto cc=dynamic_cast<Token_CONSTANT const*>(*it)) {
      a[i] = cc->data();
      assert(it!=E->begin()); // inside PARLIST. expecting STOP
      --it;
    }else if(dynamic_cast<Token_STOP const*>(*it)) {
      throw Exception("need more args");
    }else{
    }
  }

  if(dynamic_cast<Token_STOP const*>(*it)) {
  }else{
    throw Exception("too many args");
  }
}
/*--------------------------------------------------------------------------*/
void delete_args(Expression* E, int howmany)
{
  Token* pl = E->back();
  E->pop_back();
  assert(dynamic_cast<Token_PARLIST*>(pl));
  delete pl;
  for(int i=0; i<howmany; ++i){
    delete E->back();
    E->pop_back();
  }
  assert(dynamic_cast<Token_STOP const*>(E->back()));
  delete E->back();
  E->pop_back();
}
/*--------------------------------------------------------------------------*/
double get_double(Base const* b)
{
  bool ok = false;
  double p;
  if(auto f = dynamic_cast<Float const*>(b)){
    p = f->value();
    ok = true;
  }else if(auto i = dynamic_cast<Integer const*>(b)){
    p = i->value();
    ok = true;
  }else{
  }

  if(ok){
    return p;
  }else{
    throw Exception("not double\n");
  }
}
/*--------------------------------------------------------------------------*/
int32_t get_int(Base const* b)
{
  bool ok = false;
  int32_t p;
  if(auto f = dynamic_cast<Float const*>(b)){
    p = int32_t(f->value());
    ok = true;
  }else if(auto i = dynamic_cast<Integer const*>(b)){
    p = i->value();
    ok = true;
  }else{
  }

  if(ok){
    return p;
  }else{
    throw Exception("not double\n");
  }
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class RDIST : public FUNCTION {
public:
  explicit RDIST(std::string const& what) : FUNCTION() {}
public:
  void stack_op(Expression* E)const override {
    const int na = 2;
    Base const* args[na+1];

    try{
      get_args(E, args, na+1);
    }catch(Exception const& e){
      throw e;
    }

    double d[na];
    for(int i=0; i<na; ++i){
      assert(args[i]);
      d[i] = get_double(args[i]);
      trace2("RDIST", i, d[i]);
    }

    int32_t initial_seed = get_int(args[na]);
    int32_t& seed = random_seed(initial_seed);
    double u = rdist::uniform(seed, d[1], d[0]);

    delete_args(E, na+1);
    Float* f = new Float(u);
    E->push_back(new Token_CONSTANT(f));
  }
};
RDIST p_stub("AA");
DISPATCHER<FUNCTION>::INSTALL d_stub(&function_dispatcher, "$rdist_uniform", &p_stub);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// vim:ts=8:sw=2:noet:
