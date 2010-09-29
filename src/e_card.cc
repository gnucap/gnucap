/*$Id: e_card.cc,v 22.13 2002/08/01 16:27:25 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
#include "ap.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
//		CARD::CARD();
//		CARD::CARD(const CARD& proto);
//	CARD&	CARD::set_Label(const string& s);
//	CARD&	CARD::parse_label(CS& cmd);
//	CARD&	CARD::parse_Label(CS& cmd);
//	double	CARD::probe(const CARD*,const string&);
// 	int	CARD::connects_to(int node)const;
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 4;
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _evaliter(0),
   _subckt(),
   _owner(0),
   _n(0),
   _value(0),
   _constant(false)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _evaliter(0),
   _subckt(), // BUG:: isn't this supposed to copy????
   _owner(0),
   _n(0),
   _value(p._value),
   _constant(p._constant)
{
}
/*--------------------------------------------------------------------------*/
void CARD::parse_label(CS& cmd)
{
  set_label(cmd.ctos(TOKENTERM));
}
/*--------------------------------------------------------------------------*/
void CARD::parse_Label(CS& cmd)
{
  set_Label(cmd.ctos(TOKENTERM));
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
  {if (exists(This)) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }}					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{
  int count = 0;
  if (is_device()) {
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      if (node == _n[ii]) {
        ++count;
      }
    }
  }
  return count;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
