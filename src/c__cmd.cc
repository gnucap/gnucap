/*$Id: c__cmd.cc,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
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
 * command interpreter and dispatcher
 */
#include "declare.h"	/* plclose */
#include "u_opt.h"
#include "l_timer.h"
#include "ap.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
//	void	CMD::cmdproc(const char*);
/*--------------------------------------------------------------------------*/
/* cmdproc: process a command
 * parse, and act on, a command string
 */
void CMD::cmdproc(const std::string& cs)
{
  CS cmd(cs);
  static TIMER timecheck;
  bool didsomething = true;
  
  error(bTRACE, "%s\n", cmd.fullstring());
  
  timecheck.stop().reset().start();

       if (cmd.pmatch("Ac"))	   ac(cmd);
  else if (cmd.pmatch("ALArm"))    alarm(cmd);
  else if (cmd.pmatch("ALTer"))    alter(cmd);
  else if (cmd.pmatch("Build"))    build(cmd);
  else if (cmd.pmatch("CHDir"))    chdir(cmd);
  else if (cmd.pmatch("CDir"))	   chdir(cmd);
  else if (cmd.pmatch("CLEAR"))    clear(cmd);
  else if (cmd.pmatch("CRTSET"))   crtset(cmd);
  else if (cmd.pmatch("DC"))	   dc(cmd);
  else if (cmd.pmatch("DELete"))   del(cmd);
  else if (cmd.pmatch("DIsto"))    disto(cmd);
  else if (cmd.pmatch("Edit"))	   edit(cmd);
  else if (cmd.pmatch("END"))	   end(cmd);
  else if (cmd.pmatch("ENDS"))	   ends(cmd);
  else if (cmd.pmatch("EOM"))	   ends(cmd);
  else if (cmd.pmatch("EXIt"))	   quit(cmd);
  else if (cmd.pmatch("FANout"))   fanout(cmd);
  else if (cmd.pmatch("FAult"))    fault(cmd);
  else if (cmd.pmatch("FOurier"))  fourier(cmd);
  else if (cmd.pmatch("Generator"))generator(cmd);
  else if (cmd.pmatch("GET"))	   get(cmd);
  else if (cmd.pmatch("IC"))	   ic(cmd);
  else if (cmd.pmatch("INClude"))  include(cmd);
  else if (cmd.pmatch("INSert"))   insert(cmd);
  else if (cmd.pmatch("List"))	   do_list(cmd);
  else if (cmd.pmatch("LOg"))	   logger(cmd);
  else if (cmd.pmatch("MACro"))	   subckt(cmd);
  else if (cmd.pmatch("MArk"))	   mark(cmd);
  else if (cmd.pmatch("MErge"))    merge(cmd);
  else if (cmd.pmatch("MODEl"))    model(cmd);
  else if (cmd.pmatch("Modify"))   modify(cmd);
  else if (cmd.pmatch("NODeset"))  nodeset(cmd);
  else if (cmd.pmatch("NOIse"))    noise(cmd);
  else if (cmd.pmatch("OP"))	   op(cmd);
  else if (cmd.pmatch("OPTions"))  options(cmd);
  else if (cmd.pmatch("PAuse"))    pause(cmd);
  else if (cmd.pmatch("PLot"))	   plot(cmd);
  else if (cmd.pmatch("PRint"))    print(cmd);
  else if (cmd.pmatch("PRobe"))    print(cmd);
  else if (cmd.pmatch("Quit"))	   quit(cmd);
  else if (cmd.pmatch("Restore"))  restore(cmd);
  else if (cmd.pmatch("RM"))	   del(cmd);
  else if (cmd.pmatch("SAve"))	   save(cmd);
  else if (cmd.pmatch("SENs"))	   sens(cmd);
  else if (cmd.pmatch("SEt"))	   options(cmd);
  else if (cmd.pmatch("SPectrum")) fourier(cmd);
  else if (cmd.pmatch("STatus"))   status(cmd);
  else if (cmd.pmatch("SUbckt"))   subckt(cmd);
  else if (cmd.pmatch("SWeep"))    sweep(cmd);
  else if (cmd.pmatch("TEmp"))	   temp(cmd);
  else if (cmd.pmatch("TF"))	   tf(cmd);
  else if (cmd.pmatch("TItle"))    title(cmd);
  else if (cmd.pmatch("TRansient"))tr(cmd);
  else if (cmd.pmatch("UNFault"))  unfault(cmd);
  else if (cmd.pmatch("UNMark"))   unmark(cmd);
  else if (cmd.pmatch("Width"))    options(cmd);
  else if (cmd.pmatch("!$$"))	   system(cmd);
  else if (cmd.pmatch("<$$"))	   run(cmd);
  else if (cmd.pmatch(">$$"))	   file(cmd);
  else{    /* comment or error */
    comment(cmd);
    didsomething = false;
  }
  if (OPT::acct  &&  didsomething){
    //untested();
    IO::mstdout.form("time=%8.2f\n", timecheck.check().elapsed());
    //untested();
  }
  plclose();
  IO::suppresserrors = false;
  outreset();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
