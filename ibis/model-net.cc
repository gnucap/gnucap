/*$Id: model-net.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 * Name is: <prefix><model_name>
 */
#include "net.h"
/*--------------------------------------------------------------------------*/
namespace Netlist {
/*--------------------------------------------------------------------------*/
  const double iv_tol = 1e-5;
  const double vt_tol = .001;
  const double mult_tol = .01;
  const double deriv_tol = .01;
#ifdef APPLE
  #define R_TABLE "r array="
  #define C_TABLE "c array="
  #define IV_LABEL "R"
#else
  #define R_TABLE "table"
  #define C_TABLE "table"
  #define IV_LABEL "Y"
#endif
/*--------------------------------------------------------------------------*/
// quantize: returns x, quantized to resolution */
inline double quantize(double x, double resolution)
{
  return rint(x/resolution)*resolution;
}
/*--------------------------------------------------------------------------*/
/* torange: returns b, clipped to range a..c */
inline double to_range(double a, double b, double c)
{
  return (a < c) 
    ? std::min(std::max(a,b),c)
    : std::max(std::min(a,b),c);
}
/*--------------------------------------------------------------------------*/
static void make_table(std::ostream& out, const Vector& v, bool flip_sign)
{
#if 1
  assert(!v.is_empty());
  double old_x = NA;
  double old_y = NA;
  double old_dydx = NA;
  double start_run_x = NA;
  double start_table_x = NA;

  {for (Vector::const_iterator i = v.begin(); i != v.end(); ++i) {
    assert(*i);
    double y = (**i).y();
    if (y != NA) {
      double x = (flip_sign) ? -(**i).x() : (**i).x();
      y = quantize(y, iv_tol);
      double dydx = (y - old_y) / (x - old_x);
      {if (old_x == NA) {
	// first time
	start_table_x = start_run_x = x;
	assert(old_dydx == NA);
      }else if (old_y == y) {
	// continuing a run
      }else if (start_run_x == start_table_x && start_run_x != old_x) {
	// end initial run
	out << "+ " << start_run_x << ' ' << old_y << '\n';
	out << "+ " << old_x << ' ' << old_y << '\n';
	start_run_x = x;
      }else if (start_run_x != old_x) {
	// end ordinary run, use single value at middle of run
	double mid_x = (old_x + start_run_x) / 2;
	out << "+ " << mid_x << ' ' << old_y << '\n';
	start_run_x = x;
      }else if (old_dydx == 0 && old_dydx == dydx) {
	// matching derivative, drop
	start_run_x = x;
      }else if (fabs(1-dydx/old_dydx) < deriv_tol) {
	// matching derivative, drop
	start_run_x = x;
      }else{
	// normal not in a run
	out << "+ " << old_x << ' ' << old_y << '\n';
	start_run_x = x;
      }}
      old_x = x;
      old_y = y;
      old_dydx = dydx;
    }
  }}
  if (old_x != start_run_x) {
    // there's a run at the end
    out << "+ " << start_run_x << ' ' << old_y << '\n';
  }
  // out final value
  out << "+ " << old_x << ' ' << old_y << '\n';
#else
  {for (Vector::const_iterator i = v.begin(); i != v.end(); ++i) {
    assert(*i);
    double x = (flip_sign) ? -(**i).x() : (**i).x();
    double y = (**i).y();
    if (y != NA) {
      out << "+ " << x << ' ' << y << '\n' ;
    }
  }}
#endif
}
/*--------------------------------------------------------------------------*/
static void make_scaled_table(std::ostream& out, const Range<Fraction>& r,
			      double out_start, double out_end)
{
  out << "+ 0. " << out_start << "\n";
  double end_time = r.value_num().denominator() * 1./.6;
  out << "+ " << end_time << ' ' << out_end << "\n";
  // ramp specifies time for 20% to 80% and the voltages
  // ignore the voltages, multiply the time by 1/.6 (60%)
  // to get approx end time for pure ramp.
}
/*--------------------------------------------------------------------------*/
static void make_scaled_table(std::ostream& out, const Wave& w,
			      double out_start, double out_end)
{
  assert(!w.is_empty());

  double out_diff = out_end - out_start;
  double out_tol = mult_tol * fabs(out_diff);

  // scale based on voltage
  double v_start = w.front().y();
  double v_end   = w.back().y();
  double v_diff = v_end - v_start;
  double v_tol = vt_tol * fabs(v_diff);
  v_start = quantize(v_start, v_tol);
  v_end   = quantize(v_end,   v_tol);

  // make it based on current (no difference in results)
  double i_start = (v_start - w.v_fixture_val()) / w.r_fixture();
  double i_end   = (v_end   - w.v_fixture_val()) / w.r_fixture();

  double i_diff = i_end - i_start;
  double scale = out_diff / i_diff;
  double old_x = NA;
  double old_out_val = NA;

  {for (Wave::const_iterator i = w.begin(); i != w.end(); ++i) {
    assert(*i);
    if ((**i).y() != NA) {
      // voltage
      double v_this = quantize((**i).y(), v_tol);
      // current (for now, resistive part only)
      double i_this = (v_this - w.v_fixture_val()) / w.r_fixture();

      Wave::const_iterator next = i;
      ++next; // because I can't just do i+1
      {if (i != w.begin() && next != w.end()) { // not end points of table
	Wave::const_iterator prev = i;
	--prev;
	{      // + c_comp + c_fixture
	  double dv = quantize((**(next)).y(), v_tol)
	    - quantize((**(prev)).y(), v_tol);
	  double dt = (**(next)).x() - (**(prev)).x();
	  i_this += w.c_fixture() * dv/dt;
	  i_this += w.c_comp().value_num() * dv/dt;
	}
	double out_val = quantize(((i_this - i_start) * scale + out_start),
				  out_tol);
	out_val = to_range(out_start, out_val, out_end);
	bool monotonic = (out_start < out_end) == (old_out_val < out_val);
	assert( (!(out_val == old_out_val || !monotonic))
		== (out_val != old_out_val && monotonic));
	if (!(out_val == old_out_val || !monotonic)) {
	  if (old_out_val == out_start && old_x != (**(w.begin())).x() ) {
	    out << "+ " << old_x << ' ' << old_out_val << '\n';
	  }
	  out << "+ " << (**i).x() << ' ' << out_val << '\n';
	  old_out_val = out_val;
	}
	old_x = (**i).x();
      }else{ // end points of table
	double out_val = quantize(((i_this - i_start) * scale + out_start),
				  out_tol);
	out << "+ " << (**i).x() << ' ' << out_val << '\n';
	old_x = (**i).x();
	old_out_val = out_val;
      }}
    }
  }}
}
/*--------------------------------------------------------------------------*/
static Vector operator*(const Vector& a, const Float& b)
{
  untested(); // probably doesn't work.
  Base* z = a.multiply(b);
  Vector* zzz = dynamic_cast<Vector*>(z);
  assert(zzz);
  Vector zz(*zzz);
  //Vector zz;
  delete z;
  return zz;
}
/*--------------------------------------------------------------------------*/
static void make_io_base(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  const IO_Base* mm = dynamic_cast<const IO_Base*>(b);
  assert(mm);

  //if (power_on)
  {if (state == "power_on") {
    //  vsource Vpcr (power_clamp_ref gnd) V = 
    //			[POWER_Clamp_Reference] || 
    //			[Voltage_Range]
    {if (mm->power_clamp_ref().value_num() != NA) {
      out << "Vpcr (power_clamp_ref gnd) " 
	  << mm->power_clamp_ref().value_num() << '\n';
    }else if (mm->voltage_range().value_num() != NA) {
      out << "Vpcr (power_clamp_ref gnd) "
	  << mm->voltage_range().value_num() << '\n';
    }else{
      out << "need either [Power Clamp Reference] or [Voltage Range]\n";
    }}
    
    //  vsource Vgcr (gnd_clamp_ref gnd)  V = 
    //			[GND_Clamp_Reference] || 
    //			short
    {if (mm->gnd_clamp_ref().value_num() != NA) {
      out << "Vgcr (gnd_clamp_ref gnd) "
	  << mm->gnd_clamp_ref().value_num() << '\n';
    }else{
      out << "Vgcr (gnd_clamp_ref gnd) 0 \n";
    }}
    //end if
  }else{
    // expect voltages to be applied externally
  }}
  
  //resistor Rpc (pin power_clamp_ref) I = [POWER_clamp](-V)
  if (!mm->power_clamp().is_empty()) {
    out << IV_LABEL "pc (pin power_clamp_ref ) power_clamp\n"
      ".model power_clamp " R_TABLE "\n";
    make_table(out, mm->power_clamp(), true /* flip sign */);
    out << '\n';
  }

  //resistor Rgc (pin gnd_clamp_ref) I = [GND_clamp](V) || open
  if (!mm->gnd_clamp().is_empty()) {
    out << IV_LABEL "gc (pin gnd_clamp_ref) ground_clamp\n"
      ".model ground_clamp " R_TABLE "\n";
    make_table(out, mm->gnd_clamp(), false /* no flip sign */);
    out << '\n';
  }

  //define VT = .8617087e-4 * ([Temperature_Range]+273.15) || .026
  double VT = (mm->temperature_range().value_num() != NA)
    ? .8617087e-4 * (mm->temperature_range().value_num() + 273.15)
    : .026;
  //capacitor Cttpwr (pin power_clamp_ref)  
  //	C = [TTpower] * [POWER_clamp](-V) / VT || open
  if (mm->tt_pwr().value_num() != NA && !mm->power_clamp().is_empty()) {
    untested();
    Vector x = mm->power_clamp() * (mm->tt_pwr().value_num() / VT);
    out << "Cttpwr (pin power_clamp_ref) ttpwrcap\n"
      ".model ttpwrcap " C_TABLE "\n";
    make_table(out, x, true /*sign bug*/);
    out << '\n';
  }

  //capacitor Cttgnd (pin gnd_clamp_ref)
  //	C = [TTgnd]   * [GND_clamp](V) / VT    || open
  if (mm->tt_gnd().value_num() != NA && !mm->gnd_clamp().is_empty()) {
    untested();
    Vector x = mm->gnd_clamp() * (mm->tt_gnd().value_num() / VT);
    out << "Cttgnd (pin gnd_clamp_ref) ttgndcap\n"
      ".model ttgndcap " C_TABLE "=\n";
    make_table(out, x, false /* no flip sign */);
    out << '\n';
  }

  // capacitor Ccomp (pin gnd) C = C_comp
  assert(mm->c_comp().value_num() != NA);
  if (mm->c_comp().value_num() != 0.) {
    out << "Ccomp (pin gnd) " << mm->c_comp().value_num() << '\n';
  }

  //foreach line in [Add_Submodel]
  {
    int line = 0;
    for (IO_Base::Submodel_List::const_iterator i = mm->submodels().begin();
	 i != mm->submodels().end(); ++i) {
      ++line;
      std::string enable_node;
      switch ((**i).mode()) {
	//|  if ($1$ == "Non_Driving")
      case IO_Base::Submodel::Mode::smNON_DRIVING:
	//|    node $line$.enable
	//|    inverter U1 ($line$.enable gnd en) delay=0
	enable_node = "enable." + line;
	out << "Esm_enable (" << enable_node << " 0 pullup_ref en) 1 \n";
	break;
	//|  else if ($1$ == "All")
      case IO_Base::Submodel::Mode::smALL:
	//|    node $line$.enable
	//|    dsource U2 ($line$.enable gnd) state=1
	enable_node = "pullup_ref";
	break;
	//|  else if ($1$ == "Driving")
      case IO_Base::Submodel::Mode::smDRIVING:
	//|    define $line$.enable en
	enable_node = "en";
	break;
	//|  end if
      }
      //|  submodel X$line$ (pin gnd control $line$.enable
      //|        pulldown_ref pullup_ref gnd_clamp_ref power_clamp_ref) $0$
      out << "Xsm" << line << "  (pin gnd control " << enable_node
	  << " pulldown_ref pullup_ref gnd_clamp_ref power_clamp_ref) "
	  << prefix << (**i).name() << '\n';

      //end foreach
    }
  }
}
/*--------------------------------------------------------------------------*/
static void make_series_core(std::ostream& out, const Series_Core* mm)
{
  //capacitor Ccomp (pin1 pin2) C = C_comp
  //assert(mm->c_comp().value_num() != NA);
  //out << "Ccomp (pin gnd) " << mm->c_comp().value_num() << '\n';

  //resistor RLseries (t1 pin2) R = [Rl_Series] || short
  std::string t1;
  {if (mm->rl_series().value_num() != NA) {
    t1 = "t1";
    out << "RLseries (t1 pin2) " << mm->rl_series().value_num() << '\n';
  }else{
    t1 = "pin2";
  }}

  //inductor Lseries (pin1 t1) L = [L_Series]  || open
  if (mm->l_series().value_num() != NA) {
    out << "Lseries (pin1 " << t1 << " ) " <<mm->l_series().value_num() <<'\n';
  }

  //resistor Rseries (pin1 pin2) R = [R_Series]  || open
  if (mm->r_series().value_num() != NA) {
    out << "Rseries (pin1 pin2) " << mm->r_series().value_num() << '\n';
  }

  //resistor RCseries (t3 pin2) R = [Rc_Series] || short
  std::string t3;
  {if (mm->rc_series().value_num() != NA) {
    t3 = "t3";
    out << "RCseries (t3 pin2) " << mm->rc_series().value_num() << '\n';
  }else{
    t3 = "pin2";
  }}
 
  //inductor LCseries (t2 t3) L = [Lc_Series] || short
  std::string t2;
  {if (mm->lc_series().value_num() != NA) {
    t2 = "t2";
    out << "LCseries (t2 " << t3 << ") " << mm->lc_series().value_num() <<'\n';
  }else{
    t2 = t3;
  }}

  //capacitor Cseries (pin1 t2) C = [C_Series]  || open
  if (mm->c_series().value_num() != NA) {
    out << "Cseries (pin1 " << t2 << ") " << mm->c_series().value_num() <<'\n';
  }
  
  //resistor Rsercur (pin1 pin2) I = [Series_Current](V) || open
  if (!mm->series_current().is_empty()) {
    out << "Rsercur (pin1 pin2) series_current\n"
      ".model series_current " R_TABLE "\n";
    make_table(out, mm->series_current(), false /* no flip sign */);
    out << '\n';
  }

  //vcg Mosfet (pin1 pin2 0 pin2) I = [Series_MOSFET](Vc,Vo=Vds) || open
  if (!mm->series_mosfet().is_empty()) {
    incomplete();
    out << "[Series MOSFET] incomplete\n";
    // too messy ... putting off.
  }
}
/*--------------------------------------------------------------------------*/
static void make_series(std::ostream& out, const Base* b,
			const std::string&, const std::string&)
{
  const Series* mm = dynamic_cast<const Series*>(b);
  assert(mm);

  make_series_core(out, &(mm->core()));
}
/*--------------------------------------------------------------------------*/
static void make_series_switch(std::ostream& out, const Base* b,
			       const std::string&, const std::string& state)
{
  const Series_Switch* mm = dynamic_cast<const Series_Switch*>(b);
  assert(mm);

  {if (state == "on") {
    make_series_core(out, &(mm->on()));
  }else if (state == "off") {
    make_series_core(out, &(mm->off()));
  }else{
    out << "[Series Switch] bad state: " + state + '\n';
  }}
}
/*--------------------------------------------------------------------------*/
static void make_terminator(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  const Terminator* mm = dynamic_cast<const Terminator*>(b);
  assert(mm);

  make_io_base(out, b, prefix, state);

  //resistor Rac (pin t1) R = [Rac] || short
  std::string t1;
  {if (mm->r_ac().value_num() != NA) {
    t1 = "t1";
    out << "Rac (pin t1) " << mm->r_ac().value_num() << '\n';
  }else{
    t1 = "pin";
  }}
 
  //capacitor Cac (t1 gnd) C = [Cac] || open
  if (mm->c_ac().value_num() != NA) {
    out << "Cac (" << t1 << " gnd) " << mm->c_ac().value_num() << '\n';
  }

  //resistor Rpwr (pin power_clamp_ref) R = [Rpower] || open
  if (mm->r_power().value_num() != NA) {
    out << "Rpwr (pin power_clamp_ref) " << mm->r_power().value_num() << '\n';
  }

  //resistor Rgnd (pin gnd_clamp_ref) R = [Rgnd] || open
  if (mm->r_gnd().value_num() != NA) {
    out << "Rgnd (pin gnd_clamp_ref) " << mm->r_gnd().value_num() << '\n';
  }
}
/*--------------------------------------------------------------------------*/
static void make_input_base(std::ostream& out, const Base* b,
			    const std::string&, const std::string&)
{
  const IO_Base* m = dynamic_cast<const IO_Base*>(b);
  assert(m);
  const Input_Base* mm = dynamic_cast<const Input_Base*>(b);
  assert(mm);

  double vmeas = NA;
  {if (mm->model_spec().vmeas().value_num() != NA) {
    untested();
    vmeas = mm->model_spec().vmeas().value_num();
  }else if (m->vmeas() != NA) {
    vmeas = m->vmeas();
  }else if (m->vinl() != NA  &&  m->vinh() != NA) {
    vmeas = (m->vinl() + m->vinh()) / 2.;
  }else if (m->power_clamp_ref().value_num() != NA
	    && m->gnd_clamp_ref().value_num() != NA) {
    untested();
    vmeas = (m->power_clamp_ref().value_num() 
	     + m->gnd_clamp_ref().value_num()) / 2.;
  }else if (m->voltage_range().value_num() != NA) {
    untested();
    vmeas = m->voltage_range().value_num() / 2.;
  }else{
    untested();
    vmeas = 2;
  }}

  // crude approximation using Vmeas only.
  out << "Sout_down (out_of_in 0 pin gnd) thresh_down\n"
    ".model thresh_down sw vt = " << vmeas << " ron=10 roff=1e6\n"
    "Sout_up (out_of_in vvr pin gnd) thresh_up\n"
    ".model thresh_up sw vt = " << vmeas << " ron=1e6 roff=10\n"
    "Vout_in (vvr 0) DC " << m->voltage_range().value_num() << "\n";
}
/*--------------------------------------------------------------------------*/
static void make_output_base(std::ostream& out, const Base* b,
		       const std::string& prefix, const std::string& state)
{
  const IO_Base* m = dynamic_cast<const IO_Base*>(b);
  assert(m);
  const Output_Base* mm = dynamic_cast<const Output_Base*>(b);
  assert(mm);

  //if (power_on)
  {if (state == "power_on") {
    //  vsource Vpur (pullup_ref gnd)   V = 
    //			[Pullup_Reference] || 
    //			[Voltage_Range]
    {if (mm->pullup_ref().value_num() != NA) {
      out << "Vpur (pullup_ref gnd) "<< mm->pullup_ref().value_num()<<'\n';
    }else if (m->voltage_range().value_num() != NA) {
      out << "Vpur (pullup_ref gnd) "<< m->voltage_range().value_num()<<'\n';
    }else{
      out << "need either [Pullup Reference] or [Voltage Range]\n";
    }}
    
    //  vsource Vpdr (pulldown_ref gnd) V = 
    //			[Pulldown_Reference] || 
    //			short
    {if (mm->pulldown_ref().value_num() != NA) {
      out << "Vpdr (pulldown_ref gnd) "<< mm->pulldown_ref().value_num()<<'\n';
    }else{
      out << "Vpdr (pulldown_ref gnd) 0 \n";
    }}
    //end if
  }}

  // moved to later
  //node en
  std::string enable_node;
  switch (m->enable()) {
  //if (Enable == "Active-Low")
  case IO_Base::Enable::eACTIVE_LOW:
    //  inverter (en 0 enable_pin) delay=0
    enable_node = "en";
    out << "Venable_inverter en_ref 0 1\n";
    out << "Eenable_inverter (" << enable_node << " 0 en_ref enable_pin) 1 \n";
    break;
    //else if (Enable == "Active-High" || !Enable)
  case IO_Base::Enable::eACTIVE_HIGH:
    enable_node = "enable_pin";
    break;
    //end if  
  }

  //foreach line in [Driver_Schedule]
  {if (!mm->driver_sched().is_empty()) {
    int line = 0;
    for (Driver_Schedule_List::const_iterator i = mm->driver_sched().begin();
	 i != mm->driver_sched().end(); ++i) {
      ++line;
      //  node $line$.retrigger
      std::string retrigger_node("retrigger." + line);
      //  reshape U$0 ($line$.retrigger gnd control gnd) $1$ $2$ $3$ $4$
      out << "Xreshape (" << retrigger_node << " gnd control gnd) retrigger"
	" ron = " << (**i).rise_on_delay() << 
	" roff = " << (**i).rise_off_delay() <<
	" fon = " << (**i).fall_on_delay() << 
	" foff = " << (**i).fall_off_delay() << "\n";
      //|  model X$line$ (pin gnd $line$.retrigger en 
      //|	pulldown_ref pullup_ref gnd_clamp_ref power_clamp_ref) $0$
      out << "Xds" << line << " (pin gnd " << retrigger_node <<
	" en pulldown_ref pullup_ref gnd_clamp_ref power_clamp_ref) "
	  << prefix << (**i).name() << '\n';
    }
    //else
  }else{
    //|  trigger  TR   (Logic(control) == 1)
    //|  trigger  TF   (Logic(control) == 0)
    //  driver Udrv (pin gnd en TF pulldown_ref TR pullup_ref) (
    //        s1 = [Pullup](-V),  s0 = [Pulldown](V),
    //        w10 = [Falling_Waveform](T-TF,multi),
    //        w01 = [Rising_Waveform](T-TR,multi),  
    //	     ramp = [Ramp] )
    //    out << "Ucontrol (not_control 0 pullup_ref pullup_ref control)"
    //      " default inv\n"
    //      ".model default logic\n"
    //      "Spullup (pin_int pullup_ref control 0) pullup_switch\n"
    //      ".model pullup_switch nlswitch on = table order=1\n";
#if 1
    out << "Smain not_control 0 control 0 inv\n"
      ".model inv sw vt = 0.5  ron=10 roff=1e6\n"
      "Rload not_control pullup_ref 10K\n";
#else
    out << "Xcontrol (not_control 0 pullup_ref control) inv\n"
      ".subckt inv out gnd vdd in\n"
      "Smain out gnd in gnd inv\n"
      ".model inv sw vt = 0.5  ron=10 roff=1e6\n"
      "Rload out vdd 10K\n"
      ".ends inv\n";
#endif

    if (!mm->pullup().is_empty()) {
      out << "Spullup (pin_int pullup_ref control 0) pullup_switch\n"
	".model pullup_switch sw vt=0.5 on=\n";
      make_table(out, mm->pullup(), true /* flip sign */);
      out << "+ turnon =\n";
      {if (mm->rising_waveforms().is_empty()) {
	make_scaled_table(out, mm->ramp().dv_dt_r(), 0., 1.);
      }else{
	make_scaled_table(out, mm->rising_waveforms().pulldown(), 0., 1.);
      }}
      out << "+ turnoff =\n";
      {if (mm->falling_waveforms().is_empty()) {
	make_scaled_table(out, mm->ramp().dv_dt_f(), 1., 0.);
      }else{
	make_scaled_table(out, mm->falling_waveforms().pulldown(), 1., 0.);
      }}
    }

    {if (!mm->pulldown().is_empty()) {
      out << "Spulldown (pin_int pulldown_ref not_control 0) pulldown_switch\n"
	".model pulldown_switch sw vt=0.5 on=\n";
      make_table(out, mm->pulldown(), false /* no flip sign */);
      out << "+ turnon =\n";
      {if (mm->falling_waveforms().is_empty()) {
	make_scaled_table(out, mm->ramp().dv_dt_f(), 0., 1.);
      }else{
	make_scaled_table(out, mm->falling_waveforms().pullup(), 0., 1.);
      }}
      out << "+ turnoff =\n";
      {if (mm->rising_waveforms().is_empty()) {
	make_scaled_table(out, mm->ramp().dv_dt_r(), 1., 0.);
      }else{
	make_scaled_table(out, mm->rising_waveforms().pullup(), 1., 0.);
      }}
    }}
    out << "Senable (pin pin_int " << enable_node << " 0) enable_switch\n"
      ".model enable_switch sw vt=.5\n";
    //end foreach
  }}
}
/*--------------------------------------------------------------------------*/
static void make_input(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  make_io_base(out, b, prefix, state);
  make_input_base(out, b, prefix, state);
}
/*--------------------------------------------------------------------------*/
static void make_i_o(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  make_io_base(out, b, prefix, state);
  make_input_base(out, b, prefix, state);
  make_output_base(out, b, prefix, state);
}
/*--------------------------------------------------------------------------*/
static void make_output(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  make_io_base(out, b, prefix, state);
  make_output_base(out, b, prefix, state);
}
/*--------------------------------------------------------------------------*/
/* make_model
 * Make the netlist for one "model".
 * It consists of:
 * A comment line "[Model]".
 * ".subckt" line with all the connections.
 * A body with fixed topology depending on the Model_type
 * ".ends"
 * 
 * Name of the subckt is: <prefix><board_name>
 */
static void make_model(std::ostream& out, const Base* b,
		const std::string& prefix, const std::string& state)
{
  const Model_Base* m = dynamic_cast<const Model_Base*>(b);
  assert(m);

  out <<
    "*[Model] " << m->name() << "\n"
    "*Model_type " << m->model_type() << "\n"
    ".subckt " << prefix << m->name();
  out <<
    " (pin gnd out_of_in control enable_pin pulldown_ref pullup_ref"
    " gnd_clamp_ref power_clamp_ref)\n";
  {if (dynamic_cast<const Input*>(m)) {
    make_input(out, b, prefix, state);
  }else if (dynamic_cast<const I_O*>(m)) {
    make_i_o(out, b, prefix, state);
  }else if (dynamic_cast<const Output*>(m)) {
    make_output(out, b, prefix, state);
  }else if (dynamic_cast<const Terminator*>(m)) {
    make_terminator(out, b, prefix, state);
  }else if (dynamic_cast<const Series_Switch*>(m)) {
    make_series_switch(out, b, prefix, state);
  }else if (dynamic_cast<const Series*>(m)) {
    make_series(out, b, prefix, state);
  }else{
    unreachable();
  }}
  out << ".ends " << prefix << m->name() << "\n\n";
}
/*--------------------------------------------------------------------------*/
/* make_models
 * Make a bunch of models.
 * Each one gets its own subckt.
 */
void make_models(std::ostream& out, const Model_List& list,
		const std::string& prefix, const std::string& state)
{
  {for (Model_List::const_iterator i=list.begin(); i != list.end(); ++i) {
    make_model(out, *i, prefix, state);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
