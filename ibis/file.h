/*$Id: file.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "pkg.h"
#include "comp.h"
#include "ebd.h"
#include "model.h"
#include "macro.h"
/*--------------------------------------------------------------------------*/
class Redistribution
  :public Base
{
private:
  enum {dUNKNOWN, dNO, dYES, dSPECIFIC} _r;
  void parse(CS&);
  void dump(std::ostream&)const {incomplete();}
public:
  explicit Redistribution() : _r(dUNKNOWN) {}
};
/*--------------------------------------------------------------------------*/
class Overhead
  :public Base
{
private:
  Tail_String	_ibis_ver;
  Tail_String	_cn_ver;
  std::string	_file_name;
  Tail_String	_file_rev;
  Tail_String	_date;
  Text_Block	_source;
  Text_Block	_notes;
  Text_Block	_disclaimer;
  Text_Block	_copyright;
  Text_Block	_support;
  Text_Block	_redistribution_text;
  Redistribution _redistribution;

  void parse(CS& f);
  void dump(std::ostream& o)const;
public:
  explicit Overhead() {}
  const std::string&	ibis_ver()const  {return _ibis_ver;}
  const std::string&	cn_ver()const	 {return _cn_ver;}
  const std::string&	file_name()const {return _file_name;}
  const std::string&	file_rev()const	 {return _file_rev;}
  const std::string&	date()const	 {return _date;}
  const std::string&	source()const	 {return _source;}
  const std::string&	notes()const	 {return _notes;}
  const std::string&	disclaimer()const{return _disclaimer;}
  const std::string&	copyright()const {return _copyright;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Model_Choice
  :public Base
{
private:
  std::string	_name;
  Tail_String  _description;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Model_Choice(CS& file)	{parse(file);}
  const std::string& name()const	{return _name;}
  const std::string& description()const {return _description;}
};
/*--------------------------------------------------------------------------*/
class Model_Selector
  :public List<Model_Choice>
{
private:
  Tail_String	_name;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Model_Selector(CS& file) {parse(file);}
  const std::string& name()const    {return _name;}
};
/*--------------------------------------------------------------------------*/
class Model_Selector_List
  :public Collection<Model_Selector>
{
public:
  explicit Model_Selector_List() {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Ibis_File
  :public Base
{
private:
  std::string		_name;		// all files
  CS			_file;
  CS*			_macro_file;
  Overhead		_macro_overhead;
  Overhead		_overhead;
  Component_List	_components;	// "ibis"
  Model_Selector_List	_model_selectors;
  Package_Model_List	_package_models;
  Board_List		_boards;	// ebd
  Model_List		_models;
  Macro_List		_macros;	// ibis-x
  Instance_List		_instances;	// ibis-x

  void parse(CS& f);
  void dump(std::ostream& o)const;
  void fix_keywords(CS& file);
public:
  explicit Ibis_File(const std::string& file_name);
			     ~Ibis_File()	    {delete _macro_file;}
  const std::string&	     name()const	    {return _name;}
  const std::string&	     ibis_ver()const {return overhead().ibis_ver();}
  const Overhead&	     overhead()const	    {return _overhead;}
  const Component_List&	     components()const	    {return _components;}
  const Model_Selector_List& model_selectors()const {return _model_selectors;}
  const Package_Model_List&  package_models()const  {return _package_models;}
  const Board_List&	     boards()const	    {return _boards;}
  const Model_List&	     models()const	    {return _models;}
  const Macro_List&	     macros()const	    {return _macros;}
  const Instance_List&	     instances()const	    {return _instances;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
