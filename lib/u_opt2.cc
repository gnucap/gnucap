/*$Id: u_opt2.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
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
 * command and functions to access OPT class
 */
//testing=script,complete 2006.07.14
#include "globals.h"
#include "c_comand.h"
#include "u_lang.h"
#include "l_compar.h"
#include "ap.h"
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
  bool big_change = false;
  bool changed = false;
  unsigned here = cmd.cursor();
  do{
    ONE_OF
      || Get(cmd, "acct",	&acct)
      || Get(cmd, "list",	&listing)
      || Get(cmd, "mod",	&mod)
      || Get(cmd, "page",	&page)
      || Get(cmd, "node",	&node)
      || Get(cmd, "opts",	&opts)
      || Get(cmd, "gmin",	&gmin,   mPOSITIVE)
      || Get(cmd, "bypasstol",	&bypasstol, mPOSITIVE)
      || Get(cmd, "loadtol",	&loadtol,   mPOSITIVE)
      || Get(cmd, "reltol",	&reltol, mPOSITIVE)
      || Get(cmd, "abstol",	&abstol, mPOSITIVE)
      || Get(cmd, "vntol",	&vntol,  mPOSITIVE)
      || Get(cmd, "trtol",	&trtol,  mPOSITIVE)
      || Get(cmd, "chgtol",	&chgtol, mPOSITIVE)
      || Get(cmd, "pivtol",	&pivtol, mPOSITIVE)
      || Get(cmd, "pivrel",	&pivrel, mPOSITIVE)
      || Get(cmd, "numdgt",	&numdgt)
      || Get(cmd, "tnom",	&tnom_c)
      || Get(cmd, "cptime",	&cptime)
      || Get(cmd, "limtim",	&limtim)
      || Get(cmd, "limpts",	&limpts)
      || Get(cmd, "lvlcod",	&lvlcod)
      || Get(cmd, "lvltim",	&lvltim)
      || (cmd.umatch("method {=}") &&
	  (ONE_OF
	   || Set(cmd, "euler", 	&method, meEULER)
	   || Set(cmd, "eulero{nly}",	&method, meEULERONLY)
	   || Set(cmd, "trap{ezoidal}",	&method, meTRAP)
	   || Set(cmd, "trapo{nly}",	&method, meTRAPONLY)
	   || Set(cmd, "gear{2}", 	&method, meGEAR2)
	   || Set(cmd, "gear2o{nly}",	&method, meGEAR2ONLY)
	   || Set(cmd, "t{rap}g{ear}",	&method, meTRAPGEAR)
	   || Set(cmd, "t{rap}e{uler}",	&method, meTRAPEULER)
	   || cmd.warn(bWARNING, "illegal method")))
      || Get(cmd, "maxord",	   &maxord)
      || Get(cmd, "defl",	   &defl,	mPOSITIVE)
      || Get(cmd, "defw",	   &defw,	mPOSITIVE)
      || Get(cmd, "defad",	   &defad,	mPOSITIVE)
      || Get(cmd, "defas",	   &defas,	mPOSITIVE)
      || Get(cmd, "clobber",	   &clobber)
      || Get(cmd, "dampmax",	   &dampmax,	mPOSITIVE)
      || Get(cmd, "dampmin",	   &dampmin,	mPOSITIVE)
      || Get(cmd, "damps{trategy}",&dampstrategy, mOCTAL)
      || Get(cmd, "floor",	   &floor,	mPOSITIVE)
      || Get(cmd, "vfloor",	   &vfloor,	mPOSITIVE)
      || Get(cmd, "roundofftol",   &roundofftol, mPOSITIVE)
      || Get(cmd, "t{empamb}",	   &temp_c)
      || Get(cmd, "t{emperature}", &temp_c)
      || Get(cmd, "short",	   &shortckt,	mPOSITIVE)
      || Get(cmd, "out{width}",    &outwidth)
      || Get(cmd, "ydiv{isions}",  &ydivisions, mPOSITIVE)
      || Set(cmd, "nag",	   &picky,	bNOERROR)
      || Set(cmd, "nonag",	   &picky,	bTRACE)
      || Set(cmd, "trace",	   &picky,	bTRACE)
      || Set(cmd, "notrace",	   &picky,	bLOG)
      || Set(cmd, "log",	   &picky,	bLOG)
      || Set(cmd, "nolog",	   &picky,	bDEBUG)
      || Set(cmd, "debug",	   &picky,	bDEBUG)
      || Set(cmd, "nodebug",	   &picky,	bPICKY)
      || Set(cmd, "picky",	   &picky,	bPICKY)
      || Set(cmd, "nopicky",	   &picky,	bWARNING)
      || Set(cmd, "warn{ing}",	   &picky,	bWARNING)
      || Set(cmd, "nowarn",	   &picky,	bDANGER)
      || (cmd.umatch("phase {=}") &&
	  (ONE_OF
	   || Set(cmd, "d{egrees}",  &phase,	pDEGREES)
	   || Set(cmd, "+d{egrees}", &phase,	pP_DEGREES)
	   || Set(cmd, "-d{egrees}", &phase,	pN_DEGREES)
	   || Set(cmd, "r{adians}",  &phase,	pRADIANS)
	   || cmd.warn(bWARNING, "need degrees or radians")))
      || (cmd.umatch("order {=}") &&
	  (ONE_OF
	   || Set(cmd, "r{everse}", &order,	oREVERSE)
	   || Set(cmd, "f{orward}", &order,	oFORWARD)
	   || Set(cmd, "a{uto}",    &order,	oAUTO)
	   || cmd.warn(bWARNING, "need reverse, forward, or auto")))
      || (cmd.umatch("mode {=}") &&
	  (ONE_OF
	   || Set(cmd, "a{nalog}",  &mode,	moANALOG)
	   || Set(cmd, "d{igital}", &mode,	moDIGITAL)
	   || Set(cmd, "m{ixed}",   &mode,	moMIXED)
	   || cmd.warn(bWARNING, "need analog, digital, or mixed")))
      || Get(cmd, "tr{ansits}",    &transits)
      || Get(cmd, "dup{check}",    &dupcheck)
      || Get(cmd, "byp{ass}",	   &bypass)
      || Get(cmd, "inc{mode}",	   &incmode)
      || Get(cmd, "lcb{ypass}",    &lcbypass)
      || Get(cmd, "lub{ypass}",    &lubypass)
      || Get(cmd, "fbb{ypass}",	   &fbbypass)
      || Get(cmd, "tracel{oad}",   &traceload)
      || Get(cmd, "itermin",	   &itermin)
      || Get(cmd, "vmax",	   &vmax)
      || Get(cmd, "vmin",	   &vmin)
      || Get(cmd, "mrt",	   &dtmin,	mPOSITIVE)
      || Get(cmd, "dtmin",	   &dtmin,	mPOSITIVE)
      || Get(cmd, "dtr{atio}",	   &dtratio,	mPOSITIVE)
      || (Get(cmd, "rstray",	   &rstray) && (big_change = true))
      || (Get(cmd, "cstray",	   &cstray) && (big_change = true))
      || Get(cmd, "harmonics",	   &harmonics)
      || Get(cmd, "trstepgrow",    &trstepgrow,  mPOSITIVE)
      || Get(cmd, "trstephold",    &trstephold,  mPOSITIVE)
      || Get(cmd, "trstepshrink",  &trstepshrink,mPOSITIVE)
      || Get(cmd, "trreject",      &trreject,	mPOSITIVE)
      || Get(cmd, "trsteporder",   &trsteporder)
      || Get(cmd, "trstepcoef1",   &trstepcoef[1])
      || Get(cmd, "trstepcoef2",   &trstepcoef[2])
      || Get(cmd, "trstepcoef3",   &trstepcoef[3])
      || Get(cmd, "showall",	   &showall)
      || Get(cmd, "foooo",	   &foooo)
      || Get(cmd, "diode{flags}",  &diodeflags,  mOCTAL)
      || Get(cmd, "mos{flags}",    &mosflags,	mOCTAL)
      || Get(cmd, "quitconv{fail}",&quitconvfail)
      || Get(cmd, "edit",	   &edit)
      || Get(cmd, "recur{sion}",   &recursion)
      || (Get(cmd, "lang{uage}",   &language)
	  && ((case_insensitive = ((language) ? (language->case_insensitive()) : false)),
	      (units = ((language) ? (language->units()) : uSI)), true))
      || Get(cmd, "insensitive",   &case_insensitive)
      || (cmd.umatch("units {=}") &&
	  (ONE_OF
	   || Set(cmd, "si",	&units,	uSI)
	   || Set(cmd, "spice", &units,	uSPICE)
	   || cmd.warn(bWARNING, "need si or spice")))
      || Get(cmd, "itl1",	   &itl[1])
      || Get(cmd, "itl2",	   &itl[2])
      || Get(cmd, "itl3",	   &itl[3])
      || Get(cmd, "itl4",	   &itl[4])
      || Get(cmd, "itl5",	   &itl[5])
      || Get(cmd, "itl6",	   &itl[6])
      || Get(cmd, "itl7",	   &itl[7])
      || Get(cmd, "itl8",	   &itl[8])
      || (cmd.check(bWARNING, "what's this?"), cmd.skiparg());

    if (!cmd.stuck(&here)) {
      changed = true;
    }
  }while (cmd.more() && changed);

  if (big_change) {
    //_sim->uninit();
    //BUG// not sure if this is really working
    //regressions do go both ways, but not sure if it actually
    //makes the topology changes expected
  }else{
  }

  if (changed) {
    lowlim = 1 - reltol;
    uplim  = 1 + reltol;
    numdgt = to_range(3, numdgt, 20);
  }
  return changed;
}
/*--------------------------------------------------------------------------*/
/* print: "print" all option values to "o"
 * string is in a form suitable for passing to set
 */
void OPT::print(OMSTREAM& o)
{
  o.setfloatwidth(7);

  o << "* i/o\n";
  o << ".options";
  o << ((acct)   ?"  acct" :"  noacct");
  o << ((listing)?"  list" :"  nolist");
  o << ((clobber) ? "  clobber" : "  noclobber");
  o << "  out="    << outwidth;
  o << "  ydivisions=" << ydivisions;
  o << "  phase="  << phase;
  o << "  harmonics="   << harmonics;
  o << ((edit)	?"  edit"    :"  noedit");
  o << "  language=" << language;
  o << ((case_insensitive) ?"  insensitive":"  noinsensitive");
  o << "  units=" << units;
  o << "  recursion="<< recursion;
  o << "\n\n";

  o << "* accuracy, i/o\n";
  o << ".options";
  o << "  numdgt=" << numdgt;
  o << "  floor="  << floor;
  o << "  vfloor=" << vfloor;
  o << "  roundofftol=" << roundofftol;
  o << "\n\n";

  o << "* accuracy, tolerances\n";
  o << ".options";
  o << "  gmin="   << gmin;
  o << "  short="  << shortckt;
  o << "  reltol=" << reltol;
  o << "  abstol=" << abstol;
  o << "  vntol="  << vntol;
  o << "  trtol="  << trtol;
  o << "  chgtol=" << chgtol;
  o << "  pivtol=" << pivtol;
  o << "  bypasstol=" << bypasstol;
  o << "  loadtol=" << loadtol;
  o << "\n\n";

  o << "* accuracy, algorithms\n";
  o << ".options";
  o << "  method=" << method;
  o << ((bypass)   ?"  bypass"   :"  nobypass");
  o << ((incmode)  ?"  incmode"  :"  noincmode");    
  o << ((lcbypass) ?"  lcbypass" :"  nolcbypass");    
  o << ((lubypass) ?"  lubypass" :"  nolubypass");    
  o << ((fbbypass) ?"  fbbypass" :"  nofbbypass");    
  o << ((traceload)?"  traceload":"  notraceload");    
  o << "  order="  << order;
  o << "  mode="   << mode;
  o << "  transits=" << transits;
  o << ((quitconvfail)?"  quitconvfail":"  noquitconvfail");
  o << "\n\n";

  o << "* iteration limiting and heuristics\n";
  o << ".options";
  for (int ii=1;  ii<ITL_COUNT;  ii++) {
    o << "  itl@" << ii << "=" << itl[ii];
  }
  o << "  itermin="<< itermin;
  o << "  vmax="   << vmax;
  o << "  vmin="   << vmin;
  o << "  dampmax="<< dampmax;
  o << "  dampmin="<< dampmin;
  o << "  dampstrategy="<< octal(dampstrategy);
  o << "\n\n";

  o << "* time step control\n";
  o << ".options";
  o << "  dtmin="  << dtmin;
  o << "  dtratio="<< dtratio;
  o << "  trstepgrow="  << trstepgrow;
  o << "  trstephold="  << trstephold;
  o << "  trstepshrink="<< trstepshrink;
  o << "  trreject="    << trreject;
  o << "  trsteporder="	<< trsteporder;
  o << "  trstepcoef1="	<< trstepcoef[1];
  o << "  trstepcoef2="	<< trstepcoef[2];
  o << "  trstepcoef3="	<< trstepcoef[3];
  o << "\n\n";

  o << "* circuit environment\n";
  o << ".options";
  o << "  tnom="   << tnom_c;
  o << "  temperature="<< temp_c;
  o << ((rstray)?"  rstray":"  norstray");
  o << ((cstray)?"  cstray":"  nocstray");
  o << "  defl="   << defl;
  o << "  defw="   << defw;
  o << "  defad="  << defad;
  o << "  defas="  << defas;
  if (diodeflags) {
    o << "  diodeflags="  << octal(diodeflags);
  }
  if (mosflags) {
    o << "  mosflags="    << octal(mosflags);
  }
  o << "\n\n";

  // compatibility options ignored
  //o << ((mod)    ?"  mod"  :"  nomod");
  //o << ((page)   ?"  page" :"  nopage");
  //o << ((node)   ?"  node" :"  nonode");
  //o << ((opts)   ?"  opts" :"  noopts");
  //o << ((dupcheck) ?"  dupcheck" :"  nodupcheck");
  //o << "  maxord=" << maxord;
  //o << "  cptime=" << cptime;
  //o << "  limtim=" << limtim;
  //o << "  limpts=" << limpts;
  //o << "  lvlcod=" << lvlcod;
  //o << "  lvltim=" << lvltim;
  //o << "  pivrel=" << pivrel;
}
/*--------------------------------------------------------------------------*/
namespace {
  class CMD_OPT : public CMD {
  public:
    void do_it(CS& cmd, CARD_LIST*) {
      static OPT o;
      o.command(cmd);
    }
  } p5;
  DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "options|set|width", &p5);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
