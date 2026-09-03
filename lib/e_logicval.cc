/*$Id: e_logicval.cc $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * node probes
 */
//testing=none
#include "e_logicval.h"
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::_or_truth[lvNUM_STATES][lvNUM_STATES] = {
 {lv00,lv01,lv10,lv11, lv0X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv01,lv01,lv01,lv11, lv01,lv01,lv11,lv11, lvX1,lvX1,lvX1,lvX1, lvX1,lvX1,lvX1,lvX1},
 {lv10,lv01,lv10,lv11, lv1X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lv1X,lv1X,lv1X,lv1X},
 {lv11,lv11,lv11,lv11, lv11,lv11,lv11,lv11, lv11,lv11,lv11,lv11, lv11,lv11,lv11,lv11},

 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv1X,lv1X,lv1X,lv11, lv1X,lv1X,lv1X,lv1X, lv1X,lv1X,lv1X,lv1X, lv1X,lv1X,lv1X,lv1X},
 {lv1X,lv1X,lv1X,lv11, lv1X,lv1X,lv1X,lv1X, lv1X,lv1X,lv1X,lv1X, lv1X,lv1X,lv1X,lv1X},

 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvX1,lvX1,lvX1,lv11, lvX1,lvX1,lvX1,lvX1, lvX1,lvX1,lvX1,lvX1, lvX1,lvX1,lvX1,lvX1},
 {lvX1,lvX1,lvX1,lv11, lvX1,lvX1,lvX1,lvX1, lvX1,lvX1,lvX1,lvX1, lvX1,lvX1,lvX1,lvX1},

 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvX1,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::_xor_truth[lvNUM_STATES][lvNUM_STATES] = {
 {lv00,lv01,lv10,lv11, lv0X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv01,lv10,lv01,lv10, lv0X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv10,lv01,lv10,lv01, lv1X,lv1X,lv0X,lv0X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv11,lv10,lv01,lv00, lv1X,lv1X,lv0X,lv0X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},

 {lv0X,lv0X,lv1X,lv1X, lv0X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv0X,lv0X,lv1X,lv1X, lv0X,lv0X,lv1X,lv1X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv1X,lv1X,lv0X,lv0X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv1X,lv1X,lv0X,lv0X, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},

 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},

 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::_and_truth[lvNUM_STATES][lvNUM_STATES] = { //
 {lv00,lv00,lv00,lv00, lv00,lv00,lv00,lv00, lv00,lv00,lv00,lv00, lv00,lv00,lv00,lv00},
 {lv00,lv01,lv10,lv01, lv0X,lv0X,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X},
 {lv00,lv10,lv10,lv10, lv0X,lv0X,lv1X,lv1X, lvX0,lvX0,lvX0,lvX0, lvX0,lvX0,lvX0,lvX0},
 {lv00,lv01,lv10,lv11, lv0X,lv0X,lv1X,lv1X, lvX0,lvX0,lvX1,lvX1, lvXX,lvXX,lvXX,lvXX},

 {lv0X,lv0X,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X, lv00,lv00,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X},
 {lv0X,lv0X,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X, lv00,lv00,lv0X,lv0X, lv0X,lv0X,lv0X,lv0X},
 {lv0X,lv0X,lv1X,lv1X, lv0X,lv0X,lv1X,lv1X, lvX0,lvX0,lvXX,lvXX, lv0X,lv0X,lv0X,lv0X},
 {lv0X,lv0X,lv1X,lv1X, lv0X,lv0X,lv1X,lv1X, lvX0,lvX0,lvXX,lvXX, lv0X,lv0X,lv0X,lv0X},

 {lv00,lv00,lvX0,lvX0, lv00,lv00,lvX0,lvX0, lvX0,lvX0,lvX0,lvX0, lvX0,lvX0,lvX0,lvX0},
 {lv00,lv00,lvX0,lvX0, lv00,lv00,lvX0,lvX0, lvX0,lvX0,lvX0,lvX0, lvX0,lvX0,lvX0,lvX0},
 {lv00,lv01,lvX0,lvX1, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvX1,lvX1, lvXX,lvXX,lvXX,lvXX},
 {lv00,lv01,lvX0,lvX1, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvX1,lvX1, lvXX,lvXX,lvXX,lvXX},

 {lv00,lv0X,lvX0,lvXX, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv00,lv0X,lvX0,lvXX, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv00,lv0X,lvX0,lvXX, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX},
 {lv00,lv0X,lvX0,lvXX, lv0X,lv0X,lvXX,lvXX, lvX0,lvX0,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::_not_truth[lvNUM_STATES] = {
  lv11, lv10, lv01, lv00,
  lv1X, lv1X, lv0X, lv0X,
  lvX1, lvX1, lvX0, lvX0,
  lvXX, lvXX, lvXX, lvXX
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::_prop_truth[lvNUM_STATES][lvNUM_STATES] = {
 {lv00,lvXX,lvXX,lv01, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv0Z,lvXX,lvXX,lvXX},
 {lv10,lvXX,lvXX,lv01, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lv10,lvXX,lvXX,lv01, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lv10,lvXX,lvXX,lv11, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv1Z,lvXX,lvXX,lvXX},

 {lvZ0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv0Z,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvZ1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv1Z,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},

 {lvZ0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv0Z,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvZ1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lv1Z,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},

 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvZZ,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
 {lvX0,lvXX,lvXX,lvX1, lvXX,lvXX,lvXX,lvXX, lvXX,lvXX,lvXX,lvXX, lvXZ,lvXX,lvXX,lvXX},
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
