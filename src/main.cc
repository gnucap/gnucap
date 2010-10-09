/*$Id: main.cc,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
#include "e_cardlist.h"
#include "u_lang.h"
#include "ap.h"
#include "patchlev.h"
#include "c_comand.h"
#include "declare.h"	/* plclose */
/*--------------------------------------------------------------------------*/
struct JMP_BUF{
  sigjmp_buf p;
} env;
/*--------------------------------------------------------------------------*/
static void sign_on(void)
{
  IO::mstdout <<
    "Gnucap "  PATCHLEVEL  "\n"
    "The Gnu Circuit Analysis Package\n"
    "Never trust any version less than 1.0\n"
    "Copyright 1982-2009, Albert Davis\n"
    "Gnucap comes with ABSOLUTELY NO WARRANTY\n"
    "This is free software, and you are welcome\n"
    "to redistribute it under the terms of \n"
    "the GNU General Public License, version 3 or later.\n"
    "See the file \"COPYING\" for details.\n";
}
/*--------------------------------------------------------------------------*/
static void read_startup_files(void)
{
  std::string name = findfile(SYSTEMSTARTFILE, SYSTEMSTARTPATH, R_OK);
  if (name != "") {untested();
    CMD::command("get " + name, &CARD_LIST::card_list);
  }else{
  }
  name = findfile(USERSTARTFILE, USERSTARTPATH, R_OK);
  if (name != "") {untested();
    CMD::command("get " + name, &CARD_LIST::card_list);
  }else{
  }
  CMD::command("clear", &CARD_LIST::card_list);
  if (!OPT::language) {
    CMD::command(std::string("options lang=") + DEFAULT_LANGUAGE, &CARD_LIST::card_list);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* sig_abrt: trap asserts
 */
extern "C" {
  static void sig_abrt(SIGNALARGS)
  {itested();
    signal(SIGINT,sig_abrt);
    static int count = 10;
    if (--count > 0) {itested();
      error(bDANGER, "\n");
    }else{untested();
      exit(1);
    }
  }
}
/*--------------------------------------------------------------------------*/
/* sig_int: what to do on receipt of interrupt signal (SIGINT)
 * cancel batch files, then back to command mode.
 * (actually, control-c trap)
 */
extern "C" {
  static void sig_int(SIGNALARGS)
  {
    signal(SIGINT,sig_int);
    if (ENV::run_mode == rBATCH) {untested();
      exit(1);
    }else{
      IO::error << '\n';
      siglongjmp(env.p,1);
    }
  }
}
/*--------------------------------------------------------------------------*/
extern "C" {
  static void sig_fpe(SIGNALARGS)
  {
    untested();
    signal(SIGFPE,sig_fpe);
    error(bDANGER, "floating point error\n");
  }
}
/*--------------------------------------------------------------------------*/
static void setup_traps(void)
{
  signal(SIGFPE,sig_fpe);
  signal(SIGINT,sig_int);
  signal(SIGABRT,sig_abrt);
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
      if (strcasecmp(argv[ii], "-i") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CMD::command(std::string("include ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-c") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CS cmd(CS::_STRING, argv[ii++]); // command line
	  CMD::cmdproc(cmd, &CARD_LIST::card_list); 
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
	  CMD::command("end", &CARD_LIST::card_list);
	}else{untested();
	}
      }else if (strcasecmp(argv[ii], "-a") == 0) {itested();
	++ii;
	if (ii < argc) {itested();
	  CMD::command(std::string("attach ") + argv[ii++], &CARD_LIST::card_list);
	}else{untested();
	}
      }else{itested();
	CMD::command(std::string("include ") + argv[ii++], &CARD_LIST::card_list);
      }
    }catch (Exception& e) {itested();
      error(bDANGER, e.message() + '\n');
      finish();
    }
  }
}
/*--------------------------------------------------------------------------*/
int main(int argc, const char *argv[])
{
  {
    SET_RUN_MODE xx(rBATCH);
    sign_on();
    if (!sigsetjmp(env.p, true)) {
      try {
	read_startup_files();
	setup_traps();
	process_cmd_line(argc,argv);
      }catch (Exception& e) {untested();
	error(bDANGER, e.message() + '\n');
	finish();		/* error clean up (from longjmp()) */
	CMD::command("quit", &CARD_LIST::card_list);
	unreachable();
	exit(0);
      }
    }else{
      finish();		/* error clean up (from longjmp()) */
      CMD::command("quit", &CARD_LIST::card_list);
      exit(0);
    }
  }
  {itested();
    SET_RUN_MODE xx(rINTERACTIVE);
    CS cmd(CS::_STDIN);
    for (;;) {itested();
      if (!sigsetjmp(env.p, true)) {itested();
	try {
	  if (OPT::language) {
	    OPT::language->parse_top_item(cmd, &CARD_LIST::card_list);
	  }else{
	    CMD::cmdproc(cmd.get_line(I_PROMPT), &CARD_LIST::card_list);
	  }
	}catch (Exception_End_Of_Input& e) {itested();
	  error(bDANGER, e.message() + '\n');
	  finish();
	  CMD::command("quit", &CARD_LIST::card_list);
	  exit(0);
	}catch (Exception& e) {itested();
	  error(bDANGER, e.message() + '\n');
	  finish();
	}
      }else{itested();
	finish();		/* error clean up (from longjmp()) */
      }
    }
  }
  unreachable();
  return 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
