/*$Id: u_nodemap.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2002 Albert Davis
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
 * node name to number mapping -- for named nodes
 */
//testing=script,complete 2006.07.14
#include "e_node.h"
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
NODE ground_node("0",0);
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
  : _node_map()
{
  _node_map["0"] = &ground_node;
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * The std::map copy constructor does a shallow copy,
 * then replace second with a deep copy.
 */
NODE_MAP::NODE_MAP(const NODE_MAP& p)
  : _node_map(p._node_map)
{
  unreachable();
  for (iterator i = _node_map.begin(); i != _node_map.end(); ++i) {
    untested();
    if (i->first != "0") {
      untested();
      assert(i->second);
      i->second = new NODE(i->second);
    }else{
      untested();
    }
  }
}
/*--------------------------------------------------------------------------*/
NODE_MAP::~NODE_MAP()
{
  for (iterator i = _node_map.begin(); i != _node_map.end(); ++i) {
    if (i->first != "0") {
      assert(i->second);
      delete i->second;
    }
  }  
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * returns NULL pointer if no match
 */
NODE* NODE_MAP::operator[](const std::string& s)
{
  const_iterator i = _node_map.find(s);
  return (i != _node_map.end()) ? i->second : NULL;
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * creates a new one if it isn't already there.
 */
NODE* NODE_MAP::new_node(const std::string& s)
{
  NODE* node = _node_map[s];
  // increments how_many() when lookup fails (new s)  
  if (!node) {
    node = new NODE(s, how_many());
    _node_map[s] = node;
  }
  assert(node);
  return node;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
