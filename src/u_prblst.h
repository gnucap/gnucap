/*$Id: u_prblst.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * list of probes
 */
#ifndef U_PRBLST_H
#define U_PRBLST_H
#include "mode.h"
#include "u_probe.h"
/*--------------------------------------------------------------------------*/
class CS;
class CKT_BASE;
/*--------------------------------------------------------------------------*/
class PROBELIST {
private:
  typedef std::vector<PROBE> container;
  container bag;
public:
  typedef container::iterator	    iterator;
  typedef container::const_iterator const_iterator;
  void	   listing(const std::string&)const;
  void     clear();
  void	   operator-=(CS&);
  void     operator-=(CKT_BASE*);
  void     operator+=(CS&);
  int	   size()const	{return bag.size();}
  iterator begin()	{return bag.begin();}
  iterator end()	{return bag.end();}
private:
  void     add_node_list(CS&,const std::string&);
  void     add_branches(CS&,const std::string&);
  void     add_all_nodes(const std::string&);
};
/*--------------------------------------------------------------------------*/
class PROBE_LISTS {
public:
 static PROBELIST alarm[sCOUNT]; // list of alarm probes
 static PROBELIST plot[sCOUNT];  // list of plot probes
 static PROBELIST print[sCOUNT]; // list of print probes
 static PROBELIST store[sCOUNT]; // list of probes to store for postproc
 static void purge(CKT_BASE*);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
