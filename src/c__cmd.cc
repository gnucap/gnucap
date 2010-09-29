/*$Id: c__cmd.cc,v 25.96 2006/08/28 05:45:51 al Exp $ -*- C++ -*-
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
 * command interpreter and dispatcher
 */
//testing=script 2006.07.17
#include "u_status.h"
#include "declare.h"	/* plclose */
#include "u_opt.h"
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
  bool get_timer_was_running = ::status.get.is_running();
  ::status.get.stop();

  CS cmd(cs);
  static TIMER timecheck;
  bool didsomething = true;
  
  error(bTRACE, "%s\n", cmd.fullstring());
  
  timecheck.stop().reset().start();

  cmd.ematch(ANTI_COMMENT);
  while (cmd.ematch(I_PROMPT)) {itested();
    /* skip any number of these */
  }

       if (cmd.pmatch("AC"))         {            ac(cmd);}
  else if (cmd.pmatch("ALArm"))      {            alarm(cmd);}
  else if (cmd.pmatch("ALTer"))      {untested(); modify(cmd);}
  else if (cmd.pmatch("Build"))      {itested();  build(cmd);}
  else if (cmd.pmatch("CHDir"))      {untested(); chdir(cmd);}
  else if (cmd.pmatch("CDir"))       {untested(); chdir(cmd);}
  else if (cmd.pmatch("CLEAR"))      {            clear(cmd);}
  else if (cmd.pmatch("DC"))         {            dc(cmd);}
  else if (cmd.pmatch("DELete"))     {            del(cmd);}
  else if (cmd.pmatch("DIsto"))      {untested(); disto(cmd);}
  else if (cmd.pmatch("EDit"))       {untested(); edit(cmd);}
  else if (cmd.pmatch("END"))        {            end(cmd);}
  else if (cmd.pmatch("EXIt"))       {untested(); quit(cmd);}
  //else if (cmd.pmatch("FANout"))   {untested(); fanout(cmd);}
  else if (cmd.pmatch("FAult"))      {            fault(cmd);}
  else if (cmd.pmatch("FOurier"))    {            fourier(cmd);}
  else if (cmd.pmatch("GENerator"))  {            generator(cmd);}
  else if (cmd.pmatch("GET"))        {            get(cmd);}
  else if (cmd.pmatch("IC"))         {untested(); ic(cmd);}
  else if (cmd.pmatch("INClude"))    {untested(); include(cmd);}
  else if (cmd.pmatch("LISt"))       {            do_list(cmd);}
  else if (cmd.pmatch("LOg"))        {untested(); logger(cmd);}
  else if (cmd.pmatch("MACro"))      {untested(); subckt(cmd);}
  else if (cmd.pmatch("MARk"))       {untested(); mark(cmd);}
  else if (cmd.pmatch("MErge"))      {untested(); merge(cmd);}
  else if (cmd.pmatch("MODEL"))      {untested(); model(cmd);}
  else if (cmd.pmatch("Modify"))     {            modify(cmd);}
  else if (cmd.pmatch("NODeset"))    {untested(); nodeset(cmd);}
  else if (cmd.pmatch("NOIse"))      {untested(); noise(cmd);}
  else if (cmd.pmatch("OP"))         {            op(cmd);}
  else if (cmd.pmatch("OPTions"))    {            options(cmd);}
  else if (cmd.pmatch("PARameter"))  {            param(cmd);}
  else if (cmd.pmatch("PAUse"))      {untested(); pause(cmd);}
  else if (cmd.pmatch("PLot"))       {            plot(cmd);}
  else if (cmd.pmatch("PRint"))      {            print(cmd);}
  else if (cmd.pmatch("PRobe"))      {untested(); print(cmd);}
  else if (cmd.pmatch("Quit"))       {itested();  quit(cmd);}
  else if (cmd.pmatch("REstore"))    {untested(); restore(cmd);}
  else if (cmd.pmatch("RM"))         {untested(); del(cmd);}
  else if (cmd.pmatch("SAve"))       {untested(); save(cmd);}
  else if (cmd.pmatch("SENs"))       {untested(); sens(cmd);}
  else if (cmd.pmatch("SET"))        {untested(); options(cmd);}
  else if (cmd.pmatch("SPectrum"))   {untested(); fourier(cmd);}
  else if (cmd.pmatch("STatus"))     {            status(cmd);}
  else if (cmd.pmatch("SUbckt"))     {untested(); subckt(cmd);}
  else if (cmd.pmatch("SWeep"))      {untested(); sweep(cmd);}
  else if (cmd.pmatch("TEmperature")){untested(); temp(cmd);}
  else if (cmd.pmatch("TF"))         {untested(); tf(cmd);}
  else if (cmd.pmatch("TItle"))      {untested(); title(cmd);}
  else if (cmd.pmatch("TRansient"))  {            tr(cmd);}
  else if (cmd.pmatch("UNFault"))    {            unfault(cmd);}
  else if (cmd.pmatch("UNMark"))     {untested(); unmark(cmd);}
  else if (cmd.pmatch("Width"))      {            options(cmd);}
  else if (cmd.pmatch("!$$"))        {untested(); system(cmd);}
  else if (cmd.pmatch("<$$"))        {            run(cmd);}
  else if (cmd.pmatch(">$$"))        {untested(); file(cmd);}
  else{ /* comment or error */
    comment(cmd);
    didsomething = false;
  }
  if (OPT::acct  &&  didsomething) {untested();
    IO::mstdout.form("time=%8.2f\n", timecheck.check().elapsed());
  }else{
  }
  plclose();
  IO::suppresserrors = false;
  outreset();

  if (get_timer_was_running) {
    ::status.get.start();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
