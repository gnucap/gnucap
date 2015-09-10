/*$Id: u_lang.cc 2015/02/05 al $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
// testing=script 2015.01.27
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
LANGUAGE::~LANGUAGE()
{
  if (OPT::language == this) {
    OPT::language = NULL;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void LANGUAGE::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line(I_PROMPT);
  CMD::cmdproc(cmd, Scope);
}
/*--------------------------------------------------------------------------*/
const CARD* LANGUAGE::find_proto(const std::string& Name, const CARD* Scope)
{
  const CARD* p = NULL;
  if (Scope) {
    try {
      p = Scope->find_looking_out(Name);
    }catch (Exception_Cant_Find& e) {
      assert(!p);
    }
  }else{
    CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(Name);
    if (i != CARD_LIST::card_list.end()) {
      p = *i;
    }else{
      assert(!p);
    }
  }
  
  if (p) {
    return p;
  }else if ((command_dispatcher[Name])) {
    return new DEV_DOT;	//BUG// memory leak
  }else if ((p = device_dispatcher[Name])) {
    return p;
  }else if ((p = model_dispatcher[Name])) {
    return p;
  }else{
    assert(!p);
    std::string s;
    /* */if (Umatch(Name, "b{uild} "))      {untested(); s = "build";}
    else if (Umatch(Name, "del{ete} "))     {untested(); s = "delete";}
    else if (Umatch(Name, "fo{urier} "))    {untested(); s = "fourier";}
    else if (Umatch(Name, "gen{erator} "))  {		 s = "generator";}
    else if (Umatch(Name, "inc{lude} "))    {untested(); s = "include";}
    else if (Umatch(Name, "l{ist} "))       {untested(); s = "list";}
    else if (Umatch(Name, "m{odify} "))     {untested(); s = "modify";}
    else if (Umatch(Name, "opt{ions} "))    {            s = "options";}
    else if (Umatch(Name, "par{ameter} "))  {untested(); s = "param";}
    else if (Umatch(Name, "pr{int} "))      {untested(); s = "print";}
    else if (Umatch(Name, "q{uit} "))       {untested(); s = "quit";}
    else if (Umatch(Name, "st{atus} "))     {            s = "status";}
    else if (Umatch(Name, "te{mperature} ")){untested(); s = "temperature";}
    else if (Umatch(Name, "tr{ansient} "))  {            s = "transient";}
    else if (Umatch(Name, "!"))		    {untested(); s = "system";}
    else if (Umatch(Name, "<"))		    {untested(); s = "<";}
    else if (Umatch(Name, ">"))		    {untested(); s = ">";}
    else{ /* no shortcut available */
      s = Name;
    }
    if ((command_dispatcher[s])) {
      return new DEV_DOT; //BUG// we will look it up twice, //BUG// memory leak
    }else{
      return NULL;
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANGUAGE::new__instance(CS& cmd, BASE_SUBCKT* owner, CARD_LIST* Scope)
{
  if (cmd.is_end()) {untested();
    // nothing
  }else{
    std::string type = find_type_in_string(cmd);
    if (const CARD* proto = find_proto(type, owner)) {
      CARD* new_instance = proto->clone_instance();
      assert(new_instance);
      new_instance->set_owner(owner);
      CARD* x = parse_item(cmd, new_instance);
      if (x) {
	assert(Scope);
	Scope->push_back(x);
      }else{
      }
    }else{
      cmd.warn(bDANGER, type + ": no match");
    }
  }
}
/*--------------------------------------------------------------------------*/
CARD* LANGUAGE::parse_item(CS& cmd, CARD* c)
{
  // See Stroustrup 15.4.5
  // If you can think of a better way, tell me.
  // It must be in the LANGUAGE class, not CARD.

  if (BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(c)) { untested();
    if (c->is_device()){ untested();
      return parse_instance(cmd, s);
    }else{ untested();
      return parse_module(cmd, s);
    }
  }else if (dynamic_cast<COMPONENT*>(c)) {
    return parse_instance(cmd, prechecked_cast<COMPONENT*>(c));
  }else if (dynamic_cast<MODEL_CARD*>(c)) {untested();
    return parse_paramset(cmd, prechecked_cast<MODEL_CARD*>(c));
  }else if (dynamic_cast< DEV_COMMENT*>(c)) {
    return parse_comment(cmd, prechecked_cast<DEV_COMMENT*>(c));
  }else if (dynamic_cast<DEV_DOT*>(c)) {
    return parse_command(cmd, prechecked_cast<DEV_DOT*>(c));
  }else{untested();
    incomplete();
    unreachable();
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
void LANGUAGE::print_item(OMSTREAM& o, const CARD* c)
{
  // See Stroustrup 15.4.5
  // If you can think of a better way, tell me.
  // It must be in the LANGUAGE class, not CARD.
  assert(c);
  assert(dynamic_cast<const CARD*>(c));

  if (const BASE_SUBCKT* s=dynamic_cast<const BASE_SUBCKT*>(c)) { untested();
    if (s->is_device()){ untested();
      print_instance(o, s);
    }else{ untested();
      print_module(o, s);
    }
  }else if (dynamic_cast<const COMPONENT*>(c)) {
    print_instance(o, prechecked_cast<const COMPONENT*>(c));
  }else if (dynamic_cast<const MODEL_CARD*>(c)) {
    print_paramset(o, prechecked_cast<const MODEL_CARD*>(c));
  }else if (dynamic_cast<const DEV_COMMENT*>(c)) {
    print_comment(o, prechecked_cast<const DEV_COMMENT*>(c));
  }else if (dynamic_cast<const DEV_DOT*>(c)) {untested();
    print_command(o, prechecked_cast<const DEV_DOT*>(c));
  }else{untested();
    incomplete();
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
OMSTREAM& operator<<(OMSTREAM& o, LANGUAGE* x)
{
  if (x) {
    return (o << x->name());
  }else{untested();
    return (o << "none");
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, LANGUAGE** val)
{
  if (cmd.umatch(key + " {=}")) {
    LANGUAGE* lang = language_dispatcher[cmd];
    if (lang) {
      *val = lang;
    }else{untested();
      std::string choices;
      for(DISPATCHER<LANGUAGE>::const_iterator
	  i = language_dispatcher.begin(); i != language_dispatcher.end(); ++i) {untested();
	if (i->second) {untested();
	  choices += i->first + ' ';
	}else{untested();
	}
      }
      cmd.warn(bWARNING, "need a language (" + choices + ")");
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
