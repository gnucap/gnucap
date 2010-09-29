/*$Id: c_modify.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
#include "u_cardst.h" // includes e_compon.h
#include "c_comand.h"
#include "ap.h"
enum WHATTODO {FAULT, MODIFY};
/*--------------------------------------------------------------------------*/
//	void	CMD::modify(CS&);
//	void	CMD::fault(CS&);
static	void	modify_fault(CS&,WHATTODO);
static	double	sweep_fix(CS&,const CARD*);
static	void	faultbranch(CARD*,double);
//	void	CMD::restore(CS&);
//	void	CMD::unfault(CS&);
/*--------------------------------------------------------------------------*/
extern CONST bool crtplot;
extern CONST int swp_type[];
extern CONST int swp_count[], swp_steps[];
extern CONST int swp_nest;
static std::list<CARDSTASH> faultstack;
/*--------------------------------------------------------------------------*/
void CMD::modify(CS& cmd)
{
  modify_fault(cmd,MODIFY);
}
/*--------------------------------------------------------------------------*/
void CMD::fault(CS& cmd)
{
  modify_fault(cmd,FAULT);
}
/*--------------------------------------------------------------------------*/
static void modify_fault(CS& cmd, WHATTODO command)
{
  SIM::uninit();
  while (cmd.is_alpha()){    
    int mark = cmd.cursor();
    int cmax = cmd.cursor();
    CARD_LIST::fat_iterator ci(&CARD_LIST::card_list);
    for (;;){
      cmd.reset(mark);
      ci = findbranch(cmd, ci);
      cmax = std::max(cmax, cmd.cursor());
      if (ci.isend()){
	break;
      }
      CARD* brh = *ci;
      switch (command) {
      case MODIFY:
	brh->set_value(cmd.ctof(), 0);
	break;
      case FAULT:
	faultbranch(brh, sweep_fix(cmd,brh));
	break;
      }
      ++ci;
    }
    cmd.reset(cmax);
    if (mark == cmax){
      cmd.check(bWARNING, "what's this?");
      cmd.skiparg();
    }
  }
}   
/*--------------------------------------------------------------------------*/
/* sweep_fix: fix the value for sweep command.
 * (find value by interpolation)
 * if not sweeping, return "start" (the first arg).
 */
static double sweep_fix(CS& cmd, const CARD *brh)
{
  double start = cmd.ctof();
  double value = start;
  if (swp_steps[swp_nest] != 0   &&   cmd.is_float()){
    double last = cmd.ctof();
    double offset = static_cast<double>(swp_count[swp_nest]) 
      / static_cast<double>(swp_steps[swp_nest]);
    {if (swp_type[swp_nest]=='L'){
      {if (start == 0.){
	untested();
	error(bERROR, "log sweep can't pass zero\n");
	value = 0;
      }else{
	value = start * pow( (last/start), offset );
      }}
    }else{
      value = start + (last-start) * offset;
    }}
    {if (!crtplot){
      IO::mstdout.setfloatwidth(7)
	<< swp_count[swp_nest]+1 << "> sweep " << brh->long_label()
	<< " =" << value << '\n';
    }else{
      untested();
    }}
  }
  return value;
}
/*--------------------------------------------------------------------------*/
/* faultbranch: "fault" a single branch. (temporarily change a value)
 * save the existing info in "faultlist", then patch
 */
static void faultbranch(CARD* brh, double value)
{
  if (!brh->is_device()){
    untested();
    error(bWARNING, brh->long_label() + ": not a device, can't fault:\n");
  }else if (brh->subckt().exists()){
    untested();
    error(bWARNING, brh->long_label() + " has subckt, can't fault:\n");
  }else{
    faultstack.push_back(CARDSTASH(brh));
    brh->set_value(value, 0);
  }
}
/*--------------------------------------------------------------------------*/
void CMD::restore(CS& cmd)
{
  unfault(cmd);
  unmark(cmd);
}
/*--------------------------------------------------------------------------*/
/* CMD::unfault: (command) remove faults and restore pre-fault values
 */
void CMD::unfault(CS&)
{
  while (!faultstack.empty()){
    faultstack.back().restore();
    faultstack.pop_back();
  }
  SIM::uninit();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
