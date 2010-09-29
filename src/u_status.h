/*$Id: u_status.h,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * place to store all kinds of statistics
 */
#ifndef STATUS_H
#define STATUS_H
#include "l_timer.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
class CS;
/*--------------------------------------------------------------------------*/
class STATUS : public SIM {
public:
	void command(CS& cmd);
private:
  virtual void	setup(CS&)	{unreachable();}
  virtual void	sweep()		{unreachable();}
public:
  static TIMER get;
  static TIMER op;
  static TIMER dc;
  static TIMER tran;
  static TIMER four;
  static TIMER ac;
  static TIMER set_up;
  static TIMER order;
  static TIMER advance;
  static TIMER evaluate;
  static TIMER load;
  static TIMER lud;
  static TIMER back;
  static TIMER review;
  static TIMER accept;
  static TIMER output;
  static TIMER overhead;
  static TIMER aux1;
  static TIMER aux2;
  static TIMER aux3;
  static TIMER total;
  static int user_nodes;
  static int subckt_nodes;
  static int model_nodes;
  static int total_nodes;
  static int control[cCOUNT];
  static int iter[iCOUNT];

  static int newnode_subckt(){++subckt_nodes; return ++total_nodes;}
  static int newnode_model() {++model_nodes;  return ++total_nodes;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
