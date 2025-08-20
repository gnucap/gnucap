/*                             -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * Small set. Embed a few, fallback to std::set
 * T is an integer type, and 0 is not valid.
 * -------------------------------------------------
 */
#include <set>
/*--------------------------------------------------------------------------*/
template<class T, int small_size=4>
class SMALL_SET {
  typedef std::set<T> set;
  typedef typename set::const_iterator siter;
  union{
    T _i[small_size];
    set* _p[2];
  };
public:
  class const_iterator {
    T const* _it{nullptr};
    typename std::set<T>::const_iterator _iter;
  public:
    explicit const_iterator(T const* b) : _it(b) { };
    explicit const_iterator(siter const& s) : _iter(s) {}
    const_iterator(const_iterator const&) = default;

    T operator*()const {
      if(_it){
	return *_it;
      }else{
	return *_iter;
      }
    }
    bool operator==(const_iterator const& p)const {
      if(_it){
	return _it == p._it;
      }else{
	return _iter == p._iter;
      }
    }
    bool operator!=(const_iterator const& p)const {
      return !operator==(p);
    }
    const_iterator& operator++() {
      if(_it){
	++_it;
      }else{
	++_iter;
      }
      return *this;
    }
  };

  explicit SMALL_SET() {
    for(int i = 0; i<small_size; ++i){
      _i[i] = 0;
    }
    _p[1] = nullptr;
  }
  SMALL_SET (SMALL_SET  const &p) {
    for(int i = 0; i<small_size; ++i){
      _i[i] = p._i[i];
    }
    if(p._i[0]){
    }else if(p._p[1]){
      _p[1] = new set(*p._p[1]);
    }else{
      _p[1] = nullptr;
    }
  }

  ~SMALL_SET () {
    if(_i[0]){
    }else{
      delete _p[1];
      _p[1] = nullptr;
    }
  }

  int size()const {
    if(_i[0]){
      int i = 1;
      while(i<small_size && _i[i]) {
	++i;
      }
      return i;
    }else if(_p[1]){
      return int(_p[1]->size());
    }else{
      return 0;
    }
  }

  bool insert(int x) {
    assert(x); // 0 is not allowed, not needed.
    trace2("SSO::insert", x, size());
#ifndef NDEBUG
    int s = size();
#endif
    if (is_small()) {
      assert(size()<=4);
      int spot = 0;
      for(; spot<4 && _i[spot] ; ++spot){
	if(_i[spot] < x){
	  // go on.
	}else if(_i[spot] == x){
	  x = 0; // absorbed.
	  break;
	}else{
	  std::swap(_i[spot], x);
	}
      }
      if(!x){
      }else if(spot < 4) {
	_i[spot] = x;
      }else{
	auto ns = new set(_i, _i+4);
	ns->insert(x);
	_i[0] = _i[1] = 0;
	_p[1] = ns;
      }
    }else{
      assert(size()>4);
      assert(!_i[0]);
      assert(!_i[1]);
      assert(_p[1]);
      x = _p[1]->insert(x).second;
    }

    if(x){
      assert(size() == s+1);
    }else{
      assert(size() == s);
    }
    return x;
  }

  const_iterator begin()const {
    if(is_small()) {
      return const_iterator(_i);
    }else{
      assert(_p[1]);
      return const_iterator(_p[1]->begin());
    }
  }
  const_iterator end()const {
    if(is_small()) {
      int s = size(); // TODO: avoid
      return const_iterator(_i+s);
    }else{
      assert(_p[1]);
      return const_iterator(_p[1]->end());
    }
  }
  int count(int x) const{
    if(is_small()){
      for(auto y : *this){
	if(x == y){
	  return 1;
	}else{
	}
      }
      return 0;
    }else{
      return bool(_p[1]->count(x));
    }
  }
private:
  bool is_small()const {
    return _i[0] || !_p[1];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
