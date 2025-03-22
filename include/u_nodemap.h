/*$Id: u_nodemap.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
#ifndef U_NODEMAP_H
#define U_NODEMAP_H
#include "md.h"
/*--------------------------------------------------------------------------*/
class NODE;
class node_t;
/*--------------------------------------------------------------------------*/
extern NODE ground_node;
/*--------------------------------------------------------------------------*/
class NODE_MAP {
  typedef std::map<std::string, NODE*> map_t;
  typedef std::vector<node_t> vector_t;
public:
  typedef map_t::iterator iterator;
  typedef map_t::const_iterator const_iterator;
private:
  map_t* _map{nullptr};
  vector_t _nodes;

public:
  explicit  NODE_MAP(const NODE_MAP&);

public:
  explicit  NODE_MAP();
	   ~NODE_MAP();
  NODE*     operator[](std::string);
  node_t    const& at(int i)const;
  node_t&          at(int i);
  node_t    const& operator[](int i)const { untested();return at(i);}
  node_t&          operator[](int i) {return at(i);}

  NODE*     new_node(std::string);

  const_iterator begin()const		{return map().begin();}
  const_iterator end()const		{return map().end();}
  int		 how_many()const	{return static_cast<int>(map().size()-1);}
  int		 size()const		{untested(); return static_cast<int>(map().size());}

private:
  map_t& map() {assert(_map); return *_map;}
  map_t const& map() const {assert(_map); return *_map;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
