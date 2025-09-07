/*                              -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * matrix solver
 */
#include "m_matrix_solver.h"
#include "u_sim_data.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
namespace {
struct set_solver{
  BSMATRIX_SOLVER<double>* _aa_solver{nullptr};
  BSMATRIX_SOLVER<COMPLEX>* _acx_solver{nullptr};
  set_solver(){
    SIM_DATA* sim = CKT_BASE::_sim;
    assert(sim); // link order..
    _aa_solver = new LU_COPY<double>(sim->_aa);
    _acx_solver = new LU_INPLACE<COMPLEX>(sim->_acx);

    sim->_aa.set_solver(_aa_solver);
    sim->_acx.set_solver(_acx_solver);
  }
  ~set_solver(){
    SIM_DATA* sim = CKT_BASE::_sim;
    if(sim){ untested();
      sim->_aa.set_solver(nullptr);
      sim->_acx.set_solver(nullptr);
    }else{
      // shutting down.
    }
    delete _aa_solver;
    delete _acx_solver;
    _aa_solver = nullptr;
    _acx_solver = nullptr;
  }
} s;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
