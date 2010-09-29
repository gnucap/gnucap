/*$Id: mode.h,v 25.94 2006/08/08 03:22:25 al Exp $ -*- C++ -*-
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
 * several enumerated types to identify various modes
 */
//testing=script,complete 2006.07.14
#ifndef MODE_H
#define MODE_H
#include "io_.h"
/*--------------------------------------------------------------------------*/
enum smode_t   {moUNKNOWN=0, moANALOG=1, moDIGITAL, moMIXED};
inline OMSTREAM& operator<<(OMSTREAM& o, smode_t t) {
  const char* s[] = {"unknown", "analog", "digital", "mixed"};
  assert(t >= int(moUNKNOWN));
  assert(t <= int(moMIXED));
  return (o << s[t]);
}

enum SIM_MODE { // simulation types
  sNONE,	/* not doing anything, reset by cmd interpreter	*/
  sAC,  	/* AC analysis					*/
  sOP,  	/* op command					*/
  sDC,  	/* dc sweep command				*/
  sTRAN,	/* transient command				*/
  sFOURIER	/* fourier command				*/
};
const int sSTART = sNONE;
const int sCOUNT = sFOURIER + 1;
inline OMSTREAM& operator<<(OMSTREAM& o, SIM_MODE t) {
  const char* s[] = {"ALL", "AC", "OP", "DC", "TRAN", "FOURIER"};
  assert(t >= int(sNONE));
  assert(t <= int(sFOURIER));
  return (o << s[t]);
}

enum PROBE_INDEX { // iter probes (continue after SIM_MODE)
  iPRINTSTEP = sCOUNT,	/* iterations for this printed step		*/
  iSTEP,		/* iterations this internal step		*/
  iTOTAL		/* total iterations since startup		*/
};
const int iCOUNT = iTOTAL + 1;	/* number of iteration counters		*/

/* control probes							*/
#define cSTEPCAUSE (0)	/* what caused this time step			*/
#define cSTEPS	   (1)	/* count of hidden steps (+ this unhidden)	*/
#define cCOUNT	   (2)	/* number of control probes			*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
