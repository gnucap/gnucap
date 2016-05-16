/*$Id: globals.cc,v 26.138 2013/04/24 03:03:11 al Exp $ -*- C++ -*-
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
 * static objects -- construct first, destruct last
 */
#include "globals.h"
#include "u_prblst.h"
#include "e_cardlist.h"
#include "u_status.h"
#include "patchlev.h"
/*--------------------------------------------------------------------------*/
// here to make sure they get constructed and destructed in proper order
// first construct, last destruct

const char* lib_version()
{
  return PATCHLEVEL;
}

// dispatchers must be constructed before any static objects they contain
// and destructed after.
RUN_MODE ENV::run_mode = rPRE_MAIN;
DISPATCHER<CMD> command_dispatcher;
DISPATCHER<COMMON_COMPONENT> bm_dispatcher;
DISPATCHER<MODEL_CARD> model_dispatcher;
DISPATCHER<CARD> device_dispatcher;
DISPATCHER<LANGUAGE> language_dispatcher;
DISPATCHER<FUNCTION> function_dispatcher;
DISPATCHER<FUNCTION> measure_dispatcher;
DISPATCHER<CKT_BASE> status_dispatcher;
DISPATCHER<CKT_BASE> help_dispatcher;
DISPATCHER<PROBELIST> probe_dispatcher;

// for the rest, order should not matter, but it is convenient here
CARD_LIST CARD_LIST::card_list;	// root circuit 

// the rest of this should not be here, is residue of old stuff
STATUS status;
std::string head;
/*--------------------------------------------------------------------------*/
/* sweep command.  This will go away with a rewrite of looping */
const int swp_nest_max = 1;	/* no loop, "future" expansion		*/
INTERFACE int swp_count[swp_nest_max];	/* sweep counter			*/
INTERFACE int swp_steps[swp_nest_max];	/* sweep number of steps		*/
INTERFACE int swp_type[swp_nest_max];	/* type of sweep (log or linear)	*/
INTERFACE int swp_nest;			/* sweep nesting (recursion) level	*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
