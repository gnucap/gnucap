/*                     -*- C++ -*-
 * Copyright (C) 2026 Felix Salfelder
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
 * events
 */
#ifndef U_EVENT_H
#define U_EVENT_H
/*--------------------------------------------------------------------------*/
#include "constant.h" // NEVER
/*--------------------------------------------------------------------------*/
// external
class WAVE;
class CARD;
class CARD_LIST;
class LOGIC_NODE;
class CKT_BASE;
/*--------------------------------------------------------------------------*/
class EVENT {
  double    _time  {NEVER};
  CARD* _owner {nullptr};
  EVENT() = delete;
public:
  EVENT(double Time, CARD* Owner)
    : _time(Time), _owner(Owner) {}
  EVENT(const EVENT& E)
    : _time(E._time), _owner(E._owner) {}
  ~EVENT() {}
  operator double() const {return _time;}
  double time() const { return _time;}
  CARD* owner() const { assert(_owner); return _owner;}
  bool operator<(EVENT const& o)const {
    if(_time < o._time) {
      return true;
    }else if(_time == o._time) {
     return _owner < o._owner;
    }else{
      return false;
    }
  }
  bool operator>=(double const& t)const { untested();
    return _time >= t;
  }
  struct greater{
    bool operator()(EVENT const& a, EVENT const& b)const {
      return b < a;
    }
  };
};
/*--------------------------------------------------------------------------*/
class EVENT_QUEUE : private std::priority_queue<EVENT, std::deque<EVENT>, EVENT::greater > {
 typedef std::priority_queue<EVENT, std::deque<EVENT>, EVENT::greater > base;
public:
  explicit EVENT_QUEUE() {}
  ~EVENT_QUEUE() {}
public:
  void push(double Time, CARD* Owner) { base::push(EVENT(Time, Owner)); }
  bool empty()const {return base::empty();}
  EVENT const& top() {return base::top();}
  void pop() {base::pop();}
  void clear() {while(!base::empty()){base::pop();}}
public:
  void tr_advance_recursive();
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
