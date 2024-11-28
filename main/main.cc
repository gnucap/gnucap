/*$Id: main.cc  $ -*- C++ -*-
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
 * top level module
 * it all starts here
 */
//testing=script 2006.07.14
#include "config.h"
#include "globals.h"
#include "u_prblst.h"
#include "u_sim_data.h"
#include "e_cardlist.h"
#include "u_lang.h"
#include "ap.h"
#include "patchlev.h"
#include "c_comand.h"
#include "declare.h"	/* plclose */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct JMP_BUF{
  sigjmp_buf p;
} env;
/*--------------------------------------------------------------------------*/
static void sign_on(void)
{
  IO::mstdout <<
    "Gnucap : The Gnu Circuit Analysis Package\n"
    "Never trust any version less than 1.0\n"
    "Copyright 1982-2013, Albert Davis\n"
    "Gnucap comes with ABSOLUTELY NO WARRANTY\n"
    "This is free software, and you are welcome\n"
    "to redistribute it under the terms of \n"
    "the GNU General Public License, version 3 or later.\n"
    "See the file \"COPYING\" for details.\n"
    "main version: " PATCHLEVEL "\n"
    "core-lib version: " << lib_version() << "\n";  
}
/*--------------------------------------------------------------------------*/
static void prepare_env()
{
  static const char* plugpath="PLUGPATH=" GNUCAP_PLUGPATH
                              "\0         (reserved space)                 ";

  std::string ldlpath = OS::getenv("LD_LIBRARY_PATH");
  if (ldlpath != "") {
    ldlpath += ":";
  }else{
  }
  assert(strlen("PLUGPATH=") == 9);
  OS::setenv("GNUCAP_PLUGPATH", ldlpath + (plugpath+9), false);
}
/*--------------------------------------------------------------------------*/
static void read_startup_files(void)
{
  {
    std::string name = findfile(SYSTEMSTARTFILE, SYSTEMSTARTPATH, R_OK);
    if (name != "") {untested();
      CMD::command("include " + name, &CARD_LIST::card_list);
    }else{
      CMD::command(std::string("load " DEFAULT_PLUGINS), &CARD_LIST::card_list);
    }
  }
  {
    std::string name = findfile(USERSTARTFILE, USERSTARTPATH, R_OK);
    if (name != "") {untested();
      CMD::command("include " + name, &CARD_LIST::card_list);
    }else{
    }
  }
  //CMD::command("clear", &CARD_LIST::card_list);
  if (language_dispatcher.empty()) { untested();
    // go on without language.
  }else if (!OPT::language) {
    OPT::language = language_dispatcher[DEFAULT_LANGUAGE];
    
    for(DISPATCHER<LANGUAGE>::const_iterator
	  i=language_dispatcher.begin(); !OPT::language && i!=language_dispatcher.end(); ++i) {untested();
      OPT::language = prechecked_cast<LANGUAGE*>(i->second);
    }
  }else{untested();
    // already have a language specified in a startup file
  }
  if (OPT::language) {
    OPT::case_insensitive = OPT::language->case_insensitive();
    OPT::units            = OPT::language->units();
  }else{ untested();
    OPT::case_insensitive = false;
    OPT::units            = uSI;
  }
}
/*--------------------------------------------------------------------------*/
/* sig_abrt: trap asserts
 */
#if 0
extern "C" {
  static void sig_abrt(SIGNALARGS)
  {untested();
    signal(SIGABRT,sig_abrt);
    static int count = 100;
    if (--count > 0 && ENV::run_mode != rBATCH) {untested();
      IO::error << '\n';
      siglongjmp(env.p,1);
    }else{untested();
      exit(1);
    }
  }
}
#endif
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {itested();
    signal(SIGINT,sig_int);
    if (ENV::run_mode == rBATCH) {itested();
      exit(1);
    }else{itested();
      IO::error << '\n';
      siglongjmp(env.p,1);
    }
  }
}
/*--------------------------------------------------------------------------*/
extern "C" {
  static void sig_fpe(SIGNALARGS)
  {untested();
    signal(SIGFPE,sig_fpe);
    error(bDANGER, "floating point error\n");
  }
}
/*--------------------------------------------------------------------------*/
static void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
  //signal(SIGABRT,sig_abrt);
}
/*--------------------------------------------------------------------------*/
/* finish: clean up after a command
 * deallocates space, closes plot windows, resets i/o redirection, etc.
 * This is done separately for exception handling.
 * If a command aborts, clean-up is still done, leaving a consistent state.
 * //BUG// It is a function to call as a remnant of old C code.
 * Should be in a destructor, so it doesn't need to be explicitly called.
 */
static void finish(void)
{
  plclose();
  outreset();
}
/*--------------------------------------------------------------------------*/
static void process_cmd_line(int argc, const char *argv[])
{
  for (int ii = 1;  ii < argc;  /*inside*/) {
    try {
      if (strncmp(argv[ii], "--", 2) == 0) { untested();
	if (ii < argc) { untested();
	  CS cmd(CS::_STRING, argv[ii++]+2); // command line
	  CMD::cmdproc(cmd, &CARD_LIST::card_list); 
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-c") == 0) { untested();
	++ii;
	if (ii < argc) { untested();
	  CS cmd(CS::_STRING, argv[ii++]); // command line
	  CMD::cmdproc(cmd, &CARD_LIST::card_list); 
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-i") == 0) {itested();
	++ii;
	if (ii < argc) {untested();
	  CMD::command(std::string("`include ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-b") == 0) {
	try {
	  ++ii;
	  if (ii < argc) {
	    CMD::command(std::string("< ") + argv[ii++], &CARD_LIST::card_list);
	  }else{untested();
	    CMD::command(std::string("< /dev/stdin"), &CARD_LIST::card_list);
	  }
	}catch (Exception& e) {
	  error(bDANGER, e.message() + '\n');
	  finish();
	}
	if (ii >= argc) {
	  //CMD::command("end", &CARD_LIST::card_list);
	  throw Exception_Quit("");
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-a") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CMD::command(std::string("attach ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else{
	try {
	  CMD::command(std::string("`include ") + argv[ii++], &CARD_LIST::card_list);
	}catch (Exception& e) {
	  error(bDANGER, e.message() + '\n');
	  finish();
	}
	if (ii >= argc) {
	  //CMD::command("end", &CARD_LIST::card_list);
	  throw Exception_Quit("");
	}else{untested();
	}	
      }
    }catch (Exception_Quit& e) {
      throw;
    }catch (Exception& e) {itested();
      // abort command, continue loop
      error(bDANGER, e.message() + '\n');
      finish();
    }
  }
}
/*--------------------------------------------------------------------------*/
int main(int argc, const char *argv[])
{
  prepare_env();
  CKT_BASE::_sim = new SIM_DATA;
  CKT_BASE::_probe_lists = new PROBE_LISTS;
  try {
  {
    SET_RUN_MODE xx(rBATCH);
    sign_on();
    if (!sigsetjmp(env.p, true)) {
#if 0
      try {untested();
#endif
	read_startup_files();
	setup_traps();
	process_cmd_line(argc,argv);
#if 0
      }catch (Exception& e) {untested();
	error(bDANGER, e.message() + '\n');
	finish();		/* error clean up (from longjmp()) */
	//CMD::command("quit", &CARD_LIST::card_list);
	unreachable();
	exit(0);
      }
#endif
    }else{
      finish();		/* error clean up (from longjmp()) */
      //CMD::command("quit", &CARD_LIST::card_list);
      exit(0);
    }
  }
  {
    SET_RUN_MODE xx(rINTERACTIVE);
    CS cmd(CS::_STDIN);
    for (;;) {
      if (!sigsetjmp(env.p, true)) {
	try {
	  if (OPT::language) {
	    OPT::language->parse_top_item(cmd, &CARD_LIST::card_list);
	  }else{untested();
	    CMD::cmdproc(cmd.get_line(I_PROMPT), &CARD_LIST::card_list);
	  }
	}catch (Exception_End_Of_Input& e) {
	  if(e.message().size()) { untested();
	    error(bDANGER, e.message() + '\n');
	  }else {
	  }
	  finish();
	  //CMD::command("quit", &CARD_LIST::card_list);
	  //exit(0);
	  break;
	}catch (Exception& e) {
	  error(bDANGER, e.message() + '\n');
	  finish();
	}
      }else{itested();
	finish();		/* error clean up (from longjmp()) */
      }
    }
  }
  }catch (Exception_Quit&) {
  }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
  }
  
  //CARD_LIST::card_list.erase_all();
  CMD::command("clear", &CARD_LIST::card_list);
  assert(CARD_LIST::card_list.is_empty());
  CMD::command("detach_all", &CARD_LIST::card_list);
  delete CKT_BASE::_probe_lists;
  CKT_BASE::_probe_lists = nullptr;
  delete CKT_BASE::_sim;
  CKT_BASE::_sim = nullptr;
  
  return 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
