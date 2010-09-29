/*$Id: u_prblst.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * probe list functions
 */
#include "e_card.h"
#include "ap.h"
#include "e_node.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
//	void	PROBE_LISTS::purge(CKT_BASE*);
//		PROBELIST::PROBELIST();
//		PROBELIST::~PROBELIST();
//	PROBE& 	PROBELIST::operator[](int)const;
//	void	PROBELIST::listing(const string&)const;
//	void	PROBELIST::clear(void);
//  	void	PROBELIST::operator-=(CS&);
//  	void	PROBELIST::operator-=(CKT_BASE*);
//	void	PROBELIST::operator+=(CS&);
//	void	PROBELIST::add_all_nodes(const string&);
//	void	PROBELIST::add_node_list(CS&,const string&);
//	void	PROBELIST::add_branches(CS&,const string&);
//	void	PROBELIST::init(void);
/*--------------------------------------------------------------------------*/
extern NODE* nstat;
/*--------------------------------------------------------------------------*/
void PROBE_LISTS::purge(CKT_BASE* brh)
{
  for (int i = 0;  i < sCOUNT; ++i){
    alarm[i] -= brh;
    plot[i]  -= brh;
    print[i] -= brh;
    store[i] -= brh;
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::listing(const std::string& label)const
{
  IO::mstdout.form("%-7s", label.c_str());
  for (const_iterator p=bag.begin(); p!=bag.end(); ++p){
    IO::mstdout << ' ' << p->label();
    if (p->range() != 0.){
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
{
  return wmatch(prb.label(), par);
}
bool operator!=(const PROBE& prb, const std::string& par)
{
  //return !wmatch(prb.label(), par);
  return !(prb == par);
}
/*--------------------------------------------------------------------------*/
/* remove a complete probe, extract from CS
 * wild card match  ex:  vds(m*)
 */
void PROBELIST::operator-=(CS& cmd)
{ 
  int mark = cmd.cursor();
  std::string parameter(cmd.ctos(TOKENTERM) + '(');
  int paren = cmd.skiplparen();
  parameter += cmd.ctos(TOKENTERM) + ')';
  paren -= cmd.skiprparen();
  {if (paren != 0){
    untested();
    cmd.warn(bWARNING, "need )");
  }else if (parameter.empty()){
    untested();
    cmd.warn(bWARNING, "what's this?");
  }}

  iterator x = remove(bag.begin(), bag.end(), parameter);
  {if (x != bag.end()){
    bag.erase(x, bag.end());
  }else{
    untested();
    cmd.warn(bWARNING, mark, "no match");
  }}
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
bool operator!=(const PROBE& prb, const CKT_BASE* brh)
{
  return (prb.object() != brh);
}
/*--------------------------------------------------------------------------*/
/* remove a brh from a PROBELIST
 * removes all probes on brh
 */
void PROBELIST::operator-=(CKT_BASE *brh)
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

void PROBELIST::operator+=(CS& cmd)
{
  int oldcount = size();

  std::string what(cmd.ctos(TOKENTERM));/* parameter */
  if (what.empty()){
    untested();
    cmd.warn(bWARNING, "need a probe");
  }
  int paren = cmd.skiplparen();		/* device, node, etc. */
  {if (cmd.pmatch("NODES")){		/* all nodes */
    add_all_nodes(what);
  }else if (cmd.is_digit()){		/* listed nodes (numbered) */
    add_node_list(cmd,what);
  }else{				/* branches */
    add_branches(cmd,what);
  }}
  paren -= cmd.skiprparen();
  if (paren != 0){
    untested();
    cmd.warn(bWARNING, "need )");
  }

  if (cmd.skiplparen()){		/* range for plotting and alarm */
    double lo = cmd.ctof();
    double hi = cmd.ctof();
    for (iterator p = bag.begin() + oldcount; p!=bag.end(); ++p){
      p->set_limit(lo,hi);
    }    
    if (!cmd.skiprparen()){
      untested();
      cmd.check(bWARNING, "need )");
    }
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::add_all_nodes(const std::string& what)
{
  assert(nstat);
  for (int node = 1;  node <= STATUS::user_nodes;  node++){
    if (nstat[NODE::to_internal(node)].needs_analog()){
      bag.push_back(PROBE(what,node));
    }
  }
}
/*--------------------------------------------------------------------------*/
/* add_node_list: add nodes to probe list
 * 	adds a range or single node
 *	a list of numbers is consumed from cmd
 */
void PROBELIST::add_node_list(CS& cmd, const std::string& what)
{    
  while (cmd.is_digit()){
    int mark = cmd.cursor();
    int node = cmd.ctoi();
    {if (node <= STATUS::total_nodes){
      bag.push_back(PROBE(what,node));
    }else{
      untested();
      cmd.warn(bWARNING, mark, "no match");
    }}
  }
}
/*--------------------------------------------------------------------------*/
/* add_branches: add net elements to probe list
 * 	all matching a label with wildcards
 *	only one argument is consumed from cmd
 */
void PROBELIST::add_branches(CS& cmd, const std::string& what)
{
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
    bag.push_back(PROBE(what,*ci));
    ++ci;
  }
  /* BUG:: stops after finding a match in a subckt.
   * Only matters on a wildcard match.
   * This differs from the old BUG, which would work ok if the X part
   * had the wildcard, but fail if the other parts had one.
   * Old code:  v(r?.x1) found one match, v(r2.x?) found them all.
   * Now: both find one match.  
   * Correct behavior is both find all.
   */
  cmd.reset(cmax);
  if (mark == cmax){
    cmd.check(bWARNING, "no match");
    cmd.skiparg();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
