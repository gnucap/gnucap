/*$Id: lang_verilog.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_VERILOG : public LANGUAGE {
  enum MODE {mDEFAULT, mPARAMSET} _mode;
  mutable int arg_count;
  enum {INACTIVE = -1};
public:
  LANG_VERILOG() : arg_count(INACTIVE) {}
  std::string name()const {return "verilog";}
  bool case_insensitive()const {return false;}
  UNITS units()const {return uSI;}

public: // override virtual, used by callback
  std::string arg_front()const {
    switch (_mode) {
    case mPARAMSET: return " .";			    break;
    case mDEFAULT:  return (arg_count++ > 0) ? ", ." : "."; break;
    }
    unreachable();
    return "";
  }
  std::string arg_mid()const {
    switch (_mode) {
    case mPARAMSET: return "="; break;
    case mDEFAULT:  return "("; break;
    }
    unreachable();
    return "";
  }
  std::string arg_back()const {
    switch (_mode) {
    case mPARAMSET: return ";"; break;
    case mDEFAULT:  return ")"; break;
    }
    unreachable();
    return "";
  }

public: // override virtual, called by commands
  void		parse_top_item(CS&, CARD_LIST*);
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*);
  DEV_DOT*	parse_command(CS&, DEV_DOT*);
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*);
  MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*);
  COMPONENT*	parse_instance(CS&, COMPONENT*);
  std::string	find_type_in_string(CS&);

private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*);
  void print_module(OMSTREAM&, const MODEL_SUBCKT*);
  void print_instance(OMSTREAM&, const COMPONENT*);
  void print_comment(OMSTREAM&, const DEV_COMMENT*);
  void print_command(OMSTREAM& o, const DEV_DOT* c);
private: // local
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_args(OMSTREAM&, const COMPONENT*);
} lang_verilog;

DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_verilog.name(), &lang_verilog);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
static void parse_args_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);

  while (cmd >> '.') {
    unsigned here = cmd.cursor();
    std::string name, value;
    try{
      cmd >> name >> '=' >> value >> ';';
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
    }
  }
}
/*--------------------------------------------------------------------------*/
static void parse_args_instance(CS& cmd, CARD* x)
{
  assert(x);

  if (cmd >> "#(") {
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	std::string name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{
	  x->set_param_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
	}
      }
    }else{
      // by order
      int index = 1;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_param_by_index(x->param_count() - index++, value, 0/*offset*/);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }
    cmd >> ')';
  }else{
    // no args
  }
}
/*--------------------------------------------------------------------------*/
static void parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  cmd >> my_name;
  x->set_label(my_name);
}
/*--------------------------------------------------------------------------*/
static void parse_ports(CS& cmd, COMPONENT* x)
{
  assert(x);

  if (cmd >> '(') {
    if (cmd.is_alnum()) {
      // by order
      int index = 0;
      while (cmd.is_alnum()) {
	unsigned here = cmd.cursor();
	try{
	  std::string value;
	  cmd >> value;
	  x->set_port_by_index(index++, value);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }else{
      // by name
      while (cmd >> '.') {
	unsigned here = cmd.cursor();
	try{
	  std::string name, value;
	  cmd >> name >> '(' >> value >> ')' >> ',';
	  x->set_port_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, "mismatch, ignored");
	}
      }
    }
    cmd >> ')';
  }else{untested();
    cmd.warn(bDANGER, "'(' required");
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_VERILOG::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_VERILOG::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  CMD::cmdproc(cmd, scope);
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
/* "paramset" <my_name> <base_name> ";"
 *    <paramset_item_declaration>*
 *    <paramset_statement>*
 *  "endparamset"
 */
//BUG// no paramset_item_declaration, falls back to spice mode
//BUG// must be on single line

MODEL_CARD* LANG_VERILOG::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  cmd >> "paramset ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  cmd >> ';';
  parse_args_paramset(cmd, x);
  cmd >> "endparamset ";
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

MODEL_SUBCKT* LANG_VERILOG::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset();
  (cmd >> "module |macromodule ");
  parse_label(cmd, x);
  parse_ports(cmd, x);
  cmd >> ';';

  // body
  for (;;) {
    cmd.get_line("verilog-module>");

    if (cmd >> "endmodule ") {
      break;
    }else{
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_VERILOG::parse_instance(CS& cmd, COMPONENT* x)
{
  cmd.reset();
  parse_type(cmd, x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x);
  cmd >> ';';
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_VERILOG::find_type_in_string(CS& cmd)
{
  unsigned here = cmd.cursor();
  std::string type;
  if ((cmd >> "//")) {
    assert(here == 0);
    type = "dev_comment";
  }else{
    cmd >> type;
  }
  cmd.reset(here);
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-verilog>");
  new__instance(cmd, NULL, Scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_args(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " ." + x->param_name(ii) + "=" + x->param_value(ii) + ";";
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_args(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << " #(";
  if (x->use_obsolete_callback_print()) {
    arg_count = 0;
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
    arg_count = INACTIVE;
  }else{
    std::string sep = ".";
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	o << sep << x->param_name(ii) << "(" << x->param_value(ii) << ")";
	sep = ",.";
      }else{
      }
    }
  }
  o << ") ";
}
/*--------------------------------------------------------------------------*/
static void print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->dev_type();
}
/*--------------------------------------------------------------------------*/
static void print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
static void print_ports_long(OMSTREAM& o, const COMPONENT* x)
{
  // print in long form ...    .name(value)
  assert(x);

  o << " (";
  std::string sep = ".";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_name(ii) << '(' << x->port_value(ii) << ')';
    sep = ",.";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_name(ii) << '(' << x->current_port_value(ii) << ')';
    sep = ",.";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
static void print_ports_short(OMSTREAM& o, const COMPONENT* x)
{
  // print in short form ...   value only
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = ",";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = ",";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  _mode = mPARAMSET;
  o << "paramset " << x->short_label() << ' ' << x->dev_type() << ";\\\n";
  print_args(o, x);
  o << "\\\n"
    "endparmset\n\n";
  _mode = mDEFAULT;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << "module " <<  x->short_label();
  print_ports_short(o, x);
  o << ";\n";
  
  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << "endmodule // " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_type(o, x);
  print_args(o, x);
  print_label(o, x);
  print_ports_long(o, x);
  o << ";\n";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CMD_PARAMSET : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    // already got "paramset"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here = cmd.cursor();    
    cmd >> base_name;

    //const MODEL_CARD* p = model_dispatcher[base_name];
    const CARD* p = lang_verilog.find_proto(base_name, NULL);
    if (p) {
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(p->clone());
      if (exists(new_card)) {
	assert(!new_card->owner());
	lang_verilog.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{	//BUG// memory leak
	cmd.warn(bDANGER, here, "paramset: base has incorrect type");
      }
    }else{
      cmd.warn(bDANGER, here, "paramset: no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "paramset", &p1);
/*--------------------------------------------------------------------------*/
class CMD_MODULE : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    MODEL_SUBCKT* new_module = new MODEL_SUBCKT;
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    lang_verilog.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "module|macromodule", &p2);
/*--------------------------------------------------------------------------*/
class CMD_VERILOG : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=verilog", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "verilog", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
