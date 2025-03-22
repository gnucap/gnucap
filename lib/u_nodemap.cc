/*$Id: u_nodemap.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * Copyright (C) 2002 Albert Davis
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
 *------------------------------------------------------------------
 * node name to number mapping -- for named nodes
 */
//testing=script,complete 2006.07.14
#include "e_node.h"
#include "u_nodemap.h"
#include "u_node.h"
/*--------------------------------------------------------------------------*/
NODE ground_node("0", 0);
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
{
  _map = new map_t();
  map()["0"] = &ground_node;
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * replicate number of nodes and their names
 * connectivity will be dealt with elsewhere
 */
NODE_MAP::NODE_MAP(const NODE_MAP& p)
{ untested();
  _map = new map_t(); // additional names
		    // TODO: share/keep exising names
  _nodes.resize(p.size());

  { untested(); // BUG: special treatment for ground node
    map()["0"] = 0;
    assert(_nodes.size());
    _nodes[0] = &ground_node; // BUG. ground is global.
  }
}
/*--------------------------------------------------------------------------*/
NODE_MAP::~NODE_MAP()
{
  if(_map) {
    for (iterator i = map().begin(); i != map().end(); ++i) {
      if (!i->second){
	incomplete();
      }else if (i->second != &ground_node) {
	assert(i->second);
	i->second->purge();
	delete i->second;
      }else{
      }
    }
    delete _map;
    _map = nullptr;
  }else{ untested();
    incomplete();
  }
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * returns nullptr pointer if no match
 */
NODE* NODE_MAP::operator[](std::string s)
{
  iterator i = map().find(s);
  if (i != map().end()) {
    return i->second;
  }else if (OPT::case_insensitive) {
    std::string ls(s);
    notstd::to_lower(&ls);
    i = map().find(ls);
  }else{
    return nullptr;
  }
  return (i != map().end()) ? i->second : nullptr;
}
/*--------------------------------------------------------------------------*/
node_t const& NODE_MAP::at(int i)const
{
  assert(i<int(_nodes.size()));
  return _nodes[i];
}
/*--------------------------------------------------------------------------*/
node_t& NODE_MAP::at(int i)
{
  assert(i<int(_nodes.size()));
  return _nodes[i];
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * creates a new one if it isn't already there.
 */
NODE* NODE_MAP::new_node(std::string s)
{
  if (OPT::case_insensitive) {
    notstd::to_lower(&s);
  }else{
  }
  NODE*& node = map()[s];

  // increments how_many() when lookup fails (new s)  
  if (!node) {
    trace2("MAP::new_node", s, how_many());
    // temporary. may need USER_NODE here eventually.
    node = new USER_NODE(s, how_many());
    //                      ^^^^ is really the map number of the new node
  }else{
  }
  return node;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
