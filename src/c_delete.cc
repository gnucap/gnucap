/*$Id: c_delete.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * delete and clear commands
 */
#include "e_card.h"
#include "s__.h"
#include "ap.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::clear(CS&);
//	void	CMD::del(CS&);
static	void	bylabel(CS&);
/*--------------------------------------------------------------------------*/
/* cmd_clear: clear the whole circuit, including faults, etc
 *   equivalent to unfault; unkeep; delete all; title = (blank)
 */
void CMD::clear(CS&)
{
  {CS nil("");      unfault(nil);}
  {CS nil("");      unmark(nil);}
  {CS clr("clear"); ic(clr);}
  {CS clr("clear"); nodeset(clr);}
  {CS clr("clear"); alarm(clr);}
  {CS clr("clear"); plot(clr);}
  {CS clr("clear"); print(clr);}
  {CS All("all");   del(All);}
  {CS q("'");       title(q);}
}
/*--------------------------------------------------------------------------*/
/* cmd_delete:  delete command
 */
void CMD::del(CS& cmd)
{
  SIM::uninit();
  {if (cmd.pmatch("ALL")){
    CARD_LIST::card_list.destroy();
  }else{
    while (cmd.more()){
      bylabel(cmd);
    }
  }}
}
/*--------------------------------------------------------------------------*/
/* bylabel: delete circuit element by label
 * 	all lines with matching label (with wild cards * and ?) deleted.
 *	although it looks like a loop, it matches one label
 *	syntax warning if no match
 */
static void bylabel(CS& cmd)
{
  int mark = cmd.cursor(); // where we started parsing
  int cmax = cmd.cursor(); // where we got after being successful
  
  CARD_LIST::fat_iterator ci(&(CARD_LIST::card_list));
  for (;;){
    cmd.reset(mark);
    ci = findbranch(cmd, ci);
    cmax = std::max(cmax, cmd.cursor());
    {if (!ci.isend()){ // found something
      CARD_LIST::iterator here(ci.iter());
      ++ci;
      ci.list()->erase(here);
    }else{ // no more matches
      break;
    }}
  }
  cmd.reset(cmax);
  if (mark == cmax){ // didn't do anything
    cmd.check(bWARNING, "no match");
    cmd.skiparg();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
