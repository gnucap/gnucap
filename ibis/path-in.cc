/*$Id: path-in.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
void Path::Pin::parse(CS& file)
{
  file >> _name;
}
/*--------------------------------------------------------------------------*/
void Path::Seg::parse(CS& file)
{
  char junk[100];
  sprintf(junk, "%u", file.cursor());
  _name = junk;
  file >> _len;
  int here = file.cursor();
  {for (;;) {
    get(file, "R", &_r);
    get(file, "L", &_l);
    get(file, "C", &_c);
    {if (file.skip1b("/")) {
      break;
    }else if (file.stuck(&here)) {
      untested();
      file.warn(0, "need R, L, C");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Path::Fork::parse(CS& file)
{
  char junk[100];
  sprintf(junk, "%u", file.cursor());
  _name = junk;
  int here = file.cursor();
  {for (;;){
    skip_comment(file);
    Path_Item_Base* x = 0;
    {if	    (file.dmatch("Len"))    {x = new Seg(file);}
    else if (file.dmatch("Node"))   {x = new Node(file);}
    else if (file.dmatch("Pin"))    {x = new Pin(file);}
    else if (file.dmatch("Fork"))   {x = new Fork(file);}
    else if (file.dmatch("Endfork")){break;}
    else			{untested();file.warn(0,"open fork"); break;}
    }
    {if (!file.stuck(&here)) {
      push(x);
    }else{
      untested();
      delete x;
      file.warn(0, "need path parameters or Endfork");
      break;
    }}
  }}
}
/*--------------------------------------------------------------------------*/
void Path::Node::parse(CS& file)
{
  file >> _name;
}
/*--------------------------------------------------------------------------*/
void Path::parse(CS& file)
{
  file >> _name;
  {for (;;){
    skip_comment(file);
    Path_Item_Base* x = 0;
    {if	    (file.dmatch("Len"))    {x = new Seg(file);}
    else if (file.dmatch("Node"))   {x = new Node(file);}
    else if (file.dmatch("Pin"))    {x = new Pin(file);}
    else if (file.dmatch("Fork"))   {x = new Fork(file);}
    else			    {break;}
    }
    push(x);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
