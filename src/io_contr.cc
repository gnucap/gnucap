/*$Id: io_contr.cc,v 26.110 2009/05/28 15:32:04 al Exp $ -*- C++ -*-
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
 * Sets up output direction and format for most commands
 * updates pointers into the string
 * outreset starts it all over
 */
//testing=script,sparse 2006.07.17
#include "io_.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
	void	initio(OMSTREAM&);
//static	void	decipher(char*);
	void	outreset(void);
	OMSTREAM* outset(CS&,OMSTREAM*);
static	FILE*	file_open(CS&,const char*,const char*,FILE*);
/*--------------------------------------------------------------------------*/
static FILE* fn;		/* write file				    */
static FILE* to_pipe;
/*--------------------------------------------------------------------------*/
/* initio: initialize file encryption, etc
 */
void initio(OMSTREAM& Where)
{
  const char* tag = "''''";
  if (Where.cipher()) {		/* if writing an encrypted file,    */
    untested();
    Where << tag << '\n';	/* mark it as encrypted		    */
  }else{
  }
#if 0
  if (Whence) {
    char buf[BUFLEN];
    if (!fgets(buf, BUFLEN, Whence))	/* if the first line deciphers to   */
      return;				/* the cipher tag, it is encrypted  */
    IO::incipher = true;		/* otherwise,			    */
    decipher(buf);			/*	 rewind and read normally   */
    if (strcmp(buf,tag) != 0) {		/* mismatch */
      IO::incipher = false;
      fseek(Whence,0L,SEEK_SET);
    }
  }
#endif
}
/*--------------------------------------------------------------------------*/
#if 0
/* decipher: un-encrypt a line of text in place
 */
void decipher(char *buf)
{
  untested();
  if (IO::incipher) {
    for ( ; isprint(buf[1]); buf++ ) {
      int fixed = static_cast<int>(buf[1]) - static_cast<int>(buf[0]);
      while (!isascii(fixed) || !isprint(fixed))
	fixed += (0x7f-0x20);
      buf[0] = static_cast<char>(fixed);
    }
    buf[0] = '\0';
  }else{
  }
  
}
#endif
/*--------------------------------------------------------------------------*/
/* outreset: close files and set i/o flags to standard values
 */
void outreset(void)
{
  if (to_pipe) {
    untested();
    pclose(to_pipe);
    to_pipe = NULL;
  }else{
  }
  xclose(&fn);
  IO::formaat = 0;
  IO::incipher = false;
  IO::mstdout.reset();
}
/*--------------------------------------------------------------------------*/
/* outset: set up i/o for a command
 * return whether or not it did anything
 */
OMSTREAM* outset(CS& cmd, OMSTREAM* out)
{
  bool echo = false;
  for (;;) {
    if (cmd.umatch("basic ")) {
      IO::formaat = ftos_EXP;
      (*out).setformat(ftos_EXP);
    }else if (cmd.umatch("cipher ")) {untested();
      (*out).setcipher().setpack();
    }else if (cmd.umatch("pack ")) {itested();
      (*out).setpack();
    }else if (cmd.umatch("quiet ")) {itested();
      echo = false;
      (*out).detach(IO::mstdout);
    }else if (cmd.umatch("echo ") || cmd.umatch("list ")) {itested();
      echo = true;
      (*out).attach(IO::mstdout);
    }else if (cmd.umatch("save ")) {itested();
      fn = file_open(cmd,"","w",fn);
      (*out).attach(fn);
    }else if (cmd.umatch("\\|")) {untested();
      // open a pipe
      std::string command;
      cmd >> command;
      to_pipe = popen(command.c_str(), "w");
      assert(to_pipe);

      IO::stream[static_cast<int>(fileno(to_pipe))] = to_pipe;
      (*out).attach(to_pipe);

      IO::formaat = ftos_EXP;
      (*out).setformat(ftos_EXP);
      if (!echo) {untested();
	(*out).detach(IO::mstdout);
      }else{untested();
      }
    }else if (cmd.umatch(">")) {itested();
      // open a file for write or append
      const char *rwaflag;
      rwaflag = (cmd.umatch(">")) ? "a" : "w";
      fn = file_open(cmd,"",rwaflag,fn);
      (*out).attach(fn);
      IO::formaat = ftos_EXP;
      (*out).setformat(ftos_EXP);
      if (!echo) {itested();
	(*out).detach(IO::mstdout);
      }else{untested();
      }
    }else{
      break;
    }
  }
  return out;
}
/*--------------------------------------------------------------------------*/
/* file_open: a different interface for xopen
 */
static FILE *file_open(
	CS& cmd,
	const char *ext,
	const char *rwaflag,
	FILE *fileptr)
{itested();
  xclose(&fileptr);
  fileptr = xopen(cmd,ext,rwaflag);
  if (!fileptr) {itested();
    throw Exception_File_Open("");
  }else{itested();
  }
  return fileptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
