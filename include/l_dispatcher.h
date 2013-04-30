/*$Id: l_dispatcher.h,v 26.134 2009/11/29 03:47:06 al Exp $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 * dispatcher -- for dynamically loaded modules
 */
//testing=informal
#ifndef L_DISPATCHER_H
#define L_DISPATCHER_H
#include "l_stlextra.h"
#include "u_opt.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
template <class T>
class INTERFACE DISPATCHER {
private:
  std::map<std::string, T*> * _map;
public:
  DISPATCHER() {
    if (!_map) {
      _map = new std::map<std::string, T*>;
    }else{
    }
  }

  typedef typename std::map<std::string, T*>::const_iterator const_iterator;
  //class const_iterator : public std::map<std::string, T*>::const_iterator {};

  const_iterator begin()const		{assert(_map); return _map->begin();}
  const_iterator end()const		{assert(_map); return _map->end();}

  void install(const std::string& s, T* p) {
    assert(s.find(',', 0) == std::string::npos);
    if (!_map) {
      _map = new std::map<std::string, T*>;
    }else{
    }
    trace0(s.c_str());
    // loop over all keys, separated by '|'
    for (std::string::size_type			// bss: begin sub-string
	 bss = 0, ess = s.find('|', bss);	// ess: end sub-string
	 bss != std::string::npos;
	 bss = (ess != std::string::npos) ? ess+1 : std::string::npos,
	   ess = s.find('|', bss)) {
      std::string name = s.substr(bss, 
		(ess != std::string::npos) ? ess-bss : std::string::npos);
      trace2(name.c_str(), bss, ess);
      if (name == "") {
	// quietly ignore empty string
      }else if ((*_map)[name]) {
	// duplicate .. stash the old one so we can get it back
	error(bWARNING, name + ": already installed, replacing\n");
	std::string save_name = name + ":0";
	for (int ii = 0; (*_map)[save_name]; ++ii) {
	  save_name = name + ":" + to_string(ii);
	}
	(*_map)[save_name] = (*_map)[name];	
	error(bWARNING, "stashing as " + save_name + "\n");
      }else{
	// it's new, just put it in
      }
      (*_map)[name] = p;
    }
  }
  
  void uninstall(T* p) {
    assert(_map);
    for (typename std::map<std::string, T*>::iterator
	 ii = _map->begin();
	 ii != _map->end();
	 ++ii) {
      if (ii->second == p) {
	ii->second = NULL;
      }else{
      }
    }
#if !defined(NDEBUG)
    for (typename std::map<std::string, T*>::iterator
	 ii = _map->begin();
	 ii != _map->end();
	 ++ii) {
      assert(ii->second != p);
    }
#endif
  }
  
  void uninstall(const std::string& s) {
    assert(_map);
    // loop over all keys, separated by '|'
    for (std::string::size_type			// bss: begin sub-string
	 bss = 0, ess = s.find('|', bss);	// ess: end sub-string
	 bss != std::string::npos;
	 bss = (ess != std::string::npos) ? ess+1 : std::string::npos,
	   ess = s.find('|', bss)) {
      std::string name = s.substr(bss,
		(ess != std::string::npos) ? ess-bss : std::string::npos);
      if (name == "") {
	// quietly ignore empty string
      }else if ((*_map)[name]) {
	// delete, try to get back the old one
	int ii = 0;
	std::string save_name = name + ":0";
	for (ii = 0; (*_map)[save_name]; ++ii) {
	  save_name = name + ":" + to_string(ii);
	}
	if (ii > 1) {
	  save_name = name + ":" + to_string(ii-2);
	  (*_map)[name] = (*_map)[save_name];
	  (*_map)[save_name] = NULL;
	  error(bWARNING, "restoring " + save_name + " as " + name + "\n");
	}else{
	  (*_map)[name] = NULL;
	}
      }else{
	error(bWARNING, name + ": not installed, doing nothing\n");
      }
    }
  }

  T* operator[](std::string s) {
    assert(_map);
    T* rv = (*_map)[s];
    if (!rv && OPT::case_insensitive) {
      notstd::to_lower(&s);
      rv = (*_map)[s];
    }else{
    }
    return rv;
  }

  T* operator[](CS& cmd) {
    unsigned here = cmd.cursor();
    std::string s;
    cmd >> s;
    //------------------------
    T* p = (*this)[s];
    //------------------------
    if (!p) {
      cmd.reset(here);
    }else{
    }
    return p;
  }

  T* clone(std::string s) {
    T* proto = (*this)[s];
    if (proto) {
      return proto->clone();
    }else{itested();
      return NULL;
    }
  }

  class INSTALL {
  private:
    const std::string _name;
    DISPATCHER<T>* _d;
    T* _p;
  public:
    INSTALL(DISPATCHER<T>* d, const std::string& name, T* p) :
      _name(name),
      _d(d),
      _p(p)
    {
      assert(_d);
      assert(p);
      _d->install(_name, p);
    }
    
    ~INSTALL() {
      //_d->uninstall(_name);
      _d->uninstall(_p);
    }
  };
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
