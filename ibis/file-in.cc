/*$Id: file-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include <fcntl.h>
#include "file.h"
/*--------------------------------------------------------------------------*/
void Redistribution::parse(CS& file)
{
       set(file, "Yes",	    &_r, dYES)
    || set(file, "No",	    &_r, dNO)
    || set(file, "Specific",&_r, dSPECIFIC)
    || file.warn(0, "need Yes, No, or Specific");
}
/*--------------------------------------------------------------------------*/
void Overhead::parse(CS& file)
{
  skip_comment(file);
  //scan_get(file, "[Ibis_Ver]", &_ibis_ver);
  file.dmatch("[Begin_Header]");// || file.warn(0, "need [Begin_Header]");
  int here = file.cursor();
  do {
    skip_comment(file);
    get(file, "[Ibis_Ver]",		&_ibis_ver);
    get(file, "[Ibis_Cn_Model_Ver]",	&_cn_ver);
    get(file, "[File_Name]",		&_file_name);
    get(file, "[File_Rev]",		&_file_rev);
    get(file, "[Date]",			&_date);
    get(file, "[Source]",		&_source);
    get(file, "[Notes]",		&_notes);
    get(file, "[Disclaimer]",		&_disclaimer);
    get(file, "[Copyright]",		&_copyright);
    get(file, "[Support]",		&_support);
    get(file, "[Redistribution_Text]",	&_redistribution_text);
    get(file, "[Redistribution]",	&_redistribution);
  } while (file.more() && !file.stuck(&here));
  file.dmatch("[End_Header]");// || file.warn(0, "need [End_Header]");
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Model_Choice::parse(CS& file)
{
  file >> _name >> _description;
}
/*--------------------------------------------------------------------------*/
void Model_Selector::parse(CS& file)
{
  file >> _name;
  List<Model_Choice>::parse(file);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Ibis_File::parse(CS& file)
{
  int here = file.cursor();
  {for (;;) {
    skip_comment(file);
    get(file, "[Component]",		  &_components);
    get(file, "[Model_Selector]",	  &_model_selectors);
    get(file, "[Begin_Board_Description]",&_boards);
    get(file, "[Model]",		  &_models);
    get(file, "[Define_Package_Model]",   &_package_models);
    get(file, "[Define_",		  &_macros);
    {if (!file.more()) {
      untested();
      file.warn(0, "file ends without [End]");
      break;
    }else if (file.dmatch("[End]")) {
      break;
    }else if (file.stuck(&here)) {
      get(file, "[", &_instances);
      if (file.stuck(&here)) {
	file.warn(0, "what's this? -- giving up");
	break;
      }
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/* fix_keywords
 * "Fix" the capitalization so further parsing can comply
 * with the funky IBIS capitalization and tokenizing rules.
 * IBIS is mostly case sensitive, but ...
 *	Case is ignored within "keywords", which are names enclosed in
 *		square brackets.
 *	Sometimes a non-bracket word must match the funky capitalization
 *		in the documentation, so ...
 *	Case is converted so each word begins UPPER case, and continues
 *		lower case.
 * Usually, tokens are separated by spaces, but a space is equivalent to
 *	an underscore when enclosed in square brackets.
 * Don't blame me.  I didn't make the rules.
 */
void Ibis_File::fix_keywords(CS& file)
{
  char* data = const_cast<char*>(file.fullstring());
  assert(data);
  bool in_key = false;
  bool in_comment = false;
  bool in_word = false;
  {for (char* i=data; *i; ++i) {
    {if (in_key) {
      {if (*i == ' ' || *i == '_') {
	*i = '_';
	in_word = false;
      }else if (*i == '[') {
	untested();
	file.warn(0, i-data, "[ within []");
      }else if (*i == ']') {
	in_key = false;
	in_word = false;
      }else{
	{if (!in_word) {
	  *i = toupper(*i);
	  in_word = true;
	}else{
	  *i = tolower(*i);
	}}
      }}
    }else if (in_comment) {
      if (*i == '\n') {
	in_comment = false;
      }
    }else{
      {if (*i == '[') {
	in_key = true;
	assert(in_word == false);
      }else if (*i == '|' && i[1] != '|' && i[-1] != '|' ) {
	in_comment = true;
      }else if (*i == ']') {
	untested();
	file.warn(0, i-data, "] without []");
      }else{
	// leave it as is
      }}
    }}
  }}
}
/*--------------------------------------------------------------------------*/
Ibis_File::Ibis_File(const std::string& file_name)
  :_name(file_name),
   _file(CS_FILE(), file_name),
   _macro_file(0),
   _instances(&_macros)
{
  fix_keywords(_file);
  _file >> _overhead;

  if (ibis_ver() != "") {
    _macro_file = new CS(CS_FILE(), "ibis." + ibis_ver() + ".ibisx");
    assert(_macro_file);
    fix_keywords(*_macro_file);
    *_macro_file >> _macro_overhead;
    parse(*_macro_file);
  }
  parse(_file);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
