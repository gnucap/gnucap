/*$Id: model-dump.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
void Ramp::dump(std::ostream& out)const
{
  out <<
    "dv/dt_r " << dv_dt_r() << "\n"
    "dv/dt_f " << dv_dt_f() << "\n"
    "R_Load "  << r_load()  << "\n";
}
/*--------------------------------------------------------------------------*/
void Wave::dump(std::ostream& out)const
{
  out <<
    "C_Fixture = "	<< c_fixture()	   << "\n"
    "L_Fixture = "	<< l_fixture()	   << "\n"
    "R_Fixture = "	<< r_fixture()	   << "\n"
    "V_Fixture_Min = "	<< v_fixture_min() << "\n"
    "V_Fixture_Max = "	<< v_fixture_max() << "\n"
    "V_Fixture = "	<< v_fixture_typ() << "\n"
    "C_Dut = "  	<< c_dut()	   << "\n"
    "L_Dut = "  	<< l_dut()	   << "\n"
    "R_Dut = "  	<< r_dut()	   << "\n"
      << wave() << '\n';
}
/*--------------------------------------------------------------------------*/
void Wave_List::dump(std::ostream& out)const
{
  for (const_iterator i = begin(); i != end(); ++i) {
    out << _head << '\n' << **i << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Base::Type::dump(std::ostream& out)const
{
  switch (_type) {
  case mtTERMINATOR:	out << "Terminator";	 return;
  case mtINPUT_ECL:	out << "Input_ECL";	 return;
  case mtINPUT:		out << "Input";		 return;
  case mtIO_OPEN_SINK:	out << "I/O_Open_Sink";	 return;
  case mtIO_OPEN_DRAIN:	out << "I/O_Open_Drain"; return;
  case mtIO_OPEN_SOURCE:out << "I/O_Open_Source";return;
  case mtIO_ECL:	out << "I/O_ECL";	 return;
  case mtIO:		out << "I/O";		 return;
  case mt3STATE_ECL:	out << "3-State_ECL";	 return;
  case mt3STATE:	out << "3-STATE";	 return;
  case mtOUTPUT_ECL:	out << "Output_ECL";	 return;
  case mtOUTPUT:	out << "Output";	 return;
  case mtOPEN_SINK:	out << "Open_Sink";	 return;
  case mtOPEN_DRAIN:	out << "Open_Drain";	 return;
  case mtOPEN_SOURCE:	out << "Open_Source";	 return;
  case mtSERIES_SWITCH:	out << "Series_Switch";	 return;
  case mtSERIES:	out << "Series";	 return;
  case mtNA:		out << "uninitialized";	 /* drop out */
  }
  unreachable();
  out << " ?error? " << int(_type);
}
/*--------------------------------------------------------------------------*/
void Model_Base::dump(std::ostream& out)const
{
  out << 
    "[Model] "	  << name()	  << "\n"
    "Model_Type " << model_type() << "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Series_Mosfet::Entry::dump(std::ostream& out)const
{
  out << "[Series MOSFET]\n"
    "Vds = " << _vds << '\n'
      << _table << '\n';
}
/*--------------------------------------------------------------------------*/
void Series_Mosfet::dump(std::ostream& out)const
{
  for (const_iterator i = begin(); i != end(); ++i) {
    out << **i;
  }
}
/*--------------------------------------------------------------------------*/
void Series_Core::dump(std::ostream& out)const
{
  out <<
    "[R Series] "       << r_series()	   << "\n"
    "[L Series] "       << l_series()	   << "\n"
    "[RL Series] "      << rl_series()	   << "\n"
    "[C Series] "       << c_series()	   << "\n"
    "[LC Series] "      << lc_series()	   << "\n"
    "[RC Series] "      << rc_series()	   << "\n"
    "[Series Current]\n"<< series_current()<< "\n"
      << series_mosfet() << "\n";
}
/*--------------------------------------------------------------------------*/
void Series::dump(std::ostream& out)const
{
  untested();
  Model_Base::dump(out);
  out << _core << "\n";
}
/*--------------------------------------------------------------------------*/
void Series_Switch::dump(std::ostream& out)const
{
  Model_Base::dump(out);
  out << "\n[On]\n"  << on()
      << "\n[Off]\n" << off() << "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void IO_Base::Polarity::dump(std::ostream& out)const
{
  switch (_p) {
  case pNON_INVERTING:	out << "Non-Inverting"; return;
  case pINVERTING:	out << "Inverting";	return;
  }
  unreachable();
  out << " ?error? " << int(_p);  
}
/*--------------------------------------------------------------------------*/
void IO_Base::Enable::dump(std::ostream& out)const
{
  switch (_e) {
  case eACTIVE_HIGH:	out << "Active-High";	return;
  case eACTIVE_LOW:	out << "Active-Low";	return;
  }
  unreachable();
  out << " ?error? " << int(_e);  
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel::Mode::dump(std::ostream& out)const
{
  switch (_m) {
  case smNON_DRIVING:	out << "Non-Driving";	return;
  case smDRIVING:	out << "Driving";	return;
  case smALL:		out << "All";		return;
  }
  unreachable();
  out << " ?error? " << int(_m);  
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel::dump(std::ostream& out)const
{
  out << name() << ' ' << mode() << '\n';
}
/*--------------------------------------------------------------------------*/
void IO_Base::Submodel_List::dump(std::ostream& out)const
{
  for (const_iterator i = begin(); i != end(); ++i) {
    out << **i;
  }
}
/*--------------------------------------------------------------------------*/
void IO_Base::dump(std::ostream& out)const
{
  Model_Base::dump(out);
  out <<
    "Polarity "		<< polarity()	<< "\n"
    "Enable "		<< enable()	<< "\n"
    "Vinl = "		<< vinl()	<< "\n"
    "Vinh = "		<< vinh()	<< "\n"
    "Cref = "		<< cref()	<< "\n"
    "Rref = "		<< rref()	<< "\n"
    "Vref = "		<< vref()	<< "\n"
    "Vmeas = "		<< vmeas()	<< "\n"
    "C_Comp "		<< c_comp()	<< "\n"
    "[TTGnd] "		<< tt_gnd()	<< "\n"
    "[TTPower] "	<< tt_pwr()	<< "\n"
    "[Temperature Range] "	<< temperature_range()	<< "\n"
    "[Voltage Range] "		<< voltage_range()	<< "\n"
    "[Power Clamp Reference] "	<< power_clamp_ref()	<< "\n"
    "[Gnd Clamp Reference] "	<< gnd_clamp_ref()	<< "\n\n"
    "[Power Clamp]\n"	<< power_clamp()<< "\n"
    "[Gnd Clamp]\n"	<< gnd_clamp()	<< "\n"
    "[Add Submodel]\n"	<< submodels()	<< "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Spec::dump(std::ostream& out)const
{
  out << "[Model Spec]\n"
    "Vinh+ "	<< vinh_p()	<< "\n"
    "Vinh- "	<< vinh_m()	<< "\n"
    "Vinl+ "	<< vinl_p()	<< "\n"
    "Vinl- "	<< vinl_m()	<< "\n"
    "Vinh "	<< vinh()	<< "\n"
    "Vinl "	<< vinl()	<< "\n"
    "S_Overshoot_High "	<< s_overshoot_high()	<< "\n"
    "S_Overshoot_Low "	<< s_overshoot_low()	<< "\n"
    "D_Overshoot_High "	<< d_overshoot_high()	<< "\n"
    "D_Overshoot_Low "	<< d_overshoot_low()	<< "\n"
    "D_Overshoot_Time "	<< d_overshoot_time()	<< "\n"
    "Pulse_High "	<< pulse_high() 	<< "\n"
    "Pulse_Low " 	<< pulse_low()  	<< "\n"
    "Pulse_Time "	<< pulse_time() 	<< "\n"
    "Vmeas "		<< vmeas()		<< "\n";
}
/*--------------------------------------------------------------------------*/
void Input_Base::dump(std::ostream& out)const
{
  out << model_spec() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Driver_Schedule::dump(std::ostream& out)const
{
  out << name() << ' ' << rise_on_delay() << ' ' << rise_off_delay() << ' ' 
      << fall_on_delay() << ' ' << fall_off_delay() << '\n';
}
/*--------------------------------------------------------------------------*/
void Driver_Schedule_List::dump(std::ostream& out)const
{
  out << "[Driver Schedule]\n";
  for (const_iterator i = begin(); i != end(); ++i) {
    out << **i;
  }
}
/*--------------------------------------------------------------------------*/
void Output_Base::dump(std::ostream& out)const
{
  out <<
    "[Pulldown Reference] "<< pulldown_ref()	<< "\n"
    "[Pullup Reference] "  << pullup_ref()	<< "\n\n"
    "[Pulldown]\n"	   << pulldown()	<< "\n"
    "[Pullup]\n"	   << pullup()		<< "\n"
    "[Ramp]\n"		   << ramp()		<< "\n"
      << rising_waveforms()  << "\n"
      << falling_waveforms() << "\n"
      << driver_sched()	     << "\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Terminator::dump(std::ostream& out)const
{
  IO_Base::dump(out);
    out <<
      "[RGND] "	 << r_gnd()	<< "\n"
      "[RPOWER] "<< r_power()	<< "\n"
      "[RAC] "	 << r_ac()	<< "\n"
      "[CAC] "	 << c_ac()	<< "\n"
      "|[End Model]\n";
}
/*--------------------------------------------------------------------------*/
void Input::dump(std::ostream& out)const
{
  IO_Base::dump(out);
  Input_Base::dump(out);
  out << "|[End Model]\n";
}
/*--------------------------------------------------------------------------*/
void I_O::dump(std::ostream& out)const
{
  IO_Base::dump(out);
  Input_Base::dump(out);
  Output_Base::dump(out);
  out << "|[End Model]\n";
}
/*--------------------------------------------------------------------------*/
void Output::dump(std::ostream& out)const
{
  IO_Base::dump(out);
  Output_Base::dump(out);
  out << "|[End Model]\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_List::dump(std::ostream& out)const
{
  for (const_iterator i = begin(); i != end(); ++i) {
    out << **i << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
