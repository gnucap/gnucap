/*$Id: c_comand.cc,v 26.138 2013/04/24 02:44:30 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * simple commands and stubs for the missing commands
 */
//testing=script,sparse 2006.07.16
#include "constant.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_END : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)override {
    switch (ENV::run_mode) {
    case rPRE_MAIN: unreachable(); break;
    case rPRESET:   untested(); break;  //BUG// this should close the file
    case rINTERACTIVE:
      itested();
      command("quit", Scope);
      break;
    case rSCRIPT:
      if (OPT::acct) {untested();
	command("status", Scope);
      }else{untested();
      }
      untested();
      throw Exception("end");
      break;
    case rBATCH:
      if (OPT::acct) {itested();
	command("status", Scope);
      }else{
      }
      command("quit", Scope);
      break;
    }
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "end", &p0);
/*--------------------------------------------------------------------------*/
class CMD_PAUSE : public CMD {
public:
  void do_it(CS&, CARD_LIST*)override {untested();
    //BUG// buffer problem
    //BUG// redirection problem
    IO::error << "Continue? ";
    int ch = getchar();
    if (ch=='n' || ch=='N' || ch=='C'-'@' || ch=='['-'@') {untested();
      throw Exception("pause-stop");
    }else{untested();
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "pause", &p1);
/*--------------------------------------------------------------------------*/
class CMD_QUIT : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)override {
    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable(); break;
    case rINTERACTIVE:	
    case rSCRIPT:	
    case rBATCH:	command("clear", Scope); exit(0); break;
    case rPRESET:	untested(); /*nothing*/ break;
    }
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "quit|exit", &p2);
/*--------------------------------------------------------------------------*/
class CMD_TEMP : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)override {itested();
    double t = NOT_INPUT;
    size_t here = cmd.cursor();
    cmd >> '=' >> t;
    if (!cmd.stuck(&here)) {itested();
      OPT::temp_c = t;
    }else{itested();
      IO::mstdout << ".temp = " << OPT::temp_c << '\n';
    }
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "temperature|temp", &p3);
/*--------------------------------------------------------------------------*/
class CMD_TITLE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)override {
    if (cmd.more()) {
      head = cmd.tail();
    }else{itested(); 
      IO::mstdout << head << '\n';
    }
  }
} p4;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "title", &p4);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
