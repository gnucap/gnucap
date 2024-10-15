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
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class abs : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::abs(x));
  }
  void stack_op(Expression* E)const override {
    auto par = dynamic_cast<Token_PARLIST*>(E->back());
    if(par){
      E->pop_back();
    }else{ untested();
      // probably not needed.
    }

    if(auto arg0 = dynamic_cast<Token_CONSTANT*>(E->back())) {
      trace1("abs1", arg0->name());
      E->pop_back();

      Base* v;
      std::string name;

      if(auto f = dynamic_cast<Float const*>(arg0->data())) {
	double result = std::abs(f->value());
	v = new Float(result);
      }else if(auto i = dynamic_cast<Integer const*>(arg0->data())) {
	int result = std::abs(i->value());
	v = new Integer(result);
      }else{ untested();
	if(par){ untested();
	  E->push_back(par);
	}else{ untested();
	}
	throw(Exception("wrong type"));
      }

      if(!par){ untested();
	delete arg0;
	E->push_back(new Token_CONSTANT(v));
      }else if(dynamic_cast<Token_STOP const*>(E->back())) {
	delete arg0;
	delete E->back();
	E->pop_back();
	delete par;
	E->push_back(new Token_CONSTANT(v));
      }else{
	// wrong number of args, forget about it.
	E->push_back(arg0);
	E->push_back(par);
	throw(Exception("wrong argcount"));
      }

    }else if(par) { untested();
      E->push_back(par);
      throw(Exception("unsuitable paramlist"));
    }else{ untested();
      // invalid arg. no action.
      throw(Exception("unsuitable argument"));
    }


  }
} p_abs;
DISPATCHER<FUNCTION>::INSTALL d_abs(&function_dispatcher, "abs", &p_abs);
/*--------------------------------------------------------------------------*/
class sqrt : public FUNCTION {
public:
  void stack_op(Expression* E)const override {
    auto par = dynamic_cast<Token_PARLIST*>(E->back());
    if(par){
      E->pop_back();
    }else{ untested();
      // probably not needed.
    }

    if(auto arg0 = dynamic_cast<Token_CONSTANT*>(E->back())) {
      trace1("sqrt1", arg0->name());

      Base* v;
      std::string name;
      double result;

      if(auto f = dynamic_cast<Float const*>(arg0->data())) {
	result = std::sqrt(f->value());
	name = to_string(result);
      }else if(auto i = dynamic_cast<Integer const*>(arg0->data())) {
	result = std::sqrt(i->value());
	name = to_string(result);
      }else{ untested();
	if(par){ untested();
	  E->push_back(par);
	}else{ untested();
	}
	throw(Exception("wrong type"));
      }

      E->pop_back();
      v = new Float(result);

      if(!par){ untested();
	delete arg0;
	E->push_back(new Token_CONSTANT(v));
	trace2("sqrt2a", name, v);
      }else if(dynamic_cast<Token_STOP const*>(E->back())) {
	delete arg0;
	delete E->back();
	E->pop_back();
	delete par;
	E->push_back(new Token_CONSTANT(v));
	trace2("sqrt2b", name, v);
      }else{
	// wrong number of args, forget about it.
	E->push_back(arg0);
	E->push_back(par);
	throw(Exception("wrong argcount"));
      }

    }else if(par) {
      E->push_back(par);
      throw(Exception("unsuitable paramlist"));
    }else{ untested();
      // invalid arg. no action.
      throw(Exception("unsuitable argument"));
    }
  }
} p_sqrt;
DISPATCHER<FUNCTION>::INSTALL d_sqrt(&function_dispatcher, "sqrt", &p_sqrt);
/*--------------------------------------------------------------------------*/
class ln : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::log(x));
  }
} p_ln;
DISPATCHER<FUNCTION>::INSTALL d_ln(&function_dispatcher, "ln", &p_ln);
/*--------------------------------------------------------------------------*/
class log : public FUNCTION {
public:
  void stack_op(Expression* E)const override {
    error(bDEBUG, "log: Ambiguous. Use 'ln', or 'log10'\n");
    auto par = dynamic_cast<Token_PARLIST*>(E->back());
    if(par){
      E->pop_back();
    }else{ untested();
      // probably not needed.
    }

    if(auto arg0 = dynamic_cast<Token_CONSTANT*>(E->back())) {

      Base* v;
      std::string name;
      double result;

      if(auto r = dynamic_cast<vReal const*>(arg0->data())) {
	result = std::log10(r->value());
	name = to_string(result);
      }else if(auto f = dynamic_cast<Float const*>(arg0->data())) {
	result = std::log(f->value());
	name = to_string(result);
      }else if(auto vi = dynamic_cast<vInteger const*>(arg0->data())) {
	result = std::log10(vi->value());
	name = to_string(result);
      }else if(auto i = dynamic_cast<Integer const*>(arg0->data())) {
	result = std::log(i->value());
	name = to_string(result);
      }else{ untested();
	if(par){ untested();
	  E->push_back(par);
	}else{ untested();
	}
	throw(Exception("wrong type"));
      }

      E->pop_back();
      v = new Float(result);

      if(!par){ untested();
	delete arg0;
	E->push_back(new Token_CONSTANT(v));
      }else if(dynamic_cast<Token_STOP const*>(E->back())) {
	delete arg0;
	delete E->back();
	E->pop_back();
	delete par;
	E->push_back(new Token_CONSTANT(v));
      }else{ untested();
	// wrong number of args, forget about it.
	E->push_back(arg0);
	E->push_back(par);
	throw(Exception("wrong argcount"));
      }

    }else if(par) { untested();
      E->push_back(par);
      throw(Exception("unsuitable paramlist"));
    }else{ untested();
      // invalid arg. no action.
      throw(Exception("unsuitable argument"));
    }
  }
} p_log;
DISPATCHER<FUNCTION>::INSTALL d_log(&function_dispatcher, "log", &p_log);
/*--------------------------------------------------------------------------*/
class log10 : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::log10(x));
  }
} p_log10;
DISPATCHER<FUNCTION>::INSTALL d_log10(&function_dispatcher, "log10", &p_log10);
/*--------------------------------------------------------------------------*/
class exp : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::exp(x));
  }
} p_exp;
DISPATCHER<FUNCTION>::INSTALL d_exp(&function_dispatcher, "exp", &p_exp);
/*--------------------------------------------------------------------------*/
class INT : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::floor(x));
  }
} p_int;
DISPATCHER<FUNCTION>::INSTALL d_int(&function_dispatcher, "int", &p_int);
/*--------------------------------------------------------------------------*/
class pow : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x, y;
    Cmd >> x >> y;
    x.e_val(NOT_INPUT, Scope);
    y.e_val(NOT_INPUT, Scope);
    return to_string(std::pow(x,y));
  }
} p_pow;
DISPATCHER<FUNCTION>::INSTALL d_pow(&function_dispatcher, "pow", &p_pow);
/*--------------------------------------------------------------------------*/
class MAX : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x, y;
    Cmd >> x >> y;
    x.e_val(NOT_INPUT, Scope);
    y.e_val(NOT_INPUT, Scope);
    return to_string(std::max(x,y));
  }
} p_max;
DISPATCHER<FUNCTION>::INSTALL d_max(&function_dispatcher, "max", &p_max);
/*--------------------------------------------------------------------------*/
class MIN : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x, y;
    trace1("min", Cmd.fullstring());
    Cmd >> x >> y;
    x.e_val(NOT_INPUT, Scope);
    y.e_val(NOT_INPUT, Scope);
    return to_string(std::min(x,y));
  }
} p_min;
DISPATCHER<FUNCTION>::INSTALL d_min(&function_dispatcher, "min", &p_min);
/*--------------------------------------------------------------------------*/
class IF : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {itested();
    PARAMETER<double> x, y, z;
    Cmd >> x >> y >> z;
    x.e_val(NOT_INPUT, Scope);
    y.e_val(NOT_INPUT, Scope);
    z.e_val(NOT_INPUT, Scope);
    return to_string(x!=0. ? y : z);
  }
} p_if;
DISPATCHER<FUNCTION>::INSTALL d_if(&function_dispatcher, "if", &p_if);
/*--------------------------------------------------------------------------*/
class sin : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::sin(x));
  }
} p_sin;
DISPATCHER<FUNCTION>::INSTALL d_sin(&function_dispatcher, "sin", &p_sin);
/*--------------------------------------------------------------------------*/
class sinh : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::sinh(x));
  }
} p_sinh;
DISPATCHER<FUNCTION>::INSTALL d_sinh(&function_dispatcher, "sinh", &p_sinh);
/*--------------------------------------------------------------------------*/
class cos : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::cos(x));
  }
} p_cos;
DISPATCHER<FUNCTION>::INSTALL d_cos(&function_dispatcher, "cos", &p_cos);
/*--------------------------------------------------------------------------*/
class cosh : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::cosh(x));
  }
} p_cosh;
DISPATCHER<FUNCTION>::INSTALL d_cosh(&function_dispatcher, "cosh", &p_cosh);
/*--------------------------------------------------------------------------*/
class tan : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::tan(x));
  }
} p_tan;
DISPATCHER<FUNCTION>::INSTALL d_tan(&function_dispatcher, "tan", &p_tan);
/*--------------------------------------------------------------------------*/
class tanh : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> x;
    Cmd >> x;
    x.e_val(NOT_INPUT, Scope);
    return to_string(std::tanh(x));
  }
} p_tanh;
DISPATCHER<FUNCTION>::INSTALL d_tanh(&function_dispatcher, "tanh", &p_tanh);
/*--------------------------------------------------------------------------*/
class na : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override {
    PARAMETER<double> arg;
    Cmd >> arg;
    arg.e_val(NOT_INPUT, Scope);
    return "NA";
    //return (arg.has_hard_value()) ? (to_string(double(arg))) : ("NA");
  }
} p_na;
DISPATCHER<FUNCTION>::INSTALL d_na(&function_dispatcher, "na", &p_na);
/*--------------------------------------------------------------------------*/
// random funcs:
// Respectively you can give a parameter a statistical dispersion function,
// either rectangular distribution 
//	UNIF(nom_val, rel_var)
//	AUNIF(nom_val, abs_var)
// or normal distribution: 
//	GAUSS(nom_val, rel_var, sigma)
//	AGAUSS(nom_val, abs_var, sigma)
// Normal distribution can be represented with variation values of 1, 2 or 3 sigmas.
// In both cases rel_var=abs_var/nom_val.
// for now, stubs, return nom_val
/*--------------------------------------------------------------------------*/
class RANDOM_STUB : public FUNCTION {
public:
  std::string eval(CS& Cmd, const CARD_LIST* Scope)const override { untested();
    PARAMETER<double> arg;
    Cmd >> arg;
    arg.e_val(NOT_INPUT, Scope);
    return to_string(double(arg));
  }
} p_stub;
DISPATCHER<FUNCTION>::INSTALL d_stub(&function_dispatcher, "agauss|gauss|aunif|unif", &p_stub);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// vim:ts=8:sw=2:noet:
