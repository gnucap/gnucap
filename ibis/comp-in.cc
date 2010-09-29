/*$Id: comp-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 */
#include "comp.h"
/*--------------------------------------------------------------------------*/
void Component::Pin_Die::parse(CS& file)
{
       set(file, "Pin", &_pd, pdPIN)
    || set(file, "Die", &_pd, pdDIE)
    || file.warn(0, "need Pin or Die");
}
/*--------------------------------------------------------------------------*/
void Component::Package::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "R_pkg", &_r_pkg);
    get(file, "L_pkg", &_l_pkg);
    get(file, "C_pkg", &_c_pkg);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
Component::Pin::Pin(CS& file, const int* order)
  :_r_pin(NA),
   _l_pin(NA),
   _c_pin(NA)
{
  Tail_String line_buf(file);
  CS line(line_buf);

  Float arg[3];
  line >> _name >> _signal >> _model;
  if (order) {
    line >> arg[0] >> arg[1] >> arg[2];
    _r_pin = arg[order[0]];
    _l_pin = arg[order[1]];
    _c_pin = arg[order[2]];
  }
}
/*--------------------------------------------------------------------------*/
void Component::Pin_List::parse(CS& file)
{
  if (!is_empty()) {
    file.warn(0, "duplicate [Pin] list");
  }
  file.icmatch("signal_name") || file.warn(0, "need key: signal_name");
  file.icmatch("model_name")  || file.warn(0, "need key: model_name");
  int order[3] = {0};
  {for (int i=0; i<3; ++i) {
    {if (file.dmatch("R_pin")) {
      order[0] = i;
    }else if (file.dmatch("L_pin")) {
      order[1] = i;
    }else if (file.dmatch("C_pin")) {
      order[2] = i;
    }else{
      break;
      // no pin values
    }}
  }}
  int* order_fudge = 0;
  {if (order[0]==order[1] && order[0]==order[2]) {
    // none -- ok
    order_fudge = 0;
  }else if (order[0]!=order[1] && order[0]!=order[2] && order[1]!=order[2]) {
    // all different -- ok
    order_fudge = order;
  }else{
    file.warn(0, "pin columns messed up");
    order_fudge = order;
  }}

  int here = file.cursor();
  {for (;;) {
    skip_comment(file);
    {if (file.match1('[')) {
      break;
    }else{
      Pin* x = new Pin(file, order_fudge);
      {if (!file.stuck(&here)) {
	push(x);
	_map[x->name()] = x;
      }else{
	untested();
	delete x;
	file.warn(0, "need component pin entry");
	break;
      }}
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Component::Pin_Map::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _name >> _pulldown_ref >> _pullup_ref
       >> _gnd_clamp_ref >> _pwr_clamp_ref;
}
/*--------------------------------------------------------------------------*/
void Component::Pin_Map_List::parse(CS& file)
{
  skip_tail(file);
  int here = file.cursor();
  {for (;;) {
    skip_comment(file);
    {if (file.match1('[')) {
      break;
    }else{
      Pin_Map* x = new Pin_Map(file);
      {if (!file.stuck(&here)) {
	push(x);
	_map[x->name()] = x;
      }else{
	untested();
	delete x;
	file.warn(0, "need pin mapping entry");
	break;
      }}
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Component::Diff_Pin::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _pin >> _diff_pin >> _vdiff >> _tdelay;
}
/*--------------------------------------------------------------------------*/
void Component::Series_Pin::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _pin1 >> _pin2 >> _model >> _group;
}
/*--------------------------------------------------------------------------*/
void Component::Series_Switch_List::parse(CS& file)
{
  int state = false;
  int here = file.cursor();
  {for (;;) {
    skip_comment(file);
    {if (file.dmatch("On")) {
      state = true;
    }else if (file.dmatch("Off")) {
      state = false;
    }else if (file.match1('[')) {
      break;
    }else{
      std::string group;
      file >> group;
      {if (!file.stuck(&here)) {
	_list[group] = state;
      }else{
	untested();
	file.warn(0, "series switch groups - syntax error");
	break;
      }}
    }}
    file.skip1("/");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Component::parse(CS& file)
{
  file >> _name;
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "Si_location",	&_si_location);
    get(file, "Timing_location",&_timing_location);
    get(file, "[Manufacturer]",	&_manufacturer);
    get(file, "[Package_Model]",&_package_model);
    get(file, "[Package]",	&_package);
    get(file, "[Pin_Mapping]",	&_pin_mapping);
    get(file, "[Pin]",		&_pins);
    get(file, "[Diff_Pin]",	&_diff_pins);
    get(file, "[Series_Pin_Mapping]",	&_series_pins);
    get(file, "[Series_Switch_Groups]",	&_series_switches);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
