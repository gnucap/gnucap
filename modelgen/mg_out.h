/*$Id: mg_out.h,v 26.128 2009/11/10 04:21:03 al Exp $ -*- C++ -*-
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
 */
#include <fstream>
#include "mg_.h"
/*--------------------------------------------------------------------------*/
/* mg_out_root.cc */
void make_h_file(const File&);
void make_cc_file(const File&);
void make_dump_file(const File&);
/*--------------------------------------------------------------------------*/
/* mg_out_lib.cc */
void make_final_adjust_eval_parameter_list(std::ofstream&, const Parameter_List&);
void make_final_adjust_value(std::ofstream&, const Parameter&);
void make_final_adjust_value_list(std::ofstream&, const Parameter_List&);
void make_final_adjust_parameter(std::ofstream&, const Parameter&);
void make_final_adjust_parameter_list(std::ofstream&, const Parameter_List&);
void make_final_adjust(std::ofstream&, const Parameter_Block&);
void make_construct_parameter_list(std::ofstream&, const Parameter_List&);
void make_get_param_list(std::ofstream&, const Parameter_List&);
void make_print_param_list(std::ofstream&, const Parameter_List&);
void make_print_calc_param_list(std::ofstream&, const Parameter_List&);
void make_copy_construct_parameter_list(std::ofstream&, const Parameter_List&);
/*--------------------------------------------------------------------------*/
/* mg_out_model.cc */
void make_cc_model(std::ofstream&, const Model&);
/* mg_out_dev.cc */
void make_cc_dev(std::ofstream&, const Device&);
/* mg_out_common.cc */
void make_cc_common(std::ofstream&, const Device&);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
