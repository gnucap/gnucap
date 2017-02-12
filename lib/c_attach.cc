/*$Id: c_attach.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
 */
//testing=script 2017.03.12
#include "e_cardlist.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
std::map<const std::string, void*> attach_list;
/*--------------------------------------------------------------------------*/
std::string plug_path()
{
  std::string path = OS::getenv("GNUCAP_PLUGPATH") + ':' + OS::getenv("LD_LIBRARY_PATH") + ':';
  if (FILE* f = popen("gnucap-conf --pkglibdir", "r")) {
    char s[200] = "";	// no gnucap_conf still opens pipe, but nothing in it
    fgets(s, 200, f);	// so fgets leaves whatever is in s there.
    path += trim(s);
  }else{unreachable();
  }
  return path;
}  
/*--------------------------------------------------------------------------*/
class CMD_ATTACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    unsigned here = cmd.cursor();
    int dl_scope = RTLD_LOCAL;
    int check = RTLD_NOW;
    // RTLD_NOW means to resolve symbols on loading
    // RTLD_LOCAL means symbols defined in a plugin are local
    do {
      if (cmd.umatch("public ")) {untested();
	dl_scope = RTLD_GLOBAL;
	// RTLD_GLOBAL means symbols defined in a plugin are global
	// Use this when a plugin depends on another.
      }else if (cmd.umatch("lazy ")) {untested();
	check = RTLD_LAZY;
	// RTLD_LAZY means to defer resolving symbols until needed
	// Use when a plugin will not load because of unresolved symbols,
	// but it may work without it.
      }else{
      }
    } while (cmd.more() && !cmd.stuck(&here));

    std::string short_file_name;
    cmd >> short_file_name;
    
    void* handle = attach_list[short_file_name];
    if (handle) {itested();
      if (CARD_LIST::card_list.is_empty()) {itested();
	cmd.warn(bDANGER, here, "\"" + short_file_name + "\": already loaded, replacing");
	dlclose(handle);
	attach_list[short_file_name] = NULL;
      }else{itested();
	cmd.reset(here);
	throw Exception_CS("already loaded, cannot replace when there is a circuit", cmd);
      }
    }else{
    }

    if (short_file_name.find('/') == std::string::npos) {
      // no '/' in name, search for it
      std::string path = plug_path();
      std::string full_file_name = findfile(short_file_name, path, R_OK);
      if (full_file_name != "") {
	// found it in path
	handle = dlopen(full_file_name.c_str(), check | dl_scope);
      }else{itested();
	cmd.reset(here);
	throw Exception_CS("plugin not found in " + path, cmd);
      }
    }else{itested();
      // has '/' in name, don't search, we have full name
      handle = dlopen(short_file_name.c_str(), check | dl_scope);
    }

    if (handle) {
      attach_list[short_file_name] = handle;
    }else{itested();
      cmd.reset(here);
      throw Exception_CS(dlerror(), cmd);
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "attach|load", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DETACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {untested();
    if (CARD_LIST::card_list.is_empty()) {untested();
      unsigned here = cmd.cursor();
      std::string file_name;
      cmd >> file_name;
      
      void* handle = attach_list[file_name];
      if (handle) {untested();
	dlclose(handle);
	attach_list[file_name] = NULL;
      }else{untested();
	cmd.reset(here);
	throw Exception_CS("plugin not attached", cmd);
      }
    }else{untested();
      throw Exception_CS("detach prohibited when there is a circuit", cmd);
    }
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "detach|unload", &p2);
/*--------------------------------------------------------------------------*/
class CMD_DETACH_ALL : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    if (CARD_LIST::card_list.is_empty()) {
      for (std::map<std::string, void*>::iterator
	     ii = attach_list.begin(); ii != attach_list.end(); ++ii) {
	void* handle = ii->second;
	if (handle) {
	  dlclose(handle);
	  ii->second = NULL;
	}else{untested();
	  // name still in list, but has been detached already
	}
      }
    }else{untested();
      throw Exception_CS("detach prohibited when there is a circuit", cmd);
    }
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "detach_all", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
