/*$Id: u_time_pair.h  $ -*- C++ -*-
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
class TIME_PAIR {
  double _dt_estimate;
  double _event;
public:
  explicit TIME_PAIR() : _dt_estimate(NEVER), _event(NEVER) { assert(is_ok()); }
  explicit TIME_PAIR(double Error, double Event) : _dt_estimate(Error), _event(Event) {
    assert(Error>0);
  }
	   TIME_PAIR(const TIME_PAIR& P) = default;

  TIME_PAIR& operator=(const TIME_PAIR& P) = default;
  TIME_PAIR& reset() {
    _dt_estimate = NEVER;
    _event = NEVER;
    assert(is_ok());
    return *this;
  }
  TIME_PAIR& min_dt_estimate(double E) {
    assert(E > 0);
    if (E < _dt_estimate) {
      assert(is_ok());
      _dt_estimate = E;
    }else if (_dt_estimate > 0) {
      assert(is_ok());
    }else if (E < -_dt_estimate) {
      assert(!is_ok());
      _dt_estimate = -E;
      assert(!is_ok());
    }else if(is_ok()){ untested();
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
    return min_dt_estimate(P.dt_estimate()).
           min_event(P._event).set_nok(P.is_nok());
  }
  TIME_PAIR& min(double Error_Estimate, double Event) {untested();
    assert(Error_Estimate > 0);
    return min_dt_estimate(Error_Estimate).min_event(Event);
  }
  TIME_PAIR& set_nok(bool nok=true) {
    if(nok){
      _dt_estimate = - dt_estimate();
      assert(!is_ok());
    }else{
    }
    return *this;
  }
  double dt_estimate()const {
    assert(_dt_estimate != -NEVER);
    return std::fabs(_dt_estimate);
  }
  double event()const {return _event;}
  double is_ok()const {return _dt_estimate > 0;}
  double is_nok()const {return _dt_estimate < 0;}
};
/*--------------------------------------------------------------------------*/
inline TIME_PAIR min(TIME_PAIR A, const TIME_PAIR& B)
{ untested();
  return A.min(B);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
