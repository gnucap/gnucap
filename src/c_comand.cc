/*$Id: c_comand.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * simple commands and stubs for the missing commands
 */
#include "declare.h"	/* plclear */
#include "u_status.h"
#include "u_opt.h"
#include "ap.h"
#include "c_comand.h"
// testing=nonstrict
/*--------------------------------------------------------------------------*/
   void	  CMD::alter(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::disto(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::model(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::noise(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::sens(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::subckt(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::temp(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
   void	  CMD::tf(CS&cmd)	{untested(); cmd.warn(bWARNING,"bad command");}
/*--------------------------------------------------------------------------*/
   void	  CMD::options(CS&cmd)	{static OPT o;    o.command(cmd);}
   void	  CMD::status(CS&cmd)	{static STATUS s; s.command(cmd);}
/*--------------------------------------------------------------------------*/
//   void	  CMD::comment(CS&);
//   void	  CMD::end(CS&);
//   void	  CMD::pause(CS&);
//   void	  CMD::quit(CS&);
//   void	  CMD::title(CS&);
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
  if (count == oldcount+1){
    plclear();
  }
  oldcount = count;
  cmd.check(bWARNING, "bad command");
}
/*--------------------------------------------------------------------------*/
void CMD::end(CS& cmd)
{
  switch (ENV::run_mode){
  case rIGNORE:
    unreachable();
    break;
  case rPRESET:
    // BUG: this should close the file
    break;
  case rINTERACTIVE:
    {CS nil(""); quit(nil);}
    break;
  case rSCRIPT:
    if (OPT::acct){
      status(cmd);
    }
    error(bERROR, "");
    break;
  case rBATCH:
    if (OPT::acct){
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
    error(bERROR, "\r");
  }
}
/*--------------------------------------------------------------------------*/
void CMD::quit(CS&)
{
  switch (ENV::run_mode){
  case rINTERACTIVE:
  case rSCRIPT:
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
void CMD::title(CS& cmd)
{
  {if (cmd.more()){
    head = cmd.tail();
  }else{
    untested(); 
    IO::mstdout << head << '\n';
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
