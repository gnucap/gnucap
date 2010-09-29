/*$Id: main.cc,v 22.4 2002/05/27 00:00:47 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *------------------------------------------------------------------
 * top level module
 * it all starts here
 */
#include "patchlev.h"
#include "c_comand.h"
#include "declare.h"	/* lots */
#include "u_opt.h"
#include "l_jmpbuf.h"
/*--------------------------------------------------------------------------*/
	int	main(int,const char*[]);
static	void    sign_on(void);
static	void    read_startup_files(void);
static	void    process_cmd_line(int,const char*[]);
static	void	finish(void);
/*--------------------------------------------------------------------------*/
extern JMP_BUF env;
/*--------------------------------------------------------------------------*/
int main(int argc, const char *argv[])
{
  {
    SET_RUN_MODE xx(rBATCH);
    sign_on();
    if (!sigsetjmp(env.p, true)){
      //try {
      read_startup_files();
      setup_traps();
      process_cmd_line(argc,argv);
    }else{
      //}catch (...){
      finish();		/* error clean up (from longjmp())	*/
      exit(0);
    }
  }
  for (;;){
    SET_RUN_MODE xx(rINTERACTIVE);
    if (!sigsetjmp(env.p, true)){
      //try {
      char cmdbuf[BUFLEN];
      CMD::count++;
      getcmd(I_PROMPT, cmdbuf, BUFLEN);
      CMD::cmdproc(cmdbuf);
    }else{
      //}catch (...){
      finish();		/* error clean up (from longjmp())	*/
    }  
  }
  return 0;
}
/*--------------------------------------------------------------------------*/
static void sign_on(void)
{
  IO::mstdout << "Gnucap 0." << PATCHLEVEL << '\n'
	      << "The Gnu Circuit Analysis Package\n"
	      << "Never trust any version less than 1.0\n"
	      << "Copyright 1982-2002, Albert Davis\n"
	      << "Gnucap comes with ABSOLUTELY NO WARRANTY\n"
	      << "This is free software, and you are welcome\n"
	      << "to redistribute it under certain conditions\n"
	      << "according to the GNU General Public License.\n"
	      << "See the file \"COPYING\" for details\n";
}
/*--------------------------------------------------------------------------*/
static void read_startup_files(void)
{
  std::string name = findfile(SYSTEMSTARTFILE, SYSTEMSTARTPATH, R_OK);
  if (!name.empty()){
    char cmdbuf[BUFLEN];
    sprintf(cmdbuf, "get %s", name.c_str());
    CMD::cmdproc(cmdbuf);
  }
  name = findfile(USERSTARTFILE, USERSTARTPATH, R_OK);
  if (!name.empty()){
    char cmdbuf[BUFLEN];
    sprintf(cmdbuf, "get %s", name.c_str());
    CMD::cmdproc(cmdbuf);
  }
  CMD::cmdproc("clear");
}
/*--------------------------------------------------------------------------*/
static void process_cmd_line(int argc, const char *argv[])
{
  char cmdbuf[BUFLEN];
  {if ((argc > 2) && (strcasecmp(argv[1], "-i") == 0)) {
    sprintf(cmdbuf, "get %s", argv[2]);
    CMD::cmdproc(cmdbuf);
  }else if ((argc > 2) && (strcasecmp(argv[1], "-b") == 0)) {
    sprintf(cmdbuf, "< %s", argv[2]);
    CMD::cmdproc(cmdbuf);
    CMD::cmdproc("end");
  }else if (argc > 1) {
    sprintf(cmdbuf, "get %s", argv[1]);
    CMD::cmdproc(cmdbuf);
  }}
}
/*--------------------------------------------------------------------------*/
/* finish: clean up after a command
 * deallocates space, closes plot windows, resets i/o redirection, etc.
 * This is done separately for exception handling.
 * If a command aborts, clean-up is still done, leaving a consistent state.
 */
static void finish(void)
{
  plclose();
  IO::suppresserrors = false;
  outreset();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
