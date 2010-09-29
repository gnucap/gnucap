/*$Id: instance-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
void Scope::parse(CS& file)
{
  skip_comment(file);
  int here = file.cursor();
  do {
    std::string short_name;
    {if (file.match1('[')) {
      short_name = file.ctos("]")+']';
      file.skip1b("]");
    }else{
      file >> short_name;
    }}
    std::string long_name = _base_scope + short_name;
    
    const Symbol* s = (*_symbols)[long_name];
    {if (s) { // successful lookup
      Symbol_Type type = s->type();
      Base* x = 0;
      switch (type) {
      case tNODE:
	untested();
	file.warn(0, here, long_name+": can't use node name here"); break;
      case tKEYWORD:
	untested();
	file.warn(0, here, long_name+": illegal use of reserved word"); break;
	//x = new Keyword(file); break;
      case tSCALAR:
	x = new Scalar(file, short_name); break;
      case tVECTOR:
	x = new Table(file, _symbols, long_name, short_name); break;
      case tSCOPE:
	x = new Scope(file, _symbols, long_name); break;
      case tRAMP:
	x = new Ramp_Data(file, short_name); break;
      case tSTRINGVAR:
	x = new String_Var(file, short_name); break;
      case tFOREACH:
	x = new Foreach_Data(file, short_name); break;
      case tNUMBER_CONST:
      case tSTRING_CONST:
	unreachable(); break;
      }
      if (x) {
	push(x);
	_map[short_name] = x;
      }else{
	untested();
      }
      skip_comment(file);
    }else{
      file.reset(here);
      break; // mismatch leaves scope
    }}
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Foreach_Item::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  int here = line.cursor();
  {for (;;) {
    std::string x;
    line >> x;
    {if (!line.stuck(&here)) {
      _args.push_back(x);
    }else{
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void String_Var::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _data;
}
/*--------------------------------------------------------------------------*/
void Scalar::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  Float::parse(line);
}
/*--------------------------------------------------------------------------*/
void Ramp_Data::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "dV/dt_r", &_dv_dt_r);
    get(file, "dV/dt_f", &_dv_dt_f);
    get(file, "R_load",  &_r_load);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
Instance::Instance(CS& file, const Macro* proto, const std::string& base_type, 
		   const std::string& short_name, const std::string& sub_type)
  :Scope(file, &(proto->symbols()), ""),
   _proto(proto),
   _reduced(0),
   _base_type(base_type),
   _sub_type(sub_type),
   _full_name('[' + base_type + ']' + short_name)
{
  assert(proto);

  Base* x = new Keyword("open");
  push(x);
  _map["open"] = x;
  x = new Keyword("short");
  push(x);
  _map["short"] = x;
  x = new Keyword("never");
  push(x);
  _map["never"] = x;

  //parse(file); done by Scope constructor
  _reduced = new Macro(*proto, *this);
}
/*--------------------------------------------------------------------------*/
void Instance_List::parse(CS& file)
{
  std::string base_type = file.ctos("]");
  if (!file.skip1b("]")) {
    untested();
    file.warn(0, "need ']'");
  }
  std::string name;
  int name_start = file.cursor();
  file >> name;

  // BUG: missing name gives confusing error message
  
  skip_comment(file);

  std::string sub_type;
  get(file, base_type + "_type", &sub_type)
    || file.warn(0, "need " + base_type + "_type");

  std::string type = '[' + base_type + ']' + sub_type;

  const Macro* proto = (*_macros)[type];
  {if (proto) {
    int here = file.cursor();
    Instance* x = new Instance(file, proto, base_type, name, sub_type);
    {if (!file.stuck(&here)) {
      {if (!_map[x->full_name()]) {
	push(x);
	_map[x->full_name()] = x;
      }else{
	untested();
	delete x;
	file.warn(0, name_start, "duplicate rejected");
	untested();
      }}
    }else{
      untested();
      delete x;
      file.warn(0, "need " + type + " parameters");
      untested();
    }}
  }else{
    file.warn(0, type + ": no match");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
