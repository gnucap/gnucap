/*$Id: globals.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include "e_node.h"
#include "l_jmpbuf.h"
#include "e_model.h"
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
/* nodes, hold between commands */
NODE* nstat;			/* node status flags			*/
/*--------------------------------------------------------------------------*/
/* other (the ultimate in disorder) */
std::string head;
/*--------------------------------------------------------------------------*/
/* here to make sure they get constructed and destructed in proper order */
std::list<CARD*> root_model_list;
CARD_LIST CARD_LIST::card_list;
CARD_LIST::fat_iterator CARD_LIST::putbefore(&card_list, card_list.end());
MODEL_LOGIC default_logic_family;
/*--------------------------------------------------------------------------*/
/* command interpreter, control stuff */
JMP_BUF env;			/* environment for setjmp, longjmp	*/
bool crtplot = false;		/* flag: is plotting on the crt		*/

/* sweep command */
const int swp_nest_max = 1;	/* no loop, "future" expansion		*/
int swp_count[swp_nest_max];	/* sweep counter			*/
int swp_steps[swp_nest_max];	/* sweep number of steps		*/
int swp_type[swp_nest_max];	/* type of sweep (log or linear)	*/
int swp_nest;			/* sweep nesting (recursion) level	*/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
