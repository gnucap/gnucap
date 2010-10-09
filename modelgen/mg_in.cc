/*$Id: mg_in.cc,v 26.81 2008/05/27 05:33:43 al Exp $ -*- C++ -*-
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
//testing=script 2006.10.31
#include "mg_.h"
/*--------------------------------------------------------------------------*/
static C_Comment   dummy_c_comment;
static Cxx_Comment dummy_cxx_comment;
/*--------------------------------------------------------------------------*/
void Parameter::parse(CS& file)
{
  file >> _type >> _code_name >> _comment;
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || Set(file, "positive",	     &_positive,	true)
      || Set(file, "octal",	     &_octal,		true)
      || ((file >> "name =")	       && (file >> _user_name))
      || ((file >> "alt_name =")       && (file >> _alt_name))
      || ((file >> "default =")	       && (file >> _default_val))
      || ((file >> "offset =")	       && (file >> _offset))
      || ((file >> "print_test =")     && (file >> _print_test))
      || ((file >> "calc_print_test =")&& (file >> _calc_print_test))
      || ((file >> "scale =")	       && (file >> _scale))
      || ((file >> "calculate =")      && (file >> _calculate))
      || ((file >> "quiet_min =")      && (file >> _quiet_min))
      || ((file >> "quiet_max =")      && (file >> _quiet_max))
      || ((file >> "final_default =")  && (file >> _final_default))
      || ((file >> "/*")	       && (file >> dummy_c_comment))
      || ((file >> "//")	       && (file >> dummy_cxx_comment))
      ;
    if (file.skip1b(";,")) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (parameter)");
      break;
    }else if (file.stuck(&here)) {untested();
      file.warn(0, "need ;");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Parameter::print(std::ostream& out)const
{
  out << "\n"
    "      " << type() << " " << code_name() << " \"" << comment() << "\"\n        "
    "name=\"" << user_name() << "\" "
    "alt_name=\"" << alt_name() << "\" "
    "default=\"" << default_val() << "\" "
    "offset=\"" << offset() << "\"\n        ";
  if (positive()) {
    out << "positive ";
  }
  if (octal()) {
    out << "octal ";
  }
  out <<
    "print_test=\"" << print_test() << "\" "
    "calc_print_test=\"" << calc_print_test() << "\"\n        "
    "scale=\"" << scale() << "\" "
    "calculate=\"" << calculate() << "\"\n        "
    "quiet_min=\"" << quiet_min() << "\" "
    "quiet_max=\"" << quiet_max() << "\"\n        "
    "final_default=\"" << final_default() << "\";";
}
/*--------------------------------------------------------------------------*/
void Code_Block::parse(CS& file) 
{
  // skips the code block, delimited by {}
  // also checks paren balance, so you can have {} inside the block

  int paren = file.skip1("{");
  if (paren == 0) {untested();
    file.warn(0, "need {");
  }else{
  }
  unsigned here = file.cursor();
  unsigned begin = here;
  unsigned end = here;
  for (;;) {
    paren -= file.skip1b("])");
    if (paren == 0) {untested();
      file.warn(0, "unbalanced {}[]()");
      break;
    }else{
    }
    end = file.cursor();
    paren -= file.skip1b("}");
    if (paren == 0) {
      unsigned ihere = file.cursor();
      while (file.reset(--end).peek() != '\n' && end >= begin) {
      }
      ++end;
      file.reset(ihere);
      break;
    }else{
    }
    if (file.stuck(&here)) {untested();
      file.warn(0, "syntax error");
      break;
    }else{
    }
    paren += file.skip1b("{[(");
    file.skip1b(";=");
    std::string foo;
    file >> foo;
  }
  s = file.substr(begin, end - begin);
}
/*--------------------------------------------------------------------------*/
static void fill_in_default_names(Parameter_List& pl)
{
  for (Parameter_List::const_iterator
       p = pl.begin();
       p != pl.end();
       ++p) {
    (**p).fill_in_default_name();
  }
}
/*--------------------------------------------------------------------------*/
void Parameter_Block::parse(CS& file)
{ 
  int paren = file.skip1b("{");
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || ((file >> "unnamed ")		    && (file >> _unnamed_value))
      || ((file >> "override ")		    && (file >> _override))
      || ((file >> "raw_parameters ")	    && (file >> _raw))
      || ((file >> "calculated_parameters ")&& (file >> _calculated))
      || ((file >> "code_pre ")		    && (file >> _code_pre))
      || ((file >> "code_mid ")		    && (file >> _code_mid))
      || ((file >> "code_post ")	    && (file >> _code_post))
      || ((file >> "/*")	            && (file >> dummy_c_comment))
      || ((file >> "//")	            && (file >> dummy_cxx_comment))
      ;
    paren -= file.skip1b("}");
    if (paren == 0) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Parameter_Block)");
      break;
    }else if (file.stuck(&here)) {itested();
      file.warn(0, "bad Parameter_Code_Block");
      break;
    }else{
    }
  }
  fill_in_default_names(_calculated);
  fill_in_default_names(_raw);
  // but not _override
}
/*--------------------------------------------------------------------------*/
void Parameter_Block::print(std::ostream& out)const
{
  out << "{\n";
  if (unnamed_value() != "") {
    out << "    unnamed " << unnamed_value() << ";\n";
  }
  out <<
    "    override " << override() << "\n"
    "    raw_parameters " << raw() << "\n"
    "    calculated_parameters " << calculated() << "\n"
    "    code_pre {" << code_pre() << "  }\n"
    "    code_mid {" << code_mid() << "  }\n"
    "    code_post {" << code_post() << "  }\n"
    "  }\n";
}
/*--------------------------------------------------------------------------*/
void Eval::parse(CS& file)
{
  file >> _name >> _code;
}
/*--------------------------------------------------------------------------*/
void Eval::print(std::ostream& out)const
{
  out << "  eval " << name() << " {"
      << code() <<
    "  }\n";
}
/*--------------------------------------------------------------------------*/
void Function::parse(CS& file)
{
  file >> _name;
  _name += '(' + file.ctos("{", "(", ")") + ')';
  file >> _code;
}
/*--------------------------------------------------------------------------*/
void Function::print(std::ostream& out)const
{
  out << "  function " << name() << " {"
      << code() <<
    "  }\n";
}
/*--------------------------------------------------------------------------*/
void Element::parse(CS& file)
{
  file >> _dev_type >> _name >> _port_list;
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || ((file >> "eval =")	&& (file >> _eval))
      || ((file >> "value =")	&& (file >> _value))
      || ((file >> "args =")	&& (file >> _args))
      || ((file >> "omit =")	&& (file >> _omit))
      || ((file >> "reverse =")	&& (file >> _reverse))
      || ((file >> "state =")	&& (file >> _state))
      || ((file >> "/*")	&& (file >> dummy_c_comment))
      || ((file >> "//")	&& (file >> dummy_cxx_comment))
      ;
    if (file.skip1b(";")) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Element)");
      break;
    }else if (file.stuck(&here)) {untested();
      file.warn(0, "need ;");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Element::print(std::ostream& out)const
{
  out << "    " << dev_type() << " " << name() << " " << ports();
  if (eval() != "") {
    out << " eval=\"" << eval() << "\"";
  }
  if (value() != "") {
    out << " value=\"" << value() << "\"";
  }
  if (args() != "") {
    out << " args=\"" << args() << "\"";
  }
  if (omit() != "") {
    out << " omit=\"" << omit() << "\"";
  }
  if (reverse() != "") {
    out << " reverse=\"" << reverse() << "\"";
  }
  if (state() != "") {
    out << " state=\"" << state() << "\"";
  }
  out << ";\n";
}
/*--------------------------------------------------------------------------*/
void Arg::parse(CS& file)
{
  file.skipbl();
  _arg = file.get_to(";");
  file.skip1b(";");
}
/*--------------------------------------------------------------------------*/
void Circuit::parse(CS& file)
{
  int paren = file.skip1b("{");
  (file >> "/*")     && (file >> dummy_c_comment);
  (file >> "//")     && (file >> dummy_cxx_comment);
  (file >> "sync ;") && (_sync = true);
  (file >> "ports ") && ((file >> _required_nodes >> _optional_nodes >> ';')
			 || file.warn(0, "need ports"));
  (file >> "local_nodes ") && (file >> _local_nodes >> ';');
  unsigned here = file.cursor();
  do {
    (file >> "args ") && (file >> _args_list);
  } while (file.more() && !file.stuck(&here));
  for (;;) {
    paren -= file.skip1b("}");
    if (paren == 0) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Circuit)");
      break;
    }else{
    }
    ONE_OF
      || ((file >> "/*") && (file >> dummy_c_comment))
      || ((file >> "//") && (file >> dummy_cxx_comment))
      || (file >> _element_list)
      ;
    if (file.stuck(&here)) {untested();
      file.warn(0, "bad Circuit");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Circuit::print(std::ostream& out)const
{
  out << " {\n";
  if (sync()) {
    out << "  sync;\n";
  }
  out <<
    "  ports " << req_nodes() << opt_nodes() << ";\n"
    "  local_nodes " << local_nodes() << "\n"
	       << args_list()
	       << elements() <<
    "}\n";
}
/*--------------------------------------------------------------------------*/
void Model::parse(CS& file)
{
  file >> _name;
  int paren = file.skip1b("{");
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || ((file >> "hide_base ")	     && (file >> _hide_base))
      || ((file >> "level ")		     && (file >> _level))
      || ((file >> "dev_type ")		     && (file >> _dev_type))
      || ((file >> "inherit ")		     && (file >> _inherit))
      || ((file >> "public_keys ")	     && (file >> _public_key_list))
      || ((file >> "private_keys ")	     && (file >> _private_key_list))
      || ((file >> "independent ")	     && (file >> _independent))
      || ((file >> "size_dependent ")	     && (file >> _size_dependent))
      || ((file >> "temperature_dependent ") && (file >> _temperature))
      || ((file >> "tr_eval ")		     && (file >> _tr_eval))
      || ((file >> "validate ")		     && (file >> _validate))
      || ((file >> "/*")		     && (file >> dummy_c_comment))
      || ((file >> "//")		     && (file >> dummy_cxx_comment))
      ;
    paren -= file.skip1b("}");
    if (paren == 0) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Model)");
      break;
    }else if (file.stuck(&here)) {itested();
      file.warn(0, "bad Model");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Model::print(std::ostream& out)const
{
  out << "model " << name() << " {\n";
  if (hide_base()) {
    out << "  hide_base;\n";
  }
  out << 
    "  level = " << level() << ";\n"
    "  dev_type = " << dev_type() << ";\n"
    "  inherit = " << inherit() << ";\n"
    "  public_keys " << public_key_list() << "\n"
    "  private_keys " << private_key_list() << "\n"
    "  independent " << independent() << "\n"
    "  size_dependent " << size_dependent() << "\n"
    "  temperature_dependent " << temperature() << "\n"
    "  tr_eval {" << tr_eval() << "  }\n"
    "  validate {" << validate() << "  }\n"
    "}\n";
}
/*--------------------------------------------------------------------------*/
void Device::parse(CS& file)
{
  file >> _name;
  int paren = file.skip1b("{");
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || ((file >> "parse_name ") && (file >> _parse_name))
      || ((file >> "id_letter ")  && (file >> _id_letter))
      || ((file >> "model_type ") && (file >> _model_type))
      || ((file >> "circuit ")	  && (file >> _circuit))
      || ((file >> "tr_probe ")   && (file >> _probes))
      || ((file >> "device ")	  && (file >> _device))
      || ((file >> "common ") 	  && (file >> _common))
      || ((file >> "tr_eval ")	  && (file >> _tr_eval))
      || ((file >> "eval ")	  && (file >> _eval_list))
      || ((file >> "function ")   && (file >> _function_list))
      || ((file >> "/*")	  && (file >> dummy_c_comment))
      || ((file >> "//")	  && (file >> dummy_cxx_comment))
      ;
    paren -= file.skip1b("}");
    if (paren == 0) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Device)");
      break;
    }else if (file.stuck(&here)) {itested();
      file.warn(0, "bad Device");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Device::print(std::ostream& out)const
{
  out <<
    "device " << name() << " {\n"
    "  parse_name " << parse_name() << ";\n"
    "  id_letter " << id_letter() << ";\n"
    "  model_type " << model_type() << ";\n"
    "  circuit " << circuit() << "\n"
    "  tr_probe " << probes() << "\n"
    "  device " << device() << "\n"
    "  common " << common() << "\n"
    "  tr_eval {" << tr_eval() << "  }\n"
	      << eval_list()
	      << function_list() <<
    "}\n";
}
/*--------------------------------------------------------------------------*/
void C_Comment::parse(CS& file)
{
  unsigned here = file.cursor();
  for (;;) {
    file.skipto1('*');
    if (file.umatch("*/")) {
      break;  // done with comment
    }else if (file.stuck(&here)) {untested();
      file.warn(0, "unterminated C comment");
      break;
    }else{untested();
      file.skip();
    }
  }
}
/*--------------------------------------------------------------------------*/
void Cxx_Comment::parse(CS& file)
{
  unsigned here = file.cursor();
  file.skipto1('\n');
  if (file.stuck(&here)) {untested();
    file.warn(0, "unterminated C++ comment");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void Port::parse(CS& file)
{
  file >> _name;
  unsigned here = file.cursor();
  for (;;) {
    ONE_OF
      || ((file >> "short_to =") && (file >> _short_to))
      || ((file >> "short_if =") && (file >> _short_if))
      ;
    if (file.skip1b(";")) {
      break;
    }else if (!file.more()) {untested();
      file.warn(0, "premature EOF (Port)");
      break;
    }else if (file.stuck(&here)) {
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void Port::print(std::ostream& out)const
{
  if (short_to() != "" || short_if() != "") {
    out << name() << " short_to=\"" << short_to() 
	<< "\" short_if=\"" << short_if() << "\";\n";
  }else{
    out << name() << "; ";
  }
}
/*--------------------------------------------------------------------------*/
void Head::parse(CS& file)
{
  unsigned here = file.cursor();
  unsigned begin = 0;
  unsigned end = here;
  for (;;) {
    file.skipto1('*');
    if (file.umatch("*/")) {
      end = file.cursor();
      break;  // done with head
    }else if (file.stuck(&here)) {untested();
      file.warn(0, "unterminated head");
      break;
    }else{
      file.skip();
    }
  }
  s = file.substr(begin, end-begin);
}
/*--------------------------------------------------------------------------*/
File::File(const std::string& file_name)
  :_name(file_name),
   _file(CS::_WHOLE_FILE, file_name)
{
  (_file >>  "/*") && (_file >> _head);
  unsigned here = _file.cursor();
  for (;;) {
    ONE_OF
      || ((_file >> "/*")	  && (_file >> dummy_c_comment))
      || ((_file >> "//")	  && (_file >> dummy_cxx_comment))
      || ((_file >> "h_headers ") && (_file >> _h_headers))
      || ((_file >> "cc_headers ")&& (_file >> _cc_headers))
      || ((_file >> "device ")	  && (_file >> _device_list))
      || ((_file >> "model ") 	  && (_file >> _model_list))
      || ((_file >> "h_direct ")  && (_file >> _h_direct))
      || ((_file >> "cc_direct ") && (_file >> _cc_direct))
      ;
    if (!_file.more()) {
      break;
    }else if (_file.stuck(&here)) {itested();
      _file.warn(0, "syntax error, need head or model");
      break;
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
