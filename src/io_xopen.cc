/*$Id: io_xopen.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 * scan a string for a file name
 * fill in extension, if necessary
 * open file
 */
//testing=script,sparse 2006.07.17
#include "u_opt.h"
#include "constant.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
	void	xclose(FILE**);
	FILE*	xopen(CS&,const char*,const char*);
/*--------------------------------------------------------------------------*/
void xclose(FILE **fn)
{
  if (*fn) {
    fclose(*fn);
    *fn = NULL;
  }
}
/*--------------------------------------------------------------------------*/
/* xopen: open a file from a command string
 * 	scan and eat up the name from the command
 *	add default extension if appropriate
 *	if there is no name, prompt for one
 *	trap errors of not found and clobber
 */
FILE *xopen(CS& cmd, const char *ext, const char *how)
{
  char fname[BIGBUFLEN];
  
  cmd.skipbl();
  if (cmd.is_end()) {untested();
    cmd = getcmd("file name?  ",fname, BIGBUFLEN);
  }
					/* copy the name		    */
  cmd.skipbl();				/* and while we're at it ...	    */
  {					/* find out if we want to add the   */
    bool defalt = true;			/* default extension		    */
    size_t i;
    for (i = 0;   i < BIGBUFLEN;   ) {
      char c = cmd.ctoc();
      if (!c || isspace(c)) {
	break;
      }
      if (c == '$') {untested(); 
	sprintf(&(fname[i]), "%ld", static_cast<long>(time(0)));
	i = strlen(fname);
      }else{				/* we want to add the extension	    */
	fname[i++] = c;			/* if it doesn't already have one,  */
	if (c == '.') {		/* as determined by a '.'	    */
	  defalt = false;		/* not before the directory	    */
	}else if (strchr(ENDDIR,c)) {	/* separator-terminator character   */
	  itested();
	  defalt = true;		/* '\' or '/' for msdos,	    */
	}
      }  				/* ']' or '/' for vms,		    */
    }					/* '/' for unix  (in ENDDIR)	    */
    cmd.skip(-1);
    if (defalt && ext && *ext && i+strlen(ext)+2 < BIGBUFLEN) {untested(); 
      fname[i++] = '.';			/* add the extension (maybe)	    */
      strcpy(&fname[i],ext);
    }else{
      fname[i] = '\0';
    }
  }
  
  trim(fname);
  if (strlen(fname)==0) {
    return NULL;
  }

  cmd.skipcom();
  
  FILE *code = NULL;	/* a file pointer for the file we found */
  if (!OPT::clobber && (*how == 'w') && (access(fname,F_OK) == FILE_OK)) {untested();
    char buffer[BUFLEN];
    std::string msg = std::string(fname) + " exists.  replace? ";
    getcmd(msg.c_str(), buffer, BUFLEN);
    if (Umatch(buffer,"y{es} ")) {untested(); 	/* should be new file, but	    */
      code = fopen(fname,how);		/* file already exists,  ask	    */
    }else{untested();
      return NULL;
    }
  }else{
    code = fopen(fname,how);
  }
  
  if (code && fileno(code)>MAXHANDLE) {untested(); 
    error(bWARNING, "internal error: files: %d\n", fileno(code));
    fclose(code);
    code = NULL;
  }
  if (code) {
    IO::stream[static_cast<int>(fileno(code))] = code;
  }else{itested();
    error(bWARNING, "can't open %s, %s\n", fname, strerror(errno));
  }
  return code;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
