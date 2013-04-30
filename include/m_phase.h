/*$Id: m_phase.h,v 26.118 2009/08/22 21:08:57 al Exp $ -*- C++ -*-
 * Copyright (C) 2008 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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
 */
//testing=script 2008.06.06
#ifndef M_PHASE_H
#define M_PHASE_H
#include "constant.h"
#include "u_opt.h"
/*--------------------------------------------------------------------------*/
/* phase: extract phase (degrees) from COMPLEX number
 * rotates 90 degrees!  (ref to sine instead of cosine)
 */
inline double phase(COMPLEX x)
{
  double rv = NOT_VALID;
  switch (OPT::phase) {
  case pDEGREES:
    rv = arg(x)*RTOD;
    break;
  case pP_DEGREES:untested();
    rv = arg(x)*RTOD;
    if (rv < 0) {untested();
      rv += 360;
    }else{untested();
    }
    break;
  case pN_DEGREES:itested();
    rv = arg(x)*RTOD;
    if (rv > 0) {itested();
      rv -= 360;
    }else{itested();
    }
    break;
  case pRADIANS:
    rv = arg(x);
    break;
  };
  return rv;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
