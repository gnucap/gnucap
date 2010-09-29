/*$Id: u_status.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * place to store all kinds of statistics
 */
//testing=script 2006.07.14
#ifndef STATUS_H
#define STATUS_H
#include "l_timer.h"
#include "mode.h"
/*--------------------------------------------------------------------------*/
class CS;
/*--------------------------------------------------------------------------*/
class STATUS {
public:
  void command(CS& cmd);
public:
  TIMER get;
  TIMER op;
  TIMER dc;
  TIMER tran;
  TIMER four;
  TIMER ac;
  TIMER set_up;
  TIMER order;
  TIMER advance;
  TIMER queue;
  TIMER evaluate;
  TIMER load;
  TIMER lud;
  TIMER back;
  TIMER review;
  TIMER accept;
  TIMER output;
  mutable TIMER overhead;
  TIMER aux1;
  TIMER aux2;
  TIMER aux3;
  TIMER total;
  int user_nodes;
  int subckt_nodes;
  int model_nodes;
  int total_nodes;
  int control[cCOUNT];
  int iter[iCOUNT];
  
  int newnode_subckt() {++subckt_nodes; return ++total_nodes;}
  int newnode_model()  {++model_nodes;  return ++total_nodes;}

  void compute_overhead()const {
    overhead = total - advance - queue - evaluate - load - lud - back 
      - output - review - accept;
  }

  explicit STATUS() :
    get("get"),
    op("op"),
    dc("dc"),
    tran("tran"),
    four("fourier"),
    ac("ac"),
    set_up("setup"),
    order("order"),
    advance("advance"),
    queue("queue"),
    evaluate("evaluate"),
    load("load"),
    lud("lu"),
    back("back"),
    review("review"),
    accept("accept"),
    output("output"),
    overhead("overhead"),
    aux1("aux1"),
    aux2("aux2"),
    aux3("aux3"),
    total("total"),
    user_nodes(0),
    subckt_nodes(0),
    model_nodes(0),
    total_nodes(0)
  {
    std::fill_n(control, cCOUNT, 0);
    std::fill_n(iter, iCOUNT, 0);
  }
  ~STATUS() {}
private:
  explicit STATUS(const STATUS&) {unreachable(); incomplete();}
};
/*--------------------------------------------------------------------------*/
extern STATUS status;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
