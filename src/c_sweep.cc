/*$Id: c_sweep.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * Step a parameter and repeat a group of commands
 */
#include "io_.h"
#include "declare.h"	/* getcmd */
#include "c_comand.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::sweep(CS&);
static	void	buildfile(CS&);
static	void	doit();
static	void	setup(CS&);
/*--------------------------------------------------------------------------*/
extern CONST bool crtplot;
extern int swp_count[], swp_steps[];
extern int swp_type[];
extern int swp_nest;
static char tempfile[] = STEPFILE;
/*--------------------------------------------------------------------------*/
void CMD::sweep(CS& cmd)
{
  untested();
  if (cmd.more())
    buildfile(cmd);
  doit();
  unfault(cmd);
}
/*--------------------------------------------------------------------------*/
static void buildfile(CS& cmd)
{
  untested();
  static FILE *fptr;
  
  setup(cmd);
  if (fptr)
    fclose(fptr);
  fptr = fopen( tempfile, "w" );
  if (!fptr)
    error(bERROR, "can't open temporary file\n" );
  fprintf(fptr, "%s\n", cmd.fullstring() );
  
  for (;;){
    char buffer[BUFLEN];
    getcmd( ">>>", buffer, BUFLEN );
    if ( pmatch(buffer,"GO") )
      break;
    fprintf(fptr,"%s\n",buffer);
  }
  fclose(fptr);
  fptr = NULL;
}
/*--------------------------------------------------------------------------*/
static void doit()
{
  untested();
  static FILE *fptr;
  
  for ( swp_count[swp_nest]=0 ; swp_count[swp_nest]<=swp_steps[swp_nest] ;
       swp_count[swp_nest]++ ){
    if (fptr)
      fclose(fptr);
    fptr = fopen(tempfile, "r");
    if (!fptr)
      error(bERROR, "can't open %s\n", tempfile);
    char buffer[BUFLEN];
    fgets(buffer,BUFLEN,fptr);
    CS cmd(buffer);
    if (cmd.pmatch("SWeep"))
      setup(cmd);
    else
      error(bERROR, "bad file format: %s\n", tempfile);
    int ind = cmd.cursor();
    strncpy(buffer, "fault                              ", ind);
    buffer[ind-1] = ' ';		/* make sure there is a delimiter   */
    					/* in case the words run together   */
    for (;;){				/* may wipe out one letter of fault */
      CMD::cmdproc(buffer);
      if (!fgets(buffer,BUFLEN,fptr))
	break;
      if (!crtplot)
	IO::mstdout << swp_count[swp_nest]+1 << "> " << buffer;
    }
  }
  fclose(fptr);
  fptr = NULL;
  swp_count[swp_nest] = 0;
}
/*--------------------------------------------------------------------------*/
static void setup(CS& cmd)
{
  untested();
  for (;;){
    if (cmd.is_digit()){
      swp_steps[swp_nest] = cmd.ctoi() ;
      swp_steps[swp_nest] = (swp_steps[swp_nest]) 
	? swp_steps[swp_nest]-1
	: 0;
    }else if (cmd.pmatch("LInear")){
      swp_type[swp_nest] = 0;
    }else if (cmd.pmatch("LOg")){
      swp_type[swp_nest] = 'L';
    }else{
      break;
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
