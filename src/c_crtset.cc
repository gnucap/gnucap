/*$Id: c_crtset.cc,v 20.5 2001/09/17 15:43:17 al Exp $ -*- C++ -*-
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
 * set up crt plotting parameters
 */
#include "io_.h"
#include "ap.h"
#include "pixelh.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void		CMD::crtset(CS&);
static	void		setq(CS&,int*);
	int		testcrt();
	struct graph	*initcrt();
/*--------------------------------------------------------------------------*/
extern struct graph *initsun(struct graph *);
extern struct graph *initibm(struct graph *);
extern struct graph *initbasic(struct graph *);
extern struct graph *initpostscript(struct graph *);
extern struct graph *initunix(struct graph *);
extern struct graph *initx(struct graph *);
static char crttype;
static struct graph d;
/*--------------------------------------------------------------------------*/
void CMD::crtset(CS& cmd)
{
  static int sw_c, sh_c;
  static bool beenhere = false;

  if (!beenhere){
    d.sw    = 640;
    d.sh    = 200;
    sw_c    = 80;
    sh_c    = 25;
    d.ppc   = d.sw / sw_c;
    d.lpc   = d.sh / sh_c;
    d.gmode = 6;
    d.tmode = 2;
    d.pri   = 1;
    d.sec   = -2;
    d.grid  = -8;
    d.back  = 0;
    d.palette = 0;
  }
  beenhere = true;
  
  if (cmd.pmatch("Ascii")){
    crttype = '\0';
  }else if (cmd.pmatch("None")){
    crttype = '\0';
  }else if (cmd.pmatch("Basic")){
    crttype = 'b';
  }else if (cmd.pmatch("Hercules")){
    crttype = 'h';
  }else if (cmd.pmatch("Ibm")){
    crttype = 'i';
  }else if (cmd.pmatch("Postscript")){
    crttype = 'p';
  }else if (cmd.pmatch("Sun")){
    crttype = 's';
  }else if (cmd.pmatch("Unix")){
    crttype = 'u';
  }else if (cmd.pmatch("X")){
    crttype = 'x';
  }else if (cmd.pmatch("Zenith")){
    crttype = 'z';
  }else if (!cmd.peek()){
    IO::mstdout
      << crttype << '\n'
      << d.sw << 'x' << d.sh << ", " << sw_c << 'x' << sh_c << '\n'
      << d.gmode << ", " << d.tmode << '\n'
      << d.pri << ", " << d.sec << ", " << d.grid << ", " << d.back << '\n'
      << d.palette << '\n';
    return;
  }else{
    cmd.check(bERROR, "what's this?");
  }

  setq(cmd,&d.sw);
  setq(cmd,&d.sh);
  setq(cmd,&sw_c);
  setq(cmd,&sh_c);
  d.ppc = d.sw / sw_c;
  d.lpc = d.sh / sh_c;
  setq(cmd,&d.gmode);
  setq(cmd,&d.tmode);
  setq(cmd,&d.pri);
  setq(cmd,&d.sec);
  setq(cmd,&d.grid);
  setq(cmd,&d.back);
  setq(cmd,&d.palette);
}
/*--------------------------------------------------------------------------*/
static void setq(CS& cmd, int *value)
{
  cmd.skipbl();
  if (isdigit(cmd.peek()) || cmd.peek()=='-')
    *value = (int)cmd.ctof();
  else
    cmd.skiparg();
}
/*--------------------------------------------------------------------------*/
int testcrt()
{
  return crttype;
}
/*--------------------------------------------------------------------------*/
struct graph *initcrt()
{
  switch (crttype){
  case 'b':
    return initbasic(&d);
#ifdef MSDOS
  case 'i':
    return initibm(&d);
#endif
  case 'p':
    return initpostscript(&d);
#ifdef SUNVIEW
  case 's':
    return initsun(&d);
#endif
  case 'u':
    return initunix(&d);
  case 'x':
    return initx(&d);
  default:
    return (struct graph*)NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
