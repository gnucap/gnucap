/*$Id: u_opt2.cc,v 22.17 2002/08/26 04:30:28 al Exp $ -*- C++ -*-
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
 * command and functions to access OPT class
 */
#include "l_compar.h"
#include "s__.h"
#include "constant.h"
#include "ap.h"
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
//	void	OPT::command(CS& cmd);
//	bool	OPT::set(CS& cmd);
//	void	OPT::print(OMSTREAM where)
/*--------------------------------------------------------------------------*/
void OPT::command(CS& cmd)
{
  bool changed = set_values(cmd);
  if (!changed || opts){
    print(IO::mstdout);
  }
}
/*--------------------------------------------------------------------------*/
/* set:  set options from a string
 */
bool OPT::set_values(CS& cmd)
{
  bool changed = false;
  int here = cmd.cursor();
  do{
    get(cmd, "ACCT",	&acct);
    get(cmd, "LIST",	&listing);
    get(cmd, "MOD",	&nomod,	 mINVERT);
    get(cmd, "PAGE",	&page);
    get(cmd, "NODE",	&node);
    get(cmd, "OPTS",	&opts);
    get(cmd, "GMIN",	&gmin,	 mPOSITIVE);
    get(cmd, "RELTOL",	&reltol, mPOSITIVE);
    get(cmd, "ABSTOL",	&abstol, mPOSITIVE);
    get(cmd, "VNTOL",	&vntol,	 mPOSITIVE);
    get(cmd, "TRTOL",	&trtol,	 mPOSITIVE);
    get(cmd, "CHGTOL",	&chgtol, mPOSITIVE);
    get(cmd, "PIVTOL",	&pivtol, mPOSITIVE);
    get(cmd, "PIVREL",	&pivrel, mPOSITIVE);
    get(cmd, "NUMDGT",	&numdgt);
    get(cmd, "TNOM",	&tnom,	 mOFFSET, -ABS_ZERO);
    get(cmd, "CPTIME",	&cptime);
    get(cmd, "LIMTIM",	&limtim);
    get(cmd, "LIMPTS",	&limpts);
    get(cmd, "LVLCOD",	&lvlcod);
    get(cmd, "LVLTIM",	&lvltim);
    cmd.pmatch("METHOD") &&
      (   set(cmd, "Euler",	 &method, meEULER)
       || set(cmd, "EULEROnly",  &method, meEULERONLY)
       || set(cmd, "Trapezoidal",&method, meTRAP)
       || set(cmd, "TRAPOnly",	 &method, meTRAPONLY)
       || set(cmd, "Gear",	 &method, meGEAR2)
       || set(cmd, "GEAR2",	 &method, meGEAR2)
       || set(cmd, "GEAR2Only",  &method, meGEAR2ONLY)
       || set(cmd, "TRAPGear",   &method, meTRAPGEAR)
       || set(cmd, "TG",	 &method, meTRAPGEAR)
       || set(cmd, "TRAPEuler",  &method, meTRAPEULER)
       || set(cmd, "TE",	 &method, meTRAPEULER)
       || cmd.warn(bWARNING, "illegal method"));
    get(cmd, "MAXORD",	    &maxord);
    get(cmd, "DEFL", 	    &defl,	mPOSITIVE);
    get(cmd, "DEFW", 	    &defw,	mPOSITIVE);
    get(cmd, "DEFAD",	    &defad,	mPOSITIVE);
    get(cmd, "DEFAS",	    &defas,	mPOSITIVE);
    get(cmd, "Seed",	    &seed);
    get(cmd, "CLobber",	    &clobber);
    get(cmd, "NAMednodes",  &named_nodes);
    get(cmd, "WCZero",	    &wczero,	mPOSITIVE);
    get(cmd, "FLOOR",	    &floor,	mPOSITIVE);
    get(cmd, "VFLOOR",	    &vfloor,	mPOSITIVE);
    get(cmd, "DAMPMAX",	    &dampmax,	mPOSITIVE);
    get(cmd, "DAMPMIN",	    &dampmin,	mPOSITIVE);
    get(cmd, "DAMPStrategy",&dampstrategy, mOCTAL);
    get(cmd, "ROUndofftol", &roundofftol,mPOSITIVE);
    get(cmd, "Tempamb",	    &tempamb,	mOFFSET, -ABS_ZERO);
    get(cmd, "Short",	    &shortckt,	mPOSITIVE);
    get(cmd, "TRansits",    &transits);
    get(cmd, "INwidth",	    &inwidth);
    get(cmd, "OUTwidth",    &outwidth);
    get(cmd, "XDivisions",  &xdivisions, mPOSITIVE);
    get(cmd, "YDivisions",  &ydivisions, mPOSITIVE);
    /**/ set(cmd, "NAG",     &picky,	bNOERROR)
      || set(cmd, "NONAG",   &picky,	bTRACE)
      || set(cmd, "TRACE",   &picky,	bTRACE)
      || set(cmd, "NOTRACE", &picky,	bLOG)
      || set(cmd, "LOG",     &picky,	bLOG)
      || set(cmd, "NOLOG",   &picky,	bDEBUG)
      || set(cmd, "DEBUG",   &picky,	bDEBUG)
      || set(cmd, "NODEBUG", &picky,	bPICKY)
      || set(cmd, "PICKY",   &picky,	bPICKY)
      || set(cmd, "NOPICKY", &picky,	bWARNING)
      || set(cmd, "WARNing", &picky,	bWARNING)
      || set(cmd, "NOWARN",  &picky,	bDANGER)
      || set(cmd, "ERRor",   &picky,	bERROR)
      || set(cmd, "NOERRor", &picky,	bDISASTER)
      || set(cmd, "DISASTER",&picky,	bDISASTER);
    cmd.pmatch("PHase") &&
      (  set(cmd, "Degrees", &phase,	pDEGREES)
      || set(cmd, "Radians", &phase,	pRADIANS)
      || cmd.warn(bWARNING, "need degrees or radians"));
    cmd.pmatch("ORder") &&
      (  set(cmd, "Reverse", &order,	oREVERSE)
      || set(cmd, "Forward", &order,	oFORWARD)
      || set(cmd, "Auto",    &order,	oAUTO)
      || cmd.warn(bWARNING, "need reverse, forward, or auto"));
    cmd.pmatch("MODe") &&
      (  set(cmd, "Analog",  &mode,	moANALOG)
      || set(cmd, "Digital", &mode,	moDIGITAL)
      || set(cmd, "Mixed",   &mode,	moMIXED)
      || cmd.warn(bWARNING, "need analog, digital, or mixed"));
    get(cmd, "DUPcheck",    &dupcheck);
    (    set(cmd, "BYPass",  &bypass,	bYES)
      || set(cmd, "NOBYPass",&bypass,	bNO)
      || set(cmd, "VBYPass", &bypass,	bVOLT));
    get(cmd, "INCmode",	    &incmode);
    get(cmd, "LUBypasss",   &lubypass);
    get(cmd, "FBBypasss",   &fbbypass);
    get(cmd, "TRACELoad",   &traceload);
    get(cmd, "ITERMIN",	    &itermin);
    get(cmd, "LIMIT",	    &limit,	  mPOSITIVE);
    get(cmd, "VMAX",	    &vmax);
    get(cmd, "VMIN",	    &vmin);
    get(cmd, "MRT",	    &dtmin,	  mPOSITIVE);
    get(cmd, "DTMIn",	    &dtmin,	  mPOSITIVE);
    get(cmd, "DTRatio",	    &dtratio,	  mPOSITIVE);
    get(cmd, "RSTray",	    &rstray);
    get(cmd, "CSTray",	    &cstray);
    get(cmd, "Harmonics",   &harmonics);
    get(cmd, "TRSTEPGrow",  &trstepgrow,  mPOSITIVE);
    get(cmd, "TRSTEPShrink",&trstepshrink,mPOSITIVE);
    get(cmd, "TRReject",    &trreject,	  mPOSITIVE);
    get(cmd, "SHOWALL",	    &showall);
    get(cmd, "FOOOO",	    &foooo);
    get(cmd, "DIODEflags",  &diodeflags,  mOCTAL);
    diodeflags = 0;
    get(cmd, "MOSflags",    &mosflags,	  mOCTAL);
    get(cmd, "QUITCONVfail",&quitconvfail);
    get(cmd, "EDIT",	    &edit);
    get(cmd, "ITL1",	    &itl[1]);
    get(cmd, "ITL2",	    &itl[2]);
    get(cmd, "ITL3",	    &itl[3]);
    get(cmd, "ITL4",	    &itl[4]);
    get(cmd, "ITL5",	    &itl[5]);
    get(cmd, "ITL6",	    &itl[6]);
    get(cmd, "ITL7",	    &itl[7]);
    get(cmd, "ITL8",	    &itl[8]);
    if (cmd.stuck(&here)){
      cmd.check(bWARNING, "what's this?");
      cmd.skiparg();
    }else{
      changed = true;
    }
  }while (cmd.more() && changed);

  if (changed){
    SIM::uninit();
    lowlim = 1 - reltol;
    uplim  = 1 + reltol;
    numdgt = to_range(3, numdgt, 20);
  }
  return changed;
}
/*--------------------------------------------------------------------------*/
/* print: "print" all option values to "where"
 * string is in a form suitable for passing to set
 */
void OPT::print(OMSTREAM& where)
{
  where.setfloatwidth(7);
  where << ".options";
  where << ((acct) ?"  acct" :"  noacct");
  where << ((listing)?"list" :"  nolist");
  where << ((nomod)?"  nomod":"  mod");
  where << ((page) ?"  page" :"  nopage");
  where << ((node) ?"  node" :"  nonode");
  where << ((opts) ?"  opts" :"  noopts");
  where << "  gmin="   << gmin;
  where << "  reltol=" << reltol;
  where << "  abstol=" << abstol;
  where << "  vntol="  << vntol;
  where << "  trtol="  << trtol;
  where << "  chgtol=" << chgtol;
  where << "  pivtol=" << pivtol;
  where << "  pivrel=" << pivrel;
  where << "  numdgt=" << numdgt;
  where << "  tnom="   << tnom+ABS_ZERO;
  where << "  cptime=" << cptime;
  where << "  limtim=" << limtim;
  where << "  limpts=" << limpts;
  where << "  lvlcod=" << lvlcod;
  where << "  lvltim=" << lvltim;
  where << "  method=" << method;
  where << "  maxord=" << maxord;
  for (int ii=1;  ii<ITL_COUNT;  ii++)
    where << "  itl@" << ii << "=" << itl[ii];
  where << "  defl="   << defl;
  where << "  defw="   << defw;
  where << "  defad="  << defad;
  where << "  defas="  << defas;
  where << "  seed="   << seed;
  where << ((clobber) ? "  clobber" : "  noclobber");
  where << ((named_nodes) ? "  namednodes" : "  nonamednodes");
  where << "  wczero=" << wczero;
  where << "  dampmax="<< dampmax;
  where << "  dampmin="<< dampmin;
  where << "  dampstrategy="<< octal(dampstrategy);
  where << "  roundofftol=" << roundofftol;
  where << "  tempamb="<< tempamb+ABS_ZERO;
  where << "  short="  << shortckt;
  where << "  in="     << inwidth;
  where << "  out="    << outwidth;
  where << "  xdivisions=" << xdivisions;
  where << "  ydivisions=" << ydivisions;
  where << "  phase="  << phase;
  where << "  order="  << order;
  where << "  mode="   << mode;
  where << "  transits=" << transits;
  where << ((dupcheck)?"  dupcheck":"  nodupcheck");
  where << "  "        << bypass;
  where << ((incmode)  ?"  incmode"  :"  noincmode");    
  where << ((lubypass) ?"  lubypass" :"  nolubypass");    
  where << ((fbbypass) ?"  fbbypass" :"  nofbbypass");    
  where << ((traceload)?"  traceload":"  notraceload");    
  where << "  itermin="<< itermin;
  where << "  limit="  << limit;
  where << "  vmax="   << vmax;
  where << "  vmin="   << vmin;
  where << "  dtmin="  << dtmin;
  where << "  dtratio="<< dtratio;
  where << ((rstray)?"  rstray":"  norstray");
  where << ((cstray)?"  cstray":"  nocstray");
  where << "  harmonics="   << harmonics;
  where << "  trstepgrow="  << trstepgrow;
  where << "  trstepshrink="<< trstepshrink;
  where << "  trreject="    << trreject;
  where << "  diodeflags="  << octal(diodeflags);
  where << "  mosflags="    << octal(mosflags);
  where << ((quitconvfail)?"  quitconvfail":"  noquitconvfail");    
  where << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
