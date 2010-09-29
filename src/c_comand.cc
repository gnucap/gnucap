/*$Id: c_comand.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * simple commands and stubs for the missing commands
 */
//testing=script,sparse 2006.07.16
#include "constant.h"
#include "declare.h"	/* plclear */
#include "u_status.h"
#include "u_opt.h"
#include "ap.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
void CMD::disto(CS&cmd)	 {untested(); cmd.warn(bWARNING,"bad command");}
void CMD::model(CS&cmd)	 {untested(); cmd.warn(bWARNING,"bad command");}
void CMD::noise(CS&cmd)	 {untested(); cmd.warn(bWARNING,"bad command");}
void CMD::sens(CS&cmd)	 {untested(); cmd.warn(bWARNING,"bad command");}
void CMD::subckt(CS&cmd) {untested(); cmd.warn(bWARNING,"bad command");}
void CMD::tf(CS&cmd)	 {untested(); cmd.warn(bWARNING,"bad command");}
/*--------------------------------------------------------------------------*/
void CMD::options(CS&cmd)	{static OPT o;    o.command(cmd);}
/*--------------------------------------------------------------------------*/
int CMD::count = 0;
extern std::string head;
static int oldcount = 0;     /* so we can check for consecutive comments */
/*--------------------------------------------------------------------------*/
/* cmd_comment: user command
 * if there are two consecutive comments, exit graphics mode
 * (this is a kluge)
 */
void CMD::comment(CS& cmd)
{
  if (count == oldcount+1) {
    itested();
    plclear();
  }
  oldcount = count;
  cmd.check(bWARNING, "bad command");
}
/*--------------------------------------------------------------------------*/
void CMD::end(CS& cmd)
{
  switch (ENV::run_mode) {
  case rIGNORE:
    unreachable();
    break;
  case rPRESET:
    untested();
    // BUG: this should close the file
    break;
  case rINTERACTIVE:
    untested();
    {CS nil(""); quit(nil);}
    break;
  case rSCRIPT:
    untested();
    if (OPT::acct) {
      untested();
      status(cmd);
    }else{
      untested();
    }
    error(bERROR, "");
    break;
  case rBATCH:
    if (OPT::acct) {
      untested();
      status(cmd);
    }
    {CS nil(""); quit(nil);}
    break;
  }
}
/*--------------------------------------------------------------------------*/
void CMD::pause(CS&)
{
  untested(); 
  oldcount = count;
  IO::error << "Continue? ";
  int ch = getchar();
  if (ch=='n' || ch=='N' || ch=='C'-'@' || ch=='['-'@') {
    untested();
    error(bERROR, "\r");
  }else{
    untested();
  }
}
/*--------------------------------------------------------------------------*/
void CMD::quit(CS&)
{
  switch (ENV::run_mode) {
  case rSCRIPT:
    untested();
  case rINTERACTIVE:
    itested();
  case rBATCH:
    {CS nil(""); clear(nil);}
    exit(0);
    break;
  case rIGNORE:
    untested(); 
  case rPRESET:
    untested(); 
    /*nothing*/
    break;
  }
}
/*--------------------------------------------------------------------------*/
void CMD::temp(CS& cmd)
{
  untested();
  double t = NOT_INPUT;
  int here = cmd.cursor();
  cmd >> '=' >> t;
  if (!cmd.stuck(&here)) {
    untested();
    OPT::temp_c = t;
  }else{
    untested();
    IO::mstdout << ".temp = " << OPT::temp_c << '\n';
  }
}
/*--------------------------------------------------------------------------*/
void CMD::title(CS& cmd)
{
  if (cmd.more()) {
    head = cmd.tail();
  }else{
    untested(); 
    IO::mstdout << head << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
