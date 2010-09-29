/*$Id: pkg-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
/*--------------------------------------------------------------------------*/
void Package_Model::Matrix::Type::parse(CS& file)
{
       set(file, "Full_matrix",	  &_type, FULL)
    || set(file, "Banded_matrix", &_type, BANDED)
    || set(file, "Sparse_matrix", &_type, SPARSE)
    || file.warn(0, "need Full_matrix, Banded_matrix, or Sparse_matrix");
}
/*--------------------------------------------------------------------------*/
Package_Model::Matrix::Row::Row(CS& file, int size)
  :_size(size),
   _data(size,NA),
   _sparse()
{
  {if (_size > 0) {
    for (int i=0; i<_size; ++i) {
      file >> _data[i];
    }
  }else{
    int here = file.cursor();
    for (;;) {
      skip_comment(file);
      {if (file.match1('[')) {
	break;
      }else{
	int index;
	double value;
	file >> index >> value;
	{if (!file.stuck(&here)) {
	  _sparse[index] = value;
	}else{
	  untested();
	  file.warn(0, "malformed sparse data");
	  break;
	}}
      }}
    }
  }}
}
/*--------------------------------------------------------------------------*/
void Package_Model::Matrix::parse(CS& file)
{
  file >> _type;  
  {if (_type == Type::BANDED) {
    get(file, "[Bandwidth]", &_bandwidth) || file.warn(0, "need Bandwidth");
    if (_bandwidth > _n_rows) {
      untested();
      file.warn(0, "Bandwidth must be less than Number Of Pins");
    }
  }else{
    _bandwidth = _n_rows;
  }}
  int here = file.cursor();
  for (;;) {
    skip_comment(file);
    {if (file.dmatch("[Row]")) {
      int row_number;
      file >> row_number;
      {if (row_number > _n_rows) {
	untested();
	file.warn(0, "row number too high");
      }else if (_data[row_number-1]) {
	untested();
	file.warn(0, "duplicate row");
      }else{
	int row_size = 0;
	switch (_type.tag()) {
	case Type::NA:	   unreachable(); break;
	case Type::FULL:   row_size = _n_rows - row_number + 1; break;
	case Type::BANDED: row_size = _bandwidth + 1; break;
	case Type::SPARSE: row_size = 0; break;
	}
	Row* x = new Row(file, row_size);
	{if (!file.stuck(&here)) {
	  _data[row_number-1] = x;
	}else{
	  untested();
	  delete x;
	  file.warn(0, "need row data");
	}}
      }}
    }else if (file.match1('[')) {
      break;
    }else if (file.stuck(&here)) {
      untested();
      file.warn(0, "need [Row]");
      break;
    }else{
      unreachable();
    }}
  }
}
/*--------------------------------------------------------------------------*/
void Package_Model::parse(CS& file)
{
  file >> _name;
  int here = file.cursor();
  for (;;) {
    skip_comment(file);
    get(file, "[Manufacturer]",		&_manufacturer);
    get(file, "[Oem]",			&_oem);
    get(file, "[Description]",		&_description);
    get(file, "[Number_Of_Sections]",	&_n_sections);
    get(file, "[Number_Of_Pins]",	&_n_pins);
    get(file, "[Pin_Numbers]",		&_pins);
    {if (file.dmatch("[End_Package_Model]")) {
      break;
    }else if (file.dmatch("[Model_Data]")) {
      _resistance_matrix.set_size(_n_pins);
      _inductance_matrix.set_size(_n_pins);
      _capacitance_matrix.set_size(_n_pins);
      for (;;) {
	skip_comment(file);
	{if (file.dmatch("[End_Model_Data]")) {
	  break;
	}else if (file.stuck(&here)) {
	  untested();
	  file.warn(0, "need Matrix or End Model Data");
	  break;
	}else{
	  get(file, "[Resistance_Matrix]",	&_resistance_matrix);
	  get(file, "[Inductance_Matrix]",	&_inductance_matrix);
	  get(file, "[Capacitance_Matrix]",	&_capacitance_matrix);
	}}	
      }
    }else if (file.stuck(&here)) {
      untested();
      file.warn(0, "need more package parameters");
      break;
    }}
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
