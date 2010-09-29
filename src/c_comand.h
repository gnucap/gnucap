/*$Id: c_comand.h,v 20.10 2001/10/05 01:35:36 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "GnuCap", the Gnu Circuit Analysis Package
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
 * all of the commands
 */
#ifndef C_COMAND_H
#define C_COMAND_H
#include "md.h"
/*--------------------------------------------------------------------------*/
class CS;
/*--------------------------------------------------------------------------*/
class CMD {
public:
  static  void  cmdproc(const std::string&);
private:
  static  void	ac(CS&);
  static  void	alarm(CS&);
  static  void	alter(CS&);
  static  void	build(CS&);
  static  void	chdir(CS&);
  static  void	clear(CS&);
  static  void	comment(CS&);
  static  void	crtset(CS&);
  static  void	dc(CS&);
  static  void	del(CS&);
  static  void	disto(CS&);
  static  void	edit(CS&);
  static  void	end(CS&);
  static  void	ends(CS&);
  static  void	fanout(CS&);
  static  void	fault(CS&);
  static  void	file(CS&);
  static  void	fourier(CS&);
  static  void	generator(CS&);
  static  void	get(CS&);
  static  void	help(CS&);
  static  void	ic(CS&);
  static  void	include(CS&);
  static  void	insert(CS&);
  static  void	do_list(CS&);
  static  void	logger(CS&);
  static  void	mark(CS&);
  static  void	merge(CS&);
  static  void	model(CS&);
  static  void	modify(CS&);
  static  void	nodeset(CS&);
  static  void	noise(CS&);
  static  void	op(CS&);
  static  void	options(CS&);
  static  void	pause(CS&);
  static  void	plot(CS&);
  static  void	print(CS&);
  static  void	quit(CS&);
  static  void	restore(CS&);
  static  void	run(CS&);
  static  void	save(CS&);
  static  void	sens(CS&);
  static  void	status(CS&);
  static  void	subckt(CS&);
  static  void	sweep(CS&);
  static  void	system(CS&);
  static  void	temp(CS&);
  static  void	tf(CS&);
  static  void	title(CS&);
  static  void	tr(CS&);
  static  void	unfault(CS&);
  static  void	unmark(CS&);
public:
  static int count;		/* command counter, for history */
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
