/*$Id: mg_out_lib.cc,v 25.92 2006/06/28 15:03:12 al Exp $ -*- C++ -*-
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
 */
#include "mg_out.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void make_final_adjust_value(std::ofstream& out, const Parameter& p)
{
  if (!(p.calculate().empty())) {
    out << "    " << p.code_name() << " = " << p.calculate() << ";\n";
  }
  if (!(p.final_default().empty())) {
    out << "    if (" << p.code_name() << " == NA) {\n"
      "      " << p.code_name() << " = " << p.final_default() << ";\n"
      "    }\n";
  }
  if (!(p.quiet_min().empty())) {
    out << "    //" << p.code_name() << " = std::max(" 
	<< p.code_name() << ", " << p.quiet_min() << ");\n";
  }
  if (!(p.quiet_max().empty())) {
    out << "    //" << p.code_name() << " = std::min(" 
	<< p.code_name() << ", " << p.quiet_max() << ");\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_final_adjust_value_list(std::ofstream& out, const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_final_adjust_value(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
void make_final_adjust_parameter(std::ofstream& out, const Parameter& p)
{
  if (!(p.calculate().empty())) {
    out << "    this->" << p.code_name() << " = " << p.calculate() << ";\n";
  }else{
    out << "    this->" << p.code_name() << ".e_val(";
    if (!(p.final_default().empty())) {
      out << p.final_default();
    }else if (!(p.default_val().empty())) {
      out << p.default_val();
    }else{
      out << "NA";
    }
    out << ", par_scope);\n";
  }
  if (!(p.quiet_min().empty())) {
    out << "    //this->" << p.code_name() << " = std::max(" 
	<< p.code_name() << ", " << p.quiet_min() << ");\n";
  }
  if (!(p.quiet_max().empty())) {
    out << "    //this->" << p.code_name() << " = std::min(" 
	<< p.code_name() << ", " << p.quiet_max() << ");\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_final_adjust_parameter_list(std::ofstream& out, const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_final_adjust_parameter(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
void make_final_adjust(std::ofstream& out, const Parameter_Block& b)
{
  out << "    // final adjust: code_pre\n";
  out << b.code_pre();
  out << "    // final adjust: override\n";
  make_final_adjust_value_list(out, b.override());
  out << "    // final adjust: raw\n";
  make_final_adjust_parameter_list(out, b.raw());
  out << "    // final adjust: mid\n";
  out << b.code_mid();
  out << "    // final adjust: calculated\n";
  make_final_adjust_value_list(out, b.calculated());
  out << "    // final adjust: post\n";
  out << b.code_post();
  out << "    // final adjust: done\n";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void make_construct_parameter(std::ofstream& out, const Parameter& p)
{
  {if (!(p.default_val().empty())) {
    out << ",\n   " << p.code_name() << "(" << p.default_val() << ")";
  }else{
    out << ",\n   " << p.code_name() << "(NA)";
  }}
}
/*--------------------------------------------------------------------------*/
void make_construct_parameter_list(std::ofstream& out,const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_construct_parameter(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void make_get_one_param(std::ofstream& out, const Parameter& p,
		     const std::string& name)
{
  if (!(name.empty())) {
    out << "    || get(cmd, \"" << name << "\", &" << p.code_name();
    {if (!(p.offset().empty())) {
      incomplete();
      out << ", m_OFFSET, " << p.offset();
    }else if (p.positive()) {
      //incomplete();
      //out << ", mPOSITIVE";
    }else if (p.octal()) {
      incomplete();
      out << ", m_OCTAL";
    }else if (!(p.scale().empty())) {
      incomplete();       
      out << ", m_SCALE, " << p.scale();
    }}
    out << ")\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_get_param_list(std::ofstream& out, const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_get_one_param(out, **p, (**p).user_name());
    make_get_one_param(out, **p, (**p).alt_name());
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void make_print_one_param(std::ofstream& out, const Parameter& p)
{
  if (!(p.user_name().empty())) {
    {if (!(p.print_test().empty())) {
      out << "  if (" << p.print_test() << ")\n  ";
    }else if (p.default_val() == "NA") {
      out << "  if (" << p.code_name() << " != NA)\n  ";
    }}
    out << "  o << \"  " << to_lower(p.user_name()) << "=\" << " 
	<< p.code_name();
    if (!(p.offset().empty())) {
      out << "-(" << p.offset() << ")";
    }
    if (!(p.scale().empty())) {
      out << "/(" << p.scale() << ")";
    }
    out << ";\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_print_param_list(std::ofstream& out, const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_print_one_param(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void make_print_one_calc_param(std::ofstream& out, const Parameter& p)
{
  if (!(p.user_name().empty()) && !(p.calc_print_test().empty())) {
    out << "  if (" << p.calc_print_test() << ")\n  "
      "  o << \"* " << to_lower(p.user_name()) << "=\" << " 
	<< p.code_name();
    if (!(p.offset().empty())) {
      out << "-(" << p.offset() << ")";
    }
    if (!(p.scale().empty())) {
      out << "/(" << p.scale() << ")";
    }
    out << ";\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_print_calc_param_list(std::ofstream& out, const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_print_one_calc_param(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void make_copy_construct_parameter(std::ofstream&out,const Parameter&p)
{
  out << ",\n   " << p.code_name() << "(p." << p.code_name() << ")";
}
/*--------------------------------------------------------------------------*/
void make_copy_construct_parameter_list(std::ofstream& out,
					const Parameter_List& pl)
{
  for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_copy_construct_parameter(out, **p);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
