/*$Id: model-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "model.h"
/*--------------------------------------------------------------------------*/
void Ramp::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "dV/dt_r", &_dv_dt_r);
    get(file, "dV/dt_f", &_dv_dt_f);
    get(file, "R_load",  &_r_load);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Wave::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "C_fixture",	&_c_fixture);
    get(file, "L_fixture",	&_l_fixture);
    get(file, "R_fixture",	&_r_fixture);
    get(file, "V_fixture_min",	&_v_fixture_min);
    get(file, "V_fixture_max",	&_v_fixture_max);
    get(file, "V_fixture",	&_v_fixture_typ);
    get(file, "C_dut",		&_c_dut);
    get(file, "L_dut",		&_l_dut);
    get(file, "R_dut",		&_r_dut);
  } while (file.more() && !file.stuck(&here));
  _v_fixture = Range<Float>(_v_fixture_typ, _v_fixture_min, _v_fixture_max);
  file >> _wave;
}
/*--------------------------------------------------------------------------*/
void Wave_List::parse(CS& file)
{
  int here = file.cursor();
  int start = here;
  Wave* x = new Wave(file, this);
  {if (!file.stuck(&here)) {
    // This is a kluge.  Ideally, it would build a sorted list.
    // For now, only the first and last matter,
    // so the rest is a mess.  It will be ignored anyway.
    assert(x);
    // extra complexity here is for development tracing
    {if (is_empty()) {
      _list.push_back(x);
    }else if (x->v_fixture_typ() > _list.front()->v_fixture_typ()) {
      // highest v_fixture at the front of the list
      _list.push_front(x);
    }else if (x->v_fixture_typ() < _list.back()->v_fixture_typ()) {
      // lowest v_fixture at the back
      _list.push_back(x);
    }else if (x->v_fixture_typ() == _list.front()->v_fixture_typ()) {
      // highest at front.  There is more than one.
      // 3 or more waves?
      {if (x->r_fixture() < _list.front()->r_fixture()) {
	// take the lowest r_fixture
	_list.push_front(x);
	file.warn(0, start, "hiding waveform with higher rfix, same vfix");
      }else if (x->r_fixture() > _list.front()->r_fixture()) {
	delete x;
	file.warn(0, start, "dropping waveform with higher rfix, same vfix");
      }else if (x->l_fixture() < _list.front()->l_fixture()) {
	_list.push_front(x);
	file.warn(0, start, "hiding waveform with lfix != 0");
      }else if (x->l_fixture() > _list.front()->l_fixture()) {
	untested();
	delete x;
	file.warn(0, start, "dropping waveform with lfix != 0");
      }else if (x->c_fixture() < _list.front()->c_fixture()) {
	_list.push_front(x);
	file.warn(0, start, "hiding waveform with cfix != 0");
      }else if (x->c_fixture() > _list.front()->c_fixture()) {
	delete x;
	file.warn(0, start, "dropping waveform with cfix != 0");
      }else{
	untested();
	delete x;
	file.warn(0, start, "waveforms with identical fixtures, dropping one");
      }}
    }else if (x->v_fixture_typ() == _list.back()->v_fixture_typ()) {
      // lowest at back.  There is more than one.
      // 3 or more waves?
      {if (x->r_fixture() < _list.back()->r_fixture()) {
	// take the lowest r_fixture
	_list.push_back(x);
	file.warn(0, start, "hiding waveform with higher rfix, same vfix");
      }else if (x->r_fixture() > _list.back()->r_fixture()) {
	delete x;
	file.warn(0, start, "dropping waveform with higher rfix, same vfix");
      }else if (x->l_fixture() < _list.back()->l_fixture()) {
	untested();
	_list.push_back(x);
	file.warn(0, start, "hiding waveform with lfix != 0");
      }else if (x->l_fixture() > _list.back()->l_fixture()) {
	untested();
	delete x;
	file.warn(0, start, "dropping waveform with lfix != 0");
      }else if (x->c_fixture() < _list.back()->c_fixture()) {
	_list.push_back(x);
	file.warn(0, start, "hiding waveform with cfix != 0");
      }else if (x->c_fixture() > _list.back()->c_fixture()) {
	delete x;
	file.warn(0, start, "dropping waveform with cfix != 0");
      }else{
	untested();
	delete x;
	file.warn(0, start, "waveforms with identical fixtures, dropping one");
      }}
    }else{
      // for now, throw away waves in the middle
      // could lose one or more when there are more than two.
      untested();
      delete x;
      file.warn(0, start, "more than 2 waveforms, dropping one");
    }}
  }else{
    untested();
    delete x;
    file.warn(0, start, "need waveform parameters");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Base::Type::parse(CS& file)
{
       set(file, "Terminator",	   &_type, mtTERMINATOR)
    || set(file, "Input_ECL",	   &_type, mtINPUT_ECL)
    || set(file, "Input",	   &_type, mtINPUT)
    || set(file, "I/O_open_sink",  &_type, mtIO_OPEN_SINK)
    || set(file, "I/O_open_drain", &_type, mtIO_OPEN_DRAIN)
    || set(file, "I/O_open_source",&_type, mtIO_OPEN_SOURCE)
    || set(file, "I/O_ECL",	   &_type, mtIO_ECL)
    || set(file, "I/O",		   &_type, mtIO)
    || set(file, "3-state_ECL",    &_type, mt3STATE_ECL)
    || set(file, "3-state",	   &_type, mt3STATE)
    || set(file, "Output_ECL",	   &_type, mtOUTPUT_ECL)
    || set(file, "Output",	   &_type, mtOUTPUT)
    || set(file, "Open_sink",	   &_type, mtOPEN_SINK)
    || set(file, "Open_drain",	   &_type, mtOPEN_DRAIN)
    || set(file, "Open_source",    &_type, mtOPEN_SOURCE)
    || set(file, "Series_switch",  &_type, mtSERIES_SWITCH)
    || set(file, "Series",	   &_type, mtSERIES)
    || file.warn(0, "illegal model type");
}
/*--------------------------------------------------------------------------*/
void Model_Base::parse(CS& file)
{
  skip_comment(file);
  get(file, "Model_type", &_model_type);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Series_Mosfet::Entry::parse(CS& file)
{
  get(file, "Vds", &_vds) || file.warn(0, "need Vds");
  file >> _table;
}
/*--------------------------------------------------------------------------*/
void Series_Mosfet::parse(CS& file)
{
  int here = file.cursor();
  for (;;) {
    skip_comment(file);
    {if (file.match1('[')) {
      break;
    }else{
      Entry* x = new Entry(file);
      {if (!file.stuck(&here)) {
	_list.push_back(x);
      }else{
	untested();
	delete x;
	file.warn(0, "need series mosfet info");
	break;
      }}
    }}
  }
}
/*--------------------------------------------------------------------------*/
void Series_Core::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "[R_Series]",	 &_r_series);
    get(file, "[L_Series]",	 &_l_series);
    get(file, "[Rl_Series]",	 &_rl_series);
    get(file, "[C_Series]",	 &_c_series);
    get(file, "[Lc_Series]",	 &_lc_series);
    get(file, "[Rc_Series]",	 &_rc_series);
    get(file, "[Series_Current]",&_series_current);
    get(file, "[Series_Mosfet]", &_series_mosfet);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Series::parse(CS& file)
{
  untested();
  IO_Base::parse(file);
  file >> _core;
}
/*--------------------------------------------------------------------------*/
void Series_Switch::parse(CS& file)
{
  int here = file.cursor();
  do {
    IO_Base::parse(file);
    get(file, "[On]",	&_on);
    get(file, "[Off]",	&_off);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void IO_Base::Polarity::parse(CS& file)
{
       set(file, "Non-Inverting", &_p, pNON_INVERTING)
    || set(file, "Inverting",	  &_p, pINVERTING)
    || file.warn(0, "need Inverting or Non-Inverting");
}
/*--------------------------------------------------------------------------*/
void IO_Base::Enable::parse(CS& file)
{
       set(file, "Active-High", &_e, eACTIVE_HIGH)
    || set(file, "Active-Low",	&_e, eACTIVE_LOW)
    || file.warn(0, "need Active-High or Active-Low");
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel::Mode::parse(CS& file)
{
       set(file, "Non-Driving", &_m, smNON_DRIVING)
    || set(file, "Driving",	&_m, smDRIVING)
    || set(file, "All",		&_m, smALL)
    || file.warn(0, "need All, Driving, or Non-Driving");
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel::parse(CS& file)
{
  Tail_String line_buf;
  file >> line_buf;
  CS line(line_buf);
  line >> _name >> _mode;
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel_List::parse(CS& file)
{
  skip_tail(file);
  int here = file.cursor();
  for (;;) {
    skip_comment(file);
    {if (file.match1('[')) {
      break;
    }else{
      Submodel* x = new Submodel(file);
      {if (!file.stuck(&here)) {
	_list.push_back(x);
      }else{
	untested();
	delete x;
	file.warn(0, "need submodel info");
	break;
      }}
    }}
  }
}
/*--------------------------------------------------------------------------*/
void IO_Base::parse(CS& file)
{
  Model_Base::parse(file);
  get(file, "Polarity",			&_polarity);
  get(file, "Enable",			&_enable);
  get(file, "Vinl",			&_vinl);
  get(file, "Vinh",			&_vinh);
  get(file, "Cref",			&_cref);
  get(file, "Rref",			&_rref);
  get(file, "Vref",			&_vref);
  get(file, "Vmeas",			&_vmeas);
  get(file, "C_comp",			&_c_comp);
  get(file, "[Ttgnd]",			&_tt_gnd);
  get(file, "[Ttpower]",		&_tt_pwr);
  get(file, "[Temperature_Range]",	&_temperature_range);
  get(file, "[Voltage_Range]",		&_voltage_range);
  get(file, "[Power_Clamp_Reference]",	&_power_clamp_ref);
  get(file, "[Gnd_Clamp_Reference]",	&_gnd_clamp_ref);
  get(file, "[Power_Clamp]",		&_power_clamp);
  get(file, "[Gnd_Clamp]",		&_gnd_clamp);
  get(file, "[Add_Submodel]",		&_submodels);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Spec::parse(CS& file)
{
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "Vinh+",		 &_vinh_p);
    get(file, "Vinh_plus_",	 &_vinh_p);
    get(file, "Vinh-",		 &_vinh_m);
    get(file, "Vinh_minus_",	 &_vinh_m);
    get(file, "Vinl+",		 &_vinl_p);
    get(file, "Vinl_plus_",	 &_vinl_p);
    get(file, "Vinl-",		 &_vinl_m);
    get(file, "Vinl_minus_",	 &_vinl_m);
    get(file, "Vinh ",		 &_vinh);
    get(file, "Vinl ",		 &_vinl);
    get(file, "S_overshoot_high",&_s_overshoot_high);
    get(file, "S_overshoot_low", &_s_overshoot_low);
    get(file, "D_overshoot_high",&_d_overshoot_high);
    get(file, "D_overshoot_low", &_d_overshoot_low);
    get(file, "D_overshoot_time",&_d_overshoot_time);
    get(file, "Pulse_high",	 &_pulse_high);
    get(file, "Pulse_low",	 &_pulse_low);
    get(file, "Pulse_time",	 &_pulse_time);
    get(file, "Vmeas",		 &_vmeas);
  }while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Input_Base::parse(CS& file)
{
  get(file, "[Model_Spec]",&_model_spec);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Driver_Schedule::parse(CS& file)
{
  Tail_String line_buf;
  file >> line_buf;
  CS line(line_buf);
  line >> _name >> _rise_on_delay >> _rise_off_delay 
       >> _fall_on_delay >> _fall_off_delay;
}
/*--------------------------------------------------------------------------*/
void Driver_Schedule_List::parse(CS& file)
{
  skip_tail(file);
  int here = file.cursor();
  for (;;) {
    skip_comment(file);
    {if (file.match1('[')) {
      break;
    }else{
      Driver_Schedule* x = new Driver_Schedule(file);
      {if (!file.stuck(&here)) {
	_list.push_back(x);
      }else{
	untested();
	delete x;
	file.warn(0, "need driver schedule info");
	break;
      }}
    }}
  }
}
/*--------------------------------------------------------------------------*/
void Output_Base::parse(CS& file)
{
  get(file, "[Pulldown_Reference]",&_pulldown_ref);
  get(file, "[Pullup_Reference]",  &_pullup_ref);
  get(file, "[Pulldown]",	   &_pulldown);
  get(file, "[Pullup]", 	   &_pullup);
  get(file, "[Ramp]",		   &_ramp);
  get(file, "[Rising_Waveform]",   &_rising_waveforms);
  get(file, "[Falling_Waveform]",  &_falling_waveforms);
  get(file, "[Driver_Schedule]",   &_driver_sched);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Terminator::parse(CS& file)
{
  int here = file.cursor();
  do {
    IO_Base::parse(file);
    get(file, "[Rgnd]",	 &_r_gnd);
    get(file, "[Rpower]",&_r_power);
    get(file, "[Rac]",	 &_r_ac);
    get(file, "[Cac]",	 &_c_ac);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Input::parse(CS& file)
{
  int here = file.cursor();
  do {
    IO_Base::parse(file);
    Input_Base::parse(file);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void I_O::parse(CS& file)
{
  int here = file.cursor();
  do {
    IO_Base::parse(file);
    Input_Base::parse(file);
    Output_Base::parse(file);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
void Output::parse(CS& file)
{
  int here = file.cursor();
  do {
    IO_Base::parse(file);
    Output_Base::parse(file);
  } while (file.more() && !file.stuck(&here));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_List::parse(CS& file)
{
  Tail_String name;
  file >> name;
  int here = file.cursor();
  {if (file.pscan("Model_type")) {
    Model_Base* x = 0;
    {if (file.dmatch("Terminator")) {
      file.reset(here);
      x = new Terminator(file, name);
    }else if (file.dmatch("Input_ECL")
	      || file.dmatch("Input")) {
      file.reset(here);
      x = new Input(file, name);
    }else if (file.dmatch("I/O_open_sink")
	      || file.dmatch("I/O_open_drain")
	      || file.dmatch("I/O_open_source")
	      || file.dmatch("I/O_ECL")
	      || file.dmatch("I/O")
	      || file.dmatch("3-state_ECL")
	      || file.dmatch("3-state")) {
      file.reset(here);
      x = new I_O(file, name);
    }else if (file.dmatch("Output_ECL")
	      || file.dmatch("Output")
	      || file.dmatch("Open_sink")
	      || file.dmatch("Open_drain")
	      || file.dmatch("Open_source")) {
      file.reset(here);
      x = new Output(file, name);
    }else if (file.dmatch("Series_switch")) {
      file.reset(here);
      x = new Series_Switch(file, name);
    }else if (file.dmatch("Series")) {
      file.reset(here);
      x = new Series(file, name);
    }else{
      file.warn(0, "bad model type");
      assert(x == 0);
    }}
    {if (x) {
      _list.push_back(x);
    }else{
      untested();
    }}
  }else{
    untested();
    file.warn(0, "model has no type");
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
