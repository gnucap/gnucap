/*$Id: u_lang.h,v 26.109 2009/02/02 06:39:10 al Exp $ -*- C++ -*-
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
#ifndef U_LANG_H
#define U_LANG_H
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
class COMPONENT;
class MODEL_SUBCKT;
class MODEL_CARD;
class CARD;
class DEV_COMMENT;
class DEV_DOT;
class CARD_LIST;
/*--------------------------------------------------------------------------*/
class INTERFACE LANGUAGE {
public:
  const CARD* find_proto(const std::string&, const CARD*);
public:
  void new__instance(CS& cmd, MODEL_SUBCKT* owner, CARD_LIST* Scope);

public:
  virtual ~LANGUAGE() {}
  virtual std::string name()const = 0;
  virtual bool case_insensitive()const = 0;
  virtual UNITS units()const = 0;
public: // used by obsolete_callback
  virtual std::string arg_front()const = 0;
  virtual std::string arg_mid()const = 0;
  virtual std::string arg_back()const = 0;

  // in
public: // real public interface
  virtual void		parse_top_item(CS&, CARD_LIST*);
  virtual CARD*		parse_item(CS&, CARD*);
public: // called by commands and parse_item
  virtual DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*) = 0;
  virtual DEV_DOT*	parse_command(CS&, DEV_DOT*) = 0;
  virtual MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*) = 0;
  virtual MODEL_SUBCKT* parse_module(CS&, MODEL_SUBCKT*) = 0;
  virtual COMPONENT*	parse_instance(CS&, COMPONENT*) = 0;
  virtual std::string	find_type_in_string(CS&) = 0;

  // out
public: // real public interface
  virtual void print_item(OMSTREAM&, const CARD*);
private: // called by print_item
  virtual void print_paramset(OMSTREAM&, const MODEL_CARD*) = 0;
  virtual void print_module(OMSTREAM&, const MODEL_SUBCKT*) = 0;
  virtual void print_instance(OMSTREAM&, const COMPONENT*) = 0;
  virtual void print_comment(OMSTREAM&, const DEV_COMMENT*) = 0;
  virtual void print_command(OMSTREAM&, const DEV_DOT*) = 0;
};
OMSTREAM& operator<<(OMSTREAM& o, LANGUAGE* x);
bool Get(CS&, const std::string& key, LANGUAGE** val);
/*--------------------------------------------------------------------------*/
// This is for backward compatibility only.
// It will be removed in the future.
// Do not use in new code.
template <class T>
void print_pair(OMSTREAM& o, LANGUAGE* lang, const std::string& name,
		T value, bool test=true)
{
  if (test) {
    if (lang) {
      std::string front = lang->arg_front() + name + lang->arg_mid();
      o << front << value << lang->arg_back();
    }else{
      o << ' ' + name + '=' << value;
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
