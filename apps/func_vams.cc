/*$Id: func_core.cc $ -*- C++ -*-
 * Copyright (C) 2008 Albert Davis
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
#include "globals.h"
#include "u_parameter.h"
#include "u_function.h"
#include "u_sim_data.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class temp : public FUNCTION {
public:
  void stack_op(Expression* E)const override {
    assert(E);
    auto par = dynamic_cast<Token_PARLIST*>(E->back());
    if(par){
      E->pop_back();
    }else{ untested();
      // probably not needed.
    }

    if(!par){ untested();
    }else if(dynamic_cast<Token_STOP const*>(E->back())) {
    }else{ untested();
      // wrong number of args, forget about it.
      E->push_back(par);
      throw(Exception("wrong argcount"));
    }

    double v;
    if(E->_scope){
      auto P = E->_scope->deep_lookup("$temperature");
      if(P.has_hard_value()){
	v = P;
	trace1("$temp hard", v);
	vReal* r = new vReal(v);
	if(par){
	  assert(dynamic_cast<Token_STOP const*>(E->back()));
	  delete(E->back());
	  E->pop_back();
	  delete(par);
	  par = nullptr;
	}else{ untested();
	}
	E->push_back(new Token_CONSTANT(r));
      }else if(par){
	// no temperature, restore.
	assert(dynamic_cast<Token_STOP const*>(E->back()));
	delete E->back();
	E->pop_back();
	delete par;
	v = OPT::temp_k;
	vReal* r = new vReal(v);
	E->push_back(new Token_CONSTANT(r));

//	Token_SYMBOL T("$temperature");
//      E->push_back(par);
//      throw(Exception("not yet"));
//	T.stack_op(E);
      }else{ untested();
     }
    }else{ untested();
      // no scope. restore.
      if(par) { untested();
	E->push_back(par);
      }else{ untested();
      }
      throw(Exception("no scope"));
    }
  }
} p_temp;
DISPATCHER<FUNCTION>::INSTALL d_temp(&function_dispatcher, "$temperature", &p_temp);
/*--------------------------------------------------------------------------*/
class vt : public FUNCTION {
public:
  void stack_op(Expression* E)const override {
    assert(E);
    Token* par = nullptr;
    if(E->is_empty()){ untested();
    }else if((par = dynamic_cast<Token_PARLIST*>(E->back()))) {
      E->pop_back();
    }else{ untested();
      // probably not needed.
    }

    double result = NOT_VALID;
    Token* arg0;

    if(E->is_empty() || dynamic_cast<Token_STOP const*>(E->back())){
      // no arg. compute from $temperature if it's there.
      if(E->_scope){
	auto P = E->_scope->deep_lookup("$vt");
	if(P.has_hard_value()){ untested();
	  result = P;
	}else{
	  double arg;
	  auto Q = E->_scope->deep_lookup("$temperature");
	  if(Q.has_hard_value()){
	    arg = Q;
	  }else{
	    arg = OPT::temp_k;
	  }
	  result = P_K * arg / P_Q;
	}
      }else if(par) { untested();
	E->push_back(par);
	throw(Exception("$vt: no temperature"));
      }else{ untested();
	throw(Exception("$vt: no temperature"));
      }
    }else if((arg0 = dynamic_cast<Token_CONSTANT*>(E->back()))) {
      E->pop_back();

      if(auto f = dynamic_cast<Float const*>(arg0->data())) {
	double arg = f->value();
	delete(arg0);
	result = P_K * arg / P_Q;
      }else if(auto i = dynamic_cast<Integer const*>(arg0->data())) {
	int arg = i->value();
	delete(arg0);
	result = P_K * arg / P_Q;
      }else{ untested();
	incomplete();
	E->push_back(arg0);
	result = NOT_VALID;
      }
    }else if(dynamic_cast<Token_STOP const*>(E->back())) { untested();
    }else if(par) { untested();
      // wrong number of args, forget about it.
      E->push_back(par);
      throw(Exception("$vt: wrong argc"));
    }

    // getting here if there is a result.
    // still need to clean up
    if(par) {
      assert(dynamic_cast<Token_STOP const*>(E->back()));
      delete E->back();
      E->pop_back();
      delete par;
      par = nullptr;
    }else{ untested();
    }

    trace1("vt1", result);
    vReal* r = new vReal(result);
    E->push_back(new Token_CONSTANT(r));
  }
} p_vt;
DISPATCHER<FUNCTION>::INSTALL d_vt(&function_dispatcher, "$vt", &p_vt);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
