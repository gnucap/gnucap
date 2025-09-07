/*                             -*- C++ -*-
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
 */
/*--------------------------------------------------------------------------*/
#include "m_matrix.h"
#include "m_matrix_solver.h" // TODO
/*--------------------------------------------------------------------------*/
class BSMATRIX_STAMP : public MATRIX_STAMP {
  std::vector<int> _ulownode;
  std::vector<int> _llownode;
public:
  void init(int s)override {
    if(_ulownode.size()){
    }else{
    }
    _ulownode.resize(s+1);
    _llownode.resize(s+1);
    for (int ii = 0;  ii <= s;  ++ii) {
      _ulownode[ii] = _llownode[ii] = ii;
    }
  }
  void uninit()override {
    _ulownode.resize(0);
    _llownode.resize(0);
  }

  void iwant_point(int i, int j)override;
  void iwant_quad(int i, int j)override {iwant_point(i, j); iwant_point(j, i);}
  void iwant_inode(int i, int j)override {iwant_quad(i, j);}

public:
  int const* ulownode()const override {return _ulownode.data();}
  int const* llownode()const override {return _llownode.data();}
};
/*--------------------------------------------------------------------------*/
void BSMATRIX_STAMP::iwant_point(int i, int j)
{
  trace2("iwant", i, j);
  assert(i <= int(_ulownode.size()));
  assert(j <= int(_ulownode.size()));

  if (i <= 0  ||  j <= 0) {
    // node 0 is ground, and doesn't count as a connection
    // negative is invalid, not used but still may be in a node list
  }else if (i < _ulownode[j]) {
    _ulownode[j] = i;
  }else if (j < _llownode[i]) {
    _llownode[i] = j;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_DATA<T>::init(int s)
{
  assert(!_diaptr);
  assert(!_space);

  _size = s;

  if(_stamp) {
    _stamp->init(s);
    assert(!_ulownode);
    assert(!_llownode);
    trace1("data::init set ln", s);
    _ulownode = _stamp->ulownode();
    _llownode = _stamp->llownode();
    assert(_ulownode);
    assert(_llownode);
  }else{
  }

  assert(_zero == 0.);
  _trash = 0.;
  _nzcount = 0;
}
template void BSMATRIX_DATA<double>::init(int);
template void BSMATRIX_DATA<COMPLEX>::init(int);
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_DATA<T>::uninit()
{
  unallocate();
  _ulownode = _llownode = nullptr;
}
template void BSMATRIX_DATA<double>::uninit();
template void BSMATRIX_DATA<COMPLEX>::uninit();
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
