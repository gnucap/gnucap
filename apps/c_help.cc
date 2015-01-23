/*$Id: c_help.cc 2015/01/21 al $ -*- C++ -*-
 * Copyright (C) 2014 Albert Davis
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
 *------------------------------------------------------------------
 */
//testing=script,complete 2015.01.22
#include "l_dispatcher.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
  static DISPATCHER_BASE* dispatchers[] = {
    &help_dispatcher,
    &language_dispatcher,
    &command_dispatcher,
    &device_dispatcher,
    &function_dispatcher,
    &measure_dispatcher,
    &model_dispatcher,
    &bm_dispatcher,
    &status_dispatcher,
    &probe_dispatcher,
    NULL };
  // the order here determines the search order
/*--------------------------------------------------------------------------*/
class CMD_HELP : public CMD {
public:
  void do_it(CS& Cmd, CARD_LIST*)
  {
    unsigned tail = Cmd.cursor();
    
    std::string topic;
    Cmd >> topic;

    bool did_something = false;
    unsigned here = Cmd.cursor();
    for (DISPATCHER_BASE** ii = dispatchers; *ii; ++ii) {
      CKT_BASE* object = (**ii)[topic];
      if (object) {
	did_something |= object->help(Cmd, IO::mstdout);
      }else{
	// nothing, it's ok
      }
      Cmd.reset(here);
    }
    if (!did_something) {
      Cmd.warn(bWARNING, tail, "no help on topic " + Cmd.substr(tail));
    }else{
    }
  }
  
  std::string help_text()const
  {
    return 
      "help command\n"
      "Provides help on a variety of topics\n"
      "Syntax: help topic\n"
      "In some cases, help on subtopics is available\n"
      "Syntax: help topic subtopic\n"
      "For a list of subtopics: help topic ?\n\n"
      "@@subtopic\n"
      "help subtopic\n"
      "subtopics give you more detailed information\n\n"
      "@@test\n"
      "help test\n\n";//);
  }

} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "help", &p0);
/*--------------------------------------------------------------------------*/
class HELP_ERROR_TEST : public CKT_BASE {
} p1;
DISPATCHER<CKT_BASE>::INSTALL d1(&help_dispatcher, "help_error_test_with_no_help", &p1);  
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
