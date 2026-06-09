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
#include "u_nodemap.h"
#include "e_usernode.h"
#include "e_node_type.h"
/*--------------------------------------------------------------------------*/
class GROUND_NODE : public NODE {
public:
  explicit GROUND_NODE() : NODE("0", 0) {}
} ground_node;
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
{
  _map = new map_t;
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * replicate number of nodes and their names
 */
NODE_MAP::NODE_MAP(const NODE_MAP& p)
{
  _map = new map_t(); // additional names
		    // TODO: share/keep exising names
  _nodes.resize(p.size());

  for(auto const& i : p) {
    assert(i.second);
    int idx = i.second->user_number();
    // copy index, and possibly link to global node or ground
    _nodes[idx] = i.second->n_(0);
    trace3("NODE_MAP::NODE_MAP1", idx, i.first, i.second->n_(0).n_());
#if 0
    if(_nodes[idx].link()){
      // global node, ground
    }else{
      // allocate one, later.
    }
#endif
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
  assert(_map);
  const_iterator i = map().find(s);
  if (i != map().end()) {
    return i->second;
  }else if (OPT::case_insensitive) {
    notstd::to_lower(&s);
    i = _map->find(s);
  }else{
    return nullptr;
  }
  return (i != _map->end()) ? i->second : nullptr;
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
  assert(_map);
  NODE*& node = map()[s];

  // increments how_many() when lookup fails (new s)░░
  if (!node) {
    node = new USER_NODE(s, size());
    //                      ^^^^ is really the map number of the new node
    node->set_owner(nullptr); // here?
    _nodes.push_back(node_t());
    assert(node->user_number() == size()-1);
  }else{
  }
  assert(node);
  assert(_map->size() == _nodes.size());
  return node;
}
/*--------------------------------------------------------------------------*/
std::string const& NODE_MAP::name(int i) const
{
  assert(i<size());
  static std::string dunno("??????");
  node_t const& n = at(i);
  if(n.n_() && n->short_label() != "" && !dynamic_cast<NODE_TYPE const*>(n.n_())) {
    std::string const& l = n->short_label();
    assert(!_map || _map->at(l)->user_number() == i);
    trace2("NODE_MAP::name", i, l);
    return l;
  }else if(_map){
    // fallback, getting here in a few corner cases,
    // e.g. d_subckt.error4.ckt
    if(dynamic_cast<NODE_TYPE const*>(n.n_())) {
      // top level
    }else{
      // module declaration
    }
    for(auto const& p : map()){
      assert(p.second);
      if(p.second->user_number() == i){
	return p.first;
      }else{
      }
    }
    unreachable();
    return dunno;
  }else{ untested();
    unreachable();
    return dunno;
  }
}
/*--------------------------------------------------------------------------*/
// resurrect string->node map after allocation.
// nodes are _nodes[idx].n_()
void NODE_MAP::map_nodes()
{
  if(_map) {
    for(auto& p : *_map){
      assert(p.second);
      NODE* n = p.second;
      n->map_nodes();
      continue;
    }
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
