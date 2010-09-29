/*$Id: findbr.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * find a branch with matching label
 * returns the branch pointer
 */
#include "ap.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
/* findbranch: find a matching label, by (ugh) linear search
 * 	start searching at "start", stop at "stop"
 *	label to look for is in command line: &cmd[*cnt]
 * return pointer to match if exists (and eat input)
 *	  pointer to a non-existent branch if no match (don't eat input)
 * caution: caller must check return value before using
 */
CARD_LIST::fat_iterator findbranch(CS& cmd, CARD_LIST::fat_iterator here)
{
  int save = cmd.cursor();

  char labelwanted[BUFLEN+1];
  cmd.ctostr(labelwanted, BUFLEN, TOKENTERM);
  
  if (!labelwanted[0]) {		// nothing to look for
    cmd.reset(save);
    return here.end();
  }
  
  char* local_part;	    // part before last dot, the thing inside
  char* last_part;	    // part after last dot, top level
  char* dot = strrchr(labelwanted,'.');	// used as a flag
  if (dot) {		    // split the string into 2 parts at the last dot
    *dot = '\0';	    // before is the new "local_part", shortened
    last_part = dot + 1;    // after is the "last_part".
    local_part = labelwanted;
  }else{
    last_part = labelwanted;
    local_part = NULL;
  }

  for (;;) {
    if (here.isend()) {	// done -- fails
      cmd.reset(save);
      return here;
    }
    if (wmatch((**here).short_label(), last_part)) { // label matches
      {if (!local_part) {		    // found it .. done.
	return here;
      }else if((**here).subckt().exists()) {// this one has a subckt .. try it
	CS want(local_part);
	CARD_LIST::fat_iterator subbrh = findbranch(want, &(**here).subckt());
	if (!subbrh.isend()) { // found it in a subckt
	  return subbrh;
	}
      }else{ 
	// there's a dot in the name (which matches), no subckt
	// .. not it .. try again
	untested();
      }}
    }
    ++here;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
