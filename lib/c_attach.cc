/*$Id: c_attach.cc $ -*- C++ -*-
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
//testing=script 2017.06.22
#include "e_cardlist.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
std::map<std::string, void*> attach_list;
/*--------------------------------------------------------------------------*/
std::string plug_path()
{
  return OS::getenv("GNUCAP_PLUGPATH");
}  
/*--------------------------------------------------------------------------*/
void list()
{
  for (std::map<std::string, void*>::iterator
	 ii = attach_list.begin(); ii != attach_list.end(); ++ii) {
    if (ii->second) {
      IO::mstdout << ii->first << '\n';
    }else{itested();
      error(bTRACE,  ii->first + " (unloaded)\n");
    }
  }
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
    
    if (short_file_name == "") {
      // nothing, list what we have
      list();
    }else{
      // a name to look for
      // check if already loaded
      if (void* handle = attach_list[short_file_name]) {itested();
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
      
      std::string full_file_name;
      if (short_file_name[0]=='/' || short_file_name[0]=='.'){untested();
	if (OS::access_ok(short_file_name, R_OK)) {untested();
	  // found it, local or root
	  full_file_name = short_file_name;
	}else{untested();
	  cmd.reset(here);
	  throw Exception_CS(std::string("plugin not found in ") + short_file_name[0], cmd);
	}
      }else{
	std::string path = plug_path();
	full_file_name = findfile(short_file_name, path, R_OK);
	if (full_file_name != "") {
	  // found it, with search
	}else{untested();
	  cmd.reset(here);
	  throw Exception_CS("plugin not found in " + path, cmd);
	}
      }
	  
      assert(OS::access_ok(full_file_name, R_OK));

      if (void* handle = dlopen(full_file_name.c_str(), check | dl_scope)) {
	attach_list[short_file_name] = handle;
      }else{untested();
	cmd.reset(here);
	throw Exception_CS(dlerror(), cmd);
      }
    }
  }

  std::string help_text()const
  {
    return 
      "load command\n"
      "Loads plugins\n"
      "Syntax: load plugin\n"
      "Plugin search path is: " + plug_path() + " \n"
      "Path is set by GNUCAP_PLUGPATH environment variable\n"
      "With no arg, it lists plugins already loaded\n\n";
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "attach|load", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DETACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    unsigned here = cmd.cursor();	//BUG// due to the way dlopen and dlclose work
    std::string file_name;		// it doesn't really work.
    cmd >> file_name;			// the dispatcher's active instance blocks unload
    
    if (file_name == "") {
      // nothing, list what we have
      list();
    }else{untested();
      if (CARD_LIST::card_list.is_empty()) {untested();
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
  }

  std::string help_text()const
  {
    return 
      "unload command\n"
      "Unloads plugins\n"
      "Syntax: unload plugin\n"
      "The name must match the name you loaded it with.\n"
      "Prohibited when there is a circuit\n"
      "With no arg, it lists plugins already loaded\n\n";
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
	}else{itested();
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
