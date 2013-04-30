/*$Id: lang_spectre.cc,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
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
class LANG_SPECTRE : public LANGUAGE {
public:
  std::string name()const {return "spectre";}
  bool case_insensitive()const {return false;}
  UNITS units()const {return uSI;}

public: // override virtual, used by callback
  std::string arg_front()const {return " ";}
  std::string arg_mid()const {return "=";}
  std::string arg_back()const {return "";}

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
  void print_args(OMSTREAM&, const CARD*);
} lang_spectre;

DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_spectre.name(), &lang_spectre);
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
static void parse_args(CS& cmd, CARD* x)
{
  assert(x);
  
  unsigned here = 0;
  while (cmd.more() && !cmd.stuck(&here)) {
    std::string name  = cmd.ctos("=", "", "");
    cmd >> '=';
    std::string value = cmd.ctos("", "(", ")");
    try{
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
    }
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
    cmd >> ')';
  }else{
    unsigned here = cmd.cursor();
    OPT::language->find_type_in_string(cmd);
    unsigned stop = cmd.cursor();
    cmd.reset(here);

    int index = 0;
    while (cmd.cursor() < stop) {
      here = cmd.cursor();
      try{
	std::string value;
	cmd >> value;
	x->set_port_by_index(index++, value);
      }catch (Exception_Too_Many& e) {untested();
	cmd.warn(bDANGER, here, e.message());
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_SPECTRE::parse_comment(CS& cmd, DEV_COMMENT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_SPECTRE::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset().skipbl();
  if ((cmd >> "model |simulator |parameters |subckt ")) {
    cmd.reset();
    CMD::cmdproc(cmd, scope);
  }else{
    std::string label;
    cmd >> label;
    
    if (label != "-") {
      unsigned here = cmd.cursor();
      std::string command;
      cmd >> command;
      cmd.reset(here);
      std::string file_name = label + '.' + command;
      std::string s = cmd.tail() + " > " + file_name;
      CS augmented_cmd(CS::_STRING, s);
      CMD::cmdproc(augmented_cmd, scope);
    }else{
      CMD::cmdproc(cmd, scope);
    }
  }
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
MODEL_CARD* LANG_SPECTRE::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset().skipbl();
  cmd >> "model ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
MODEL_SUBCKT* LANG_SPECTRE::parse_module(CS& cmd, MODEL_SUBCKT* x)
{
  assert(x);

  // header
  cmd.reset().skipbl();
  cmd >> "subckt ";
  parse_label(cmd, x);
  parse_ports(cmd, x);

  // body
  for (;;) {
    cmd.get_line("spectre-subckt>");

    if (cmd >> "ends ") {
      break;
    }else{
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_SPECTRE::parse_instance(CS& cmd, COMPONENT* x)
{
  cmd.reset();
  parse_label(cmd, x);
  parse_ports(cmd, x);
  parse_type(cmd, x);
  parse_args(cmd, x);
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_SPECTRE::find_type_in_string(CS& cmd)
{
  // known to be not always correct

  cmd.reset().skipbl();
  unsigned here = 0;
  std::string type;
  if ((cmd >> "*|//")) {itested();
    assert(here == 0);
    type = "dev_comment";
  }else if ((cmd >> "model |simulator |parameters |subckt ")) {
    // type is first, it's a control statement
    type = cmd.trimmed_last_match();
  }else if (cmd.reset().skiparg().match1("(") && cmd.scan(")")) {
    // node list surrounded by parens
    // type follows
    here = cmd.cursor();
    cmd.reset(here);
    cmd >> type;
  }else if (cmd.reset().scan("=")) {itested();
    // back up two, by starting over
    cmd.reset().skiparg();
    unsigned here1 = cmd.cursor();
    cmd.skiparg();
    unsigned here2 = cmd.cursor();
    cmd.skiparg();
    unsigned here3 = cmd.cursor();
    while (here2 != here3 && !cmd.match1('=')) {
      cmd.skiparg();
      here1 = here2;
      here2 = here3;
      here3 = cmd.cursor();
    }
    here = here1;
    cmd.reset(here);
    cmd >> type;
  }else{
    // type is second
    cmd.reset().skiparg();
    here = cmd.cursor();
    cmd.reset(here);
    cmd >> type;
  }
  cmd.reset(here);
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-spectre>");
  new__instance(cmd, NULL, Scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_args(OMSTREAM& o, const CARD* x)
{
  assert(x);
  o << ' ';
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " " + x->param_name(ii) + "=" + x->param_value(ii);
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
static void print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << ' ' << x->dev_type();
}
/*--------------------------------------------------------------------------*/
static void print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
static void print_ports(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = " ";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = " ";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  o << "model " << x->short_label() << ' ' << x->dev_type() << ' ';
  print_args(o, x);
  o << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << "subckt " <<  x->short_label();
  print_ports(o, x);
  o << "\n";

  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << "ends " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_label(o, x);
  print_ports(o, x);
  print_type(o, x);
  print_args(o, x);
  o << "\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CMD_MODEL : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    // already got "model"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here = cmd.cursor();    
    cmd >> base_name;

    //const MODEL_CARD* p = model_dispatcher[base_name];
    const CARD* p = lang_spectre.find_proto(base_name, NULL);
    if (p) {
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(p->clone());
      if (exists(new_card)) {
	assert(!new_card->owner());
	lang_spectre.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{	//BUG// memory leak
	cmd.warn(bDANGER, here, "model: base has incorrect type");
      }
    }else{
      cmd.warn(bDANGER, here, "model: no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "model", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SUBCKT : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    MODEL_SUBCKT* new_module = new MODEL_SUBCKT;
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    lang_spectre.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "subckt", &p2);
/*--------------------------------------------------------------------------*/
class CMD_SIMULATOR : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    command("options " + cmd.tail(), Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "simulator", &p3);
/*--------------------------------------------------------------------------*/
class CMD_SPECTRE : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=spectre", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "spectre", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
