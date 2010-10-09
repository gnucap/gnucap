/*$Id: c__cmd.cc,v 26.130 2009/11/15 21:51:59 al Exp $ -*- C++ -*-
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
 * command interpreter and dispatcher
 */
//testing=obsolete
#include "u_status.h"
#include "declare.h"	/* plclose */
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
/* cmdproc: process a command
 * parse, and act on, a command string
 */
void CMD::cmdproc(CS& cmd, CARD_LIST* scope)
{
  bool get_timer_was_running = ::status.get.is_running();
  ::status.get.stop();

  static TIMER timecheck;
  bool didsomething = true;
  
  error(bTRACE, ">>>>>" + cmd.fullstring() + "\n");
  
  timecheck.stop().reset().start();

  cmd.umatch(ANTI_COMMENT);
  while (cmd.umatch(I_PROMPT)) {itested();
    /* skip any number of these */
  }

  unsigned here = cmd.cursor();
  std::string s;

  // Map possible short names to full ones.
  // If this if/else block is removed, the only loss is the short names.
  // Although it looks like it can be used to make aliases, don't.
  if (cmd.umatch("'|*|#|//|\""))	{itested(); s = "xxxxcomment";}
  else if (cmd.umatch("b{uild} "))      {itested();  s = "build";}
  else if (cmd.umatch("del{ete} "))     {            s = "delete";}
  else if (cmd.umatch("fo{urier} "))    {            s = "fourier";}
  else if (cmd.umatch("gen{erator} "))  {	     s = "generator";}
  else if (cmd.umatch("inc{lude} "))    {itested();  s = "include";}
  else if (cmd.umatch("l{ist} "))       {            s = "list";}
  else if (cmd.umatch("m{odify} "))     {            s = "modify";}
  else if (cmd.umatch("opt{ions} "))    {            s = "options";}
  else if (cmd.umatch("par{ameter} "))  {            s = "param";}
  else if (cmd.umatch("pr{int} "))      {            s = "print";}
  else if (cmd.umatch("q{uit} "))       {	     s = "quit";}
  else if (cmd.umatch("st{atus} "))     {            s = "status";}
  else if (cmd.umatch("te{mperature} ")){itested();  s = "temperature";}
  else if (cmd.umatch("tr{ansient} "))  {            s = "transient";}
  else if (cmd.umatch("!"))		{	     s = "system";}
  else if (cmd.umatch("<"))		{untested(); s = "<";}
  else if (cmd.umatch(">"))		{untested(); s = ">";}
  else{ /* no shortcut available */
    cmd >> s;
    didsomething = false;
  }

  if (s == "xxxxcomment") {itested();
    // nothing
  }else if (s != "") {
    CMD* c = command_dispatcher[s];
    if (c) {
      c->do_it(cmd, scope);
      didsomething = true;
    }else{itested();
      cmd.warn(bWARNING, here, "what's this?");
    }
  }else if (!didsomething) {itested();
    cmd.check(bWARNING, "bad command");
    didsomething = false;
  }else{itested();
  }
  
  if (OPT::acct  &&  didsomething) {itested();
    IO::mstdout.form("time=%8.2f\n", timecheck.check().elapsed());
  }else{
  }
  plclose();
  outreset();

  if (get_timer_was_running) {
    ::status.get.start();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void CMD::command(const std::string& cs, CARD_LIST* scope)
{
  CS cmd(CS::_STRING, cs); // from string, full command
  std::string s;
  cmd >> s;

  CMD* c = command_dispatcher[s];
  if (c) {
    c->do_it(cmd, scope);
  }else{itested();
    throw Exception("bad internal command: " + s);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
