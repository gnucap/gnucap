/*$Id: e_cardlist.h,v 22.17 2002/08/26 04:30:28 al Exp $ -*- C++ -*-
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
 * "card" container -- list of circuit elements
 */
#ifndef E_CARDLIST_H
#define E_CARDLIST_H
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
// defined here
class CARD_LIST;
/*--------------------------------------------------------------------------*/
// external
class CARD;
/*--------------------------------------------------------------------------*/
class CARD_LIST {
private:
  std::list<CARD*> _cl;
public:
  NODE_MAP _nm;
public:
  // internal types
  typedef std::list<CARD*>::iterator iterator;
  typedef std::list<CARD*>::const_iterator const_iterator;
  class fat_iterator {
  private:
    CARD_LIST* _list;
    iterator   _iter;
  private:
    explicit	  fat_iterator()	{unreachable();}
  public:
    explicit	  fat_iterator(CARD_LIST* l, iterator i): _list(l), _iter(i) {}
    explicit	  fat_iterator(CARD_LIST* l): _list(l) {_iter=_list->begin();}
    bool	  isend()const		{return _iter == _list->end();}
    CARD*	  operator*()		{assert(!isend()); return *_iter;}
    fat_iterator& operator++()	{assert(!isend()); ++_iter; return *this;}
    fat_iterator  operator++(int)
		{assert(!isend()); fat_iterator t(*this); ++_iter; return t;}
    bool	  operator==(const fat_iterator& x)const
			{assert(_list==x._list); return (_iter==x._iter);}
    bool	  operator!=(const fat_iterator& x)const
			{assert(_list==x._list); return (_iter!=x._iter);}
    iterator	  iter()const		{return _iter;}
    CARD_LIST*	  list()const		{return _list;}
    fat_iterator  end()const	{return fat_iterator(_list, _list->end());}
    
  };

  // status queries
  bool empty()const			{return _cl.empty();}
  bool exists()const			{return !empty();}

  // return an iterator
  iterator begin()			{return _cl.begin();}
  iterator end()			{return _cl.end();}
  iterator find(const std::string& short_name);

  // return a const_iterator
  const_iterator begin()const		{return _cl.begin();}
  const_iterator end()const		{return _cl.end();}
  const_iterator find(const std::string& short_name)const;

  // add to it
  CARD_LIST& push_front(CARD* c)	{_cl.push_front(c); return *this;}
  CARD_LIST& push_back(CARD* c)		{_cl.push_back(c);  return *this;}
  CARD_LIST& insert(CARD_LIST::iterator i, CARD* c)
					{_cl.insert(i, c);  return *this;}
  CARD_LIST& insert(CARD_LIST::fat_iterator i, CARD* c)
	{assert(i.list()==this); _cl.insert(i.iter(),c); return *this;}

  // take things out
  CARD_LIST& erase(iterator i);
  CARD_LIST& erase(fat_iterator i)	{return erase(i.iter());}
  CARD_LIST& erase(CARD* c);

  // operations on the whole list
  CARD_LIST& set_slave();
  CARD_LIST& expand();
  CARD_LIST& map_nodes();
  CARD_LIST& precalc();
  CARD_LIST& tr_alloc_matrix();
  CARD_LIST& dc_begin();
  CARD_LIST& tr_begin();
  CARD_LIST& tr_restore();
  CARD_LIST& dc_advance();
  CARD_LIST& tr_advance();
  bool	     tr_needs_eval();
  CARD_LIST& tr_queue_eval();
  bool	     do_tr();
  CARD_LIST& tr_load();
  double     tr_review();
  CARD_LIST& tr_accept();
  CARD_LIST& tr_unload();
  CARD_LIST& ac_alloc_matrix();
  CARD_LIST& ac_begin();
  CARD_LIST& do_ac();
  CARD_LIST& ac_load();
  CARD_LIST& destroy();

  ~CARD_LIST()				{destroy();}
public:
  static CARD_LIST card_list;
public: // d_subckt, c_getckt
  static CARD_LIST::fat_iterator putbefore;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
