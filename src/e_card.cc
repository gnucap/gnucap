/*$Id: e_card.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2006.07.12
#include "e_node.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 4;
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _evaliter(-1),
   _subckt(0),
   _owner(0),
   _n(0),
   _value(0),
   _constant(false)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _evaliter(-1),
   _subckt(0), // BUG:: isn't this supposed to copy????
   _owner(0),
   _n(0),
   _value(p._value),
   _constant(p._constant)
{
}
/*--------------------------------------------------------------------------*/
const std::string CARD::long_label()const
{
  std::string buffer(short_label());
  for (const CARD* brh = owner(); exists(brh); brh = brh->owner()) {
    buffer += '.' + brh->short_label();
  }
  return buffer;
}
/*--------------------------------------------------------------------------*/
/*static*/ double CARD::probe(const CARD *This, const std::string& what)
{
  if (exists(This)) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{
  untested();
  int count = 0;
  if (is_device()) {
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      untested();
      if (node.n_() == _n[ii].n_()) {
        ++count;
      }
    }
  }
  return count;
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::scope()
{
  if (makes_own_scope()) {
    return subckt();		// subckt header, makes its own scope
  }else if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
const CARD_LIST* CARD::scope()const
{
  if (makes_own_scope()) {
    untested();
    return subckt();		// subckt header, makes its own scope
  }else if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception, prints error, if can't find.
 */
CARD* CARD::find_in_my_scope(const std::string& name, int warnlevel)
{
  CARD* c = (*(scope()))[name];
  if (!c) {
    error(warnlevel, long_label() + ": can't find: " + name + " in " 
	  + ((owner()) ? owner()->long_label() : "(root)") + "\n");
  }
  return c;
}
/*--------------------------------------------------------------------------*/
/* find_in_parent_scope: find in parent's scope
 * parent is what my scope is a copy of.
 * capable of finding my parent, who should be just like me.
 * throws exception, prints error, if can't find.
 */
const CARD* CARD::find_in_parent_scope(const std::string& name,
				       int warnlevel)const
{
  const CARD_LIST* my_scope = scope();
  const CARD_LIST* p_scope = my_scope->parent();
  if (!p_scope) {
    p_scope = my_scope;
  }
  const CARD* c = (*p_scope)[name];
  if (!c) {
    error(warnlevel, long_label() + ": can't find: " + name + " in " 
	  + ((owner()) ? owner()->long_label() : "(root)") + "\n");
  }
  return c;
}
/*--------------------------------------------------------------------------*/
/* find_looking_out: find in my or enclosing scope
 * capable of finding me, or anything back to root.
 * throws exception, prints error, if can't find.
 */
const CARD* CARD::find_looking_out(const std::string& name, int warnlevel)const
{
  const CARD* c = find_in_parent_scope(name, bDEBUG);
  if (!c) {
    if (owner()) {
      c = owner()->find_looking_out(name, warnlevel);
    }else{
      error(warnlevel, "can't find: " + name + " in " + long_label() + "\n");
    }
  }
  return c;
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt()
{
  delete _subckt;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
