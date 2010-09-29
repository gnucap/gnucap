/*$Id: u_nodemap.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
#ifndef U_NODEMAP_H
#define U_NODEMAP_H
#include "md.h"
/*--------------------------------------------------------------------------*/
class NODE;
/*--------------------------------------------------------------------------*/
class NODE_MAP {
private:
  std::map<const std::string, NODE*> _node_map;
  explicit  NODE_MAP(const NODE_MAP&);

public:
  explicit  NODE_MAP();
	   ~NODE_MAP();
  NODE*     operator[](const std::string&);
  NODE*     new_node(const std::string&);

  typedef std::map<const std::string, NODE*>::iterator iterator;
  typedef std::map<const std::string, NODE*>::const_iterator const_iterator;

  const_iterator begin()const		{return _node_map.begin();}
  const_iterator end()const		{return _node_map.end();}
  int		 how_many()const	{return _node_map.size()-1;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
