/*$Id: mg_out_lib.cc,v 24.5 2003/04/27 01:04:58 al Exp $ -*- C++ -*-
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
 */
#include "mg_out.h"
/*--------------------------------------------------------------------------*/
void make_final_adjust_parameter(std::ofstream& out, const Parameter& p)
{
  if (!(p.calculate().empty())) {
    out << "  " << p.code_name() << " = " << p.calculate() << ";\n";
  }
  if (!(p.final_default().empty())) {
    out << "  if (" << p.code_name() << " == NA) {\n"
      "    " << p.code_name() << " = " << p.final_default() << ";\n"
      "  }\n";
  }
  if (!(p.quiet_min().empty())) {
    out << "  " << p.code_name() << " = std::max(" 
	<< p.code_name() << ", " << p.quiet_min() << ");\n";
  }
  if (!(p.quiet_max().empty())) {
    out << "  " << p.code_name() << " = std::min(" 
	<< p.code_name() << ", " << p.quiet_max() << ");\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_final_adjust_parameter_list(std::ofstream& out, const Parameter_List& pl)
{
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_final_adjust_parameter(out, **p);
  }}
}
/*--------------------------------------------------------------------------*/
void make_final_adjust(std::ofstream& out, const Parameter_Block& b)
{
  out << b.code_pre();
  make_final_adjust_parameter_list(out, b.override());
  make_final_adjust_parameter_list(out, b.raw());
  out << b.code_mid();
  make_final_adjust_parameter_list(out, b.calculated());
  out << b.code_post();
}
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
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_construct_parameter(out, **p);
  }}
}
/*--------------------------------------------------------------------------*/
static void make_get_one_param(std::ofstream& out, const Parameter& p,
		     const std::string& name)
{
  if (!(name.empty())) {
    out << "    || get(cmd, \"" << name << "\", &" << p.code_name();
    {if (!(p.offset().empty())) {
      out << ", mOFFSET, " << p.offset();
    }else if (p.positive()) {
      out << ", mPOSITIVE";
    }else if (p.octal()) {
      out << ", mOCTAL";
    }else if (!(p.scale().empty())) {
      out << ", mSCALE, " << p.scale();
    }}
    out << ")\n";
  }
}
/*--------------------------------------------------------------------------*/
void make_get_param_list(std::ofstream& out, const Parameter_List& pl)
{
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_get_one_param(out, **p, (**p).user_name());
    make_get_one_param(out, **p, (**p).alt_name());
  }}
}
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
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_print_one_param(out, **p);
  }}
}
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
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_print_one_calc_param(out, **p);
  }}
}
/*--------------------------------------------------------------------------*/
static void make_copy_construct_parameter(std::ofstream&out,const Parameter&p)
{
  out << ",\n   " << p.code_name() << "(p." << p.code_name() << ")";
}
/*--------------------------------------------------------------------------*/
void make_copy_construct_parameter_list(std::ofstream& out,
					const Parameter_List& pl)
{
  {for (Parameter_List::const_iterator p = pl.begin(); p != pl.end(); ++p) {
    make_copy_construct_parameter(out, **p);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
