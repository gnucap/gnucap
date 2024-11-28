/*$Id: c_file.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * log and > commands
 * log == commands log to a file
 * >   == all output to a file (redirect stdout)
 * bare command closes the file
 */
//testing=none 2006.07.16
#include "u_lang.h"
#include "c_comand.h"
#include "globals.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
extern OMSTREAM mout;		/* > file bitmap		*/
extern OMSTREAM mlog;		/* log file bitmap		*/
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_INCLUDE : public CMD {
private:
  bool _relative{false};
public:
  explicit CMD_INCLUDE(bool rel=false) : CMD(), _relative(rel) {}
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {
    }else{ untested();
    }
    size_t here = cmd.cursor();
    std::string cwd;
    std::string file_name;
    cmd >> file_name;
    std::string full_file_name;

    if (OS::access_ok(file_name, R_OK)) {
      // prefer local, relative or absolute.
      full_file_name = file_name;
    }else{ untested();
      std::string incl = OS::getenv("GNUCAP_INCLUDEPATH");
      full_file_name = findfile(file_name, incl, R_OK);
    }

    if(_relative){ itested();
      cwd = OS::getcwd();
      std::string dir = OS::dirname(full_file_name);
      full_file_name = OS::basename(full_file_name);
      OS::chdir(dir);
    }else{
    }


    try {
      CS file(CS::_INC_FILE, full_file_name);
      for (;;) {
	if (OPT::language) { untested();
	  OPT::language->parse_top_item(file, Scope);
	}else{ untested();
	  CMD::cmdproc(file.get_line(""), Scope);
	}
      }
    }catch (Exception_File_Open& e) {itested();
      cmd.warn(bDANGER, here, e.message() + '\n');
    }catch (Exception_End_Of_Input& e) { untested();
      // done
    }
    if(_relative){ untested();
      OS::chdir(cwd);
    }else{
    }
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "include", &p0);
CMD_INCLUDE p0_rel(true);
DISPATCHER<CMD>::INSTALL d0_rel(&command_dispatcher, "`include", &p0_rel);
/*--------------------------------------------------------------------------*/
/* cmd_log: "log" command processing
 * open a file for logging (history)
 * arg is name of file
 * no arg closes the one most recently opened
 * the file will contain a list of commands executed, for use by "<"
 * multiple files can be open, they are nested, output to all.
 */
class CMD_LOG : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {itested();
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {untested();
    }else{ untested();
    }
    static std::list<FILE*> filestack;
    
    if (cmd.more()) {			/* a file name .. open it */
      const char *access = "w";
      while (cmd.match1('>')) { untested();
	access = "a";
	cmd.skip();
	cmd.skipbl();
      }
      FILE* newfile = xopen(cmd,"",access);
      if (newfile) { untested();
        filestack.push_back(newfile);
	mlog.attach(newfile);
      }else{ untested();
      }
    }else{				/* empty command -- close a file */
      if (filestack.empty()) { untested();
	error(bWARNING, "no files open\n");
      }else{ untested();
	FILE* oldfile = filestack.back();
	filestack.pop_back();
	mlog.detach(oldfile);
	fclose(oldfile);
      }
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "log|`log", &p1);
/*--------------------------------------------------------------------------*/
/* cmd_file: ">" command processing
 * open a file for all output
 * the file will contain a copy of all screen output.
 * arg is name of file
 * no arg closes it
 * the file will contain all that would go to stdout
 */
class CMD_FILE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {itested();
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) {itested();
    }else{ untested();
    }
    static std::list<FILE*> filestack;
    
    if (cmd.more()) {			/* a file name .. open it */
      const char* access = "w";
      while (cmd.match1('>')) { untested();
	access = "a";
	cmd.skip();
	cmd.skipbl();
      }
      FILE* newfile = xopen(cmd,"",access);
      if (newfile) { untested();
	filestack.push_back(newfile);
	mout.attach(newfile);
	IO::mstdout.attach(newfile);
      }else{ untested();
      }
    }else{				/* empty command -- close a file */
      if (filestack.empty()) { untested();
	error(bWARNING, "no files open\n");
      }else{ untested();
	FILE* oldfile = filestack.back();
	filestack.pop_back();
	mout.detach(oldfile);
	IO::mstdout.detach(oldfile);
	fclose(oldfile);
      }
    }
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, ">", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
