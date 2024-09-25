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
//testing=script 2023.12.02
#include "e_cardlist.h"
#include "c_comand.h"
#include "globals.h"
#include "io_dir.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
std::map<std::string, void*> attach_list;
const std::string SUFFIX(".so");
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
    }else{untested();
      error(bTRACE,  ii->first + " (unloaded)\n");
    }
  }
}
/*--------------------------------------------------------------------------*/
void tach_dir(CS&, std::string const&, DIRECTORY const&,
		CARD_LIST const*, int, size_t);
void attach_file(CS&, std::string const& file_name, CARD_LIST const*, int, size_t);
void detach_file(CS&, std::string const& file_name, CARD_LIST const*, size_t);
void load_or_unload(CS& cmd, CARD_LIST const* Scope, int flags)
{
  size_t here = cmd.cursor();
  std::string short_file_name;
  cmd >> short_file_name;
  if (short_file_name == "") {
    // nothing, list what we have
    list();
  }else{
    std::string full_file_name;
    if (short_file_name[0]=='/' || short_file_name[0]=='.'){itested();
      if (OS::access_ok(short_file_name, R_OK)) {itested();
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

    DIRECTORY dir(full_file_name);
    trace2("cmd_attach", full_file_name, dir.exists());

    if(dir.exists()) {itested();
      tach_dir(cmd, full_file_name, dir, Scope, flags, here);
    }else{
      if(flags){
	attach_file(cmd, full_file_name, Scope, flags, here);
      }else{itested();
	detach_file(cmd, full_file_name, Scope, here);
      }
    }
  }
}
/*--------------------------------------------------------------------------*/

class CMD_ATTACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
    size_t here = cmd.cursor();
    int dl_scope = RTLD_LOCAL;
    int check = RTLD_NOW;
    // RTLD_NOW means to resolve symbols on loading
    // RTLD_LOCAL means symbols defined in a plugin are local
    do {
      if (cmd.umatch("public ")) {itested();
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

    load_or_unload(cmd, Scope, check | dl_scope);
  }

  std::string help_text()const override {
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
void tach_dir(CS& cmd, std::string const& dirname, DIRECTORY const& dir,
              CARD_LIST const* Scope, int flags, size_t here)
{itested();
  DIRECTORY::const_iterator i;
  std::vector<std::string> sos;
  for(i=dir.begin(); i!=dir.end(); ++i) {itested();
    std::string fname = i->d_name;
    size_t s = fname.size();
    size_t slen = SUFFIX.size();
    if(s<slen){itested();
    }else if(fname.substr(s-slen, s) == SUFFIX){itested();
      sos.push_back(fname);
    }else{itested();
    }
  }
  std::sort(sos.begin(), sos.end());
  std::vector<std::string>::const_iterator ni;
  error(bLOG, "Plugins in " + dirname + "\n");
  for(ni=sos.begin(); ni!=sos.end(); ++ni) {itested();
    error(bLOG, " .. " + *ni + "\n");
    if(flags){itested();
      attach_file(cmd, dirname + "/" + *ni, Scope, flags, here);
    }else{ itested();
      detach_file(cmd, dirname + "/" + *ni, Scope, here);
    }
  }
}
/*--------------------------------------------------------------------------*/
void attach_file(CS& cmd, std::string const& file_name,
                             CARD_LIST const* Scope, int flags,
                             size_t here)
{
  // a name to look for
  // check if already loaded
  assert(flags);
  if (void* handle = attach_list[file_name]) { untested();
    if (Scope->is_empty()) { untested();
      cmd.warn(bDANGER, here, "\"" + file_name + "\": already loaded, replacing");
      dlclose(handle);
      attach_list[file_name] = nullptr;
    }else{untested();
      cmd.reset(here);
      throw Exception_CS("already loaded, cannot replace when there is a circuit", cmd);
    }
  }else{
  }

  assert(OS::access_ok(file_name, R_OK));

  if (!flags) { untested();
  }else if (void* handle = dlopen(file_name.c_str(), flags)) {
    attach_list[file_name] = handle;
  }else{itested();
    throw Exception_CS(dlerror(), cmd);
  }
}
/*--------------------------------------------------------------------------*/
void detach_file(CS& cmd, std::string const& file_name,
                 CARD_LIST const* Scope, size_t here)
{itested();
  if (Scope->is_empty()) {itested();
    void* handle = attach_list[file_name];
    if (handle) {itested();
      dlclose(handle);
      attach_list[file_name] = nullptr;
    }else{untested();
      cmd.reset(here);
      throw Exception_CS("plugin not attached", cmd);
    }
  }else{untested();
    cmd.reset(here);
    throw Exception_CS("detach prohibited when there is a circuit", cmd);
  }
}
/*--------------------------------------------------------------------------*/
class CMD_DETACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) override {
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
    //BUG// due to the way dlopen and dlclose work
    // it doesn't really work.
    // the dispatcher's active instance blocks unload

    load_or_unload(cmd, Scope, 0);
  }

  std::string help_text()const override {
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
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
    if (Scope->is_empty()) {
      for (std::map<std::string, void*>::iterator
	     ii = attach_list.begin(); ii != attach_list.end(); ++ii) {
	void* handle = ii->second;
	if (handle) {
	  dlclose(handle);
	  ii->second = nullptr;
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
