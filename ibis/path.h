/*$Id: path.h,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#ifndef IBIS_PATH_H_INCLUDED
#define IBIS_PATH_H_INCLUDED
#include "base.h"
/*--------------------------------------------------------------------------*/
class Path_Item_Base
  :public Base
{
protected:
  std::string _name;
public:
  explicit Path_Item_Base() {}
  void parse(CS&) = 0;
  void dump(std::ostream& o)const = 0;
  const std::string& name()const {return _name;}
};
/*--------------------------------------------------------------------------*/
class Path
  :public List_Base<Path_Item_Base>
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
public:
  class Pin
    :public Path_Item_Base
  {
  private:
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Pin(CS& file) {parse(file);}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Seg
    :public Path_Item_Base
  {
  private:
    Float _len;
    Float _r;
    Float _l;
    Float _c;
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Seg(CS& file): _len(0.), _r(0.), _l(0.), _c(0.) {parse(file);}
    bool   lumped()const{return _len == 0;}
    double len()const	{return _len;}
    double r()const	{return _r;}
    double l()const	{return _l;}
    double c()const	{return _c;}
    double z0()const	{return sqrt(_l / _c);}
    double delay()const	{return _len * sqrt(_l * _c);}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Fork
    :public Path_Item_Base, public List_Base<Path_Item_Base>
  {
  private:
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Fork(CS& file) {parse(file);}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  class Node
    :public Path_Item_Base
  {
  private:
    void parse(CS&);
    void dump(std::ostream& o)const;
  public:
    explicit Node(CS& file) {parse(file);}
  };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
private:
  std::string _name;
  void parse(CS&);
  void dump(std::ostream& o)const;
public:
  explicit Path(CS& file) {parse(file);}
  const std::string&	name()const  {return _name;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
