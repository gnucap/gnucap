/*$Id: u_opt2.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * command and functions to access OPT class
 */
//testing=script,complete 2006.07.14
#include "l_compar.h"
#include "ap.h"
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
void OPT::command(CS& cmd)
{
  bool changed = set_values(cmd);
  if (!changed || opts) {
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
    ONE_OF
      || get(cmd, "ACCT",	 &acct)
      || get(cmd, "LIST",	 &listing)
      || get(cmd, "MOD",	 &mod)
      || get(cmd, "PAGE",	 &page)
      || get(cmd, "NODE",	 &node)
      || get(cmd, "OPTS",	 &opts)
      || get(cmd, "GMIN",	 &gmin,		mPOSITIVE)
      || get(cmd, "RELTOL",	 &reltol,	mPOSITIVE)
      || get(cmd, "ABSTOL",	 &abstol,	mPOSITIVE)
      || get(cmd, "VNTOL",	 &vntol,	mPOSITIVE)
      || get(cmd, "TRTOL",	 &trtol,	mPOSITIVE)
      || get(cmd, "CHGTOL",	 &chgtol,	mPOSITIVE)
      || get(cmd, "PIVTOL",	 &pivtol,	mPOSITIVE)
      || get(cmd, "PIVREL",	 &pivrel,	mPOSITIVE)
      || get(cmd, "NUMDGT",	 &numdgt)
      || get(cmd, "TNOM",	 &tnom_c)
      || get(cmd, "CPTIME",	 &cptime)
      || get(cmd, "LIMTIM",	 &limtim)
      || get(cmd, "LIMPTS",	 &limpts)
      || get(cmd, "LVLCOD",	 &lvlcod)
      || get(cmd, "LVLTIM",	 &lvltim)
      || (cmd.pmatch("METHOD") &&
	  ((cmd.skip1b('='), false)
	   || set(cmd, "Euler",	     &method,	meEULER)
	   || set(cmd, "EULEROnly",  &method,	meEULERONLY)
	   || set(cmd, "Trapezoidal",&method,	meTRAP)
	   || set(cmd, "TRAPOnly",   &method,	meTRAPONLY)
	   || set(cmd, "Gear",	     &method,	meGEAR2)
	   || set(cmd, "GEAR2",	     &method,	meGEAR2)
	   || set(cmd, "GEAR2Only",  &method,	meGEAR2ONLY)
	   || set(cmd, "TRAPGear",   &method,	meTRAPGEAR)
	   || set(cmd, "TG",	     &method,	meTRAPGEAR)
	   || set(cmd, "TRAPEuler",  &method,	meTRAPEULER)
	   || set(cmd, "TE",	     &method,	meTRAPEULER)
	   || cmd.warn(bWARNING, "illegal method")))
      || get(cmd, "MAXORD",	 &maxord)
      || get(cmd, "DEFL",	 &defl,		mPOSITIVE)
      || get(cmd, "DEFW",	 &defw,		mPOSITIVE)
      || get(cmd, "DEFAD",	 &defad,	mPOSITIVE)
      || get(cmd, "DEFAS",	 &defas,	mPOSITIVE)
      || get(cmd, "CLobber",	 &clobber)
      || get(cmd, "DAMPMAX",	 &dampmax,	mPOSITIVE)
      || get(cmd, "DAMPMIN",	 &dampmin,	mPOSITIVE)
      || get(cmd, "DAMPStrategy",&dampstrategy, mOCTAL)
      || get(cmd, "FLOOR",	 &floor,	mPOSITIVE)
      || get(cmd, "VFLOOR",	 &vfloor,	mPOSITIVE)
      || get(cmd, "ROUndofftol", &roundofftol,	mPOSITIVE)
      || get(cmd, "Tempamb",	 &temp_c)
      || get(cmd, "Temperature", &temp_c)
      || get(cmd, "Short",	 &shortckt,	mPOSITIVE)
      || get(cmd, "INwidth",	 &inwidth)
      || get(cmd, "OUTwidth",	 &outwidth)
      || get(cmd, "YDivisions",	 &ydivisions,	mPOSITIVE)
      || set(cmd, "NAG",	 &picky,	bNOERROR)
      || set(cmd, "NONAG",	 &picky,	bTRACE)
      || set(cmd, "TRACE",	 &picky,	bTRACE)
      || set(cmd, "NOTRACE",	 &picky,	bLOG)
      || set(cmd, "LOG",	 &picky,	bLOG)
      || set(cmd, "NOLOG",	 &picky,	bDEBUG)
      || set(cmd, "DEBUG",	 &picky,	bDEBUG)
      || set(cmd, "NODEBUG",	 &picky,	bPICKY)
      || set(cmd, "PICKY",	 &picky,	bPICKY)
      || set(cmd, "NOPICKY",	 &picky,	bWARNING)
      || set(cmd, "WARNing",	 &picky,	bWARNING)
      || set(cmd, "NOWARN",	 &picky,	bDANGER)
      || set(cmd, "ERRor",	 &picky,	bERROR)
      || set(cmd, "NOERRor",	 &picky,	bDISASTER)
      || set(cmd, "DISASTER",	 &picky,	bDISASTER)
      || (cmd.pmatch("PHase") &&
	  ((cmd.skip1b('='), false)
	   || set(cmd, "Degrees", &phase,	pDEGREES)
	   || set(cmd, "Radians", &phase,	pRADIANS)
	   || cmd.warn(bWARNING, "need degrees or radians")))
      || (cmd.pmatch("ORder") &&
	  ((cmd.skip1b('='), false)
	   || set(cmd, "Reverse", &order,	oREVERSE)
	   || set(cmd, "Forward", &order,	oFORWARD)
	   || set(cmd, "Auto",    &order,	oAUTO)
	   || cmd.warn(bWARNING, "need reverse, forward, or auto")))
      || (cmd.pmatch("MODe") &&
	  ((cmd.skip1b('='), false)
	   || set(cmd, "Analog",  &mode,	moANALOG)
	   || set(cmd, "Digital", &mode,	moDIGITAL)
	   || set(cmd, "Mixed",   &mode,	moMIXED)
	   || cmd.warn(bWARNING, "need analog, digital, or mixed")))
      || get(cmd, "TRansits",	 &transits)
      || get(cmd, "DUPcheck",	  &dupcheck)
      || get(cmd, "BYPass",	  &bypass)
      || get(cmd, "INCmode",	  &incmode)
      || get(cmd, "LCBypasss",    &lcbypass)
      || get(cmd, "LUBypasss",    &lubypass)
      || get(cmd, "FBBypasss",    &fbbypass)
      || get(cmd, "TRACELoad",    &traceload)
      || get(cmd, "ITERMIN",	  &itermin)
      || get(cmd, "VMAX",	  &vmax)
      || get(cmd, "VMIN",	  &vmin)
      || get(cmd, "MRT",	  &dtmin,	mPOSITIVE)
      || get(cmd, "DTMIn",	  &dtmin,	mPOSITIVE)
      || get(cmd, "DTRatio",	  &dtratio,	mPOSITIVE)
      || get(cmd, "RSTray",	  &rstray)
      || get(cmd, "CSTray",	  &cstray)
      || get(cmd, "Harmonics",    &harmonics)
      || get(cmd, "TRSTEPGrow",   &trstepgrow,  mPOSITIVE)
      || get(cmd, "TRSTEPHold",   &trstephold,  mPOSITIVE)
      || get(cmd, "TRSTEPShrink", &trstepshrink,mPOSITIVE)
      || get(cmd, "TRReject",     &trreject,	mPOSITIVE)
      || get(cmd, "SHOWALL",	  &showall)
      || get(cmd, "FOOOO",	  &foooo)
      || get(cmd, "DIODEflags",   &diodeflags,  mOCTAL)
      || get(cmd, "MOSflags",     &mosflags,	mOCTAL)
      || get(cmd, "QUITCONVfail", &quitconvfail)
      || get(cmd, "EDIT",	  &edit)
      || get(cmd, "RECURsion",	  &recursion)
      || get(cmd, "ITL1",	  &itl[1])
      || get(cmd, "ITL2",	  &itl[2])
      || get(cmd, "ITL3",	  &itl[3])
      || get(cmd, "ITL4",	  &itl[4])
      || get(cmd, "ITL5",	  &itl[5])
      || get(cmd, "ITL6",	  &itl[6])
      || get(cmd, "ITL7",	  &itl[7])
      || get(cmd, "ITL8",	  &itl[8])
#ifdef KNEECHORD
      || (cmd.pmatch("STRategy") &&
	  ((cmd.skip1b('='), false)
	   || set( cmd, "Newton", &strategy, stNEWTON )
	   || set( cmd, "Kneechord", &strategy, stKNEECHORD )
	   || cmd.warn( bWARNING,
			"available strategies are `Newton' and `Kneechord'")))
#endif
      || (cmd.check(bWARNING, "what's this?"), cmd.skiparg());

    if (!cmd.stuck(&here)) {
      changed = true;
    }
  }while (cmd.more() && changed);

  if (changed) {
    //SIM::uninit();
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
  where << ((acct)   ?"  acct" :"  noacct");
  where << ((listing)?"  list" :"  nolist");
  where << ((mod)    ?"  mod"  :"  nomod");
  where << ((page)   ?"  page" :"  nopage");
  where << ((node)   ?"  node" :"  nonode");
  where << ((opts)   ?"  opts" :"  noopts");
  where << "  gmin="   << gmin;
  where << "  reltol=" << reltol;
  where << "  abstol=" << abstol;
  where << "  vntol="  << vntol;
  where << "  trtol="  << trtol;
  where << "  chgtol=" << chgtol;
  where << "  pivtol=" << pivtol;
  where << "  pivrel=" << pivrel;
  where << "  numdgt=" << numdgt;
  where << "  tnom="   << tnom_c;
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
  where << ((clobber) ? "  clobber" : "  noclobber");
  where << "  dampmax="<< dampmax;
  where << "  dampmin="<< dampmin;
  where << "  dampstrategy="<< octal(dampstrategy);
  where << "  floor="  << floor;
  where << "  vfloor=" << vfloor;
  where << "  roundofftol=" << roundofftol;
  where << "  temperature="<< temp_c;
  where << "  short="  << shortckt;
  where << "  in="     << inwidth;
  where << "  out="    << outwidth;
  where << "  ydivisions=" << ydivisions;
  where << "  phase="  << phase;
  where << "  order="  << order;
  where << "  mode="   << mode;
  where << "  transits=" << transits;
  where << ((dupcheck) ?"  dupcheck" :"  nodupcheck");
  where << ((bypass)   ?"  bypass"   :"  nobypass");
  where << ((incmode)  ?"  incmode"  :"  noincmode");    
  where << ((lcbypass) ?"  lcbypass" :"  nolcbypass");    
  where << ((lubypass) ?"  lubypass" :"  nolubypass");    
  where << ((fbbypass) ?"  fbbypass" :"  nofbbypass");    
  where << ((traceload)?"  traceload":"  notraceload");    
  where << "  itermin="<< itermin;
  where << "  vmax="   << vmax;
  where << "  vmin="   << vmin;
  where << "  dtmin="  << dtmin;
  where << "  dtratio="<< dtratio;
  where << ((rstray)?"  rstray":"  norstray");
  where << ((cstray)?"  cstray":"  nocstray");
  where << "  harmonics="   << harmonics;
  where << "  trstepgrow="  << trstepgrow;
  where << "  trstephold="  << trstephold;
  where << "  trstepshrink="<< trstepshrink;
  where << "  trreject="    << trreject;
  if (diodeflags) {
    where << "  diodeflags="  << octal(diodeflags);
  }
  if (mosflags) {
    where << "  mosflags="    << octal(mosflags);
  }
  where << ((quitconvfail)?"  quitconvfail":"  noquitconvfail");
  where << ((edit)	?"  edit"    :"  noedit");
  where << "  recursion="<< recursion;
  where << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
