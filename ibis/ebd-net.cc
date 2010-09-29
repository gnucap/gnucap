/*$Id: ebd-net.cc,v 1.61 2004/02/01 07:12:00 al Exp $ -*- C++ -*-
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
 * Complete: all but map to externals
 * Incomplete:
 *	node list mapping to externals
 * Makes a subckt for the whole board, containing internal ones per net.
 * Name is: <file>::<board_name>
 * Nodelist is: from [Pin List] in order
 * Calls internal nets named: path.<pin_name>
 * Nodelist is: external pin, followed by nodes in order.
 */
#include "ebd.h"
/*--------------------------------------------------------------------------*/
namespace Netlist {
/*--------------------------------------------------------------------------*/
static void make_path_element(std::ostream& out, const Path_Item_Base* here,
			const Path_Item_Base* next_item, std::string* node1);
  // forward
/*--------------------------------------------------------------------------*/
static void make_path_pin(std::ostream& out, const Path_Item_Base* here, 
			  std::string* node1)
{
  *node1 = (*here).name();
  out << "*pin " << *node1 << '\n';
}
/*--------------------------------------------------------------------------*/
static void make_path_node(std::ostream& out, const Path_Item_Base* here, 
			   std::string* node1)
{
  *node1 = (*here).name();
  out << "*node " << *node1 << '\n';
}
/*--------------------------------------------------------------------------*/
static void make_path_seg(std::ostream& out, const Path_Item_Base* here,
			  const Path_Item_Base* next_item, std::string* node1)
{
  const Path::Seg* seg = dynamic_cast<const Path::Seg*>(here);
  assert(seg);

  std::string node2 = (next_item) ? (*next_item).name() : *node1 + "_end";

  {if (seg->lumped()) {
    {if (seg->l() != 0. && seg->r() != 0.) {
      out << 'L' << seg->name() 
	  << " (" << *node1 << ' ' << *node1 << "a) " << seg->l() << '\n'
	  << 'R' << seg->name() 
	  << " (" << *node1 << "a " << node2 << ") " << seg->r() << '\n';
    }else if (seg->l() != 0.) {
      assert(seg->r() == 0.);
      out << 'L' << seg->name() 
	  << " (" << *node1 << ' ' << node2 << ") " << seg->l() << '\n';
    }else if (seg->r() != 0.) {
      untested();
      assert(seg->l() == 0.);
      out << 'R' << seg->name() 
	  << " (" << *node1 << ' ' << node2 << ") " << seg->r() << '\n';
    }else{
      untested();
      assert(seg->r() == 0.);
      assert(seg->l() == 0.);
      node2 = *node1;
    }}
    if (seg->c() != 0.) {
      out << 'C' << seg->name() 
	  << " (" << *node1 << ' ' << 0 << ") " << seg->c() << '\n';
    }
  }else{
    out << 'T' << seg->name() 
	<< " (" << *node1 << ' ' << node2 << ")"
      " Z0=" << seg->z0() << " TD=" << seg->delay();
    if (seg->r() != 0) {
      untested();
      out << " R=" << seg->r();
    }else{
      untested();
    }
    out << '\n';
  }}
  *node1 = node2;
}
/*--------------------------------------------------------------------------*/
static void make_path_fork(std::ostream& out, const Path_Item_Base* here, 
			   const std::string* node1)
{
  const Path::Fork* fork = dynamic_cast<const Path::Fork*>(here);
  assert(fork);

  out << "*fork\n";
  std::string node2 = *node1;
  {for (Path::Fork::const_iterator i = fork->begin(); i != fork->end(); ++i) {
    Path::Fork::const_iterator next = i;
    while (++next,
	   next != fork->end() && dynamic_cast<const Path::Fork*>(*next)) {
      // a fork in a fork, find next non-fork
    }
    {if (next == fork->end()) {
      make_path_element(out, *i, 0, &node2);
      //	      updates node2 ^^^^^^
    }else{
      assert(next != i);
      assert(!dynamic_cast<const Path::Fork*>(*next));
      make_path_element(out, *i, *next, &node2);
      //		  updates node2 ^^^^^^
    }}
  }}
  out << "*endfork\n";
}
/*--------------------------------------------------------------------------*/
static void make_path_element(std::ostream& out, const Path_Item_Base* here,
			const Path_Item_Base* next_item, std::string* node1)
{
  assert(here);
  assert(node1);
  {if (dynamic_cast<const Path::Pin*>(here)) {
    make_path_pin(out, here, node1);
  }else if (dynamic_cast<const Path::Node*>(here)) {
    make_path_node(out, here, node1);
  }else if (dynamic_cast<const Path::Seg*>(here)) {
    make_path_seg(out, here, next_item, node1);
  }else if (dynamic_cast<const Path::Fork*>(here)) {
    make_path_fork(out, here, node1);
  }else{
    unreachable();
    std::string node2 = "error";
    out << "ERROR (" << *node1 << ' ' << node2 << ")\n";
  }}
}
/*--------------------------------------------------------------------------*/
/* make_path_subckt
 * Make the netlist for one "path description"
 */
static void make_path_subckt(std::ostream& out, const Path& p)
{
  // header
  out << "*[Path Description] " << p.name() << '\n';

  std::string node1 = p.front()->name();
  {for (Path::const_iterator i = p.begin(); i != p.end(); ++i) {
    Path::const_iterator next = i;
    while (++next, next != p.end() && dynamic_cast<const Path::Fork*>(*next)) {
      // find next item that isn't fork, so the nodes can be matched
    }
    {if (next == p.end()) {
      make_path_element(out, *i, 0, &node1);
      //	      updates node1 ^^^^^^
    }else{
      assert(next != i);
      assert(!dynamic_cast<const Path::Fork*>(*next));
      make_path_element(out, *i, *next, &node1);
      //		  updates node1 ^^^^^^
    }}
  }}

  // close
  out << "*[End Path Description] " << p.name() << "\n\n";
}
/*--------------------------------------------------------------------------*/
/* make_board
 * Make the netlist for one "board" (ebd).
 * It consists of:
 * Comment line "[Begin Board Description]".
 * ".subckt" line with all the connections.
 * ".lib" and "X" lines for all things refered to be "ref des map".
 * A section for each "path description", just listed.
 * ".ends"
 *
 * Name of the subckt is: <prefix><board_name>
 */
static void make_board(std::ostream& out, const Board& b,
		       const std::string& prefix)
{
  // header
  out << "*[Begin Board Description] " << b.name() << "\n"
    ".subckt " << prefix << b.name() << " ( ";
  {for (Board::Pin_List::const_iterator
	 i = b.pins().begin(); i != b.pins().end(); ++i) {
    out << (**i).name() << ' ';
  }}
  out << ")\n\n";

  // ".lib" statements and calls to externals
  out << "*[Reference Designator Map]\n";
  {for (Board::Ref_Des_Map_List::const_iterator
	 i = b.ref_des_map().begin(); i != b.ref_des_map().end(); ++i) {
    out << '*' << (**i) <<
      ".lib " << (**i).file() << ' ' << (**i).file() << "::"
	<< (**i).component() << "\n"
      "X" << (**i).name() << " ( ??? ) " << (**i).file() << "::"
	<< (**i).component() << "\n\n";
  }}
  out << '\n';

  // path subckts, define
  {for (Board::Path_List::const_iterator
	 i = b.paths().begin(); i != b.paths().end(); ++i) {
    make_path_subckt(out, **i);
  }}

  // done
  out << ".ends " << prefix << b.name() << "\n\n";
}
/*--------------------------------------------------------------------------*/
/* make_ebd
 * Make a bunch of boards.
 * Usually, there is only one, but you can have any number
 */
void make_ebd(std::ostream& out, const Board_List& list,
		  const std::string& prefix, const std::string&)
{
  {for (Board_List::const_iterator i = list.begin(); i != list.end(); ++i) {
    make_board(out, **i, prefix);
  }}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
