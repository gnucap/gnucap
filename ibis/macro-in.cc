/*$Id: macro-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 *------------------------------------------------------------------
 */
#include "macro.h"
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol_Type type, const std::string& name)
  :_type(type),
   _name(name)
{
}
/*--------------------------------------------------------------------------*/
void Symbol_Table::check_add_declare(CS& file, int here,
	Symbol_Type type, const std::string& name)
{
  const Symbol* old = _map[name];
  {if (!old) {
    Symbol* x = new Symbol(type, name);
    _map[name] = x;
    // correctly defining new symbol
    //file.warn(0, here, "adding: " + name + " as type " + c_str(type));
  }else if (old->type() != type) {
    untested();
    file.warn(0, here, "redeclaration:" + name + "  with type mismatch\n"
	      "old type = " + c_str(old->type()) + ", new type = " + c_str(type));
  }else{
    untested();
    file.warn(0,here, "redeclaration:" + name + "  same type = " + c_str(type));
  }}
}
/*--------------------------------------------------------------------------*/
void Symbol_Table::check_add_use(CS& file, int here,
	Symbol_Type type, const std::string& name)
{
  const Symbol* old = _map[name];
  {if (!old) {
    untested();
    Symbol* x = new Symbol(type, name);
    _map[name] = x;
    file.warn(0, here, "undefined, adding: " + name + " as type " + c_str(type));
  }else if (old->type() != type) {
    {if (type == tSCALAR 
	 && (old->type() == tKEYWORD 
	     || old->type() == tNODE
	     || old->type() == tSTRINGVAR)) {
      // ok.  ignore the mismatch.
      // keyword, node, stringvar can be used as a scalar.
    }else{
      untested();
      file.warn(0, here, "type mismatch error: " + name
		+ "\nsymbol is type = " + c_str(old->type())
		+ ", used as type = " + c_str(type));
    }}
  }else{
    // using good symbol correctly
  }}
}
/*--------------------------------------------------------------------------*/
void Symbol_Table::check_add(CS& file, int here, Symbol_Mode mode,
	Symbol_Type type, const std::string& name)
{
  switch (mode) {
  case mDECLARE:
    check_add_declare(file, here, type, name);
    break;
  case mUSE:
    check_add_use(file, here, type, name);
    break;
  }
}
/*--------------------------------------------------------------------------*/
void Symbol_Table::add_symbols(const Symbol_Table& s, const std::string&)
{
  // Loop over all symbols in s.  Add them to "this" symbol table.
  // BUG:: what if s and this have a conflict?
  for (const_iterator i = s.begin(); i != s.end(); ++i) {
    _map[i->first] = i->second;
  }
  // scope BUG
}
/*--------------------------------------------------------------------------*/
Symbol_Table::Symbol_Table()
{
  _map["0"]	= new Symbol(tNODE,	"0");
  _map["open"]	= new Symbol(tKEYWORD,  "open");
  _map["short"]	= new Symbol(tKEYWORD,  "short");
  _map["never"]	= new Symbol(tKEYWORD,  "never");
}
/*--------------------------------------------------------------------------*/
void Ramp_Name::parse(CS& file)
{
  int here = file.cursor();
  file >> _name;
  _sym->check_add_use(file, here, tRAMP, _name);
  // scope BUG
}
/*--------------------------------------------------------------------------*/
Test::Test(CS& file, const Macro_Body* m)
  :_expr(m)
{
  file >> _expr;
}
/*--------------------------------------------------------------------------*/
void Macro_Body::if_body(CS& file)
{
  Item_Base* x = new If(file, this);
  push(x);
}
/*--------------------------------------------------------------------------*/
void Macro_Body::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    Item_Base* x = 0;
    if (0) {}
    else if (file.dmatch("key"))      {x = new Declare(file, this, tKEYWORD);}
    else if (file.dmatch("node"))     {x = new Declare(file, this, tNODE);}
    else if (file.dmatch("ramp"))     {x = new Declare(file, this, tRAMP);}
    else if (file.dmatch("scalar"))   {x = new Declare(file, this, tSCALAR);}
    else if (file.dmatch("string"))   {x = new Declare(file, this,tSTRINGVAR);}
    else if (file.dmatch("table"))    {x = new Declare(file, this, tVECTOR);}

    else if (file.dmatch("assert"))   {x = new Assert(file, this);}
    else if (file.dmatch("define"))   {x = new Define(file, this);}
    else if (file.dmatch("export"))   {x = new Export(file, this);}
    else if (file.dmatch("foreach"))  {x = new Foreach(file, this);}
    else if (file.dmatch("if"))       {x = new If(file, this);}
    else if (file.dmatch("inherit"))  {x = new Inherit(file, this);}
    else if (file.dmatch("select"))   {x = new Select(file, this);}

    else if (file.dmatch("alarm"))    {x = new Alarm(file, this);}
    else if (file.dmatch("trigger"))  {untested();x = new Trigger(file, this);}

    else if (file.dmatch("capacitor")){x = new Capacitor(file, this);}
    else if (file.dmatch("inductor")) {x = new Inductor(file, this);}
    else if (file.dmatch("resistor")) {x = new Resistor(file, this);}
    else if (file.dmatch("vcg"))      {x = new VCG(file, this);}
    else if (file.dmatch("vccs"))     {x = new VCCS(file, this);}
    else if (file.dmatch("vcvs"))     {x = new VCVS(file, this);}
    else if (file.dmatch("vsource"))  {x = new Vsource(file, this);}
    else if (file.dmatch("isource"))  {x = new Isource(file, this);}

    else if (file.dmatch("driver"))   {x = new Driver(file, this);}
    else if (file.dmatch("dsource"))  {untested();x = new Dsource(file, this);}
    else if (file.dmatch("inverter")) {x = new Inverter(file, this);}
    else if (file.dmatch("tline"))    {x = new T_Line(file, this);}
    else if (file.dmatch("reshape"))  {x = new Reshape(file, this);}
    else if (file.dmatch("subckt"))   {x = new Subckt(file, this);}
    if (x) {
      push(x);
    }
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Port_List::parse(CS& file)
{
  if (_nominal_size != 0) {
    {if (file.skip1b("(")) {
      while (!file.skip1b(")")) {
	int here = file.cursor();
	int old_here = here;
	std::string* x = new std::string;
	file >> *x;
	{if (!file.stuck(&here)) {
	  _list.push_back(x);
	  _sym->check_add(file, old_here, _mode, tNODE, *x);
	  old_here = here;
	  // scope BUG
	}else{
	  untested();
	  delete x;
	  file.warn(0, "need port name");
	  break;
	}}
      }
    }else{
      // no connection list
    }}
  }
  if (_nominal_size != SIZE_UNKNOWN  
      &&  static_cast<int>(_list.size()) != _nominal_size) {
    file.warn(0, "wrong number of ports");
  }
}
/*--------------------------------------------------------------------------*/
void Ckt_Block::parse(CS& file)
{
  file >> _name >> _ports >> _lvalue >> _value;
}
/*--------------------------------------------------------------------------*/
Ckt_Block::Ckt_Block(CS&, const Macro_Body* m, int N)
  :_name(),
   _ports(m->sym(), m->ml(), mUSE, N),
   _lvalue(),
   _value(m)
{
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Driver::parse(CS& file)
{
  file >> _name >> _ports;
  file.skip1b("(") || file.warn(0, "need '('");
  int here = file.cursor();
  do {
    get(file, "s0", &_s0);
    get(file, "s1", &_s1);
    get(file, "w01", &_w01);
    get(file, "w10", &_w10);
    get(file, "ramp", &_ramp);
    file.skipcom();
  } while (file.more() && !file.stuck(&here));
  file.skip1b(")") || file.warn(0, "need ')'");
}
/*--------------------------------------------------------------------------*/
void Reshape::parse(CS& file)
{
  file >> _name >> _ports >> _rise_on >> _rise_off >> _fall_on >> _fall_off;
}
/*--------------------------------------------------------------------------*/
void Subckt::parse(CS& file)
{
  file >> _name >> _ports;
  int here = file.cursor();
  file >> _subckt;
  _sym->check_add_use(file, here, tSTRINGVAR, _subckt);
  // scope BUG
}
/*--------------------------------------------------------------------------*/
void T_Line::parse(CS& file)
{
  file >> _name >> _ports;
  int here = file.cursor();
  do {
    get(file, "z0", &_z0);
    get(file, "delay", &_delay);
    file.skipcom();
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Trigger::parse(CS& file)
{
  int here = file.cursor();
  file >> _name >> _value;
  _sym->check_add_use(file, here, tNODE, _name);
  // scope BUG
}
/*--------------------------------------------------------------------------*/
Define::Define(CS& file, const Macro_Body* m)
  :_value(m)
{
  int here = file.cursor();
  _name = file.ctos("=");
  m->sym()->check_add_declare(file, here, tSCALAR, _name);
  file >> _value;
}
/*--------------------------------------------------------------------------*/
Export::Export(CS& file, const Macro_Body*)
{
  file >> _string;
}
/*--------------------------------------------------------------------------*/
Foreach::Foreach(CS& file, const Macro_Body* m)
  :_body(0, m),
   _false_part(0, m)
{
  (file >> _iterator) .dmatch("in");
  int here = file.cursor();  
  file >> _name >> _body;
  m->sym()->check_add_declare(file, here, tFOREACH, _name);
  get(file, "else", &_false_part);
  file.dmatch("end foreach") 
    || file.warn(0, "need circuit or 'end foreach'");
}
/*--------------------------------------------------------------------------*/
If::If(CS& file, const Macro_Body* m)
  :_test(file, m),
   _true_part(file, m),
   _false_part(0, m)
{
  {if (file.dmatch("else if")) {
    _false_part.if_body(file);
  }else{
    if (file.dmatch("else")) {
      file >> _false_part;
    }
    skip_comment(file);
    file.dmatch("end if") || file.warn(0, "need circuit or 'end if'");
  }}
}
/*--------------------------------------------------------------------------*/
Inherit::Inherit(CS& file, const Macro_Body* m)
  :_macro(0)
{
  assert(m);
  assert(m->sym());
  assert(m->ml());

  int here = file.cursor();
  file >> _string;
  _macro = (*m->ml())[_string];
  {if (_macro) {
    m->sym()->add_symbols(_macro->symbols(), m->scope());
  }else{
    file.warn(0, here, "not found");
  }}
}
/*--------------------------------------------------------------------------*/
Declare::Declare(CS& file, const Macro_Body* m, Symbol_Type t)
  :_type(t)
{
  assert(m);
  assert(m->sym());

  int line_start = file.cursor();
  file >> _string;
  CS line(_string);
  int here = line.cursor();
  int old_here = here;
  for (;;) {
    std::string node_name;
    line >> node_name;
    {if (!line.stuck(&here)) {
      m->sym()->check_add_declare(file, old_here+line_start, t, node_name);
      old_here = here;
    }else{
      break;
    }}
  }
}
/*--------------------------------------------------------------------------*/
Case::Case(CS& file, const Macro_Body* m)
  :_here(file.cursor()),
   _name(file),
   _body(file, m)
{
  m->sym()->check_add_declare(file, _here, tSCOPE, name());
  file.dmatch("end case")
    || file.warn(0, "need circuit component or 'end case'");
}
/*--------------------------------------------------------------------------*/
Select::Select(CS& file, const Macro_Body* m)
{
  if (!file.skip1b("(")) {
    file.warn(0, "need '('");
  }
  file >> _control;
  if (!file.skip1b(")")) {
    file.warn(0, "need ')'");
  }
  int here = file.cursor();
  do {
    skip_comment(file);
    Case* x = 0;
    if (file.dmatch("case")) {
      x = new Case(file, m);
    }
    if (x) {
      push(x);
    }
  } while (file.more() && !file.stuck(&here));
  file.dmatch("end select") || file.warn(0, "need 'case' or 'end select'");
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Macro::parse(CS& file)
{
  file >> _type;
  {if ((_type.length() > 0) && (_type[_type.length()-1] == ']')) {
    _type.erase(_type.length()-1);
  }else{
    file.skip1b("]") || file.warn(0, "need ]");    
  }}
  file >> _name >> _ports >> _body;
  {if (!file.dmatch("[End_Define_")) {
    file.warn(0, "need circuit component or '[End Define ....]'");
  }else if (!file.dmatch(_type)) {
    file.warn(0, "define type mismatch, need " + type());
  }else{
    file.skip1b("]") || file.warn(0, "need ]");
  }}
  if (_ports.is_empty()) {
    Macro_Body::const_iterator i(_body.begin());
    {if (i != _body.end()) {
      const Inherit* inherit = dynamic_cast<const Inherit*>(*i);
      {if (inherit) {
	_ports = inherit->macro()->ports();
      }else{
	file.warn(0, "macros with no connections are not very useful");
      }}
    }else{
      file.warn(0, "empty macros with no connections are not very useful");
    }}
  }
}
/*--------------------------------------------------------------------------*/
void Macro_List::parse(CS& file)
{
  int here = file.cursor();
  Macro* x = new Macro(file, this, "");
  {if (!file.stuck(&here)) {
    push(x);
    _map['[' + x->type() + ']' + x->name()] = x;
  }else{
    untested();
    assert(x);
    delete x;
    file.warn(0, "need macro parameters");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
