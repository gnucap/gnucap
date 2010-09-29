/*$Id: mg_out_model.cc,v 25.92 2006/06/28 15:03:12 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "mg_out.h"
/*--------------------------------------------------------------------------*/
static void make_sdp_constructor(std::ofstream& out, const Model& m)
{
  out << "SDP_" << m.name() << "::SDP_" << m.name() 
      << "(const COMMON_COMPONENT* cc)\n"
      << "  :SDP_" << m.inherit() << "(cc)\n"
      << "{\n";
  if (!m.size_dependent().is_empty()) {
    out << "  assert(cc);\n"
      "  const COMMON_" << m.dev_type()
	<< "* c = prechecked_cast<const COMMON_" << m.dev_type() << "*>(cc);\n"
      "  assert(c);\n"
      "  const MODEL_" << m.name() 
	<< "* m = prechecked_cast<const MODEL_" 
	<< m.name() << "*>(c->model());\n"
      "  assert(m);\n";
    if (!(m.size_dependent().raw().is_empty())) {
      out << "  const CARD_LIST* par_scope = m->scope();\n"
	"  assert(par_scope);\n";
    }
    out << m.size_dependent().code_pre();

    out << "  // adjust: override\n";
    make_final_adjust_parameter_list(out, m.size_dependent().override());


    out << "  // adjust: raw\n";
    for (Parameter_List::const_iterator
	   p = m.size_dependent().raw().begin();
	 p != m.size_dependent().raw().end(); ++p) {
      {if (!((**p).final_default().empty())) {
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, " << (**p).final_default() << ", par_scope);\n";
      }else if (!((**p).default_val().empty())) {
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, " << (**p).default_val() << ", par_scope);\n";
      }else{
	untested();
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, 0., par_scope);\n";
      }}
      make_final_adjust_value(out, **p);
    }

    out << "  // adjust: calculated\n";
    make_final_adjust_value_list(out, m.size_dependent().calculated());

    out << "  // code_post\n"
	<< m.size_dependent().code_post();
  }
  
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_tdp_constructor(std::ofstream& out, const Model& m)
{
  out << "TDP_" << m.name() << "::TDP_" << m.name() 
      << "(const DEV_" << m.dev_type() << '*';
  if (!m.is_base() || !m.temperature().is_empty()) {
    out << " d";
  }
  out << ")\n";
  if (!m.is_base()) {
    out << "  :TDP_" << m.inherit() << "(d)\n";
  }
  out << "{\n";
  if (!m.temperature().is_empty()) {
    out << "  assert(d);\n"
      "  const COMMON_" << m.dev_type()
	<< "* c = prechecked_cast<const COMMON_" << m.dev_type()
	<< "*>(d->common());\n"
      "  assert(c);\n"
      "  const SDP_" << m.name() << "* s = prechecked_cast<const SDP_" 
	<< m.name() << "*>(c->sdp());\n"
      "  assert(s);\n"
      "  const MODEL_" << m.name() << "* m = prechecked_cast<const MODEL_" 
	<< m.name() << "*>(c->model());\n"
      "  assert(m);\n";
    make_final_adjust(out, m.temperature());
  }

  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_constructor(std::ofstream& out, const Model& m)
{
  out << "MODEL_" << m.name() << "::MODEL_" << m.name() << "()\n"
    "  :MODEL_" << m.inherit() << "()";

  make_construct_parameter_list(out, m.size_dependent().raw());
  make_construct_parameter_list(out, m.independent().raw());
  make_construct_parameter_list(out, m.independent().calculated());
  out << "\n{\n"
    "  ++_count;\n";
  for (Parameter_List::const_iterator
	 p = m.independent().override().begin();
       p != m.independent().override().end();
       ++p) {
    if (!((**p).final_default().empty())) {
      out << "  " << (**p).code_name() << " = NA;\n";
    }
    if (!((**p).default_val().empty())) {
      out << "  " << (**p).code_name() << " = " << (**p).default_val() <<";\n";
    }
  }
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_parse_front(std::ofstream& out, const Model& m)
{
  out << "bool MODEL_" << m.name() << "::parse_front(CS&";
  {if (!m.key_list().is_empty()) {
    out << " cmd)\n"
      "{\n"
      "  return ONE_OF\n";
    for (Key_List::const_iterator k = m.key_list().begin();
	  k != m.key_list().end();
	  ++k) {
      out << "    || set(cmd, \"" << (**k).name() << "\", &" 
	  << (**k).var() << ", " << (**k).value() << ")\n";
    }
    if (!m.is_base()) {
      untested();
      out << "    || MODEL_" << m.inherit() << "::parse_front(cmd)\n";
    }
    out << "    ;\n";
  }else{
    {if (!m.is_base()) {
      out << " cmd)\n"
	"{\n"
	"  return MODEL_" << m.inherit() << "::parse_front(cmd);\n";
    }else{
      untested();
      out << ")\n"
	"{\n"
	"  return true;\n";
    }}
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_parse_params(std::ofstream& out, const Model& m)
{
  out << "bool MODEL_" << m.name() << "::parse_params(CS& cmd)\n"
    "{\n"
    "  return ONE_OF\n";
  make_get_param_list(out, m.independent().override());
  make_get_param_list(out, m.size_dependent().raw());
  make_get_param_list(out, m.independent().raw());
  if (!m.is_base()) {
    out << "    || MODEL_" << m.inherit() << "::parse_params(cmd)\n";
  }
  out << "    ;\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_parse_finish(std::ofstream& out, const Model& m)
{
  out <<
    "void MODEL_" << m.name() << "::elabo1()\n"
    "{\n"
    "  if (1 || !evaluated()) {\n"
    "    const CARD_LIST* par_scope = scope();\n"
    "    assert(par_scope);\n";
  if (!m.is_base()) {
    out << "    MODEL_" << m.inherit() << "::elabo1();\n";
  }
  make_final_adjust(out, m.independent());
  out <<
    "  }else{\n"
    "    untested();\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_new_sdp(std::ofstream& out, const Model& m)
{
  out << "SDP_CARD* MODEL_" << m.name() 
      << "::new_sdp(const COMMON_COMPONENT* c)const\n"
    "{\n"
    "  assert(c);\n"
    "  {if (dynamic_cast<const COMMON_" << m.dev_type() << "*>(c)) {\n"
    "    return new SDP_" << m.name() << "(c);\n"
    "  }else{\n"
    "    return MODEL_" << m.inherit() << "::new_sdp(c);\n"
    "  }}\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_print_front(std::ofstream& out, const Model& m)
{
  out << "void MODEL_" << m.name() << "::print_front(OMSTREAM&";
  {if (!m.key_list().is_empty()) {
    out << " o)const\n"
      "{\n"
      "  {";
    Key_List::const_iterator k = m.key_list().begin();
    for (;;) {
      out << "if (" << (**k).var() << " == " << (**k).value() << ") {\n"
	  << "    o << \"  " << to_lower((**k).name()) << "\";\n";
      ++k;
      if (k == m.key_list().end()) {
	break;
      }
      out << "  }else ";
    }
    out << "  }else{\n";
    {if (!m.is_base()) {
      out << "    MODEL_" << m.inherit() << "::print_front(o);\n";
    }else{
      out << "    unreachable();\n";
    }}
    out << "  }}\n";
  }else{
    {if (!m.is_base()) {
      out << " o)const\n"
	"{\n"
	"  MODEL_" << m.inherit() << "::print_front(o);\n";
    }else{
      out << ")const\n"
	"{\n";
    }}
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_print_params(std::ofstream& out, const Model& m)
{
  out << "void MODEL_" << m.name() << "::print_params(OMSTREAM& o)const\n{\n";
  
  if (m.level() != "") {
    out << "  o << \"level=" << m.level() << "\";\n";
  }
  if (!m.is_base()) {
    out << "  MODEL_" << m.inherit() << "::print_params(o);\n";
  }
  make_print_param_list(out, m.independent().override());

  for (Parameter_List::const_iterator
	 p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end(); ++p) {
    if (!((**p).user_name().empty())) {
      out << "  " << (**p).code_name() << ".print(o, \"" 
	  << to_lower((**p).user_name()) << "\");\n";
    }
  }

  make_print_param_list(out, m.independent().raw());
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_print_calculated(std::ofstream& out, const Model& m)
{
  out << "void MODEL_" << m.name() 
      << "::print_calculated(OMSTREAM& o)const\n{\n";
  {if (!m.is_base()) {
    out << "  MODEL_" << m.inherit() << "::print_calculated(o);\n";
  }else{
    out << "  o << \"\";\n"; // print nothing, suppresses a warning
  }}
  make_print_calc_param_list(out, m.independent().override());
  make_print_calc_param_list(out, m.independent().raw());
  make_print_calc_param_list(out, m.independent().calculated());
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_is_valid(std::ofstream& out, const Model& m)
{
  out <<
    "bool MODEL_" << m.name()
      << "::is_valid(const COMMON_COMPONENT* cc)const\n"
    "{\n";
  {if (m.validate().is_empty()) {
    out << "  return MODEL_" << m.inherit() << "::is_valid(cc);\n";
  }else{
    out << "  const COMMON_" << m.dev_type()
	<< "* c = dynamic_cast<const COMMON_" << m.dev_type() << "*>(cc);\n"
      "  {if (!c) {\n"
      "    return MODEL_" << m.inherit() << "::is_valid(cc);\n"
      "  }else{\n"
      "    const MODEL_" << m.name() << "* m = this;"
	<< m.validate() <<
      "  }}\n";
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_tr_eval(std::ofstream& out, const Model& m)
{
  out << "void MODEL_" << m.name() << "::tr_eval(COMPONENT*";
  {if (m.tr_eval().is_empty() && m.temperature().is_empty()) {
    out << ")const\n{\n";
  }else{
    out << " brh)const\n{\n"
      "  DEV_" << m.dev_type() << "* d = prechecked_cast<DEV_"
	<< m.dev_type() << "*>(brh);\n"
      "  assert(d);\n"
      "  const COMMON_" << m.dev_type()
	<< "* c = prechecked_cast<const COMMON_" << m.dev_type() 
	<< "*>(d->common());\n"
      "  assert(c);\n"
      "  const SDP_" << m.name() << "* s = prechecked_cast<const SDP_" 
	<< m.name() << "*>(c->sdp());\n"
      "  assert(s);\n"
      "  const MODEL_" << m.name() << "* m = this;\n";
    if (!m.temperature().is_empty()) {
      out << "  const TDP_" << m.name() << " T(d);\n"
	"  const TDP_" << m.name() << "* t = &T;\n";
    }
    out << m.tr_eval();
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_cc_model(std::ofstream& out, const Model& m)
{
  out << "int MODEL_" << m.name() << "::_count = 0;\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
  make_sdp_constructor(out, m);
  make_tdp_constructor(out, m);
  make_model_constructor(out, m);
  make_model_parse_front(out, m);
  make_model_parse_params(out, m);
  make_model_parse_finish(out, m);
  make_model_new_sdp(out, m);
  make_model_print_front(out, m);
  make_model_print_params(out, m);
  make_model_print_calculated(out, m);
  make_model_is_valid(out, m);
  make_tr_eval(out, m);
  out << "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
