/* $Id: u_limit.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
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
 *------------------------------------------------------------------
 * device limiting library
 */
//testing=script 2006.07.14
#ifndef U_LIMIT_H
#define U_LIMIT_H
#include "md.h"
/*--------------------------------------------------------------------------*/
/* Spice style PN junction limiting */
inline double pnj_limit(double vnew, double vold, double vt, double vcrit)
{
  trace4("limit-in", vnew, vold, vt, vcrit);
  if ((vnew > vcrit) && (std::abs(vnew - vold) > (2 * vt))) {
    if (vold > 0) {
      double arg = 1 + (vnew - vold) / vt;
      double vlim = (arg > 0) ? (vold + vt * log(arg)) : vcrit;
      trace3("limit-1", vlim, vt*log(arg), arg);
      return vlim;
    }else{
      double vlim = vt * log(vnew/vt);
      trace3("limit-2", vlim, vt*log(vnew/vt), vnew/vt);
      return vlim;
    }
  }else{
    trace1("limit-3", vnew);
    return vnew;
  }
}
/*--------------------------------------------------------------------------*/
inline double fet_limit_vds(double vnew, double vold)
{
  if (vold >= 3.5) {
    if (vnew > (3*vold + 2)) {itested();
      return 3 * vold + 2;
    }else if (vnew < 2) {
      return 2;
    }else{
      return vnew;
    }
  }else{
    if (vnew > 4) {
      return 4;
    }else if (vnew < -.5) {
      return -.5;
    }else{
      return vnew;
    }
  }
}
/*--------------------------------------------------------------------------*/
inline double fet_limit_vgs(double vnew, double vold, double vto)
{
  assert(vnew == vnew);
  assert(vold == vold);

  double vgst_old = vold - vto;
  double vgst_new = vnew - vto;
  double v_limited;
  if (vgst_old >= 3.5) { /* was strong on */
    if (vgst_new < 2) {
      v_limited = 2;
    }else if (vgst_new > (3 * vgst_old + 2)) {itested();
      v_limited = 3*vgst_old + 2;
    }else{
      v_limited = vgst_new;
    }
  }else if (vgst_old >= 0) { /* middle region, on */
    assert(vgst_old < 3.5);
    if (vgst_new < -.5) {
      v_limited = -.5;
    }else if (vgst_new > 4) {
      v_limited = 4;
    }else{
      v_limited = vgst_new;
    }
  }else if (vgst_old <= 0) { /* was off, vgst_old < 0 */
    assert(vgst_old < 0);
    if (vgst_new < (3 * vgst_old - 2)) {
      v_limited = 3*vgst_old - 2;
    }else if (vgst_new > .5) {
      v_limited = .5;
    }else{
      v_limited = vgst_new;
    }
  }else{ /* overflow or other numeric error */
    // not sure what to do in this case.
    // It could actually happen with overflow or divide by zero.
    // but supposedly this is trapped elsewhere.
    unreachable();
    v_limited = 0.;
    trace3("huh?", vnew, vold, vto);
    trace3("    ", vgst_new, vgst_old, v_limited);
  }
  return v_limited + vto;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
