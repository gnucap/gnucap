/*$Id: c_list.cc,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
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
 * list and save commands.
 * save is list with direction to file
 */
//testing=script 2006.07.17
#include "e_card.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::list(CS&);
//	void	CMD::save(CS&);
static	void	list_save(CS&, OMSTREAM);
/*--------------------------------------------------------------------------*/
extern const std::string head;
/*--------------------------------------------------------------------------*/
void CMD::do_list(CS& cmd)
{
  list_save(cmd, IO::mstdout);
}
/*--------------------------------------------------------------------------*/
void CMD::save(CS& cmd)
{untested();
  cmd.reset();			/* back up to beginning of input line */
  OMSTREAM out; // = IO::mstdout;
  list_save(cmd, *outset(cmd,&out));
}
/*--------------------------------------------------------------------------*/
static void list_save(CS& cmd, OMSTREAM out)
{
  switch (ENV::run_mode) {
  case rIGNORE:
    untested();
  case rPRESET:
    /* do nothing */
    return;
  case rBATCH:
    untested();
  case rINTERACTIVE:
    itested();
  case rSCRIPT:
    /* keep going */
    break;
  }

  all_except(out, IO::mstdout) << head << '\n';

  if (cmd.is_end()) {			/* no args: list all		    */
    for (CARD_LIST::const_iterator
	 ci=CARD_LIST::card_list.begin();ci!=CARD_LIST::card_list.end();++ci) {
      (**ci).print_spice(out, false);
    }
  }else{				/* some args: be selective	    */
    int arg1 = cmd.cursor();
    CARD_LIST::fat_iterator ci = findbranch(cmd, &CARD_LIST::card_list);
    if (ci.is_end()) {
      untested();
      cmd.warn(bERROR, "can't find");
    }
    
    if (cmd.match1('-')) {		/* there is a dash:  a range	    */
      cmd.skip();
      CARD_LIST::fat_iterator stop = ci;
      stop = findbranch(cmd, ++stop);
      if (stop.is_end()) {
	cmd.check(bERROR, "can't find");
	do {			// else go to the end
	  (**ci).print_spice(out, false);
	} while (++ci != stop);
      }else{
	do {
	  (**ci).print_spice(out, false);
	} while (ci++ != stop); // note subtle difference
      }
    }else{				/* no dash: a list		    */
      do {				/* each arg			    */
	int next = cmd.cursor();
	do {				/* all that match this arg	    */
	  (**ci).print_spice(out, true);
	  cmd.reset(arg1);
	  assert(!ci.is_end());
	  ci = findbranch(cmd, ++ci); // next match
	} while (!ci.is_end());
	cmd.reset(arg1 = next);
	ci = findbranch(cmd, &CARD_LIST::card_list); // next arg
      } while (!ci.is_end());
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
