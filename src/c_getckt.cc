/*$Id: c_getckt.cc,v 25.95 2006/08/26 01:23:57 al Exp $ -*- C++ -*-
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
 * build, get, merge, "<" commands
 * process circuit files, and keyboard entry
 */
//testing=script,sparse 2006.07.17
#include "bmm_semi.h"
#include "bmm_table.h"
#include "d_admit.h"
#include "d_bjt.h"
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
#include "d_mos8.h"
#include "d_res.h"
#include "d_subckt.h"
#include "d_trln.h"
#include "d_vcr.h"
#include "d_vcvs.h"
#include "d_vs.h"
#include "d_switch.h"
#include "ap.h"
#include "declare.h"	/* getcmd */
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
enum Skip_Header {NO_HEADER, SKIP_HEADER};
/*--------------------------------------------------------------------------*/
//	void	CMD::build(CS&);
static	void	do_build(CARD* owner, CARD_LIST::fat_iterator putbefore);
//	void	CMD::get(CS&);
//	void	CMD::merge(CS&);
//	void	CMD::run(CS&);
static  void    getmerge(CS&, Skip_Header);
static	void	do_getmerge(CARD* owner, FILE* filen, bool, bool);
static	CARD*	check_create_insert_parse(CS&,bool,CARD_LIST::fat_iterator&,
					  CARD*);
static	void	skip_pre_stuff(CS& cmd);
static	CARD*	new_device(CS&);
static	CARD*	do_G(CS&);
static	CARD*	do_dot(CS&);
static	CARD*	do_model(CS&);
static	CARD*	do_mos_model(CS& cmd);
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
  itested();
  SET_RUN_MODE xx(rPRESET);
  ::status.get.reset().start();
  SIM::uninit();
  do_build(NULL, findbranch(cmd, &CARD_LIST::card_list));
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
static void do_build(CARD* owner, CARD_LIST::fat_iterator putbefore)
{
  itested();
  for (;;) {
    itested();
    // get a line
    char buffer[BIGBUFLEN];
    getcmd(CKT_PROMPT, buffer, BIGBUFLEN);
    CS cmd(buffer);

    // exit if done
    if (cmd.is_end() || cmd.pmatch(".ENDs") || cmd.pmatch(".EOM")) {
      itested();
      break;
    }else{
      itested();
    }
    // check for dups, create, insert, parse
    CARD* brh = check_create_insert_parse(cmd, true, putbefore, owner);
    //                                         ^^dup_check

    // recursively process subcircuits
    if (dynamic_cast<MODEL_SUBCKT*>(brh)) {
      untested();
      assert(brh->subckt());
      do_build(brh, 
	CARD_LIST::fat_iterator((brh->subckt()),brh->subckt()->end()));
    }else{
      itested();
    }
  }
}
/*--------------------------------------------------------------------------*/
/* cmd_get: get command
 * get circuit from a file, after clearing the old one
 * preset, but do not execute "dot cards"
 */
void CMD::get(CS& cmd)
{
  SET_RUN_MODE xx(rPRESET);
  ::status.get.reset().start();
  CS nil("");
  clear(nil);
  getmerge(cmd, SKIP_HEADER);
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_include: include command
 * as get or run, but do not clear first, inherit the run-mode.
 */
void CMD::include(CS& cmd)
{
  untested();
  ::status.get.reset().start();
  getmerge(cmd, NO_HEADER);
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_merge: merge command
 * as get, but do not clear first
 */
void CMD::merge(CS& cmd)
{
  untested();
  SET_RUN_MODE xx(rPRESET);
  ::status.get.reset().start();
  getmerge(cmd, NO_HEADER);
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* cmd_run: "<" and "<<" commands
 * run in batch mode.  Spice format.
 * "<<" clears old circuit first, "<" does not
 * get circuit from file, execute dot cards in sequence
 */
void CMD::run(CS& cmd)
{
  ::status.get.reset().start();
  while (cmd.match1('<')) {
    untested();
    CS nil("");
    clear(nil);
    cmd.skip();
    cmd.skipbl();
  }
  SET_RUN_MODE xx(rSCRIPT);
  getmerge(cmd, SKIP_HEADER);
  ::status.get.stop();
}
/*--------------------------------------------------------------------------*/
/* getmerge: actually do the work for "get", "merge", etc.
 */
static void getmerge(CS& cmd, Skip_Header skip_header)
{
  SIM::uninit();

  FILE* filen = xopen(cmd,"","r");
  if (!filen) {
    untested();
    error(bERROR, "");
  }
  
  bool  echoon = false;		/* echo on/off flag (echo as read from file) */
  bool  liston = false;		/* list on/off flag (list actual values) */
  bool  quiet = false;		/* don't echo title */
  int here = cmd.cursor();
  do{
    ONE_OF
      || get(cmd, "Echo",  &echoon)
      || get(cmd, "List",  &liston)
      || get(cmd, "Quiet", &quiet)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.check(bWARNING, "need echo, list, or quiet");

  if (skip_header) { // get and store the header line
    std::string buffer(getlines(filen)); // title
    if (buffer.empty()) {
      untested();
      error(bWARNING, "empty circuit file\n");
    }
    head = buffer;

    if (!quiet) {
      IO::mstdout << head << '\n';
    }else{
      untested();
    }
  }else{
    untested();
  }

  do_getmerge(NULL, filen, echoon, liston);
  
  xclose(&filen);
}
/*--------------------------------------------------------------------------*/
static void do_getmerge(CARD* owner, FILE* filen, bool echoon, bool liston)
{
  CARD_LIST* scope = (owner) 
    ? owner->subckt() 
    : &CARD_LIST::card_list;
  assert(scope);

  CARD_LIST::fat_iterator putbefore(scope, scope->end());
  
  for (;;) {
    // get a line, with extensions
    std::string buffer(getlines(filen));
    CS cmd(buffer);
    
    // print as in file
    if (echoon) {
      untested();
      IO::mstdout << buffer << '\n';
    }
    
    // exit if done
    if (buffer.empty() || cmd.pmatch(".ENDs") || cmd.pmatch(".EOM")) {
      break;
    }
    
    // check for dups, create, insert, parse
    CARD* brh = check_create_insert_parse(cmd,OPT::dupcheck,putbefore,owner);
    
    // print as interpreted
    if (liston  &&  exists(brh)) {
      untested();
      brh->print_spice(IO::mstdout, false);
    }
    // recursively process subcircuits
    if (dynamic_cast<MODEL_SUBCKT*>(brh)) {
      do_getmerge(brh, filen, echoon, liston);
    }
  }
}
/*--------------------------------------------------------------------------*/
/* check_create_insert_parse: parse an input line, process it, store it.
 */
static CARD *check_create_insert_parse(CS& cmd, bool dup_check,
				       CARD_LIST::fat_iterator& putbefore,
				       CARD* owner)
{
  CARD* brh = NULL;
  
  // check for dups
  if (dup_check) {
    itested();
    skip_pre_stuff(cmd);
    CARD_LIST::fat_iterator ci = findbranch(cmd, putbefore.list());
    cmd.reset();
    brh = *ci;
  }
  
  // create
  if (!exists(brh)) {
    brh = new_device(cmd);
    if (exists(brh)) {
      brh->set_owner(owner);
      skip_pre_stuff(cmd);
      brh->parse_spice(cmd);
      // parse must be before insert, so get or clear doesn't clear self
      putbefore.insert(brh);
    }
  }else{
    untested();
    assert(brh->owner() == owner);
    skip_pre_stuff(cmd);
    brh->parse_spice(cmd);
    // no insert.  it's already there
  }
  
  return brh;
}
/*--------------------------------------------------------------------------*/
static void skip_pre_stuff(CS& cmd)
{
  cmd.skipbl();
  while (cmd.ematch(CKT_PROMPT)) {
    untested();
    /* skip any number of copies of the prompt */
  }
  cmd.ematch(ANTI_COMMENT); /* skip mark so spice ignores but gnucap reads */
}
/*--------------------------------------------------------------------------*/
/* new_device: create a new device of specified type
 * Looks at first letter or keyword to determine type.
 */
static CARD *new_device(CS& cmd)
{
  skip_pre_stuff(cmd);

  CARD* brh = NULL;
  char id_letter = toupper(cmd.peek());
  switch (id_letter) {
    case '\0':	/* nothing */			    break;
    case '.':	brh = do_dot(cmd);		    break;
    case '"':	untested();
    case ';':	untested();
    case '\'':
    case '#':
    case '*':	brh = new DEV_COMMENT;		    break;
    case 'A':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'B':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'C':	brh = new DEV_CAPACITANCE;	    break;
    case 'D':	brh = new DEV_DIODE;		    break;
    case 'E':	brh = new DEV_VCVS;		    break;
    case 'F':	brh = new DEV_CCCS;		    break;
    case 'G':	brh = do_G(cmd);		    break;
    case 'H':	brh = new DEV_CCVS;		    break;
    case 'I':	brh = new DEV_CS;		    break;
    case 'J':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'K':	brh = new DEV_MUTUAL_L;		    break;
    case 'L':	brh = new DEV_INDUCTANCE;	    break;
    case 'M':	brh = new DEV_MOS;		    break;
    case 'N':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'O':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'P':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    case 'Q':	brh = new DEV_BJT;		    break;
    case 'R':	brh = new DEV_RESISTANCE;	    break;
    case 'S':	brh = new DEV_VSWITCH;		    break;
    case 'T':	brh = new DEV_TRANSLINE;	    break;
    case 'U':	brh = new DEV_LOGIC;		    break;
    case 'V':	brh = new DEV_VS;		    break;
    case 'W':	brh = new DEV_CSWITCH;		    break;
    case 'X':	brh = new DEV_SUBCKT;		    break;
    case 'Y':	brh = new DEV_ADMITTANCE;	    break;
    case 'Z':	untested(); cmd.warn(bWARNING, "illegal type"); break;
    default:	untested(); cmd.warn(bWARNING, "illegal type"); break;
  }
  return brh;
}
/*--------------------------------------------------------------------------*/
// accept device type between the nodes.  HSPICE compatibility kluge.
static CARD* do_G(CS& cmd)
{
  int here = cmd.cursor();
  CARD* brh = NULL;
  if (cmd.pscan("VC$$")) {
    if      (cmd.pmatch("CAP"))	{untested(); brh = new DEV_VCCAP;}
    else if (cmd.pmatch("G")) 	{untested(); brh = new DEV_VCG;}
    else if (cmd.pmatch("R")) 	{brh = new DEV_VCR;}
    else    /* hope it's CS */	{untested(); brh = new DEV_VCCS;}
  }else{
    brh = new DEV_VCCS;
  }
  cmd.reset(here);
  return brh;
}
/*--------------------------------------------------------------------------*/
static CARD* do_dot(CS& cmd)
{
  cmd.skip(); // skip the dot
  if	  (cmd.pmatch("MODel"))		 {return do_model(cmd);}
  else if (cmd.pmatch("MACro"))		 {return new MODEL_SUBCKT;}
  else if (cmd.pmatch("SUBckt"))	 {return new MODEL_SUBCKT;}
  else if (cmd.pmatch("ADMittance"))	 {untested(); return new DEV_ADMITTANCE;}
  else if (cmd.pmatch("BJT"))		 {untested(); return new DEV_BJT;}
  else if (cmd.pmatch("CAPacitor"))	 {untested(); return new DEV_CAPACITANCE;}
  else if (cmd.pmatch("CCCs"))  	 {untested(); return new DEV_CCCS;}
  else if (cmd.pmatch("CCVs"))  	 {untested(); return new DEV_CCVS;}
  else if (cmd.pmatch("CSOurce"))	 {untested(); return new DEV_CS;}
  else if (cmd.pmatch("CSWitch"))	 {untested(); return new DEV_CSWITCH;}
  else if (cmd.pmatch("DIOde")) 	 {untested(); return new DEV_DIODE;}
  else if (cmd.pmatch("INDuctor"))	 {untested(); return new DEV_INDUCTANCE;}
  else if (cmd.pmatch("ISOurce"))	 {return new DEV_CS;}
  else if (cmd.pmatch("ISWitch"))	 {untested(); return new DEV_CSWITCH;}
  else if (cmd.pmatch("MOSfet"))	 {untested(); return new DEV_MOS;}
  else if (cmd.pmatch("MUTual_inductor")){untested(); return new DEV_MUTUAL_L;}
  else if (cmd.pmatch("RESistor"))	 {untested(); return new DEV_RESISTANCE;}
  else if (cmd.pmatch("TCApacitor")) 	 {return new DEV_TRANSCAP;}
  else if (cmd.pmatch("TLIne")) 	 {untested(); return new DEV_TRANSLINE;}
  else if (cmd.pmatch("VCCAp"))  	 {return new DEV_VCCAP;}
  else if (cmd.pmatch("VCCS"))  	 {untested(); return new DEV_VCCS;}
  else if (cmd.pmatch("VCG"))   	 {return new DEV_VCG;}
  else if (cmd.pmatch("VCR"))   	 {return new DEV_VCR;}
  else if (cmd.pmatch("VCVs"))  	 {untested(); return new DEV_VCVS;}
  else if (cmd.pmatch("VSOurce"))	 {return new DEV_VS;}
  else if (cmd.pmatch("VSWitch"))	 {untested(); return new DEV_VSWITCH;}
  else					 {return new DEV_DOT;}
}
/*--------------------------------------------------------------------------*/
static CARD* do_model(CS& cmd)
{
  cmd.skiparg(); // skip name
  if      (cmd.pmatch("D"    )) {return new MODEL_DIODE;}
  else if (cmd.pmatch("NPN"  )) {return new MODEL_BJT;}
  else if (cmd.pmatch("PNP"  )) {return new MODEL_BJT;}
  else if (cmd.pmatch("NJF"  )) {untested(); cmd.warn(bWARNING, "not implemented");}
  else if (cmd.pmatch("PJF"  )) {untested(); cmd.warn(bWARNING, "not implemented");}
  else if (cmd.pmatch("NMOS" )) {return do_mos_model(cmd);}
  else if (cmd.pmatch("PMOS" )) {return do_mos_model(cmd);}
  else if (cmd.pmatch("Logic")) {return new MODEL_LOGIC;}
  else if (cmd.pmatch("SW"   )) {return new MODEL_SWITCH;}
  else if (cmd.pmatch("CSW"  )) {return new MODEL_SWITCH;}
  else if (cmd.pmatch("Res"  )) {return new MODEL_SEMI_RESISTOR;}
  else if (cmd.pmatch("Cap"  )) {return new MODEL_SEMI_CAPACITOR;}
  else if (cmd.pmatch("TABLE")) {return new MODEL_TABLE;}
  else				{untested(); cmd.warn(bWARNING, "illegal type");}
  {untested(); return NULL;}
}
/*--------------------------------------------------------------------------*/
static CARD* do_mos_model(CS& cmd)
{
  /* already parsed to know it is either NMos or PMos */
  cmd.skip1b('(');
  int level = 1;
  scan_get(cmd, "LEvel", &level) 
    || cmd.warn(bWARNING, "no level specified, using 1");
  switch (level) {
  default:untested();
  case 1: return new MODEL_MOS1;
  case 2: return new MODEL_MOS2;
  case 3: return new MODEL_MOS3;
  case 4: return new MODEL_MOS4;
  case 5: return new MODEL_MOS5;
  case 6: return new MODEL_MOS6;
  case 7: return new MODEL_MOS7;
  case 8: return new MODEL_MOS8;
  case 49:untested(); return new MODEL_MOS8;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
