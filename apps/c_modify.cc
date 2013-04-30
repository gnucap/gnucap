/*$Id: c_modify.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
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
 */
//testing=script,sparse 2006.07.17
#include "e_elemnt.h"
#include "u_cardst.h"
#include "c_comand.h"

extern const int swp_type[];
extern const int swp_count[], swp_steps[];
extern const int swp_nest;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
enum WHATTODO {FAULT, MODIFY};
std::list<CARDSTASH> faultstack;
/*--------------------------------------------------------------------------*/
/* faultbranch: "fault" a single branch. (temporarily change a value)
 * save the existing info in "faultlist", then patch
 */
void faultbranch(CARD* brh, double value)
{
  if (!brh->is_device()) {
    untested();
    error(bWARNING, brh->long_label() + ": not a device, can't fault:\n");
  }else if (brh->subckt()) {
    untested();
    error(bWARNING, brh->long_label() + " has subckt, can't fault:\n");
  }else{
    faultstack.push_back(CARDSTASH(brh));
    ELEMENT* e = prechecked_cast<ELEMENT*>(brh);
    assert(e);
    e->set_value(value, 0);
    //BUG// messy way to do this, loses other attributes
  }
}
/*--------------------------------------------------------------------------*/
/* sweep_fix: fix the value for sweep command.
 * (find value by interpolation)
 * if not sweeping, return "start" (the first arg).
 */
double sweep_fix(CS& cmd, const CARD *brh)
{
  double start = cmd.ctof();
  double value = start;
  if (swp_steps[swp_nest] != 0   &&   cmd.is_float()) {
    untested();
    double last = cmd.ctof();
    double offset = static_cast<double>(swp_count[swp_nest]) 
      / static_cast<double>(swp_steps[swp_nest]);
    if (swp_type[swp_nest]=='L') {
      untested();
      if (start == 0.) {
	untested();
	throw Exception("log sweep can't pass zero");
	value = 0;
      }else{
	untested();
	value = start * pow( (last/start), offset );
      }
    }else{
      untested();
      value = start + (last-start) * offset;
    }
    IO::mstdout.setfloatwidth(7)
      << swp_count[swp_nest]+1 << "> sweep " << brh->long_label()
      << " =" << value << '\n';
  }
  return value;
}
/*--------------------------------------------------------------------------*/
void modify_fault(CS& cmd, WHATTODO command, CARD_LIST* scope)
{
  CKT_BASE::_sim->uninit();
  while (cmd.is_alpha()) {
    unsigned mark = cmd.cursor();
    unsigned cmax = cmd.cursor();
    CARD_LIST::fat_iterator ci(scope, scope->begin());
    for (;;) {
      cmd.reset(mark);
      ci = findbranch(cmd, ci);
      cmax = std::max(cmax, cmd.cursor());
      if (ci.is_end()) {
	break;
      }
      cmd.skip1b('=');
      CARD* brh = *ci;
      switch (command) {
      case MODIFY:{
	ELEMENT* e = prechecked_cast<ELEMENT*>(brh);
	assert(e);
	e->set_value(cmd.ctof(), 0);
	//BUG// messy way to do this, loses other attributes
      } break;
      case FAULT:{
	faultbranch(brh, sweep_fix(cmd,brh));
      }	break;
      }
      cmax = std::max(cmax, cmd.cursor());
      ++ci;
    }
    cmd.reset(cmax);
    if (mark == cmax) {
      untested();
      cmd.check(bWARNING, "what's this?");
      cmd.skiparg();
    }
  }
}   
/*--------------------------------------------------------------------------*/
class CMD_MODIFY : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    modify_fault(cmd, MODIFY, Scope);
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "modify|alter", &p1);
/*--------------------------------------------------------------------------*/
class CMD_FAULT : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)
  {
    modify_fault(cmd, FAULT, Scope);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "fault", &p2);
/*--------------------------------------------------------------------------*/
class CMD_RESTORE : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {untested();
    command("unfault", Scope);
    command("unmark", Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "restore", &p3);
/*--------------------------------------------------------------------------*/
class CMD_UNFAULT : public CMD {
public:
  void do_it(CS&, CARD_LIST*)
  {
    while (!faultstack.empty()) {
      faultstack.back().restore();
      faultstack.pop_back();
    }
    _sim->uninit();
  }
} p4;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "unfault", &p4);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
