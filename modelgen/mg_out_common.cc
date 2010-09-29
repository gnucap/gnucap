/*$Id: mg_out_common.cc,v 25.92 2006/06/28 15:03:12 al Exp $ -*- C++ -*-
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
static void make_common_default_constructor(std::ofstream& out,const Device& d)
{
  out << "COMMON_" << d.name() << "::COMMON_" << d.name() << "(int c)\n"
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
  out << "\n{\n"
    "  ++_count;\n";
  for (Parameter_List::const_iterator
	 p = d.common().override().begin();
       p != d.common().override().end();
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
static void make_common_copy_constructor(std::ofstream& out, const Device& d)
{
  out << "COMMON_" << d.name() << "::COMMON_" << d.name() 
      << "(const COMMON_" << d.name() << "& p)\n"
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
  out << "\n{\n"
    "  ++_count;\n";
  for (Parameter_List::const_iterator
	 p = d.common().override().begin();
       p != d.common().override().end();
       ++p) {
    out << ",\n   " << (**p).code_name() << "(p." << (**p).code_name() << ")";
  }
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_destructor(std::ofstream& out, const Device& d)
{
  out << "COMMON_" << d.name() << "::~COMMON_" << d.name() << "()\n"
    "{\n";
  for (Args_List::const_iterator
	  p = d.circuit().args_list().begin();
	p != d.circuit().args_list().end();
	++p) {
    out << "  detach_common(&_" << (**p).name() << ");\n";
  }
  out << "  --_count;\n"
    "  delete _sdp;\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_operator_equal(std::ofstream& out, const Device& d)
{
  out << "bool COMMON_" << d.name() 
      << "::operator==(const COMMON_COMPONENT& x)const\n"
    "{\n"
    "  const COMMON_" << d.name() << "* p = dynamic_cast<const COMMON_" 
      << d.name() << "*>(&x);\n"
    "  return (p\n";
  for (Parameter_List::const_iterator
	  p = d.common().raw().begin();
	p != d.common().raw().end();
	++p) {
    out << "    && " << (**p).code_name() 
	<< " == p->" << (**p).code_name() << '\n';
  }
  out << 
    "    && _sdp == p->_sdp\n"
    "    && COMMON_COMPONENT::operator==(x));\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_parse(std::ofstream& out, const Device& d)
{
  out << "void COMMON_" << d.name() << "::parse(CS& cmd)\n"
    "{\n"
    "  parse_modelname(cmd);\n";
  if (!d.common().unnamed_value().is_empty()) {
    out << "  if (cmd.is_float()) {\n"
      "    cmd >> " << d.common().unnamed_value() << ";\n"
      "  }\n";
  }
  out << "  int here = cmd.cursor();\n"
    "  do{\n"
    "    ONE_OF\n";
  make_get_param_list(out, d.common().raw());
  out << "    ;\n"
    "  }while (cmd.more() && !cmd.stuck(&here));\n"
    "  cmd.check(bWARNING, \"what's this?\");\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_common_print(std::ofstream& out, const Device& d)
{
  out << "void COMMON_" << d.name() << "::print(OMSTREAM& o)const\n"
    "{\n"
    "  o << \"  \" << modelname();\n"
    "  o.setfloatwidth(7);\n";
  make_print_param_list(out, d.common().override());
  make_print_param_list(out, d.common().raw());
  out << "  o << '\\n';\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_common_expand(std::ofstream& out, const Device& d)
{
  out << "void COMMON_" << d.name() << "::elabo3(const COMPONENT* d)\n"
    "{\n"
    "  COMMON_" << d.name() << "* c = this;\n"
    "  const MODEL_" << d.model_type() << "* m = dynamic_cast<const MODEL_" 
      << d.model_type() << "*>(attach_model(d));\n"
    "  if (!m) {\n"
    "    error(bERROR, d->long_label() + \": model \" + modelname()\n"
    "          + \" is not a " << d.parse_name() << "\\n\");\n"
    "  }\n";

  out << "\n"
    "  assert(c);\n"
    "  const CARD_LIST* par_scope = d->scope();\n"
    "  assert(par_scope);\n";
  make_final_adjust(out, d.common());

  out << "\n"
    "  // size dependent\n"
    "  delete _sdp;\n"
    "  _sdp = m->new_sdp(this);\n"
    "  assert(_sdp);\n"
    "  const SDP_" << d.model_type() << "* s = dynamic_cast<const SDP_" 
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
    for (Arg_List::const_iterator a = (**p).begin(); a != (**p).end(); ++a) {
      out << "  " << (**p).name() << "->" << (**a).arg() << ";\n";
    }
    out << "  attach_common(" << (**p).name() << ", &_" << (**p).name()
	<< ");\n\n";
  }

  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_cc_common(std::ofstream& out, const Device& d)
{
  make_common_default_constructor(out, d);
  make_common_copy_constructor(out, d);
  make_common_destructor(out, d);
  make_common_operator_equal(out, d);
  make_common_parse(out, d);
  make_common_print(out, d);
  make_common_expand(out, d);
  out << "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
