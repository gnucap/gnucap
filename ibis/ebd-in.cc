/*$Id: ebd-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
#include "ebd.h"
/*--------------------------------------------------------------------------*/
void Board::Pin::parse(CS& file)
{
  Tail_String line_buf(file);
  CS line(line_buf);
  line >> _name >> _signal;
}
/*--------------------------------------------------------------------------*/
void Board::Ref_Des_Map::parse(CS& file)
{
  file >> _name >> _file >> _component;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Board::parse(CS& file)
{
  file >> _name;
  int here = file.cursor();
  {for (;;) {
    skip_comment(file);
    get(file, "[Manufacturer]",		&_manufacturer);
    get(file, "[Number_Of_Pins]",	&_n_pins);
    get(file, "[Pin_List]",		&_pins);
    get(file, "[Path_Description]",	&_paths);
    get(file, "[Reference_Designator_Map]", &_ref_des_map);
    {if (file.dmatch("[End_Board_Description]")) {
      break;
    }else if (file.stuck(&here)) {
      untested();
      file.warn(0, "need more board parameters");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
