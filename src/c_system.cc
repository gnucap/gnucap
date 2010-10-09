/*$Id: c_system.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
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
 * system calls: change directory, invoke another program, invoke editor, etc.
 */
//testing=none 2006.07.17
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/* cmd_edit: (command) invoke user defined editor on the netlist
 * if command has an argument, it edits that file instead
 * else actually edits a temporary file, and reads it back.
 */
class CMD_EDIT : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {itested();
    std::string editor(OS::getenv("EDITOR"));
    if (editor == "") {
      throw Exception("no editor defined\n"
	    "You need to set the EDITOR environment variable.");
    }else{
      if (cmd.more()) {itested();
	OS::system(editor + ' ' + cmd.tail());
      }else{
	std::string temp_file("/tmp/gnucap" + to_string(unsigned(time(NULL))));
	command("save " + temp_file + " quiet", Scope);
	OS::system(editor + ' ' + temp_file);
	command("get " + temp_file + " quiet", Scope);
	OS::remove(temp_file);
      }
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "edit", &p1);
/*--------------------------------------------------------------------------*/
class CMD_SYSTEM : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {itested();
    if (cmd.more()) {itested();
      OS::system(cmd.tail());
    }else{
      OS::system(SHELL);
    }
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "system|!", &p2);
/*--------------------------------------------------------------------------*/
class CMD_CHDIR : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {itested();
    if (cmd.more()) {
      OS::chdir(cmd.ctos(""));
    }else{
    }
    IO::mstdout << OS::getcwd() << '\n';
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "chdir|cd", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
