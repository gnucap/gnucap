/*$Id: mg_out_common.cc,v 26.130 2009/11/15 21:51:08 al Exp $ -*- C++ -*-
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
static void make_common_default_constructor(std::ofstream& out,const Device& d)
{
  make_tag();
  out <<
    "COMMON_" << d.name() << "::COMMON_" << d.name() << "(int c)\n"
    "  :COMMON_COMPONENT(c)";
  make_construct_parameter_list(out, d.common().raw());
  out << ",\n   _sdp(0)";
  make_construct_parameter_list(out, d.common().calculated());
  for (Args_List::const_iterator
       p = d.circuit().args_list().begin();
       p != d.circuit().args_list().end();
       ++p) {
    out << ",\n   _" << (**p).name() << "(0)";
  }
  out <<
    "\n"
    "{\n"
    "  ++_count;\n";
  for (Parameter_List::const_iterator
       p = d.common().override().begin();
       p != d.common().override().end();
       ++p) {untested();
    if (!((**p).final_default().empty())) {untested();
      out << "  " << (**p).code_name() << " = NA;\n";
    }else{untested();
    }
    if (!((**p).default_val().empty())) {untested();
      out << "  " << (**p).code_name() << " = " << (**p).default_val() <<";\n";
    }else{untested();
    }
  }
  out <<
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_copy_constructor(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "COMMON_" << d.name() << "::COMMON_" << d.name() << "(const COMMON_" << d.name() << "& p)\n"
    "  :COMMON_COMPONENT(p)";
  make_copy_construct_parameter_list(out, d.common().raw());
  out << ",\n   _sdp(0)";
  make_copy_construct_parameter_list(out, d.common().calculated());
  for (Args_List::const_iterator
       p = d.circuit().args_list().begin();
       p != d.circuit().args_list().end();
       ++p) {
    out << ",\n   _" << (**p).name() << "(0)";
  }
  out << 
    "\n"
    "{\n"
    "  ++_count;\n";
  for (Parameter_List::const_iterator
       p = d.common().override().begin();
       p != d.common().override().end();
       ++p) {untested();
    out << ",\n   " << (**p).code_name() << "(p." << (**p).code_name() << ")";
  }
  out <<
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_destructor(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "COMMON_" << d.name() << "::~COMMON_" << d.name() << "()\n"
    "{\n";
  for (Args_List::const_iterator
       p = d.circuit().args_list().begin();
       p != d.circuit().args_list().end();
       ++p) {
    out << "  detach_common(&_" << (**p).name() << ");\n";
  }
  out <<
    "  --_count;\n"
    "  delete _sdp;\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_operator_equal(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "bool COMMON_" << d.name() << "::operator==(const COMMON_COMPONENT& x)const\n"
    "{\n"
    "  const COMMON_" << d.name() << "* p = dynamic_cast<const COMMON_" << d.name() << "*>(&x);\n"
    "  return (p\n";
  for (Parameter_List::const_iterator
       p = d.common().raw().begin();
       p != d.common().raw().end();
       ++p) {
    out << "    && " << (**p).code_name() << " == p->" << (**p).code_name() << '\n';
  }
  out << 
    "    && _sdp == p->_sdp\n"
    "    && COMMON_COMPONENT::operator==(x));\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_set_param_by_index(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "void COMMON_" << d.name() << "::set_param_by_index(int I, std::string& Value, int Offset)\n"
    "{\n"
    "  switch (COMMON_" << d.name() << "::param_count() - 1 - I) {\n";
  size_t i = 0;
  for (Parameter_List::const_iterator 
       p = d.common().override().begin(); 
       p != d.common().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  " << (**p).code_name() << " = Value; break;\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size());
  for (Parameter_List::const_iterator 
       p = d.common().raw().begin(); 
       p != d.common().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  " << (**p).code_name() << " = Value; break;\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size() + d.common().raw().size());

  out <<
    "  default: COMMON_COMPONENT::set_param_by_index(I, Value, Offset);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_param_is_printable(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "bool COMMON_" << d.name() << "::param_is_printable(int i)const\n"
    "{\n"
    "  switch (COMMON_" << d.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  for (Parameter_List::const_iterator 
       p = d.common().override().begin(); 
       p != d.common().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return (";
      if (!((**p).print_test().empty())) {
	out << (**p).print_test() << ");\n";
      }else if ((**p).default_val() == "NA") {
	out << (**p).code_name() << " != NA);\n";
      }else{
	out << "true);\n";
      }
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size());
  for (Parameter_List::const_iterator 
       p = d.common().raw().begin(); 
       p != d.common().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return (";
      if (!((**p).print_test().empty())) {
	out << (**p).print_test() << ");\n";
      }else if ((**p).default_val() == "NA") {
	out << (**p).code_name() << " != NA);\n";
      }else{
	out << "true);\n";
      }
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size() + d.common().raw().size());
  out <<
    "  default: return COMMON_COMPONENT::param_is_printable(i);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_param_name(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "std::string COMMON_" << d.name() << "::param_name(int i)const\n"
    "{\n"
    "  switch (COMMON_" << d.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  for (Parameter_List::const_iterator 
       p = d.common().override().begin(); 
       p != d.common().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return \"" << to_lower((**p).user_name()) << "\";\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size());
  for (Parameter_List::const_iterator 
       p = d.common().raw().begin(); 
       p != d.common().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return \"" << to_lower((**p).user_name()) << "\";\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size() + d.common().raw().size());
  out <<
    "  default: return COMMON_COMPONENT::param_name(i);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_param_name_or_alias(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "std::string COMMON_" << d.name() << "::param_name(int i, int j)const\n"
    "{\n"
    "  if (j == 0) {\n"
    "    return param_name(i);\n"
    "  }else if (j == 1) {\n"
    "    switch (COMMON_" << d.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  for (Parameter_List::const_iterator 
       p = d.common().override().begin(); 
       p != d.common().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "    case " << i++ << ":  return \"" << to_lower((**p).alt_name()) << "\";\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size());
  for (Parameter_List::const_iterator 
       p = d.common().raw().begin(); 
       p != d.common().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "    case " << i++ << ":  return \"" << to_lower((**p).alt_name()) << "\";\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size() + d.common().raw().size());
  out <<
    "    default: return \"\";\n"
    "    }\n"
    "  }else{untested();//281\n"
    "    return COMMON_COMPONENT::param_name(i, j);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_param_value(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "std::string COMMON_" << d.name() << "::param_value(int i)const\n"
    "{\n"
    "  switch (COMMON_" << d.name() << "::param_count() - 1 - i) {\n";
  size_t i = 0;
  for (Parameter_List::const_iterator 
       p = d.common().override().begin(); 
       p != d.common().override().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return " << (**p).code_name() << ".string();\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size());
  for (Parameter_List::const_iterator 
       p = d.common().raw().begin(); 
       p != d.common().raw().end();
       ++p) {
    if (!((**p).user_name().empty())) {
      out << "  case " << i++ << ":  return " << (**p).code_name() << ".string();\n";
    }else{unreachable();
    }
  }
  assert(i == d.common().override().size() + d.common().raw().size());
  out <<
    "  default: return COMMON_COMPONENT::param_value(i);\n"
    "  }\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_expand(std::ofstream& out, const Device& d)
{
  make_tag();
  out <<
    "void COMMON_" << d.name() << "::expand(const COMPONENT* d)\n"
    "{\n"
    "  COMMON_COMPONENT::expand(d);\n"
    "  attach_model(d);\n"
    "  COMMON_" << d.name() << "* c = this;\n"
    "  const MODEL_" << d.model_type() << "* m = dynamic_cast<const MODEL_" 
      << d.model_type() << "*>(model());\n"
    "  if (!m) {\n"
    "    throw Exception_Model_Type_Mismatch(d->long_label(), modelname(), \"" 
      << d.parse_name() << "\");\n"
    "  }else{\n"
    "  }\n"
    "  // size dependent\n"
    "  //delete _sdp;\n"
    "  _sdp = m->new_sdp(this);\n"
    "  assert(_sdp);\n"
    "  const SDP_" << d.model_type() << "* s = prechecked_cast<const SDP_" 
      << d.model_type() << "*>(_sdp);\n"
    "  assert(s);\n"
    "\n"
    "  // subcircuit commons, recursive\n";
  for (Args_List::const_iterator
       p = d.circuit().args_list().begin();
       p != d.circuit().args_list().end();
       ++p) {
    out << "  COMMON_" << (**p).type() << "* " << (**p).name() 
	<< " = new COMMON_" << (**p).type() << ";\n";
    for (Arg_List::const_iterator
	 a = (**p).begin();
	 a != (**p).end();
	 ++a) {
      out << "  " << (**p).name() << "->" << (**a).arg() << ";\n";
    }
    out << "  attach_common(" << (**p).name() << ", &_" << (**p).name() << ");\n\n";
  }
  out <<
    "  assert(c == this);\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n"
    "void COMMON_" << d.name() << "::precalc_first(const CARD_LIST* par_scope)\n"
    "{\n"
    "  assert(par_scope);\n"
    "  COMMON_COMPONENT::precalc_first(par_scope);\n";

  make_final_adjust_eval_parameter_list(out, d.common().raw());

  out <<
    "}\n"
    "/*--------------------------------------------------------------------------*/\n"
    "void COMMON_" << d.name() << "::precalc_last(const CARD_LIST* par_scope)\n"
    "{\n"
    "  assert(par_scope);\n"
    "  COMMON_COMPONENT::precalc_last(par_scope);\n"
    "  COMMON_" << d.name() << "* c = this;\n"
    "  const MODEL_" << d.model_type() << "* m = prechecked_cast<const MODEL_" 
      << d.model_type() << "*>(model());\n";

  make_final_adjust(out, d.common());

  out <<
    "\n"
    "  // size dependent\n"
    "  //delete _sdp;\n"
    "  _sdp = m->new_sdp(this);\n"
    "  assert(_sdp);\n"
    "  const SDP_" << d.model_type() << "* s = prechecked_cast<const SDP_" 
      << d.model_type() << "*>(_sdp);\n"
    "  assert(s);\n"
    "\n"
    "  // subcircuit commons, recursive\n";
  for (Args_List::const_iterator
       p = d.circuit().args_list().begin();
       p != d.circuit().args_list().end();
       ++p) {
    out << "  COMMON_" << (**p).type() << "* " << (**p).name() 
	<< " = new COMMON_" << (**p).type() << ";\n";
    for (Arg_List::const_iterator
	 a = (**p).begin();
	 a != (**p).end();
	 ++a) {
      out << "  " << (**p).name() << "->" << (**a).arg() << ";\n";
    }
    out << "  attach_common(" << (**p).name() << ", &_" << (**p).name() << ");\n\n";
  }

  out <<
    "  assert(c == this);\n"
    "}\n"
    "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_cc_common(std::ofstream& out, const Device& d)
{
  make_tag();
  make_common_default_constructor(out, d);
  make_common_copy_constructor(out, d);
  make_common_destructor(out, d);
  make_common_operator_equal(out, d);
  make_common_set_param_by_index(out, d);
  make_common_param_is_printable(out, d);
  make_common_param_name(out, d);
  make_common_param_name_or_alias(out, d);
  make_common_param_value(out, d);
  make_common_expand(out, d);
  out << "/*--------------------------------------------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
