/*$Id: c_getckt.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * build, get, merge, "<" commands
 * process circuit files, and keyboard entry
 */
#include "bmm_semi.h"
#include "bmm_table.h"
#include "d_admit.h"
#include "d_cap.h"
#include "d_cccs.h"
#include "d_ccvs.h"
#include "d_coil.h"
#include "d_coment.h"
#include "d_cs.h"
#include "d_dot.h"
#include "d_diode.h"
#include "d_logic.h"
#include "d_mos1.h"
#include "d_mos2.h"
#include "d_mos3.h"
#include "d_mos4.h"
#include "d_mos5.h"
#include "d_mos6.h"
#include "d_mos7.h"
#include "d_res.h"
#include "d_subckt.h"
//#include "d_trigger.h"
#include "d_trln.h"
#include "d_vcr.h"
#include "d_vcvs.h"
#include "d_vs.h"
#include "d_switch.h"
#include "ap.h"
#include "declare.h"	/* getcmd */
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::build(CS&);
//	void	CMD::get(CS&);
//	void	CMD::merge(CS&);
//	void	CMD::run(CS&);
static  void    getmerge(CS&);
static	CARD* parsebranch(CS&,bool);
static	CARD* cparse(CS&);
static	CARD* do_G(CS&);
static	CARD* do_dot(CS&);
static	CARD* do_model(CS&);
static	CARD* do_mos_model(CS& cmd);
/*--------------------------------------------------------------------------*/
extern std::string head;	    /* place to store title line            */
/*--------------------------------------------------------------------------*/
/* cmd_build: build command
 * get circuit description direct from keyboard (or stdin if redirected)
 * Command syntax: build <before>
 * Bare command: add to end of list
 * If there is an arg: add before that element
 * null line exits this mode
 * preset, but do not execute "dot cards"
 */
void CMD::build(CS& cmd)
{
  SET_RUN_MODE xx(rPRESET);
  STATUS::get.reset().start();
  SIM::uninit();
  CARD_LIST::putbefore = findbranch(cmd, &CARD_LIST::card_list);
  CARD* brh;
  do{
    char buffer[BIGBUFLEN];
    getcmd(">",buffer,BIGBUFLEN);
    CS cs(buffer);
    brh = parsebranch(cs, true);
  }while (exists(brh));
  STATUS::get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_get: get command
 * get circuit from a file, after clearing the old one
 * preset, but do not execute "dot cards"
 */
void CMD::get(CS& cmd)
{
  SET_RUN_MODE xx(rPRESET);
  STATUS::get.reset().start();
  CS nil("");
  clear(nil);
  getmerge(cmd);
  STATUS::get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_include: include command
 * as getor run, but do not clear first, inherit the run-mode.
 */
void CMD::include(CS& cmd)
{
  STATUS::get.reset().start();
  getmerge(cmd);
  STATUS::get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_merge: merge command
 * as get, but do not clear first
 */
void CMD::merge(CS& cmd)
{
  SET_RUN_MODE xx(rPRESET);
  STATUS::get.reset().start();
  getmerge(cmd);
  STATUS::get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_run: "<" and "<<" commands
 * run in batch mode
 * "<<" clears old circuit first, "<" does not
 * get circuit from file, execute dot cards in sequence
 */
void CMD::run(CS& cmd)
{
  STATUS::get.reset().start();
  while (cmd.match1('<')) {
    CS nil("");
    clear(nil);
    cmd.skip();
    cmd.skipbl();
  }
  SET_RUN_MODE xx(rSCRIPT);
  getmerge(cmd);
  STATUS::get.stop();
}
/*--------------------------------------------------------------------------*/
/* getmerge: actually do the work for "get", "merge", etc.
 */
static void getmerge(CS& cmd)
{
  SIM::uninit();

  FILE* filen = xopen(cmd,"","r");
  if (!filen)
    error(bERROR, "");
  
  bool  echoon = false;		/* echo on/off flag (echo as read from file) */
  bool  liston = false;		/* list on/off flag (list actual values) */
  bool  quiet = false;		/* don't echo title */
  int here = cmd.cursor();
  do{
    get(cmd, "Echo",  &echoon);
    get(cmd, "List",  &liston);
    get(cmd, "Quiet", &quiet);
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "need echo, list, or quiet");
  
  {
    std::string buffer(getlines(filen)); // title
    if (buffer.empty()) {
      untested();
      error(bWARNING, "empty circuit file\n");
    }
    head = buffer;

    {if (!quiet) {
      IO::mstdout << head << '\n';
    }else{
      untested();
    }}
  }
  CARD_LIST::putbefore = CARD_LIST::fat_iterator(&(CARD_LIST::card_list),
					    CARD_LIST::card_list.end());
  for (;;) {
    std::string buffer(getlines(filen));
    if (buffer.empty()) {
      break;
    }
    if (echoon) {
      IO::mstdout << buffer << '\n';
    }
    CS cs(buffer);
    CARD* brh = parsebranch(cs, false);
    if (liston  &&  exists(brh)) {
      brh->print(IO::mstdout, false);
    }
  }
  xclose(&filen);
}
/*--------------------------------------------------------------------------*/
/* parsebranch: parse an input line, and process it
 */
static CARD *parsebranch(CS& cmd, bool alwaysdupcheck)
{
  CARD_LIST::fat_iterator ci(CARD_LIST::putbefore);
  if (OPT::dupcheck ||  alwaysdupcheck) {
    CS buf(cmd);
    ci = findbranch(buf, ci.list());
  }
 
  CARD* brh = NULL;
  {if (!ci.isend()) { // found one like it, reparse same one
    brh = *ci;
    error(bWARNING, "replacing [" + brh->long_label() + "]\n");
    brh->parse(cmd);
  }else{
    CARD_LIST::fat_iterator before = CARD_LIST::putbefore;
    /* Need to stash where to put it, because a subckt will change it.
     * The subckt card itself needs to be placed at the old location,
     * even though parsing it (cparse) changed it.
     */
    brh = cparse(cmd);
    if (exists(brh)) {
      before.list()->insert(before, brh);
    }
  }}
  if (exists(brh)) {
    if (brh->is_device()) {
      SIM::uninit();
    }
  }
  return brh;
}
/*--------------------------------------------------------------------------*/
/* cparse: circuit parse: parse one line of a netlist
 * mostly, dispatches to the proper function.
 */
static CARD *cparse(CS& cmd)
{
  cmd.skipbl();
  if (cmd.is_digit())
    cmd.ctoi();	/* ignore line numbers */
  
  CARD* brh = NULL;
  char id_letter = toupper(cmd.peek());
  switch (id_letter) {
    case '\0':	/* nothing */			    break;
    case '.':	brh = do_dot(cmd);		    break;
    case '\'':
    case '"':
    case ';':
    case '#':
    case '*':	brh = new DEV_COMMENT;		    break;
    case 'A':	cmd.warn(bWARNING, "illegal type"); break;
    case 'B':	cmd.warn(bWARNING, "illegal type"); break;
    case 'C':	brh = new DEV_CAPACITANCE;	    break;
    case 'D':	brh = new DEV_DIODE;		    break;
    case 'E':	brh = new DEV_VCVS;		    break;
    case 'F':	brh = new DEV_CCCS;		    break;
    case 'G':	brh = do_G(cmd);		    break;
    case 'H':	brh = new DEV_CCVS;		    break;
    case 'I':	brh = new DEV_CS;		    break;
    case 'J':	cmd.warn(bWARNING, "illegal type"); break;
    case 'K':	brh = new DEV_MUTUAL_L;		    break;
    case 'L':	brh = new DEV_INDUCTANCE;	    break;
    case 'M':	brh = new DEV_MOS;		    break;
    case 'N':	cmd.warn(bWARNING, "illegal type"); break;
    case 'O':	cmd.warn(bWARNING, "illegal type"); break;
    case 'P':	cmd.warn(bWARNING, "illegal type"); break;
    case 'Q':	cmd.warn(bWARNING, "illegal type"); break;
    case 'R':	brh = new DEV_RESISTANCE;	    break;
    case 'S':	brh = new DEV_VSWITCH;		    break;
    case 'T':	brh = new DEV_TRANSLINE;	    break;
    case 'U':	brh = new DEV_LOGIC;		    break;
    case 'V':	brh = new DEV_VS;		    break;
    case 'W':	brh = new DEV_CSWITCH;		    break;
    case 'X':	brh = new DEV_SUBCKT;		    break;
    case 'Y':	brh = new DEV_ADMITTANCE;	    break;
    case 'Z':	cmd.warn(bWARNING, "illegal type"); break;
    default:	cmd.warn(bWARNING, "illegal type"); break;
  }
  if (exists(brh)) {
    assert(brh);
    brh->parse(cmd);
  }else if (brh) {
    unreachable();
    error(bWARNING, "internal error: branch has no type <%s>\n",
    	  cmd.fullstring());
    delete brh;
    brh = NULL;
  }
  return brh;
}
/*--------------------------------------------------------------------------*/
// accept device type between the nodes.  HSPICE compatibility kluge.
static CARD* do_G(CS& cmd)
{
  int here = cmd.cursor();
  CARD* brh = NULL;
  {if (cmd.pscan("VC$$")) {
    if      (cmd.pmatch("CAP"))	brh = new DEV_VCCAP;
    else if (cmd.pmatch("G")) 	brh = new DEV_VCG;
    else if (cmd.pmatch("R")) 	brh = new DEV_VCR;
    else    /* hope it's CS */	brh = new DEV_VCCS;
  }else{
    brh = new DEV_VCCS;
  }}
  cmd.reset(here);
  return brh;
}
/*--------------------------------------------------------------------------*/
static CARD* do_dot(CS& cmd)
{
  cmd.skip(); // skip the dot
  if	  (cmd.pmatch("MODel"))		 return do_model(cmd);
  else if (cmd.pmatch("MACro"))		 return new MODEL_SUBCKT;
  else if (cmd.pmatch("SUBckt"))	 return new MODEL_SUBCKT;
  else if (cmd.pmatch("ADMittance"))	 return new DEV_ADMITTANCE;
  else if (cmd.pmatch("CAPacitor"))	 return new DEV_CAPACITANCE;
  else if (cmd.pmatch("CCCs"))  	 return new DEV_CCCS;
  else if (cmd.pmatch("CCVs"))  	 return new DEV_CCVS;
  else if (cmd.pmatch("CSOurce"))	 return new DEV_CS;
  else if (cmd.pmatch("CSWitch"))	 return new DEV_CSWITCH;
  else if (cmd.pmatch("DIOde")) 	 return new DEV_DIODE;
  else if (cmd.pmatch("INDuctor"))	 return new DEV_INDUCTANCE;
  else if (cmd.pmatch("ISOurce"))	 return new DEV_CS;
  else if (cmd.pmatch("ISWitch"))	 return new DEV_CSWITCH;
  else if (cmd.pmatch("MOSfet"))	 return new DEV_MOS;
  else if (cmd.pmatch("MUTual_inductor"))return new DEV_MUTUAL_L;
  else if (cmd.pmatch("RESistor"))	 return new DEV_RESISTANCE;
  else if (cmd.pmatch("TCApacitor")) 	 return new DEV_TRANSCAP;
  else if (cmd.pmatch("TLIne")) 	 return new DEV_TRANSLINE;
  //else if (cmd.pmatch("TRIgger"))  	 return new DEV_TRIGGER;
  else if (cmd.pmatch("VCCAp"))  	 return new DEV_VCCAP;
  else if (cmd.pmatch("VCCS"))  	 return new DEV_VCCS;
  else if (cmd.pmatch("VCG"))   	 return new DEV_VCG;
  else if (cmd.pmatch("VCR"))   	 return new DEV_VCR;
  else if (cmd.pmatch("VCVs"))  	 return new DEV_VCVS;
  else if (cmd.pmatch("VSOurce"))	 return new DEV_VS;
  else if (cmd.pmatch("VSWitch"))	 return new DEV_VSWITCH;
  else					 return new DEV_DOT;	
}
/*--------------------------------------------------------------------------*/
static CARD* do_model(CS& cmd)
{
  cmd.skiparg(); // skip name
  if      (cmd.pmatch("D"    )) return new MODEL_DIODE;
  else if (cmd.pmatch("NPN"  )) cmd.warn(bWARNING, "not implemented");
  else if (cmd.pmatch("PNP"  )) cmd.warn(bWARNING, "not implemented");
  else if (cmd.pmatch("NJF"  )) cmd.warn(bWARNING, "not implemented");
  else if (cmd.pmatch("PJF"  )) cmd.warn(bWARNING, "not implemented");
  else if (cmd.pmatch("NMOS" )) return do_mos_model(cmd);
  else if (cmd.pmatch("PMOS" )) return do_mos_model(cmd);
  else if (cmd.pmatch("Logic")) return new MODEL_LOGIC;
  else if (cmd.pmatch("SW"   )) return new MODEL_SWITCH;
  else if (cmd.pmatch("CSW"  )) return new MODEL_SWITCH;
  else if (cmd.pmatch("Res"  )) return new MODEL_SEMI_RESISTOR;
  else if (cmd.pmatch("Cap"  )) return new MODEL_SEMI_CAPACITOR;
  else if (cmd.pmatch("TABLE")) return new MODEL_TABLE;
  else				cmd.warn(bWARNING, "illegal type");
  return NULL;
}
/*--------------------------------------------------------------------------*/
static CARD* do_mos_model(CS& cmd)
{
  /* already parsed to know it is either NMos or PMos */
  cmd.skiplparen();
  int level = 1;
  scan_get(cmd, "LEvel", &level) 
    || cmd.warn(bWARNING, "no level specified, using 1");
  switch (level) {
  default:
  case 1: return new MODEL_MOS1;
  case 2: return new MODEL_MOS2;
  case 3: return new MODEL_MOS3;
  case 4: return new MODEL_MOS4;
  case 5: return new MODEL_MOS5;
  case 6: return new MODEL_MOS6;
  case 7: return new MODEL_MOS7;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
