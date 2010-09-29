/*$Id: e_cardlist.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * "card" container -- list of circuit elements
 */
//testing=script,complete 2006.07.11
#ifndef E_CARDLIST_H
#define E_CARDLIST_H
#include "md.h"
/*--------------------------------------------------------------------------*/
// defined here
class CARD_LIST;
/*--------------------------------------------------------------------------*/
// external
class CARD;
class PARAM_LIST;
class NODE_MAP;
/*--------------------------------------------------------------------------*/
class CARD_LIST {
private:
  const CARD_LIST* _parent;
  mutable NODE_MAP* _nm;
  PARAM_LIST* _params;
  std::list<CARD*> _cl;
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
		  fat_iterator(const fat_iterator& p)
			: _list(p._list), _iter(p._iter) {}
    explicit	  fat_iterator(CARD_LIST* l, iterator i)
			: _list(l), _iter(i) {}
    explicit	  fat_iterator(CARD_LIST* l)
			: _list(l) {_iter=_list->begin();}
    bool	  is_end()const		{return _iter == _list->end();}
    CARD*	  operator*()		{return (is_end()) ? NULL : *_iter;}
    fat_iterator& operator++()	{assert(!is_end()); ++_iter; return *this;}
    fat_iterator  operator++(int)
		{assert(!is_end()); fat_iterator t(*this); ++_iter; return t;}
    //bool	  operator==(const fat_iterator& x)const
    //		{untested(); assert(_list==x._list); return (_iter==x._iter);}
    bool	  operator!=(const fat_iterator& x)const
			{assert(_list==x._list); return (_iter!=x._iter);}
    iterator	  iter()const		{return _iter;}
    CARD_LIST*	  list()const		{return _list;}
    fat_iterator  end()const	{return fat_iterator(_list, _list->end());}

    void	  insert(CARD* c)	{list()->insert(iter(),c);}
  };

  // status queries
  //bool is_empty()const		{untested(); return _cl.empty();}
  const CARD_LIST* parent()const	{return _parent;}

  // return an iterator
  iterator begin()			{return _cl.begin();}
  iterator end()			{return _cl.end();}
  iterator find(const std::string& short_name);
  CARD* operator[](const std::string& short_name);

  // return a const_iterator
  const_iterator begin()const		{return _cl.begin();}
  const_iterator end()const		{return _cl.end();}
  const_iterator find(const std::string& short_name)const;
  const CARD* operator[](const std::string& short_name)const;

  // add to it
  CARD_LIST& push_front(CARD* c)	{_cl.push_front(c); return *this;}
  CARD_LIST& push_back(CARD* c)		{_cl.push_back(c);  return *this;}
  CARD_LIST& insert(CARD_LIST::iterator i, CARD* c)
					{_cl.insert(i, c);  return *this;}

  // take things out
  CARD_LIST& erase(iterator i);
  CARD_LIST& erase(CARD* c);
  CARD_LIST& erase_all();

  // operations on the whole list
  CARD_LIST& set_owner(CARD* owner);
  CARD_LIST& set_slave();
  CARD_LIST& elabo2();
  CARD_LIST& map_nodes();
  CARD_LIST& precalc();
  CARD_LIST& tr_iwant_matrix();
  CARD_LIST& dc_begin();
  CARD_LIST& tr_begin();
  CARD_LIST& tr_restore();
  CARD_LIST& dc_advance();
  CARD_LIST& tr_advance();
  bool	     tr_needs_eval()const;
  CARD_LIST& tr_queue_eval();
  bool	     do_tr();
  CARD_LIST& tr_load();
  DPAIR      tr_review();
  CARD_LIST& tr_accept();
  CARD_LIST& tr_unload();
  CARD_LIST& ac_iwant_matrix();
  CARD_LIST& ac_begin();
  CARD_LIST& do_ac();
  CARD_LIST& ac_load();

  NODE_MAP*   nodes()const {assert(_nm); return _nm;}
  PARAM_LIST* params();
  const PARAM_LIST* params()const;

  // more complex stuff
  void attach_params(PARAM_LIST* p, const CARD_LIST* scope);
  void shallow_copy(const CARD_LIST*);
  void map_subckt_nodes(const CARD* model, const CARD* owner);

  CARD_LIST();
  ~CARD_LIST();
private:
  explicit CARD_LIST(const CARD_LIST&) {unreachable(); incomplete();}
public:
  static CARD_LIST card_list; // in globals.cc
public: // d_subckt, c_getckt
  static CARD_LIST::fat_iterator putbefore;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
