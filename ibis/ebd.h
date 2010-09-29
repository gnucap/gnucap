/*$Id: ebd.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "path.h"
/*--------------------------------------------------------------------------*/
class Board
  :public Base
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Pin
    :public Base
  {
  private:
    std::string	_name;
    std::string _signal;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Pin(CS& file)		{parse(file);}
    const std::string& name()const	{return _name;}
    const std::string& signal()const	{return _signal;}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
public:
  class Pin_List
    :public List<Pin>
  { 
  private:
    void parse(CS& f) {skip_tail(f); List<Pin>::parse(f);}
  public:
    explicit Pin_List() {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Path_List
    :public Collection<Path>
  {
  private:
    void dump(std::ostream& o)const;
  public:
    explicit Path_List() {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  class Ref_Des_Map
    :public Base
  {
  private:
    std::string  _name;
    std::string  _file;
    Tail_String  _component;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Ref_Des_Map(CS& file)	{parse(file);}
    const std::string& name()const	{return _name;}
    const std::string& file()const	{return _file;}
    const std::string& component()const	{return _component;}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
public:
  class Ref_Des_Map_List
    :public List<Ref_Des_Map>
  {
  public:
    explicit Ref_Des_Map_List() :List<Ref_Des_Map>() {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  Tail_String	    _name;
  Tail_String	    _manufacturer;
  int		    _n_pins;
  Pin_List	    _pins;
  Path_List	    _paths;
  Ref_Des_Map_List  _ref_des_map;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Board(CS& file) : _n_pins(0) {parse(file);}
  const std::string&	  name()const		{return _name;}
  const std::string&	  manufacturer()const	{return _manufacturer;}
  int			  n_pins()const		{return _n_pins;}
  const Pin_List&	  pins()const		{return _pins;}
  const Path_List&	  paths()const		{return _paths;}
  const Ref_Des_Map_List& ref_des_map()const	{return _ref_des_map;}
};
/*--------------------------------------------------------------------------*/
class Board_List
  :public Collection<Board>
{
public:
  explicit Board_List() {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
