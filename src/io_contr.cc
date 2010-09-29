/*$Id: io_contr.cc,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
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
 * Sets up output direction and format for most commands
 * updates pointers into the string
 * outreset starts it all over
 */
#include "io_.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
	void	initio(OMSTREAM&);
	void	decipher(char*);
	void	outreset(void);
	OMSTREAM* outset(CS&,OMSTREAM*);
static	FILE*	file_open(CS&,const char*,const char*,FILE*);
/*--------------------------------------------------------------------------*/
static FILE *fn;		/* write file				    */
/*--------------------------------------------------------------------------*/
/* initio: initialize file encryption, etc
 */
void initio(OMSTREAM& Where)
{
  const char* tag = "''''";
  if (Where.cipher())			/* if writing an encrypted file,    */
    Where << tag << '\n';		/* mark it as encrypted		    */
#if 0
  if (Whence){
    char buf[BUFLEN];
    if (!fgets(buf, BUFLEN, Whence))	/* if the first line deciphers to   */
      return;				/* the cipher tag, it is encrypted  */
    IO::incipher = true;		/* otherwise,			    */
    decipher(buf);			/*	 rewind and read normally   */
    if (strcmp(buf,tag) != 0){		/* mismatch */
      IO::incipher = false;
      fseek(Whence,0L,SEEK_SET);
    }
  }
#endif
}
/*--------------------------------------------------------------------------*/
/* decipher: un-encrypt a line of text in place
 */
void decipher(char *buf)
{
  untested();
  if (IO::incipher){
    for ( ; isprint(buf[1]); buf++ ){
      int fixed = static_cast<int>(buf[1]) - static_cast<int>(buf[0]);
      while (!isascii(fixed) || !isprint(fixed))
	fixed += (0x7f-0x20);
      buf[0] = static_cast<char>(fixed);
    }
    buf[0] = '\0';
  }
}
/*--------------------------------------------------------------------------*/
/* outreset: close files and set i/o flags to standard values
 */
void outreset(void)
{
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
  for (;;){
    if (cmd.pmatch("Basic")){
      IO::formaat = ftos_EXP;
      (*out).setformat(ftos_EXP);
    }else if (cmd.pmatch("Cipher")){
      (*out).setcipher().setpack();
    }else if (cmd.pmatch("Pack")){
      (*out).setpack();
    }else if (cmd.pmatch("Quiet")){
      (*out).detach(IO::mstdout);
    }else if (cmd.pmatch("Echo") || cmd.pmatch("List")){
      (*out).attach(IO::mstdout);
    }else if (cmd.pmatch("SAve")){
      fn = file_open(cmd,"","w",fn);
      (*out).attach(fn);
    }else if (cmd.pmatch(">$$")){
      const char *rwaflag;
      rwaflag = (cmd.pmatch(">$$")) ? "a" : "w";
      fn = file_open(cmd,"",rwaflag,fn);
      (*out).attach(fn);
      IO::formaat = ftos_EXP;
      (*out).setformat(ftos_EXP);
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
{
  xclose(&fileptr);
  fileptr = xopen(cmd,ext,rwaflag);
  if (!fileptr)
    error(bERROR, "");
  return fileptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
