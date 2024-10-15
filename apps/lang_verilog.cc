/*$Id: lang_verilog.cc $ -*- C++ -*-
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
//testing=script 2023.10.25
#include "u_nodemap.h"
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "e_subckt.h"
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
  ~LANG_VERILOG() {}
  std::string name()const override {return "verilog";}
  bool case_insensitive()const override {return false;}
  UNITS units()const override {return uSI;}
  bool is_verilog()const override {return true;}

public: // override virtual, used by callback
  std::string arg_front()const override {
    switch (_mode) {
    case mPARAMSET:untested(); return " .";			    break;
    case mDEFAULT:  return (arg_count++ > 0) ? ", ." : "."; break;
    }
    unreachable();
    return "";
  }
  std::string arg_mid()const override {
    switch (_mode) {
    case mPARAMSET:untested(); return "="; break;
    case mDEFAULT:  return "("; break;
    }
    unreachable();
    return "";
  }
  std::string arg_back()const override {
    switch (_mode) {
    case mPARAMSET:untested(); return ";"; break;
    case mDEFAULT:  return ")"; break;
    }
    unreachable();
    return "";
  }

public: // override virtual, called by commands
  void		parse_top_item(CS&, CARD_LIST*)override;
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*)override;
  DEV_DOT*	parse_command(CS&, DEV_DOT*)override;
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*)override;
  BASE_SUBCKT*  parse_module(CS&, BASE_SUBCKT*)override;
  COMPONENT*	parse_instance(CS&, COMPONENT*)override;
  std::string	find_type_in_string(CS&)override;
private: // local
  void skip_attributes(CS& cmd);
  std::string  parse_attributes(CS& cmd);
  void store_attributes(std::string attrib_string, tag_t x);
  void parse_attributes(CS& cmd, tag_t x);
  void parse_type(CS& cmd, CARD* x);
  void parse_args_paramset(CS& cmd, MODEL_CARD* x);
  void parse_args_instance(CS& cmd, CARD* x); 
  void parse_label(CS& cmd, CARD* x);
  void parse_ports(CS& cmd, COMPONENT* x, bool all_new);

private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*)override;
  void print_module(OMSTREAM&, const BASE_SUBCKT*)override;
  void print_instance(OMSTREAM&, const COMPONENT*)override;
  void print_comment(OMSTREAM&, const DEV_COMMENT*)override;
  void print_command(OMSTREAM& o, const DEV_DOT*)override;
private: // local
  void print_attributes(OMSTREAM&, tag_t);
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_args(OMSTREAM&, const COMPONENT*);
  void print_type(OMSTREAM& o, const COMPONENT* x);
  void print_label(OMSTREAM& o, const COMPONENT* x);
  void print_ports_long(OMSTREAM& o, const COMPONENT* x);
  void print_ports_short(OMSTREAM& o, const COMPONENT* x);
} lang_verilog;

DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_verilog.name(), &lang_verilog);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::skip_attributes(CS& cmd)
{
  while (cmd >> "(*") {
    cmd.skipto1('*') && (cmd >> "*)");
  }
}
/*--------------------------------------------------------------------------*/
std::string LANG_VERILOG::parse_attributes(CS& cmd)
{
  std::string attrib_string = "";
  std::string comma = "";
  while (cmd >> "(*") {
    attrib_string += comma;
    while(cmd.ns_more() && !(cmd >> "*)")) {
      attrib_string += cmd.ctoc();
    }
    comma = ", ";
  }
  return attrib_string;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::store_attributes(std::string attrib_string, tag_t x)
{
  assert(x);
  if(attrib_string!=""){
    set_attributes(x).add_to(attrib_string, x);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_attributes(CS& cmd, tag_t x)
{
  assert(x);
  store_attributes(parse_attributes(cmd), x);
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_type(CS& cmd, CARD* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_args_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);

  while (cmd >> '.') {
    size_t here = cmd.cursor();
    std::string Name, value;
    try{
      cmd >> Name >> '=' >> value >> ';';
      x->set_param_by_name(Name, value);
    }catch (Exception_No_Match&) {untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + Name + " ignored");
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_args_instance(CS& cmd, CARD* x)
{
  assert(x);

  if (cmd >> "#(") {
    std::string attribs = parse_attributes(cmd);
    size_t here = cmd.cursor();
    
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
	std::string Name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{
	  int Index = x->set_param_by_name(Name, value);
	  trace3("pai", Index, Name, value);
	  store_attributes(attribs,  x->param_id_tag(Index));
	}catch (Exception_No_Match&) {
	  cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + Name + " ignored");
	}catch (Exception_Clash&) {
	  cmd.warn(bDANGER, here, x->long_label() + ": already set " + Name + ", ignored");
	}
	attribs = parse_attributes(cmd);
	here = cmd.cursor();
      }
    }else{
      // by order
      for (int Index = x->param_count() - 1;  cmd.is_alnum() || cmd.match1("+-.");  --Index) {
	try{
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_param_by_index(Index, value, 0/*offset*/);
	  store_attributes(attribs,  x->param_id_tag(Index));
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}catch (Exception_Clash&) {untested();
	  unreachable();
	  cmd.warn(bDANGER, here, x->long_label() + ": already set, ignored");
	}
	attribs = parse_attributes(cmd);
	here = cmd.cursor();
      }
    }
    cmd >> ')';
  }else{
    // no args
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_label(CS& cmd, CARD* x)
{
  assert(x);
  std::string my_name;
  if (cmd >> my_name) {
    x->set_label(my_name);
  }else{
    x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
    cmd.warn(bDANGER, "label required");
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_ports(CS& cmd, COMPONENT* x, bool all_new)
{
  assert(x);

  if (cmd >> '(') {
    std::string attribs = parse_attributes(cmd);
    size_t here = cmd.cursor();
    
    if (cmd.match1('.')) {
      // by name
      while (cmd >> '.') {
	std::string Name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{
	  int Index = x->set_port_by_name(Name, value);
	  store_attributes(attribs,  x->port_id_tag(Index));
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": mismatch " + Name + " ignored");
	}catch (Exception_Clash&) {untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": already set " + Name + ", ignored");
	}
	attribs = parse_attributes(cmd);
	here = cmd.cursor();
      }
      for (int Index = 0;  Index < x->min_nodes();  ++Index) {
	//BUG// This may be a bad idea.
	//BUG// It's definitely wrong with all_new.
	//BUG// What should we do with unconnected ports?
	if (!(x->node_is_connected(Index))) {untested();
	  cmd.warn(bDANGER, x->port_name(Index) + ": port unconnected, grounding");
	  x->set_port_to_ground(Index);
	}else{
	}
      }
    }else{
      // by order
      int Index;
      for (Index = 0;  cmd.is_alnum();  ++Index) {
	try{
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_port_by_index(Index, value);
	  store_attributes(attribs,  x->port_id_tag(Index));
	  if (all_new) {
	    if (x->node_is_grounded(Index)) {
	      cmd.warn(bDANGER, here, "node 0 not allowed here");
	      --Index;
	    }else if (x->subckt() && x->subckt()->nodes()->how_many() != Index+1) {
	      cmd.warn(bDANGER, here, "duplicate port name, skipping");
	      --Index;
	    }else{
	    }
	  }else{
	  }
	}catch (Exception_Too_Many& e) {
	  cmd.warn(bDANGER, here, e.message());
	}catch (Exception_Clash&) {untested();
	  unreachable();
	  cmd.warn(bDANGER, here, x->long_label() + ": already set, ignored");
	}
	attribs = parse_attributes(cmd);
	here = cmd.cursor();
      }
      if (Index < x->min_nodes()) {
	//BUG// This may be a bad idea.
	//BUG// It's definitely wrong with all_new.
	//BUG// What should we do with unconnected ports?
	if (all_new) {untested();
	}else{
	}
	cmd.warn(bDANGER, "need " + to_string(x->min_nodes()-Index) +" more nodes, grounding");
	for (  ;  Index < x->min_nodes();  ++Index) {
	  x->set_port_to_ground(Index);
	}
      }else{
      }
    }
    cmd >> ')';
  }else{
    cmd.warn(bDANGER, "'(' required (parse ports) (grounding)");
    for (int Index = 0;  Index < x->min_nodes();  ++Index) {
      if (!(x->node_is_connected(Index))) {
	if (all_new) {untested();
	}else{
	}
	cmd.warn(bDANGER, x->port_name(Index) + ": port unconnected, grounding");
	x->set_port_to_ground(Index);
      }else{untested();
	unreachable();
      }
    }
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
  parse_attributes(cmd, x->id_tag());
  CMD::cmdproc(cmd, scope);
  x->purge();
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

MODEL_CARD* LANG_VERILOG::parse_paramset(CS& cmd, MODEL_CARD* x)
{
  assert(x);
  cmd.reset();
  parse_attributes(cmd, x->id_tag());
  cmd >> "paramset ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  cmd >> ';';

  for (;;) {
    parse_args_paramset(cmd, x);
    if (cmd >> "endparamset ") {
      break;
    }else if (!cmd.more()) {
      cmd.get_line("verilog-paramset>");
    }else{untested();
      cmd.check(bWARNING, "what's this?");
      break;
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
class CMD_MODULE_PARAM : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    PARAM_LIST* pl = Scope->params();
    if (cmd.is_end()) { untested();
      pl->print(IO::mstdout, OPT::language);
      IO::mstdout << '\n';
    }else{
      parse(cmd, pl);
    }
  }
private:
  void parse(CS& cmd, PARAM_LIST* pl);
} module_param;
/*--------------------------------------------------------------------------*/
class PARAM_ANY : public PARA_BASE {
  Base* _value{nullptr};
public:
  explicit PARAM_ANY() : PARA_BASE () {}
  explicit PARAM_ANY(PARAM_ANY const&p) : PARA_BASE (p) {
    if(p._value){
      _value = p._value->assign(p._value);
    }else{
    }
  }
  ~PARAM_ANY() {delete _value;}
  PARA_BASE* clone()const override{ untested();return new PARAM_ANY(*this);}
  PARA_BASE* pclone(void*p)const override{return new(p) PARAM_ANY(*this);}
  bool operator==(const PARA_BASE& v)const { untested();
    // PARAMETER const* p = dynamic_cast<PARAMETER const*>(&b);
    // return (p && _v == p->_v  &&  _s == p->_s);
    Base* eq = nullptr;
   // if(_s != v._s){ untested();
   //   return false;
   // }else
    if(auto f = dynamic_cast<Float const*>(v.value())){ untested();
      eq = f->equal(_value);
    }else if(auto i = dynamic_cast<Integer const*>(v.value())){ untested();
      eq = i->equal(_value);
    }else{ untested();
      // incomplete();
    }

    bool ret = false;
    if(auto ii=dynamic_cast<Integer const*>(eq)){ untested();
      ret = ii->value();
    }else{ untested();
    }
    delete eq;

    //if (_value) { untested();
    //  Integer* eq = _value->equal(v.value());
    //  if(eq){ untested();
    //    bool eq = v->_value;
    //    delete eq;
    //    return eq;
    //  }else{ untested();
    //  }
    //}else{ untested();
    //  return !v._value || !has_hard_value();
    //}
    if(ret){ untested();
    }else{ untested();
    }
    return ret;
  }

  void parse(CS& cmd) override {
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {
      if (cmd.match1('(')) { untested();
	_s = name + '(' + cmd.ctos("", "(", ")") + ')';
      }else{
	_s = name;
      }
    }else{
    }
  }
  PARA_BASE& operator=(const std::string&) override{ untested();unreachable(); return *this;}
  PARA_BASE& operator=(Base const* v)override {
    delete _value;
    if(dynamic_cast<Float const*>(v)){
      // assert(dynamic_cast<vReal const*>(v));
      vReal n;
      _value = n.assign(v);
      trace3("now real", _s, v->val_string(), _value->val_string());
    }else if(dynamic_cast<Integer const*>(v)){ untested();
      vInteger n;
      _value = n.assign(v);
      trace3("now integer", _s, v->val_string(), _value->val_string());
    }else{ untested();
      _value = v->assign(v);
      assert(_value);
      trace3("don't know", _s, v->val_string(), _value->val_string());
    }
    _s = "#";
    return *this;
  }
  std::string string()const override {
    if (_s == "#") {
      if (_value) {
	return _value->val_string();
      }else{ untested();
	return "";
      }
    }else if (_s == "") { untested();
      return "NA(" + _value->val_string() + ")";
    }else{
      return _s;
    }
  }

  Base const* value()const override {
    if(_value){
    }else{
    }
    return _value;
  }
  bool has_good_value()const override { untested();unreachable(); return false;}
  Base const* e_val_(const Base*, const CARD_LIST*, int)const {unreachable(); return nullptr;}
}param_any;
/*--------------------------------------------------------------------------*/
void CMD_MODULE_PARAM::parse(CS& cmd, PARAM_LIST* pl)
{
  assert(pl);
  int type = 0;
  if(cmd >> "real"){
    type = 1;
  }else if(cmd >> "integer"){
    type = 2;
// TODO: realtime | time | string
  }else{
  }
  size_t here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }else{
    }
    std::string Name;
    PARAM_INSTANCE par;
    switch(type){
    case 1:
      par = PARAMETER<vReal>();
      break;
    case 2:
      par = PARAMETER<vInteger>();
      break;
    default:
      par = param_any;
      // par = PARAMETER<vReal>();
      break;
    }

    cmd >> Name >> '=' >> par;

    trace1("parsed", par.string());
    if (cmd.stuck(&here)) {untested();
      break;
    }else{
    }
    if (OPT::case_insensitive) {
      notstd::to_lower(&Name);
    }else{
    }
    pl->set(Name, par);
  }
  cmd.check(bDANGER, "syntax error");
}
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

BASE_SUBCKT* LANG_VERILOG::parse_module(CS& cmd, BASE_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());
  x->subckt()->set_verilog_math();

  // header
  cmd.reset();
  parse_attributes(cmd, x->id_tag());
  (cmd >> "module |macromodule ");
  parse_label(cmd, x);
  parse_ports(cmd, x, true/*all new*/);
  cmd >> ';';

  // body
  for (;;) {
    cmd.get_line("verilog-module>");

    if (cmd >> "endmodule ") {
      break;
    }else if (cmd >> "parameter ") {
      trace1("parameter", cmd.tail());
      module_param.do_it(cmd, x->subckt());
      trace1("/parameter", cmd.tail());
    }else{
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_VERILOG::parse_instance(CS& cmd, COMPONENT* x)
{
  assert(x);
  cmd.reset();
  parse_attributes(cmd, x->id_tag());
  parse_type(cmd, x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x, false/*allow dups*/);
  cmd >> ';';
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_VERILOG::find_type_in_string(CS& cmd)
{
  skip_attributes(cmd);
  size_t here = cmd.cursor();
  std::string type;
  if ((cmd >> "//")) {
    //assert(here == 0);
    type = "dev_comment";
  }else{
    cmd >> type;
  }
  cmd.reset(here); // where the type is.
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
void LANG_VERILOG::print_attributes(OMSTREAM& o, tag_t x)
{
  assert(x);
  if (attributes(x)) {
    o << "(* " << attributes(x)->string(tag_t(0)) << " *) ";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_args(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  if (x->use_obsolete_callback_print()) {untested();
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " ." + x->param_name(ii) + '=' + x->param_value(ii) + ';';
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
    std::string sep = "";
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	o << sep;
	print_attributes(o, x->param_id_tag(ii));
	o << '.' << x->param_name(ii) << '(' << x->param_value(ii) << ')';
	sep = ',';
      }else{
      }
    }
  }
  o << ") ";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->dev_type();
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_ports_long(OMSTREAM& o, const COMPONENT* x)
{
  // print in long form ...    .name(value)
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep;
    print_attributes(o, x->port_id_tag(ii));
    o << '.' << x->port_name(ii) << '(' << x->port_value(ii) << ')';
    sep = ',';
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
    o << sep;
    //////print_attributes(o, x->port_id_tag(ii));
    o << '.' << x->current_port_name(ii) << '(' << x->current_port_value(ii) << ')';
    sep = ',';
  }
  o << ')';
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_ports_short(OMSTREAM& o, const COMPONENT* x)
{
  // print in short form ...   value only
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep;
    print_attributes(o, x->port_id_tag(ii));
    o << x->port_value(ii);
    sep = ',';
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
    o << sep;
    //////print_attributes(o, x->port_id_tag(ii));
    o << x->current_port_value(ii);
    sep = ',';
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  _mode = mPARAMSET;
  print_attributes(o, x->id_tag());
  o << "paramset " << x->short_label() << ' ' << x->dev_type() << ";\n";
  print_args(o, x);
  o << "\n"
    "endparamset\n\n";
  _mode = mDEFAULT;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_module(OMSTREAM& o, const BASE_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  print_attributes(o, x->id_tag());
  o << "module " <<  x->short_label();
  print_ports_short(o, x);
  o << ";\n";
  
  for (CARD_LIST::const_iterator ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << "endmodule // " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_attributes(o, x->id_tag());
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
  if ((x->comment().compare(0, 2, "//")) != 0) {
    o << "//";
  }else{
  }
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_command(OMSTREAM& o, const DEV_DOT* x)
{untested();
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CMD_PARAMSET : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    // already got "paramset"
    std::string my_name, base_name;
    cmd >> my_name;
    size_t here = cmd.cursor();
    cmd >> base_name;

    //const MODEL_CARD* p = model_dispatcher[base_name];
    const CARD* p = lang_verilog.find_proto(base_name, NULL);
    if (p) {
      CARD* cl = p->clone();
      MODEL_CARD* new_card = dynamic_cast<MODEL_CARD*>(cl);
      if (new_card) {
	assert(!new_card->owner());
	lang_verilog.parse_paramset(cmd, new_card);
	Scope->push_back(new_card);
      }else{untested();
	delete(cl);
	cmd.warn(bDANGER, here, "paramset: base has incorrect type");
      }
    }else{untested();
      cmd.warn(bDANGER, here, "paramset: no match");
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "paramset", &p1);
/*--------------------------------------------------------------------------*/
class CMD_MODULE : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    BASE_SUBCKT* new_module = dynamic_cast<BASE_SUBCKT*>(device_dispatcher.clone("module"));
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    assert(!new_module->is_device());
    lang_verilog.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "module|macromodule", &p2);
/*--------------------------------------------------------------------------*/
class CMD_VERILOG : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)override {
    if(Scope == &CARD_LIST::card_list) {
    }else{ untested();
    }
    command("options lang=verilog", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "verilog", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
