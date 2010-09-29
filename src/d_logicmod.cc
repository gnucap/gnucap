/*$Id: d_logicmod.cc,v 24.5 2003/04/27 01:05:05 al Exp $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx vdd out in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
#include "ap.h"
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
//		MODEL_LOGIC::MODEL_LOGIC();
//	void	MODEL_LOGIC::parse(CS& cmd);
// 	void	MODEL_LOGIC::print(int,int)const;
/*--------------------------------------------------------------------------*/
int MODEL_LOGIC::_count = -1; // there is one in e_node.cc
/*--------------------------------------------------------------------------*/
MODEL_LOGIC::MODEL_LOGIC()
  :MODEL_CARD(),
   delay  (1e-9),
   vmax   (5.),
   vmin	  (0.),
   unknown((vmax+vmin)/2),
   range  (vmax - vmin),
   rise   (delay / 2),
   fall   (delay / 2),
   rs     (100.),
   rw     (1e9),
   th1    (.75),
   th0    (.25),
   mr     (5.),
   mf     (5.),
   over   (.1)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::parse(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();		/* skip known ".model" */
  parse_label(cmd);
  cmd.skiparg();		/* skip known "logic" */
  cmd.skiplparen();
  int here = cmd.cursor();
  do{
    0
      || get(cmd, "DElay",	&delay,	mPOSITIVE)
      || get(cmd, "RIse",	&rise,	mPOSITIVE)
      || get(cmd, "FAll",	&fall,	mPOSITIVE)
      || get(cmd, "RS",		&rs,	mPOSITIVE)
      || get(cmd, "RW",		&rw,	mPOSITIVE)
      || get(cmd, "THH",	&th1)
      || get(cmd, "THL",	&th0)
      || get(cmd, "MR",		&mr,	mPOSITIVE)
      || get(cmd, "MF",		&mf,	mPOSITIVE)
      || get(cmd, "OVer",	&over,	mPOSITIVE)
      || get(cmd, "VMAx",	&vmax)
      || get(cmd, "VMIn",	&vmin)
      || get(cmd, "Unknown",	&unknown)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skiprparen();
  cmd.check(bWARNING, "what's this?");
  range = vmax - vmin;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::print(OMSTREAM& where, int)const
{
  where.setfloatwidth(7)
    << ".model " << short_label() << " logic ("
    <<   "delay="  << delay
    << "  rise="   << rise
    << "  fall="   << fall
    << "  rs="     << rs
    << "  rw="     << rw
    << "  thh="    << th1
    << "  thl="    << th0
    << "  mr="     << mf
    << "  mf="     << mr
    << "  over="   << over
    << "  vmax="   << vmax
    << "  vmin="   << vmin
    << "  unknown="<< unknown
    << ")\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
