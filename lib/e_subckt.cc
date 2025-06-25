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
static HS_PARAM const* get_hsparam(CARD const* c)
{
  if(auto comp = dynamic_cast<COMPONENT const*>(c)) {
    assert(comp->has_hsparam());
    return comp->hsparam();
  }else{
    return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::precalc_first()
{
  COMPONENT::precalc_first();

  // check HS_PARAM
  if(!get_hsparam(owner())) {
    // not needed or explicit (already attached)
  }else if(dynamic_cast<HS_PARAM const*>(common())) {
    incomplete();
  }else if(has_common()) {
    if(dynamic_cast<HS_PARAM*>(mutable_common()->next_common())){
      // already there.
    }else{
      // create one.
      HS_PARAM* hspl = new HS_PARAM();
      hspl->attach_next(mutable_common()->next_common());
      assert(!mutable_common()->has_next());
      mutable_common()->attach_next(hspl);
    }
  }else{
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
    subckt()->params()->set_try_again(nullptr);
    subckt()->params()->eval_copy(h->_params, scope()->params());
  }else{
  }


  { // is this needed? DEV_SUBCKT could take care of it.
    COMMON_PARAMLIST const* c = dynamic_cast<COMMON_PARAMLIST const*>(common());
    if(c){
      subckt()->params()->set_try_again(&c->_params);
    }else{
    }
  }
  // TODO: handle in common??
  double mfactor_hier = hsparam()->mfactor();
  trace3("DEV_SUBCKT::precalc_last", long_label(), mfactor_hier, mfactor());
  subckt()->params()->set("$mfactor_hier", "");
  subckt()->params()->set("$mfactor_hier", to_string(mfactor_hier));
  assert(mfactor() == mfactor_hier);

  subckt()->precalc_last();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
