/*$Id: l_pool.h $ -*- C++ -*-
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
 * pool objects with ordering
 */
#ifndef L_POOL_H
#define L_POOL_H
#include "md.h"
/*--------------------------------------------------------------------------*/
template <class T>
class POOL {
  struct pcompare{
    bool operator()(T const* a, T const* b)const {
      assert(a);
      assert(b);
      return *a < *b;
    }
  };
  std::set<T*, pcompare> _cache;
private:
  POOL(const POOL&) = delete;
public:
  POOL() : _cache() {}
  ~POOL() {
    // assert(_cache.size() <= 5); // BUG: figure depends on plugins loaded
  }
  T* first(){return *_cache.begin();}
  T* second(){return *(++_cache.begin());}

  size_t size()const {return _cache.size();}
  T* operator[](T* x) {
#ifndef NDEBUG
    T* prev = nullptr;
    for(auto o : _cache){
      if(prev){
	assert(*prev < *o);
	assert(*prev != *o);
      }else{
	prev = o;
      }
    }
#endif
    T* ret = *_cache.insert(_cache.end(), x);
    if(size() <= 1){
    }else if(size() <=   2){
    }else if(size() <=   4){
    }else if(size() <=   8){
    }else if(size() <=  16){
    }else if(size() <=  32){
    }else if(size() <=  64){
      // d_coil.ports1.ckt
    }else if(size() <= 128){ untested();
    }else if(size() <= 256){ untested();
    }else if(size() <= 512){ untested();
    }else if(size() <=1024){ untested();
    }else{ untested();
    }
    return ret;
  }
  size_t unlink(T* x) {
    size_t howmany = _cache.erase(x);
#ifndef NDEBUG
    assert(howmany);
    for(auto t : _cache){
      assert(t != x);
    }
#endif
    return howmany;
  }
#ifdef DEBUG_POOL
  // this is expensive..
  void consistency_check()const {
    for(auto r : _cache){
      trace1("pool check", typeid(*r).name());
      bool before_r = true;
      bool past_r = false;
      for(auto i : _cache){
	if(*r==*i){
	  assert(*i==*r);
	  before_r = false;
	  past_r = true;
	}else{
	}
	if(*i < *r) {
	  if(past_r){
	    trace5("POOL clash", r, i, *r < *i, *r == *i, *i < *r);
	    assert(0);
	  }else{
	  }
	}else{
	  if(before_r){
	    trace5("POOL clash", r, i, *r < *i, *r == *i, *i < *r);
	    assert(0);
	  }else{
	  }
	}
      }
    }
  }
#endif
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
