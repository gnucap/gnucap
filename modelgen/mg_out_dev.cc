/*$Id: mg_out_dev.cc,v 22.9 2002/07/23 20:08:57 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include "mg_out.h"
/*--------------------------------------------------------------------------*/
static void make_dev_eval(std::ofstream& out, const Eval& e,
		const String_Arg& dev_type, const String_Arg& model_type)
{
  std::string class_name = "EVAL_" + dev_type.to_string() + '_'
    + e.name().to_string();
  out << "static " << class_name << " Eval_" << e.name().to_string() 
      << "(CC_STATIC);\n"
    "void " << class_name << "::tr_eval(ELEMENT* d)const\n"
    "{\n"
    "  assert(d);\n"
    "  DEV_" << dev_type << "* p = prechecked_cast<DEV_"
      << dev_type << "*>(d->owner());\n"
    "  assert(p);\n"
    "  const COMMON_" << dev_type << "* c = prechecked_cast<const COMMON_"
      << dev_type << "*>(p->common());\n"
    "  assert(c);\n"
    "  const SDP_" << model_type << "* s = prechecked_cast<const SDP_"
      << model_type << "*>(c->sdp());\n"
    "  assert(s);\n"
    "  const MODEL_" << model_type << "* m = prechecked_cast<const MODEL_"
      << model_type << "*>(c->model());\n"
    "  assert(m);\n"
      << e.code() <<
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_evals(std::ofstream& out, const Device& d)
{
  {for (Eval_List::const_iterator
	 e = d.eval_list().begin(); e != d.eval_list().end(); ++e) {
    make_dev_eval(out, **e, d.name(), d.model_type());
  }}
}
/*--------------------------------------------------------------------------*/
void make_dev_default_constructor(std::ofstream& out,const Device& d)
{
  out << "DEV_" << d.name() << "::DEV_" << d.name() << "()\n"
    "  :BASE_SUBCKT()";
  make_construct_parameter_list(out, d.device().raw());
  make_construct_parameter_list(out, d.device().calculated());
  {for (Element_List::const_iterator
	 p = d.circuit().elements().begin();
       p != d.circuit().elements().end(); ++p) {
    out << ",\n   _" << (**p).name() << "(0)";
  }}
  int local_nodes = d.circuit().local_nodes().size();
  out << "\n{\n"
    "  _n = _nodes + " << local_nodes << ";\n"
    "  attach_common(&Default_" << d.name() << ");\n"
    "  ++_count;\n";
  {for (Parameter_List::const_iterator
	 p = d.device().override().begin();
       p != d.device().override().end();
       ++p) {
    if (!((**p).final_default().empty())) {
      out << "  " << (**p).code_name() << " = NA;\n";
    }
    if (!((**p).default_val().empty())) {
      out << "  " << (**p).code_name() << " = " << (**p).default_val() <<";\n";
    }
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
void make_dev_copy_constructor(std::ofstream& out, const Device& d)
{
  out << "DEV_" << d.name() << "::DEV_" << d.name()
      << "(const DEV_" << d.name() << "& p)\n"
    "  :BASE_SUBCKT(p)";
  make_copy_construct_parameter_list(out, d.device().raw());
  make_copy_construct_parameter_list(out, d.device().calculated());
  {for (Element_List::const_iterator
	 p = d.circuit().elements().begin();
       p != d.circuit().elements().end(); ++p) {
    out << ",\n   _" << (**p).name() << "(0)";
  }}
  int local_nodes = d.circuit().local_nodes().size();
  int port_nodes  = d.circuit().ports().size();
  out << "\n{\n"
    "  _n = _nodes + " << local_nodes << ";\n"
    "  for (int ii = -" <<local_nodes <<"; ii < " <<port_nodes <<"; ++ii) {\n"
    "    _n[ii] = p._n[ii];\n"
    "  }\n"
    "  ++_count;\n";
  {for (Parameter_List::const_iterator
	 p = d.device().override().begin();
       p != d.device().override().end();
       ++p) {
    out << ",\n   " << (**p).code_name() << "(p." << (**p).code_name() << ")";
  }}
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_parse(std::ofstream& out, const Device& d)
{
  out << "void DEV_" << d.name() << "::parse(CS& cmd)\n"
    "{\n"
    "  assert(has_common());\n"
    "  COMMON_" << d.name() << "* c = prechecked_cast<COMMON_"
      << d.name() << "*>(common()->clone());\n"
    "  assert(c);\n"
    "\n"
    "  parse_Label(cmd);\n"
    "  parse_nodes(cmd, max_nodes(), min_nodes());\n"
    "  c->parse(cmd);\n"
    "  attach_common(c);\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_print(std::ofstream& out, const Device& d)
{
  out << "void DEV_" << d.name() << "::print(OMSTREAM& o, int)const\n"
    "{\n"
    "  const COMMON_" << d.name() << "* c = prechecked_cast<const COMMON_"
      << d.name() << "*>(common());\n"
    "  assert(c);\n"
    "\n"
    "  o << short_label();\n"
    "  printnodes(o);\n"
    "  c->print(o);\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_set_parameters(std::ofstream& out, const Element& e)
{
  out << "      _" << e.name() << "->set_parameters(\""
      << e.name() << "\", this, ";
  {if (e.eval() != "") {
    out << "&Eval_" << e.eval();
  }else if (e.args() != "") {
    out << "c->_" << e.args();
  }else{
    out << "NULL";
  }}
  out << ", " << ((e.value() != "") ? e.value() : "0.");
  {if (e.state() != "") {
    out << ", " << e.ports().size()/2+1 << ", &" << e.state();
  }else{
    out << ", 0, 0";
  }}
  out << ", " << e.ports().size() << ", nodes);\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_expand_one_element(std::ofstream& out, const Element& e)
{
  {if (!(e.omit().empty())) {
    out <<
      "  {if (" << e.omit() << ") {\n"
      "    if (_" << e.name() << ") {\n"
      "      subckt().erase(_" << e.name() << ");\n"
      "      _" << e.name() << " = NULL;\n"
      "    }\n"
      "  }else{\n";
  }else{
    out << "  {{\n";
  }}
  
  out <<
    "    if (!_" << e.name() << ") {\n"
    "      _" << e.name() << " = new " << e.class_name() << ";\n"
    "      subckt().push_front(_" << e.name() << ");\n"
    "    }\n";
  
  {if (!(e.reverse().empty())) {
    out << "    {if (" << e.reverse() << ") {\n";
    out << "      node_t nodes[] = {";
    
    Port_List::const_iterator p = e.ports().begin();
    if (p != e.ports().end()) {
      Port_List::const_iterator even = p;
      ++p;
      assert(p != e.ports().end());
      out<< "_n[n_" << (**p).name() << "], _n[n_" << (**even).name() << "]";
      bool even_node = true;
      while (++p != e.ports().end()) {
	{if (even_node) {
	  even_node = false;
	  even = p;
	}else{
	  even_node = true;
	  out<< ", _n[n_"<< (**p).name()<< "], _n[n_"<<(**even).name()<< "]";
	}}
      }
    }
    
    out << "};\n";
    make_set_parameters(out, e);
    out << "    }else{\n";
  }else{
    out << "    {{\n";
  }}
  
  out << "      node_t nodes[] = {";
  
  Port_List::const_iterator p = e.ports().begin();
  if (p != e.ports().end()) {
    assert(*p);
    out << "_n[n_" << (**p).name() << "]";
    while (++p != e.ports().end()) {
      out << ", _n[n_" << (**p).name() << "]";
    }
  }
  
  out << "};\n";
  make_set_parameters(out, e);
  out << "    }}\n";
  out << "  }}\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_allocate_local_nodes(std::ofstream& out, const Port& p)
{
  {if (p.short_if().empty()) {
    out << "  _n[n_" << p.name() << "].new_model_node();\n";
  }else{
    out <<
      "  {if (" << p.short_if() << ") {\n"
      "    _n[n_" << p.name() << "] = _n[n_" << p.short_to() << "];\n"
      "  }else{\n"
      "    _n[n_" << p.name() << "].new_model_node();\n"
      "  }}\n";
  }}
}
/*--------------------------------------------------------------------------*/
static void make_dev_expand(std::ofstream& out, const Device& d)
{
  out << "void DEV_" << d.name() << "::expand()\n"
    "{\n"
    "  COMMON_" << d.name() << "* c = prechecked_cast<COMMON_"
      << d.name() << "*>(mutable_common());\n"
    "  assert(c);\n"
    "  c->expand(this);\n"
    "  const MODEL_" << d.model_type() << "* m = prechecked_cast<const MODEL_"
      << d.model_type() << "*>(c->model());\n"
    "  assert(m);\n"
    "  const SDP_" << d.model_type() << "* s = prechecked_cast<const SDP_"
      << d.model_type() << "*>(c->sdp());\n"
    "  assert(s);\n"
    "\n";
  {for (Port_List::const_iterator
	 p = d.circuit().local_nodes().begin();
       p != d.circuit().local_nodes().end(); ++p) {
    make_dev_allocate_local_nodes(out, **p);
  }}
  out << "\n";
  {for (Element_List::const_iterator
	 e = d.circuit().elements().begin();
       e != d.circuit().elements().end(); ++e) {
    make_dev_expand_one_element(out, **e);
  }}
  out <<
    "  assert(subckt().exists());\n"
    "  subckt().expand();\n"
    "  assert(!constant());\n";
  if (d.circuit().sync()) {
    out << "  subckt().set_slave();\n";
  }
  out << "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static std::string fix_expression(const std::string& in)
{
  //std::string out;
  char out[BIGBUFLEN];
  out[0] = '\0';
  
  CS x(in);
  {for (;;) {
    {if (x.peek() == '@') {
      x.skip1('@');
      //std::string object(x.ctos("[,"));
      char object[BIGBUFLEN];
      strcpy(object, x.ctos("[,").c_str());
      x.skip1('[');
      //std::string attrib(x.ctos("]"));
      char attrib[BIGBUFLEN];
      strcpy(attrib, x.ctos("]").c_str());
      x.skip1(']');
      {if (object[0] == 'n') {
	//out += " _n[" + object + "]";
	strcat(out, " _n[");
	strcat(out, object);
	strcat(out, "]");
	//if (attrib != "") {
	if (*attrib) {
	  //out += ".v0()";
	  strcat(out, ".v0()");
	}
      }else{
	//out += " CARD::probe(_" + object + ",\"" + attrib + "\")";
	strcat(out, " CARD::probe(_");
	strcat(out, object);
	strcat(out, ",\"");
	strcat(out, attrib);
	strcat(out, "\")");
      }}
    }else if (x.more()) {
      //out += ' ' + x.ctos("@");
      strcat(out, " ");
      strcat(out, x.ctos("@").c_str());
    }else{
      break;
    }}
  }}

  return out;
}

// BUG:: sometimes craches here with g++-3.0.1 (but not any other
// version) when using string class.  I don't know why.  Perhaps bug
// in 3.0.1 string class. (Mandrake 8.1)
// Use old style C functions instead.
/*--------------------------------------------------------------------------*/
static void make_dev_probe(std::ofstream& out, const Device& d)
{
  out << "double DEV_" << d.name() << "::tr_probe_num(CS& cmd)const\n"
    "{\n"
    "  const COMMON_" << d.name() << "* c = prechecked_cast<const COMMON_"
      << d.name() << "*>(common());\n"
    "  assert(c);\n"
    "  const MODEL_" << d.model_type() << "* m = prechecked_cast<const MODEL_"
      << d.model_type() << "*>(c->model());\n"
    "  assert(m);\n"
    "  const SDP_" << d.model_type() << "* s = prechecked_cast<const SDP_"
      << d.model_type() << "*>(c->sdp());\n"
    "  assert(s);\n"
    "\n"
    "  {";
  {for (Probe_List::const_iterator
	  p = d.probes().begin(); p != d.probes().end(); ++p) {
    assert(*p);
    out << "if (cmd.pmatch(\"" << (**p).name() << "\")) {\n"
      "    return " << fix_expression((**p).expression()) << ";\n"
      "  }else ";
  }}
  out << "{\n"
    "    return BASE_SUBCKT::tr_probe_num(cmd);\n"
    "  }}\n"
    "}\n"
    "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
static void make_dev_aux(std::ofstream& out, const Device& d)
{
  {for (Function_List::const_iterator
	  p = d.function_list().begin();
	p != d.function_list().end(); ++p) {
    out << "void DEV_" << d.name() << "::" << (**p).name() << "\n"
      "{\n"
	<< (**p).code() <<
      "}\n"
      "/*--------------------------------------"
      "------------------------------------*/\n";
  }}
}
/*--------------------------------------------------------------------------*/
void make_cc_dev(std::ofstream& out, const Device& d)
{
  make_dev_evals(out, d);
  make_dev_default_constructor(out, d);
  make_dev_copy_constructor(out, d);
  make_dev_parse(out, d);
  make_dev_print(out, d);
  make_dev_expand(out, d);
  make_dev_probe(out, d);
  make_dev_aux(out, d);
  out << "/*--------------------------------------"
    "------------------------------------*/\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
