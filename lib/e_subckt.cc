/*                               -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * subcircuit and hierarchy implementation
 */
#include "e_subckt.h"
#include "e_hsparam.h"
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::precalc_first()
{
  COMPONENT::precalc_first();

  // check HS_PARAM
  if(has_common()) {
    if(dynamic_cast<HS_PARAM*>(mutable_common()->next_common())){
      // already there.
    }else{ untested();
      // create one.
      HS_PARAM* hspl = new HS_PARAM();
      hspl->attach_next(mutable_common()->next_common());
      assert(!mutable_common()->has_next());
      mutable_common()->attach_next(hspl);
    }
  }else{ untested();
    // device without common, but with hs params
    attach_common(new HS_PARAM());
  }
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::precalc_last()
{
  COMPONENT::precalc_last();
  assert(subckt());

  if(HS_PARAM const* h = hsparam()){
    h->export_to(subckt()->params());
  }else{
    trace1("no export", long_label());
  }

  subckt()->precalc_last();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
