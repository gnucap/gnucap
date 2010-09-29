/*$Id: d_logicmod.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx vdd out in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=script 2006.07.17
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
//		MODEL_LOGIC::MODEL_LOGIC();
//	void	MODEL_LOGIC::parse_spice(CS& cmd);
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
   rise   (delay / 2),
   fall   (delay / 2),
   rs     (100.),
   rw     (1e9),
   th1    (.75),
   th0    (.25),
   mr     (5.),
   mf     (5.),
   over   (.1),
   range  (vmax - vmin)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::parse_spice(CS& cmd)
{
  cmd.reset();
  cmd.skiparg();		/* skip known ".model" */
  parse_label(cmd);
  cmd.skiparg();		/* skip known "logic" */
  cmd.skip1b('(');
  int here = cmd.cursor();
  do{
    ONE_OF
      || get(cmd, "DElay",	&delay)	//,	mPOSITIVE)
      || get(cmd, "VMAx",	&vmax)
      || get(cmd, "VMIn",	&vmin)
      || get(cmd, "Unknown",	&unknown)
      || get(cmd, "RIse",	&rise)	//,	mPOSITIVE)
      || get(cmd, "FAll",	&fall)	//,	mPOSITIVE)
      || get(cmd, "RS",		&rs)	//,	mPOSITIVE)
      || get(cmd, "RW",		&rw)	//,	mPOSITIVE)
      || get(cmd, "THH",	&th1)
      || get(cmd, "THL",	&th0)
      || get(cmd, "MR",		&mr)	//,	mPOSITIVE)
      || get(cmd, "MF",		&mf)	//,	mPOSITIVE)
      || get(cmd, "OVer",	&over)	//,	mPOSITIVE)
      ;
  }while (cmd.more() && !cmd.stuck(&here));
  cmd.skip1b(')');
  cmd.check(bWARNING, "what's this?");
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::elabo1()
{
  if (1 || !evaluated()) {
    const CARD_LIST* par_scope = scope();
    assert(par_scope);
    MODEL_CARD::elabo1();
    delay.e_val(1e-9, par_scope);
    vmax.e_val(5., par_scope);
    vmin.e_val(0., par_scope);
    unknown.e_val((vmax+vmin)/2, par_scope);
    rise.e_val(delay / 2, par_scope);
    fall.e_val(delay / 2, par_scope);
    rs.e_val(100., par_scope);
    rw.e_val(1e9, par_scope);
    th1.e_val(.75, par_scope);
    th0.e_val(.25, par_scope);
    mr.e_val(5., par_scope);
    mf.e_val(5., par_scope);
    over.e_val(.1, par_scope);
    range = vmax - vmin;
  }else{
    unreachable();
  }

}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::print_spice(OMSTREAM& where, int)const
{untested();
  where.setfloatwidth(7)
    << ".model " << short_label() << " logic ("
    <<   "delay="  << delay
    << "  vmax="   << vmax
    << "  vmin="   << vmin
    << "  unknown="<< unknown
    << "  rise="   << rise
    << "  fall="   << fall
    << "  rs="     << rs
    << "  rw="     << rw
    << "  thh="    << th1
    << "  thl="    << th0
    << "  mr="     << mf
    << "  mf="     << mr
    << "  over="   << over
    << ")\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
