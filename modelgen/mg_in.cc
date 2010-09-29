/*$Id: mg_in.cc,v 20.14 2001/10/19 06:21:15 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
#include <fcntl.h>
#include <stdio.h>
#include "mg_.h"
/*--------------------------------------------------------------------------*/
static C_Comment   dummy_c_comment;
static Cxx_Comment dummy_cxx_comment;
/*--------------------------------------------------------------------------*/
template <class T, char BEGIN, char END>
void List<T, BEGIN, END>::parse(CS& file)
{
  int paren = file.skip1b(BEGIN);
  int here = file.cursor();
  {for (;;) {
    get(file, "/*$$", &dummy_c_comment);
    get(file, "//$$", &dummy_cxx_comment);
    paren -= file.skip1b(END);
    if (paren == 0) {
      break;
    }
    T* p = new T(file);
    {if (!file.stuck(&here)) {
      _list.push_back(p);
    }else {
      delete p;
      file.warn(0, "not valid here");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
void Collection<T>::parse(CS& file)
{
  int here = file.cursor();
  T* m = new T(file);
  {if (!file.stuck(&here)) {
    _list.push_back(m);
  }else{
    delete m;
    file.warn(0, "what's this??");
  }}
}
/*--------------------------------------------------------------------------*/
void Parameter::parse(CS& file)
{
  file >> _type >> _code_name >> _comment;
  int here = file.cursor();
  {for (;;) {
    get(file, "NAME",		&_user_name);
    get(file, "ALT_name",	&_alt_name);
    get(file, "DEFAult",	&_default_val);
    get(file, "OFFSet",		&_offset);
    set(file, "POSItive",	&_positive,	true);
    set(file, "OCTAl",		&_octal,	true);
    get(file, "PRINt_test",	&_print_test);
    get(file, "CALC_Print_test",&_calc_print_test);
    get(file, "SCALe",		&_scale);
    get(file, "CALCUlate",	&_calculate);
    get(file, "QUIET_MIn",	&_quiet_min);
    get(file, "QUIET_MAx",	&_quiet_max);
    get(file, "FINAl_default",	&_final_default);
    get(file, "/*$$",		&dummy_c_comment);
    get(file, "//$$",		&dummy_cxx_comment);
    {if (file.skip1(";")) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Parameter)");
      break;
    }else if (file.stuck(&here)) {
      file.warn(0, "need ;");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Code_Block::parse(CS& file) 
{
  int paren = file.skipbl().skip1("{");
  int here = file.cursor();
  _begin = file.tail();
  {for (;;) {
    paren -= file.skip1b("])");
    if (paren == 0) {
      file.warn(0, "unbalanced {}[]()");
      break;
    }
    _end = file.tail();
    paren -= file.skip1b("}");
    if (paren == 0) {
      while (*(--_end) != '\n' && _end >= _begin) {
      }
      ++_end;
      break;
    }
    if (file.stuck(&here)) {
      file.warn(0, "syntax error");
      break;
    }
    paren += file.skip1b("{[(");
    file.skip1b(";");
    std::string foo;
    file >> foo;
  }}
}
/*--------------------------------------------------------------------------*/
void Parameter_Block::parse(CS& file)
{ 
  int paren = file.skip1b("{");
  int here = file.cursor();
  {for (;;) {
    get(file, "UNNAmed",		&_unnamed_value);
    get(file, "OVERride",		&_override);
    get(file, "RAW_parameters",		&_raw);
    get(file, "CALCulated_parameters",	&_calculated);
    get(file, "CODE_PRE",		&_code_pre);
    get(file, "CODE_POST",		&_code_post);
    get(file, "/*$$",			&dummy_c_comment);
    get(file, "//$$",			&dummy_cxx_comment);
    paren -= file.skip1b("}");
    {if (paren == 0) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Parameter_Block)");
      break;
    }else if (file.stuck(&here)) {
      file.warn(0, "bad Parameter_Code_Block");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Eval::parse(CS& file)
{
  file >> _name >> _code;
}
/*--------------------------------------------------------------------------*/
void Function::parse(CS& file)
{
  file >> _name;
  _name += '(' + file.ctos("{", '(', ')') + ')';
  file >> _code;
}
/*--------------------------------------------------------------------------*/
void Port::parse(CS& file)
{
  file >> _name;
  int here = file.cursor();
  for (;;) {
    get(file, "SHORT_TO",	&_short_to);
    get(file, "SHORT_IF",	&_short_if);
    {if (file.skip1(";,")) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Port)");
      break;
    }else if (file.stuck(&here)) {
      break;
    }}
  }
}
/*--------------------------------------------------------------------------*/
void Element::parse(CS& file)
{
  file >> _dev_type >> _name >> _port_list;
  int here = file.cursor();
  {for (;;) {
    get(file, "EVAL",	&_eval);
    get(file, "VALue",	&_value);
    get(file, "ARGS",	&_args);
    get(file, "OMIT",	&_omit);
    get(file, "REVerse",&_reverse);
    get(file, "STAte",  &_state);
    get(file, "/*$$",	&dummy_c_comment);
    get(file, "//$$",	&dummy_cxx_comment);
    {if (file.skip1(";")) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Element)");
      break;
    }else if (file.stuck(&here)) {
      file.warn(0, "need ;");
      break;
    }}
  }}
  _class_name = "DEV_" + to_upper(_dev_type);
}
/*--------------------------------------------------------------------------*/
void Arg::parse(CS& file)
{
  file.skipbl();
  _arg = file.get_to(";");
  file.skip1(";");
}
/*--------------------------------------------------------------------------*/
void Circuit::parse(CS& file)
{
  int paren = file.skip1b("{");
  get(file, "/*$$", &dummy_c_comment);
  get(file, "//$$", &dummy_cxx_comment);
  get(file, "SYNC", &_sync) && file.skip1(";");
  (get(file, "PORts", &_port_list) && file.skip1(";"))
    || file.warn(0, "need Ports");
  get(file, "LOCal_nodes", &_local_nodes) && file.skip1(";");
  int here = file.cursor();
  do {
    get(file, "ARGs", &_args_list);
  } while (file.more() && !file.stuck(&here));
  {for (;;) {
    paren -= file.skip1b("}");
    {if (paren == 0) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Circuit)");
      break;
    }}
    file >> _element_list;
    get(file, "/*$$",			&dummy_c_comment);
    get(file, "//$$",			&dummy_cxx_comment);
    if (file.stuck(&here)) {
      file.warn(0, "bad Circuit");
      break;
    }
  }}
}
/*--------------------------------------------------------------------------*/
void Model::parse(CS& file)
{
  file >> _name;
  int paren = file.skip1b("{");
  int here = file.cursor();
  {for (;;) {
    get(file, "BASE",			&_is_base);
    get(file, "LEVEl",			&_level);
    get(file, "DEV_type",		&_dev_type);
    get(file, "INHErit",		&_inherit);
    get(file, "KEYS",			&_key_list);
    get(file, "INDEpendent",		&_independent);
    get(file, "SIZE_dependent",		&_size_dependent);
    get(file, "TEMPerature_dependent",	&_temperature);
    get(file, "TR_Eval",		&_tr_eval);
    get(file, "/*$$",			&dummy_c_comment);
    get(file, "//$$",			&dummy_cxx_comment);
    paren -= file.skip1b("}");
    {if (paren == 0) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Model)");
      break;
    }else if (file.stuck(&here)) {
      file.warn(0, "bad Model");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Device::parse(CS& file)
{
  file >> _name;
  int paren = file.skip1b("{");
  int here = file.cursor();
  {for (;;) {
    get(file, "PARSe_name",	&_parse_name);
    get(file, "ID_letter",	&_id_letter);
    get(file, "MODel_type",	&_model_type);
    get(file, "CIRcuit",	&_circuit);
    get(file, "TR_Probes",	&_probes);
    get(file, "DEVIce", 	&_device);
    get(file, "COMmon", 	&_common);
    get(file, "TR_Eval",	&_tr_eval);
    get(file, "EVAL",		&_eval_list);
    get(file, "FUNction",	&_function_list);
    get(file, "/*$$",		&dummy_c_comment);
    get(file, "//$$",		&dummy_cxx_comment);
    paren -= file.skip1b("}");
    {if (paren == 0) {
      break;
    }else if (!file.more()) {
      file.warn(0, "premature EOF (Device)");
      break;
    }else if (file.stuck(&here)) {
      file.warn(0, "bad Device");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void C_Comment::parse(CS& file)
{
  int here = file.cursor();
  {for (;;) {
    file.skipto1('*');
    {if (file.pmatch("*/$$")) {
      break;  // done with comment
    }else if (file.stuck(&here)) {
      file.warn(0, "unterminated C comment");
      break;
    }else{
      file.skip();
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Cxx_Comment::parse(CS& file)
{
  int here = file.cursor();
  file.skipto1('\n');
  if (file.stuck(&here)) {
    file.warn(0, "unterminated C++ comment");
  }
}
/*--------------------------------------------------------------------------*/
void Head::parse(CS& file)
{
  int here = file.cursor();
  _begin = file.fullstring();
  {for (;;) {
    file.skipto1('*');
    {if (file.pmatch("*/$$")) {
      _end = file.tail();
      break;  // done with head
    }else if (file.stuck(&here)) {
      file.warn(0, "unterminated head");
      break;
    }else{
      file.skip();
    }}
  }}
}
/*--------------------------------------------------------------------------*/
File::File(const std::string& file_name)
  :_name(file_name),
   _file(CS_FILE(), file_name)
{
  get(_file, "/*$$",		&_head);
  int here = _file.cursor();
  {for (;;) {
    get(_file, "H_Headers",	&_h_headers);
    get(_file, "CC_Headers",	&_cc_headers);
    get(_file, "DEVice", 	&_device_list);
    get(_file, "MODel", 	&_model_list);
    get(_file, "H_Direct", 	&_h_direct);
    get(_file, "CC_Direct",	&_cc_direct);
    get(_file, "/*$$",		&dummy_c_comment);
    get(_file, "//$$",		&dummy_cxx_comment);
    {if (!_file.more()) {
      break;
    }else if (_file.stuck(&here)) {
      _file.warn(0, "syntax error, need head or model");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
