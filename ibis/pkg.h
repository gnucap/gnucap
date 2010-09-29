/*$Id: pkg.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
/*--------------------------------------------------------------------------*/
class Package_Model
  :public Base
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Path_List
    :public List<Path>
  {
  public:
    explicit Path_List() {}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Matrix
    :public Base
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    class Type
      :public Base
    {
    public:
      enum Tag {NA, FULL, BANDED, SPARSE};
    private:
      Tag _type;
      void parse(CS&);
      void dump(std::ostream& o)const;
    public:
      explicit Type() : _type(NA) {}
      Tag tag()const {return _type;}
      bool operator==(Type::Tag x)const {return (_type == x);}
    };
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    class Row
      :public Base
    {
    private:
      int		   _size;
      std::vector<double>  _data;
      std::map<int,double> _sparse;
      void parse(CS&)  {unreachable();}
      void dump(std::ostream& o)const;
    public:
      explicit Row(CS& file, int size);
    };
    /*- - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  private:
    Type	_type;
    int		_n_rows;
    int		_bandwidth;
    std::vector<const Row*> _data;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Matrix(): _type(), _n_rows(0), _bandwidth(0) {}
    ~Matrix() {for (int i=0; i<_n_rows; ++i) {delete _data[i];}}
    void set_size(int n) {_n_rows = n; _data.resize(n);
      std::fill(_data.begin(), _data.end(), static_cast<const Row*>(0));}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  Tail_String	_name;
  Tail_String	_manufacturer;
  Tail_String	_oem;
  Tail_String	_description;
  int		_n_sections;
  int		_n_pins;
  Path_List	_pins;
  Matrix	_resistance_matrix;
  Matrix	_inductance_matrix;
  Matrix	_capacitance_matrix;

  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Package_Model(CS& file):_n_sections(0), _n_pins(0) {parse(file);}
  const std::string&	name()const		{return _name;}
  const std::string&	manufacturer()const	{return _manufacturer;}
  const std::string&	oem()const		{return _oem;}
  const std::string&	description()const	{return _description;}
  const int		n_sections()const	{return _n_sections;}
  const int		n_pins()const		{return _n_pins;}
  const Path_List&	pins()const		{return _pins;}
  const Matrix& resistance_matrix()const	{return _resistance_matrix;}
  const Matrix& inductance_matrix()const	{return _inductance_matrix;}
  const Matrix& capacitance_matrix()const	{return _capacitance_matrix;}
};
/*--------------------------------------------------------------------------*/
class Package_Model_List
  :public Collection<Package_Model>
{
public:
  explicit Package_Model_List() {}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
