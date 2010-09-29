/*$Id: c_file.cc,v 23.1 2002/11/06 07:47:50 al Exp $ -*- C++ -*-
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
 * log and > commands
 * log == commands log to a file
 * >   == all output to a file (redirect stdout)
 * bare command closes the file
 */
#include "u_opt.h"
#include "io_.h"
#include "declare.h"	/* pllocate */
#include "ap.h"
#include "c_comand.h"

#if defined(HAS_READLINE)
  #include <readline/readline.h>
  #include <readline/history.h>
#endif
/*--------------------------------------------------------------------------*/
//	void	CMD::logger(CS&);
//	void	CMD::file(CS&);
	char*	getcmd(const char*,char*,int);
/*--------------------------------------------------------------------------*/
static OMSTREAM mout;		/* > file bitmap		*/
static OMSTREAM mlog;		/* log file bitmap		*/
/*--------------------------------------------------------------------------*/
/* cmd_log: "log" command processing
 * open a file for logging (history)
 * arg is name of file
 * no arg closes the one most recently opened
 * the file will contain a list of commands executed, for use by "<"
 * multiple files can be open, they are nested, output to all.
 */
void CMD::logger(CS& cmd)
{
  static std::list<FILE*> filestack;

  {if (cmd.more()) {			/* a file name .. open it */
      const char *access = "w";
      while (cmd.match1('>')) {
	access = "a";
	cmd.skip();
	cmd.skipbl();
      }
      FILE* newfile = xopen(cmd,"",access);
      if (newfile) {
        filestack.push_back(newfile);
	mlog.attach(newfile);
      }
  }else{				/* empty command -- close a file */
    {if (filestack.empty()) {
      error(bWARNING, "no files open\n");
    }else{
      FILE* oldfile = filestack.back();
      filestack.pop_back();
      mlog.detach(oldfile);
      fclose(oldfile);
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/* cmd_file: ">" command processing
 * open a file for all output
 * the file will contain a copy of all screen output.
 * arg is name of file
 * no arg closes it
 * the file will contain all that would go to stdout
 */
void CMD::file(CS& cmd)
{
  static std::list<FILE*> filestack;
  
  {if (cmd.more()) {			/* a file name .. open it */
    const char* access = "w";
    while (cmd.match1('>')) {
      access = "a";
      cmd.skip();
      cmd.skipbl();
    }
    FILE* newfile = xopen(cmd,"",access);
    if (newfile) {
      filestack.push_back(newfile);
      mout.attach(newfile);
      IO::mstdout.attach(newfile);
    }
  }else{				/* empty command -- close a file */
    {if (filestack.empty()) {
      error(bWARNING, "no files open\n");
    }else{
      FILE* oldfile = filestack.back();
      filestack.pop_back();
      mout.detach(oldfile);
      IO::mstdout.detach(oldfile);
      fclose(oldfile);
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/* getcmd: get a command.
 * if "fin" is stdin, display a prompt first.
 * Also, actually do logging, echo, etc.
 */
char *getcmd(const char *prompt, char *buffer, int buflen)
{
  assert(prompt);
  assert(buffer);
  pllocate();
#if defined(HAS_READLINE)
  if (OPT::edit) {
    char* line_read = readline(prompt);
    if (!line_read) {
      error(bEXIT, "EOF on stdin\n");
    }
    // readline gets a new buffer every time, so copy it to where we want it
    char* end_of_line = (char*)memccpy(buffer, line_read, 0, buflen-1);
    {if (!end_of_line) {
      buffer[buflen-1] = '\0';
    }else{
      *end_of_line = '\0';
    }}
    free(line_read);
    
    if (*buffer) {
      add_history(buffer);
    }
  }else
#endif
    {
      IO::mstdout << prompt;	/* prompt & flush buffer */
      if (!fgets(buffer, buflen, stdin)) {	/* read line */
	error(bEXIT, "EOF on stdin\n");
      }
    }
  all_except(IO::mstdout, mout) << '\n';	/* reset col counter */
  trim(buffer);
  all_of(mlog, mout) << buffer << '\n';
  return buffer;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
