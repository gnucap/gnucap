/*$Id: globals.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * kluges and remnants of old C code
 *
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
 */
//testing=trivial 2006.07.17
#include "l_jmpbuf.h"
#include "d_logic.h"
#include "u_status.h"
/*--------------------------------------------------------------------------*/
STATUS status;
/*--------------------------------------------------------------------------*/
/* nodes, hold between commands */
LOGIC_NODE* nstat;		/* node status flags			*/
/*--------------------------------------------------------------------------*/
/* other (the ultimate in disorder) */
std::string head;
/*--------------------------------------------------------------------------*/
/* here to make sure they get constructed and destructed in proper order */
CARD_LIST CARD_LIST::card_list;
MODEL_LOGIC default_logic_family;
/*--------------------------------------------------------------------------*/
/* command interpreter, control stuff */
JMP_BUF env;			/* environment for setjmp, longjmp	*/

/* sweep command */
const int swp_nest_max = 1;	/* no loop, "future" expansion		*/
int swp_count[swp_nest_max];	/* sweep counter			*/
int swp_steps[swp_nest_max];	/* sweep number of steps		*/
int swp_type[swp_nest_max];	/* type of sweep (log or linear)	*/
int swp_nest;			/* sweep nesting (recursion) level	*/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
