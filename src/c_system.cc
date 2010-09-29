/*$Id: c_system.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * system calls: change directory, invoke another program, invoke editor, etc.
 */
//testing=none 2006.07.17
#include "io_.h"
#include "ap.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::edit(CS&);
//	void	CMD::system(CS&);
//	void	CMD::chdir(CS&);
/*--------------------------------------------------------------------------*/
/* cmd_edit: (command) invoke user defined editor on the netlist
 * if command has an argument, it edits that file instead
 * else actually edits a temporary file, and reads it back.
 */
void CMD::edit(CS& cmd)
{
  std::string editor(OS::getenv("EDITOR"));
  if (editor.empty()) {
    error(bERROR, "no editor defined\n"
	  "You need to set the EDITOR environment variable.");
  }else{
    if (cmd.more()) {
      OS::system(editor + ' ' + cmd.tail());
    }else{
      //std::string temp_file(::tmpnam(0));
      std::string temp_file("/tmp/foo");
      if (temp_file.empty()) {
	error(bERROR, "cannot create temp file\n");
      }else{
	cmdproc("save " + temp_file + " quiet");
	OS::system(editor + ' ' + temp_file);
	cmdproc("get " + temp_file + " quiet");
	OS::remove(temp_file);
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void CMD::system(CS& cmd)
{
  if (cmd.more()) {
    OS::system(cmd.tail());
  }else{
    OS::system(SHELL);
  }
}
/*--------------------------------------------------------------------------*/
void CMD::chdir(CS& cmd)
{
  if (cmd.more()) {
    OS::chdir(cmd.ctos(""));
  }else{
  }
  IO::mstdout << OS::getcwd() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
