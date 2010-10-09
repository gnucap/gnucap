/*$Id: mg_out_model.cc,v 26.128 2009/11/10 04:21:03 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 */
//testing=script 2006.11.01
#include "mg_out.h"
/*--------------------------------------------------------------------------*/
static void make_model_dispatcher(std::ofstream& out, const Model& m)
{
  if (m.level() != "") {
    out << "const int LEVEL(" << m.level() <<");\n"
      "/*--------------------------------------------------------------------------*/\n";
  }else{
  }
  if (!m.public_key_list().is_empty()) {
    out << 
      "namespace MODEL_" << m.name() << "_DISPATCHER { \n"
      "  static DEV_" << m.dev_type() << " p1d;\n"
      "  static MODEL_" << m.name() << " p1(&p1d);\n"
      "  static DISPATCHER<MODEL_CARD>::INSTALL\n"
      "    d1(&model_dispatcher, \"";
    for (Key_List::const_iterator
	 k = m.public_key_list().begin();
	 k != m.public_key_list().end();
	 ++k) {
      if (k != m.public_key_list().begin()) {
	out << '|';
      }else{
      }
      out << (**k).name();
    }
    out << "\", &p1);\n"
      "}\n"
      "/*--------------------------------------------------------------------------*/\n";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
static void make_sdp_constructor(std::ofstream& out, const Model& m)
{
#if 0
  out <<
    "SDP_" << m.name() << "::SDP_" << m.name() << "(const COMMON_COMPONENT* cc)\n"
    "  :SDP_" << m.inherit() << "(cc)\n"
    "{\n";
#else
  out << 
    "void SDP_" << m.name() << "::init(const COMMON_COMPONENT* cc)\n"
    "{\n"
    "  assert(cc);\n"
    "  SDP_" << m.inherit() << "::init(cc);\n";
#endif
  if (!m.size_dependent().is_empty()) {
    out <<
      "  const COMMON_" << m.dev_type()
	<< "* c = prechecked_cast<const COMMON_" << m.dev_type() << "*>(cc);\n"
      "  assert(c);\n"
      "  const MODEL_" << m.name() << "* m = prechecked_cast<const MODEL_" 
	<< m.name() << "*>(c->model());\n"
      "  assert(m);\n"
      "  const CARD_LIST* par_scope = m->scope();\n"
      "  assert(par_scope);\n";
    
    out << m.size_dependent().code_pre();

    out << "  // adjust: override\n";
    make_final_adjust_parameter_list(out, m.size_dependent().override());


    out << "  // adjust: raw\n";
    for (Parameter_List::const_iterator
	 p = m.size_dependent().raw().begin();
	 p != m.size_dependent().raw().end();
	 ++p) {
      if (!((**p).final_default().empty())) {
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, " << (**p).final_default() << ", par_scope);\n";
      }else if (!((**p).default_val().empty())) {
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, " << (**p).default_val() << ", par_scope);\n";
      }else{untested();
	out << "  " << (**p).code_name() << " = m->" << (**p).code_name()
	    << "(L, W, 0., par_scope);\n";
      }
      make_final_adjust_value(out, **p);
    }

    out << "  // adjust: calculated\n";
    make_final_adjust_value_list(out, m.size_dependent().calculated());

    out << "  // code_post\n"
	<< m.size_dependent().code_post();
  }else{
  }
  
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_tdp_constructor(std::ofstream& out, const Model& m)
{
  out << "TDP_" << m.name() << "::TDP_" << m.name() 
      << "(const DEV_" << m.dev_type() << '*';
  if (!m.hide_base() || !m.temperature().is_empty()) {
    out << " d";
  }else{
  }
  out << ")\n";
  if (!m.hide_base()) {
    out << "  :TDP_" << m.inherit() << "(d)\n";
  }else{
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
      "  assert(m);\n"
      "  const CARD_LIST* par_scope = d->scope();\n"
      "  assert(par_scope);\n";
    make_final_adjust_eval_parameter_list(out, m.temperature().raw());
    make_final_adjust(out, m.temperature());
  }else{
  }

  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_default_constructor(std::ofstream& out, const Model& m)
{
  out << "MODEL_" << m.name() << "::MODEL_" << m.name() << "(const BASE_SUBCKT* p)\n"
    "  :MODEL_" << m.inherit() << "(p)";

  make_construct_parameter_list(out, m.size_dependent().raw());
  make_construct_parameter_list(out, m.independent().raw());
  make_construct_parameter_list(out, m.independent().calculated());
  out << "\n{\n"
    "  if (ENV::run_mode != rPRE_MAIN) {\n"
    "    ++_count;\n"
    "  }else{\n"
    "  }\n";
  for (Parameter_List::const_iterator
       p = m.independent().override().begin();
       p != m.independent().override().end();
       ++p) {
    if (!((**p).final_default().empty())) {
      //out << "  " << (**p).code_name() << " = NA;\n";
      out << "  set_default(&" << (**p).code_name() << ", NA);\n";
    }else{
    }
    if (!((**p).default_val().empty())) {
      //out << "  " << (**p).code_name() << " = " 
      //	  << (**p).default_val() << ";\n";
      out << "  set_default(&" << (**p).code_name() << ", " 
      	  << (**p).default_val() << ");\n";
    }else{
    }
  }
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_copy_constructor(std::ofstream& out, const Model& m)
{
  out <<
    "MODEL_" << m.name() << "::MODEL_" << m.name() << "(const MODEL_" << m.name() << "& p)\n"
    "  :MODEL_" << m.inherit() << "(p)";

  make_copy_construct_parameter_list(out, m.size_dependent().raw());
  make_copy_construct_parameter_list(out, m.independent().raw());
  make_copy_construct_parameter_list(out, m.independent().calculated());
  out << "\n{\n"
    "  if (ENV::run_mode != rPRE_MAIN) {\n"
    "    ++_count;\n"
    "  }else{untested();//194\n"
    "  }\n";
#if 0
  for (Parameter_List::const_iterator
       p = m.independent().override().begin();
       p != m.independent().override().end();
       ++p) {untested();
    out << "  itested();\n";
    //out << ",\n   " << (**p).code_name() << "(p." << (**p).code_name() << ")";
  }
#endif
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_dev_type(std::ofstream& out, const Model& m)
{
  out << "std::string MODEL_" << m.name() << "::dev_type()const\n";
  if (!m.public_key_list().is_empty() || !m.private_key_list().is_empty()) {
    out << 
      "{\n"
      "  ";
    Key_List::const_iterator k = 
      (m.public_key_list().is_empty())
      ? m.private_key_list().begin()
      : m.public_key_list().begin();
    for (;;) {
      out << "if (" << (**k).var() << " == " << (**k).value() << ") {\n"
	"    return \"" << (**k).name() << "\";\n";
      ++k;
      if (k == m.public_key_list().end()) {
	k = m.private_key_list().begin();
      }else{
      }
      if (k == m.private_key_list().end()) {
	break;
      }else{
      }
      out << "  }else ";
    }
    out <<
      "  }else{untested();//235\n"
      "    return MODEL_" << m.inherit() << "::dev_type();\n"
      "  }\n";
  }else{
    out <<
      "{untested();//240\n"
      "  return MODEL_" << m.inherit() << "::dev_type();\n";
  }
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_set_dev_type(std::ofstream& out, const Model& m)
{
  out <<
    "void MODEL_" << m.name() << "::set_dev_type(const std::string& new_type)\n"
    "{\n"
    "  ";
  if (!m.public_key_list().is_empty() || !m.private_key_list().is_empty()) {
    Key_List::const_iterator k = 
      (m.public_key_list().is_empty())
      ? m.private_key_list().begin()
      : m.public_key_list().begin();
    for (;;) {
      out << "if (Umatch(new_type, \"" << (**k).name() << " \")) {\n"
	"    " <<  (**k).var() << " = " << (**k).value() << ";\n";
      ++k;
      if (k == m.public_key_list().end()) {
	k = m.private_key_list().begin();
      }else{
      }
      if (k == m.private_key_list().end()) {
	break;
      }else{
      }
      out << "  }else ";
    }
    out << "  }else{\n";
  }else{
    out << "{\n";
  }
  if (!m.hide_base()) {
    out << "    MODEL_" << m.inherit() << "::set_dev_type(new_type);\n";
  }else{
    out << "    MODEL_CARD::set_dev_type(new_type);\n";
  }
  out << "  }\n";
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_precalc(std::ofstream& out, const Model& m)
{
  out <<
    "void MODEL_" << m.name() << "::precalc_first()\n"
    "{\n"
    "    const CARD_LIST* par_scope = scope();\n"
    "    assert(par_scope);\n";
  if (!m.hide_base()) {
    out << "    MODEL_" << m.inherit() << "::precalc_first();\n";
  }else{
    out << "    MODEL_CARD::precalc_first();\n";
  }
  make_final_adjust_eval_parameter_list(out, m.independent().raw());
  make_final_adjust(out, m.independent());
  out <<
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
  out <<
    "void MODEL_" << m.name() << "::precalc_last()\n"
    "{\n";
  if (!m.hide_base()) {
    out << "    MODEL_" << m.inherit() << "::precalc_last();\n";
  }else{
    out << "    MODEL_CARD::precalc_last();\n";
  }
  out <<
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_new_sdp(std::ofstream& out, const Model& m)
{
  out << "SDP_CARD* MODEL_" << m.name() << "::new_sdp(COMMON_COMPONENT* c)const\n"
    "{\n"
    "  assert(c);\n"
    "  if (COMMON_" << m.dev_type() << "* cc = dynamic_cast<COMMON_" 
      << m.dev_type() << "*>(c)) {\n"
    "    if (cc->_sdp) {\n"
    "      cc->_sdp->init(cc);\n"
    "      return cc->_sdp;\n"
    "    }else{\n"
    "      delete cc->_sdp;\n"
    "      return new SDP_" << m.name() << "(c);\n"
    "    }\n"
    "  }else{\n"
    "    return MODEL_" << m.inherit() << "::new_sdp(c);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_set_param_by_index(std::ofstream& out, const Model& m)
{
  out << 
    "void MODEL_" << m.name() << "::set_param_by_index(int i, std::string& value, int offset)\n"
    "{\n"
    "  switch (MODEL_" << m.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  if (m.level() != "") {
    out << "  case " << i++ << ": level = value; break; //" << m.level() << "\n";
  }else{
    out << "  case " << i++ << ": untested(); break;\n";
  }
  for (Parameter_List::const_iterator 
       p = m.independent().override().begin(); 
       p != m.independent().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ": " << (**p).code_name() << " = value; break;\n";
    }else{
      out << "  case " << i++ << ": unreachable(); break;\n";
    }
  }
  assert(i == 1 + m.independent().override().size());

  for (Parameter_List::const_iterator
       p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end();
       ++p) {
    out << "  case " << i++ << ": " << (**p).code_name() << ".set_nom(value); break;\n";
    out << "  case " << i++ << ": " << (**p).code_name() << ".set_w(value); break;\n";
    out << "  case " << i++ << ": " << (**p).code_name() << ".set_l(value); break;\n";
    out << "  case " << i++ << ": " << (**p).code_name() << ".set_p(value); break;\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size());

  for (Parameter_List::const_iterator 
       p = m.independent().raw().begin(); 
       p != m.independent().raw().end();
       ++p) {
    out << "  case " << i++ << ": " << (**p).code_name() << " = value; break;\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size()
	 + m.independent().raw().size());

  if (!m.hide_base()) {
    out << "  default: MODEL_" << m.inherit() 
	<< "::set_param_by_index(i, value, offset); break;\n";
  }else{
    out << "  default: throw Exception_Too_Many(i, " << i-1 << ", offset); break;\n";
  }
  out <<
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_model_param_is_printable(std::ofstream& out, const Model& m)
{
  make_tag();
  out << "bool MODEL_" << m.name() 
      << "::param_is_printable(int i)const\n"
    "{\n"
    "  switch (MODEL_" << m.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  if (m.level() != "") {
    out << "  case " << i++ << ":  return (true);\n";
  }else{
    out << "  case " << i++ << ":  return (false);\n";
  }
  for (Parameter_List::const_iterator 
       p = m.independent().override().begin(); 
       p != m.independent().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return (";
      if (!((**p).print_test().empty())) {
	out << (**p).print_test() << ");\n";
	//}else if ((**p).default_val() == "NA" && (**p).final_default().empty()) {untested();
	//out << (**p).code_name() << ".has_hard_value());\n"; //" != NA);\n";
      }else if ((**p).default_val() == "NA") {
	out << (**p).code_name() << ".has_hard_value());\n";
      }else{
	out << "true);\n";
      }
    }else{
      out << "  case " << i++ << ":  return (false);\n";
    }
  }
  assert(i == 1 + m.independent().override().size());

  for (Parameter_List::const_iterator
       p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end();
       ++p) {
    out << "  case " << i++ << ":  return (";
    if (!((**p).print_test().empty())) {
      out << (**p).print_test() << ");\n";
    }else if ((**p).default_val() == "NA") {
      out << (**p).code_name() << ".has_value());\n";
    }else{
      out << "true);\n";
    }
    out << "  case " << i++ << ":  return (" << (**p).code_name() << ".w_has_value());\n";
    out << "  case " << i++ << ":  return (" << (**p).code_name() << ".l_has_value());\n";
    out << "  case " << i++ << ":  return (" << (**p).code_name() << ".p_has_value());\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size());

  for (Parameter_List::const_iterator 
       p = m.independent().raw().begin(); 
       p != m.independent().raw().end();
       ++p) {
    out << "  case " << i++ << ":  return (";
    if (!((**p).print_test().empty())) {
      out << (**p).print_test() << ");\n";
      //}else if ((**p).default_val() == "NA" && (**p).final_default().empty()) {
      //out << (**p).code_name() << ".has_hard_value());\n"; //" != NA);\n";
    }else if ((**p).default_val() == "NA") {
      out << (**p).code_name() << ".has_hard_value());\n";
    }else{
      out << "true);\n";
    }
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size()
	 + m.independent().raw().size());

  if (!m.hide_base()) {
    out << "  default: return MODEL_" << m.inherit() << "::param_is_printable(i);\n";
  }else{
    out << "  default: return false;\n";
  }
  out <<
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_model_param_name(std::ofstream& out, const Model& m)
{
  make_tag();
  out << "std::string MODEL_" << m.name() << "::param_name(int i)const\n"
    "{\n"
    "  switch (MODEL_" << m.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  if (m.level() != "") {
    out << "  case " << i++ << ":  return \"level\";\n";
  }else{
    out << "  case " << i++ << ":  return \"=====\";\n";
  }
  for (Parameter_List::const_iterator 
       p = m.independent().override().begin(); 
       p != m.independent().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return \"" << to_lower((**p).user_name()) << "\";\n";
    }else{
      out << "  case " << i++ << ":  return \"=====\";\n";
    }
  }
  assert(i == 1 + m.independent().override().size());

  for (Parameter_List::const_iterator
       p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return \""  << to_lower((**p).user_name()) << "\";\n";
      out << "  case " << i++ << ":  return \"w" << to_lower((**p).user_name()) << "\";\n";
      out << "  case " << i++ << ":  return \"l" << to_lower((**p).user_name()) << "\";\n";
      out << "  case " << i++ << ":  return \"p" << to_lower((**p).user_name()) << "\";\n";
    }else{unreachable();
      out << "  case " << i++ << ":  return \"=====\";\n";
    }
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size());

  for (Parameter_List::const_iterator 
       p = m.independent().raw().begin(); 
       p != m.independent().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return \"" << to_lower((**p).user_name()) << "\";\n";
    }else{unreachable();
      out << "  case " << i++ << ":  return \"=====\";\n";
    }
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size()
	 + m.independent().raw().size());

  if (!m.hide_base()) {
    out << "  default: return MODEL_" << m.inherit() << "::param_name(i);\n";
  }else{
    out << "  default: return \"\";\n";
  }
  out <<
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_model_param_name_or_alias(std::ofstream& out, const Model& m)
{
  make_tag();
  out << "std::string MODEL_" << m.name() << "::param_name(int i, int j)const\n"
    "{\n"
    "  if (j == 0) {\n"
    "    return param_name(i);\n"
    "  }else if (j == 1) {\n"
    "    switch (MODEL_" << m.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  if (m.level() != "") {
    out << "    case " << i++ << ":  return \"\";\n";
  }else{
    out << "    case " << i++ << ":  return \"\";\n";
  }
  for (Parameter_List::const_iterator 
       p = m.independent().override().begin(); 
       p != m.independent().override().end();
       ++p) {
    out << "    case " << i++ << ":  return \"" << to_lower((**p).alt_name()) << "\";\n";
  }
  assert(i == 1 + m.independent().override().size());

  for (Parameter_List::const_iterator
       p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      if (!((**p).alt_name().empty())) {
	out << "    case " << i++ << ":  return \""  << to_lower((**p).alt_name()) << "\";\n";
	out << "    case " << i++ << ":  return \"w" << to_lower((**p).alt_name()) << "\";\n";
	out << "    case " << i++ << ":  return \"l" << to_lower((**p).alt_name()) << "\";\n";
	out << "    case " << i++ << ":  return \"p" << to_lower((**p).alt_name()) << "\";\n";
      }else{
	out << "    case " << i++ << ":  return \"\";\n";
	out << "    case " << i++ << ":  return \"\";\n";
	out << "    case " << i++ << ":  return \"\";\n";
	out << "    case " << i++ << ":  return \"\";\n";
      }
    }else{unreachable();
      out << "    case " << i++ << ":  return \"\";\n";
      out << "    case " << i++ << ":  return \"\";\n";
      out << "    case " << i++ << ":  return \"\";\n";
      out << "    case " << i++ << ":  return \"\";\n";
    }
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size());

  for (Parameter_List::const_iterator 
       p = m.independent().raw().begin(); 
       p != m.independent().raw().end();
       ++p) {
    out << "    case " << i++ << ":  return \"" << to_lower((**p).alt_name()) << "\";\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size()
	 + m.independent().raw().size());

  if (!m.hide_base()) {
    out << "    default: return MODEL_" << m.inherit() << "::param_name(i, j);\n";
  }else{
    out << "    default: return \"\";\n";
  }
  out << "    }\n";
  if (!m.hide_base()) {
    out << 
      "  }else if (i < " << i << ") {\n"
      "    return \"\";\n"
      "  }else{\n"
      "    return MODEL_" << m.inherit() << "::param_name(i, j);\n";
  }else{
    out << 
      "  }else{\n"
      "    return \"\";\n";
  }
  out <<
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_model_param_value(std::ofstream& out, const Model& m)
{
  make_tag();
  out << "std::string MODEL_" << m.name() << "::param_value(int i)const\n"
    "{\n"
    "  switch (MODEL_" << m.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  if (m.level() != "") {
    out << "  case " << i++ << ":  return \"" << m.level() << "\";\n";
  }else{
    out << "  case " << i++ << ":  unreachable(); return \"\";\n";
  }
  for (Parameter_List::const_iterator 
       p = m.independent().override().begin(); 
       p != m.independent().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return " << (**p).code_name() << ".string();\n";
    }else{
      out << "  case " << i++ << ":  unreachable(); return \"\";\n";
    }
  }
  assert(i == 1 + m.independent().override().size());

  for (Parameter_List::const_iterator
       p = m.size_dependent().raw().begin();
       p != m.size_dependent().raw().end();
       ++p) {
    out << "  case " << i++ << ":  return " << (**p).code_name() << ".string();\n";
    out << "  case " << i++ << ":  return " << (**p).code_name() << ".w_string();\n";
    out << "  case " << i++ << ":  return " << (**p).code_name() << ".l_string();\n";
    out << "  case " << i++ << ":  return " << (**p).code_name() << ".p_string();\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size());

  for (Parameter_List::const_iterator 
       p = m.independent().raw().begin(); 
       p != m.independent().raw().end();
       ++p) {
    out << "  case " << i++ << ":  return " << (**p).code_name() << ".string();\n";
  }
  assert(i == 1 + m.independent().override().size() + 4 * m.size_dependent().raw().size()
	 + m.independent().raw().size());

  if (!m.hide_base()) {
    out << "  default: return MODEL_" << m.inherit() << "::param_value(i);\n";
  }else{
    out << "  default: return \"\";\n";
  }
  out <<
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_model_is_valid(std::ofstream& out, const Model& m)
{
  out <<
    "bool MODEL_" << m.name() << "::is_valid(const COMPONENT* d)const\n"
    "{\n"
    "  assert(d);\n";
  if (m.validate().is_empty()) {
    out << "  return MODEL_" << m.inherit() << "::is_valid(d);\n";
  }else{
    out <<
      "  const COMMON_" << m.dev_type() << "* c = dynamic_cast<const COMMON_"
	<< m.dev_type() << "*>(d->common());\n"
      "  if (!c) {\n"
      "    return MODEL_" << m.inherit() << "::is_valid(d);\n"
      "  }else{\n"
      "    const MODEL_" << m.name() << "* m = this;"
	<< m.validate() <<
      "  }\n";
  }
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_tr_eval(std::ofstream& out, const Model& m)
{
  out << "void MODEL_" << m.name() << "::tr_eval(COMPONENT*";
  if (m.tr_eval().is_empty() && m.temperature().is_empty()) {
    out << ")const\n{untested();//425\n";
  }else{
    out << " brh)const\n{\n"
      "  DEV_" << m.dev_type() << "* d = prechecked_cast<DEV_" << m.dev_type() << "*>(brh);\n"
      "  assert(d);\n"
      "  const COMMON_" << m.dev_type() << "* c = prechecked_cast<const COMMON_"
	<< m.dev_type() << "*>(d->common());\n"
      "  assert(c);\n"
      "  const SDP_" << m.name() << "* s = prechecked_cast<const SDP_" 
	<< m.name() << "*>(c->sdp());\n"
      "  assert(s);\n"
      "  const MODEL_" << m.name() << "* m = this;\n";
    if (!m.temperature().is_empty()) {
      out << "  const TDP_" << m.name() << " T(d);\n"
	"  const TDP_" << m.name() << "* t = &T;\n";
    }else{
    }
    out << m.tr_eval();
  }
  out << "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_cc_model(std::ofstream& out, const Model& m)
{
  out << "int MODEL_" << m.name() << "::_count = 0;\n"
    "/*--------------------------------------------------------------------------*/\n";
  make_model_dispatcher(out, m);
  make_sdp_constructor(out, m);
  make_tdp_constructor(out, m);
  make_model_default_constructor(out, m);
  make_model_copy_constructor(out, m);
  make_model_dev_type(out, m);
  make_model_set_dev_type(out, m);
  make_model_precalc(out, m);
  make_model_new_sdp(out, m);
  make_model_set_param_by_index(out, m);
  make_model_param_is_printable(out, m);
  make_model_param_name(out, m);
  make_model_param_name_or_alias(out, m);
  make_model_param_value(out, m);
  make_model_is_valid(out, m);
  make_tr_eval(out, m);
  out << "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
