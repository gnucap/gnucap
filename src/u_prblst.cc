/*$Id: u_prblst.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * probe list functions
 */
//testing=script,sparse 2006.07.14
#include "u_nodemap.h"
#include "e_node.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
extern LOGIC_NODE* nstat;
/*--------------------------------------------------------------------------*/
void PROBE_LISTS::purge(CKT_BASE* brh)
{
  for (int
       i = 0;
       i < sCOUNT;
       ++i) {
    alarm[i].remove_one(brh);
    plot[i] .remove_one(brh);
    print[i].remove_one(brh);
    store[i].remove_one(brh);
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::listing(const std::string& label)const
{
  IO::mstdout.form("%-7s", label.c_str());
  for (const_iterator
       p = bag.begin();
       p != bag.end();
       ++p) {
    IO::mstdout << ' ' << p->label();
    if (p->range() != 0.) {untested();
      IO::mstdout.setfloatwidth(5) 
	<< '(' << p->lo() << ',' << p->hi() << ')';
    }
  }
  IO::mstdout << '\n';
}
/*--------------------------------------------------------------------------*/
void PROBELIST::clear(void)
{
  bag.erase(bag.begin(), bag.end());
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of STL
 */
bool operator==(const PROBE& prb, const std::string& par)
{untested();
  return wmatch(prb.label(), par);
}
#if 0
bool operator!=(const PROBE& prb, const std::string& par)
{untested();
  //return !wmatch(prb.label(), par);
  return !(prb == par);
}
#endif
/*--------------------------------------------------------------------------*/
/* remove a complete probe, extract from CS
 * wild card match  ex:  vds(m*)
 */
void PROBELIST::remove_list(CS& cmd)
{untested(); 
  int mark = cmd.cursor();
  std::string parameter(cmd.ctos(TOKENTERM) + '(');
  int paren = cmd.skip1b('(');
  parameter += cmd.ctos(TOKENTERM) + ')';
  paren -= cmd.skip1b(')');
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }else if (parameter.empty()) {untested();
    cmd.warn(bWARNING, "what's this?");
  }else{untested();
  }

  iterator x = remove(bag.begin(), bag.end(), parameter);
  if (x != bag.end()) {untested();
    bag.erase(x, bag.end());
  }else{untested();
    cmd.warn(bWARNING, mark, "no match");
  }
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of stl
 */
bool operator==(const PROBE& prb, const CKT_BASE* brh)
{
  return (prb.object() == brh);
}
#if 0
bool operator!=(const PROBE& prb, const CKT_BASE* brh)
{untested();
  return (prb.object() != brh);
}
#endif
/*--------------------------------------------------------------------------*/
/* remove a brh from a PROBELIST
 * removes all probes on brh
 */
void PROBELIST::remove_one(CKT_BASE *brh)
{
  assert(brh);
  bag.erase(remove(bag.begin(), bag.end(), brh), bag.end());
}
/*--------------------------------------------------------------------------*/
/* add_list: add a "list" of probes, usually only one
 * This means possibly several probes with a single parameter
 * like "v(r*)" meaning all resistors
 * but not "v(r4) v(r5)" which has two parameters.
 * It also takes care of setting the range for plot or alarm.
 */

void PROBELIST::add_list(CS& cmd)
{
  int oldcount = size();
  std::string what(cmd.ctos(TOKENTERM));/* parameter */
  if (what.empty()) {untested();
    cmd.warn(bWARNING, "need a probe");
  }

  int paren = cmd.skip1b('(');		/* device, node, etc. */
  if (cmd.pmatch("NODES")) {
    // all nodes
    add_all_nodes(what);
  }else if (cmd.ematch("0")) {
    // node 0 means system stuff
    bag.push_back(PROBE(what,0));
  }else if (cmd.is_alnum() || cmd.match1("*?")) {
    // branches or named nodes
    int mark = cmd.cursor();
    bool found_something = add_branches(cmd.ctos(),what,&CARD_LIST::card_list);
    if (!found_something) {
      cmd.warn(bWARNING, mark, "no match");
    }
    for (;;) {
      // a list, as in v(r1,r2,r3) or v(1,2,3)
      if (!(cmd.is_alnum() || cmd.match1("*?"))) {
	break;
      }
      int mark = cmd.cursor();
      found_something = add_branches(cmd.ctos(),what,&CARD_LIST::card_list);
      if (!found_something) {untested();
	cmd.reset(mark);
	break;
      }
    }
  }else{untested();
    cmd.warn(bDANGER, "need device or node");
  }
  paren -= cmd.skip1b(')');
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }

  if (cmd.skip1b('(')) {	/* range for plotting and alarm */
    double lo = cmd.ctof();
    double hi = cmd.ctof();
    for (iterator
	 p = bag.begin() + oldcount;
	 p != bag.end();
	 ++p) {
      p->set_limit(lo,hi);
    }    
    if (!cmd.skip1b(')')) {untested();
      cmd.check(bWARNING, "need )");
    }
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::add_all_nodes(const std::string& what)
{
  for (NODE_MAP::const_iterator
       i = CARD_LIST::card_list.nodes()->begin();
       i != CARD_LIST::card_list.nodes()->end();
       ++i) {
    if (i->first != "0") {
      NODE* node = i->second;
      assert (node);
      bag.push_back(PROBE(what,node));
    }
  }
}
/*--------------------------------------------------------------------------*/
/* add_branches: add net elements to probe list
 * 	all matching a label with wildcards
 */
bool PROBELIST::add_branches(const std::string&device, 
			     const std::string&param,
			     const CARD_LIST* scope)
{
  assert(scope);
  bool found_something = false;

  std::string::size_type dotplace = device.find_last_of(".");
  if (dotplace != std::string::npos) {
    // has a dot, look deeper
    std::string container = device.substr(dotplace+1, std::string::npos);
    std::string dev = device.substr(0, dotplace);
    for (CARD_LIST::const_iterator
	 i = scope->begin();
	 i != scope->end();
	 ++i) {
      CARD* card = *i;
      if (wmatch(card->short_label(), container)) {
	found_something |= add_branches(dev, param, card->subckt());
      }
    }
  }else{
    // no dots, look here
    if (device.find_first_of("*?") != std::string::npos) {
      // there's a wild card.  do linear search for all
      { // nodes
	for (NODE_MAP::const_iterator 
	     i = scope->nodes()->begin();
	     i != scope->nodes()->end();
	     ++i) {
	  if (i->first != "0") {
	    NODE* node = i->second;
	    assert (node);
	    if (wmatch(node->short_label(), device)) {untested();
	      bag.push_back(PROBE(param,node));
	      found_something = true;
	    }
	  }
	}
      }
      {// components
	for (CARD_LIST::const_iterator 
	     i = scope->begin();
	     i != scope->end();
	     ++i) {
	  CARD* card = *i;
	  if (wmatch(card->short_label(), device)) {
	    bag.push_back(PROBE(param,card));
	    found_something = true;
	  }
	}
      }
    }else{
      // no wild card.  do fast search for one
      { // nodes
	NODE* node = (*scope->nodes())[device];
	if (node) {
	  bag.push_back(PROBE(param,node));
	  found_something = true;
	}
      }
      { //components
	CARD* card = const_cast<CARD*>((*scope)[device]);
	if (card) {
	  bag.push_back(PROBE(param,card));
	  found_something = true;
	}
      }
    }
  }

  return found_something;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
