/*$Id: comp-net.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 * Complete: subckt based on [Pin], [Pin Mapping]
 * Incomplete: subckt based on: [Package Model], [Diff Pin], 
 *	[Series Pin Mapping], [Series Switch Groups], [Model Selector]
 * Makes a subckt for the whole chip.
 * Name is: <file>:<component_name>
 * Nodelist is: from [Pin], in order
 * Calls models named: <file>[MODEL]<model_name>
 */
#include "net.h"
/*--------------------------------------------------------------------------*/
namespace Netlist {
/*--------------------------------------------------------------------------*/
/* make_pin_ckt
 * output a circuit for one "pin"
 *
 * form is:
 *  [pin] line from IBIS file as a comment
 *  R, L, C lines representing the parameters
 *  X line representing the [model] attached there
 *
 * Sample 1: (no pin mapping)
 * IBIS:
 * [Pin] signal_name    model_name
 * 1         OEAB_Z     OE_CNTRL
 *
 * output:
 * *pin 1 OEAB_Z OE_CNTRL
 * C1 (1 0) 0
 * R1 (1 1.int) 0
 * L1 (1.int OEAB_Z) 0
 * X1 (OEAB_Z 0 control enable) 18t612.ibs[MODEL]OE_CNTRL
 *
 * Sample 2: (with pin mapping)
 * IBIS:
 * [Pin]  signal_name  model_name  R_pin  L_pin  C_pin   
 * 1	  Q1	       SSTL_OUT	   140m   3.57n  0.2p  
 * [Pin Mapping] pulldown_ref pullup_ref gnd_clamp_ref power_clamp_ref
 * 1		 GNDBUS1      PWRBUS1
 *
 * output:
 * *pin 1 Q1 SSTL_OUT 0.14 3.57e-09 2e-13
 * C1 (1 0) 2e-13
 * R1 (1 1.int) 0.14
 * L1 (1.int Q1) 3.57e-09
 * X1 (Q1 0 control enable GNDBUS1 PWRBUS1  ) sst16857.ibs[MODEL]SSTL_OUT
 *
 * Name of [model] subckt is: <prefix><model_name>
 */
static void make_pin_ckt(std::ostream& out,
			 const Component::Pin& p,
			 const Component::Pin_Map* m,
			 const Component::Package& def,
			 const std::string& prefix)
{
  out << "*pin " << p << // dump
    // C_pin_name ( pin c_pin ) value
    "C" << p.name()
      << " (" << p.name() << " 0) "
      << ((p.c_pin() != NA) ? p.c_pin() : def.c_pkg().value_num()) << "\n";
  {if (p.model() != "NC") {
    // R_pin_name ( pin pin.int ) value    
    out << "R" << p.name()
	<< " (" << p.name() << ' ' << p.name() << ".int) " 
	<< ((p.r_pin() != NA) ? p.r_pin() : def.r_pkg().value_num()) << "\n"
    // L_pin_name ( pin.int l_pin ) value    
      "L" << p.name()
	<< " (" << p.name() << ".int " << p.signal() << ") " 
	<< ((p.l_pin() != NA) ? p.l_pin() : def.l_pkg().value_num()) << "\n";
    {if (p.model() != "GND" && p.model() != "POWER") {
      // X_pin_name ( signalname 0 control enable pdr pur gcr pcr ) modelname
      out << "X" << p.name()
	  << " (" << p.signal() << " 0 control enable";
      {if (m) {
	out << ' ' << m->pulldown_ref() << ' ' << m->pullup_ref()
	    << ' ' << m->gnd_clamp_ref() << ' ' << m->pwr_clamp_ref();
      }}
      out << ") " << prefix << p.model() << "\n";
    }}
  }}
  out << '\n';
}
/*--------------------------------------------------------------------------*/
/* make_component
 * Make the netlist for one "component".
 * It consists of:
 * A comment line "[COMPONENT]".
 * ".subckt" line with all the connections.
 * A section for each "pin" in the "pin list".
 * ".ends"
 * 
 * Name of the subckt is: <prefix><name>
 */
static void make_component(std::ostream& out, const Component& c,
			   const std::string& prefix)
{
  // header
  out << "*[COMPONENT] " << c.name() << "\n"
    ".subckt " << prefix << c.name() << " ( ";
  {for (Component::Pin_List::const_iterator
	 i = c.pins().begin(); i != c.pins().end(); ++i) {
    out << (**i).name() << ' ';
  }}
  out << ")\n\n";

  // pin circuits
  {for (Component::Pin_List::const_iterator
	 i = c.pins().begin(); i != c.pins().end(); ++i) {
    make_pin_ckt(out, **i, c.pin_mapping()[(**i).name()], c.package(), prefix);
  }}

  if (!c.diff_pins().is_empty()) {
    incomplete();
    out << "Diff Pin incomplete\n";
  }

  if (!c.series_pins().is_empty()) {
    incomplete();
    out << "Series Pin Mapping incomplete\n";
  }

  if (!c.series_switches().is_empty()) {
    incomplete();
    out << "Series Switch Groups incomplete\n";
  }

  // done
  out << ".ends " << prefix << c.name() << "\n\n";
}
/*--------------------------------------------------------------------------*/
/* make_components
 * Make a bunch of components.
 * Each one gets its own subckt.
 */
void make_components(std::ostream& out, const Component_List& list,
		     const std::string& prefix, const std::string&)
{
  {for (Component_List::const_iterator i=list.begin(); i != list.end(); ++i) {
    make_component(out, **i, prefix);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
