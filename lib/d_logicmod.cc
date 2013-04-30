/*$Id: d_logicmod.cc,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx vdd out in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=script 2006.07.17
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
MODEL_LOGIC::MODEL_LOGIC(const DEV_LOGIC* p)
  :MODEL_CARD(p),
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
MODEL_LOGIC::MODEL_LOGIC(const MODEL_LOGIC& p)
  :MODEL_CARD(p),
   delay  (p.delay),
   vmax   (p.vmax),
   vmin	  (p.vmin),
   unknown(p.unknown),
   rise   (p.rise),
   fall   (p.fall),
   rs     (p.rs),
   rw     (p.rw),
   th1    (p.th1),
   th0    (p.th0),
   mr     (p.mr),
   mf     (p.mf),
   over   (p.over),
   range  (p.range)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::precalc_first()
{
  MODEL_CARD::precalc_first();

  const CARD_LIST* par_scope = scope();
  assert(par_scope);

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
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: delay = value; break;
  case 1: vmax = value; break;
  case 2: vmin = value; break;
  case 3: unknown = value; break;
  case 4: rise = value; break;
  case 5: fall = value; break;
  case 6: rs = value; break;
  case 7: rw = value; break;
  case 8: th1 = value; break;
  case 9: th0 = value; break;
  case 10: mr = value; break;
  case 11: mf = value; break;
  case 12: over = value; break;
  default: MODEL_CARD::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC::param_is_printable(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: 
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5: 
  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
  case 11:
  case 12: return true;
  default: return MODEL_CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_name(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: return "delay";
  case 1: return "vmax";
  case 2: return "vmin";
  case 3: return "unknown";
  case 4: return "rise";
  case 5: return "fall";
  case 6: return "rs";
  case 7: return "rw";
  case 8: return "thh";
  case 9: return "thl";
  case 10: return "mr";
  case 11: return "mf";
  case 12: return "over";
  default: return MODEL_CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= MODEL_CARD::param_count()) {
    return "";
  }else{
    return MODEL_CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_value(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: return delay.string();
  case 1: return vmax.string();
  case 2: return vmin.string();
  case 3: return unknown.string();
  case 4: return rise.string();
  case 5: return fall.string();
  case 6: return rs.string();
  case 7: return rw.string();
  case 8: return th1.string();
  case 9: return th0.string();
  case 10: return mr.string();
  case 11: return mf.string();
  case 12: return over.string();
  default: return MODEL_CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
