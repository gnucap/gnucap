/*$Id: base-math.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "base.h"
/*--------------------------------------------------------------------------*/
std::string new_name()
{
  static int c = 0;
  char s[30];
  sprintf(s, "EX%04u", ++c);
  return s;
}
/*--------------------------------------------------------------------------*/
Base* String::not_equal(const String& x)const
{
  {if (_data == x._data) {
    untested();
    return new String(_data);
  }else{
    untested();
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* String::equal(const String& x)const
{
  {if (_data == x._data) {
    return new String(_data);
  }else{
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* Float::less_rev(const Float& x)const
{
  {if (_data > x._data) {
    untested();
    return new Float(x._data);
  }else{
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* Float::greater_rev(const Float& x)const
{
  {if (_data < x._data) {
    return new Float(x._data);
  }else{
    untested();
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* Float::not_equal(const Float& x)const
{
  {if (_data != x._data) {
    untested();
    return new Float(_data);
  }else{
    untested();
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* Float::equal(const Float& x)const
{
  {if (_data == x._data) {
    return new Float(_data);
  }else{
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
Base* Vector::minus()const
{
  Vector* new_vector = new Vector;
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    Vector_Item* vi = new Vector_Item((**i).x(), -(**i).y());
    new_vector->push(vi);
  }}
  return new_vector;
}
/*--------------------------------------------------------------------------*/
Base* Vector::invert()const
{
  Vector* new_vector = new Vector;
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    Vector_Item* vi = new Vector_Item((**i).x(), 1 / (**i).y());
    new_vector->push(vi);
  }}
  return new_vector;
}
/*--------------------------------------------------------------------------*/
Base* Vector::add(const Float& x)const
{
  Vector* new_vector = new Vector;
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    Vector_Item* vi = new Vector_Item((**i).x(), (**i).y() + x);
    new_vector->push(vi);
  }}
  return new_vector;
}
/*--------------------------------------------------------------------------*/
Base* Vector::multiply(const Float& x)const
{
  Vector* new_vector = new Vector;
  {for (const_iterator i = begin(); i != end(); ++i) {
    assert(*i);
    Vector_Item* vi = new Vector_Item((**i).x(), (**i).y() * x);
    new_vector->push(vi);
  }}
  return new_vector;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
