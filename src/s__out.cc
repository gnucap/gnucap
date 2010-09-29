/*$Id: s__out.cc,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * tr,dc analysis output functions (and some ac)
 */
//testing=obsolete,script 2005.09.17
#include "u_prblst.h"
#include "declare.h"	/* plottr, plopen */
#include "s__.h"
/*--------------------------------------------------------------------------*/
//	PROBELIST& SIM::alarmlist()
//	PROBELIST& SIM::plotlist()
//	PROBELIST& SIM::printlist()
//	PROBELIST& SIM::storelist()
//	void	SIM::outdata(double);
//	void	SIM::head(double,double,const char*);
//	void	SIM::print(double);
//	void	SIM::alarm(void);
//	void	SIM::store(void);
/*--------------------------------------------------------------------------*/
/* SIM::____list: access probe lists
 */
PROBELIST& SIM::alarmlist()
{
  return PROBE_LISTS::alarm[mode];
}
PROBELIST& SIM::plotlist()
{
  return PROBE_LISTS::plot[mode];
}
PROBELIST& SIM::printlist()
{
  return PROBE_LISTS::print[mode];
}
PROBELIST& SIM::storelist()
{
  untested();
  return PROBE_LISTS::store[mode];
}
/*--------------------------------------------------------------------------*/
/* SIM::out: output the data, "keep" for ac reference
 */
void SIM::outdata(double x)
{
  ::status.output.start();
  plottr(x);
  print(x);
  alarm();
  store();
  reset_iteration_counter(iPRINTSTEP);
  ::status.control[cSTEPS] = 0;
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
/* SIM::head: print column headings and draw plot borders
 */
void SIM::head(double start, double stop, const char *col1)
{
  if (!plopen(start,stop)) {
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    sprintf(format, "%%c%%-%u.%us", width, width);
    if (col1  &&  *col1) {      
      out.form(format, '#', col1);
    }else{
      unreachable();
      incomplete();
    }
    for (PROBELIST::const_iterator
	   p=printlist().begin();  p!=printlist().end();  ++p) {
      out.form(format, ' ', p->label().c_str());
    }
    out << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::print: print the list of results (text form) to out
 * The argument is the first column (independent variable, aka "x")
 */
void SIM::print(double x)
{
  if (!IO::plotout.any()) {
    out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    if (x != NOT_VALID) {
      out << x;
    }else{
      unreachable();
      incomplete();
    }
    for (PROBELIST::const_iterator
	   p=printlist().begin();  p!=printlist().end();  ++p) {
      out << p->value();
    }
    out << '\n';
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::alarm: print a message when a probe is out of range
 */
void SIM::alarm(void)
{
  out.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
  for (PROBELIST::const_iterator
	 p=alarmlist().begin();  p!=alarmlist().end();  ++p) {
    if (!p->in_range()) {
      out << p->label() << '=' << p->value() << '\n';
    }
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::store: a stub: will be store data in preparation for post processing
 */
void SIM::store(void)
{
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
