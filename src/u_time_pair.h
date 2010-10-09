/*$Id: u_time_pair.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------

 */
//testing=script 2008.05.09
#ifndef U_TIME_PAIR_H_INCLUDED
#define U_TIME_PAIR_H_INCLUDED
#include "constant.h"
/*--------------------------------------------------------------------------*/
struct TIME_PAIR {
  double _error_estimate;
  double _event;
  TIME_PAIR() : _error_estimate(NEVER), _event(NEVER) {}
  TIME_PAIR(double Error, double Event) : _error_estimate(Error), _event(Event) {}
  TIME_PAIR(const TIME_PAIR& P)
    : _error_estimate(P._error_estimate), _event(P._event) {}
  TIME_PAIR& reset() {
    _error_estimate = NEVER;
    _event = NEVER;
    return *this;
  }
  TIME_PAIR& min_error_estimate(double E) {
    if (E < _error_estimate) {
      _error_estimate = E;
    }else{
    }
    return *this;
  }
  TIME_PAIR& min_event(double E) {
    if (E < _event) {
      _event = E;
    }else{
    }
    return *this;
  }
  TIME_PAIR& min(const TIME_PAIR& P) {
    return min_error_estimate(P._error_estimate).min_event(P._event);
  }
  TIME_PAIR& min(double Error_Estimate, double Event) {untested();
    return min_error_estimate(Error_Estimate).min_event(Event);
  }
};
/*--------------------------------------------------------------------------*/
inline TIME_PAIR min(TIME_PAIR A, const TIME_PAIR& B)
{
  return A.min(B);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
