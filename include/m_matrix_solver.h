/*                             -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *                    Felix Salfelder
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
#ifndef M_MATRIX_SOLVER_H
#define M_MATRIX_SOLVER_H
/*--------------------------------------------------------------------------*/
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
template<class T>
struct longer{
  typedef T type;
};
template<>
struct longer< std::complex<double> > {
  typedef std::complex<long double> type;
};
template<>
struct longer<double> {
  typedef long double type;
};
/*--------------------------------------------------------------------------*/
}
template <class T>
class BSMATRIX_SOLVER {
protected: // private?
  BSMATRIX_DATA<T>& _data; // _aa, _acx and the like
public:
  explicit BSMATRIX_SOLVER(BSMATRIX<T>& m) : _data(m._data) {
  }
  explicit BSMATRIX_SOLVER(BSMATRIX_SOLVER const&) = delete;
  virtual ~BSMATRIX_SOLVER() { }
public:
  int  size()const { return _data.size(); }

  virtual void set_stamp(MATRIX_STAMP*) {}
  virtual void init(int) {}
  virtual void uninit() {}
  virtual void allocate() = 0;
  virtual void unallocate() = 0;
  virtual void set_min_pivot(double x) = 0;
//  virtual void set_changed(int n, bool x = true)const = 0;

  virtual void lu_decomp(bool do_partial) = 0;
  virtual void fbsub(T*) const { untested(); unreachable(); }
  virtual void fbsub(T* x, const T* b, T* c = nullptr) const = 0;
  virtual void fbsubt(T*) const { untested(); unreachable(); }

  T const& d(int r) const {
    return _data.d(r);
  }
  T const& s(int r, int c) const { untested();
    return _data.s(r, c);
  }
  virtual void zero() {
    return _data.zero();
  }

protected: // matrix data xs
  BSMATRIX_DATA<T> const& data_(BSMATRIX<T> const& d)const { untested();
    return d.data();
  }
  BSMATRIX_DATA<T>& rw_data_(BSMATRIX<T>& d)const {
    return d.rw_data();
  }
  int ulownode_(BSMATRIX_DATA<T> const& d, int r)const {
    return d.ulownode(r);
  }
  int llownode_(BSMATRIX_DATA<T> const& d, int r)const {
    return d.llownode(r);
  }
  T const& u_(BSMATRIX_DATA<T> const& d, int r, int c)const {
    return d.u(r,c);
  }
  T const& l_(BSMATRIX_DATA<T> const& d, int r, int c)const {
    return d.l(r,c);
  }
  T const& m_(BSMATRIX_DATA<T> const& d, int r, int c)const { untested();
    return d.m(r,c);
  }
  T const& d_(BSMATRIX_DATA<T> const& d, int r)const {
    return d.d(r);
  }
  T const& m(int r, int c)const { untested();
    return _data.m(r, c);
  }

private:
  T& u(int r, int c) { untested();
    return _data.u(r, c);
  }
  T& l(int r, int c) { untested();
    return _data.l(r, c);
  }
  T& d(int r) {
    return _data.d(r);
  }
  T& m(int r, int c) {
    return _data.m(r, c);
  }

public: // load
  virtual void load_diagonal_point(int i, T value);
  virtual void load_point(int i, int j, T value);
  virtual void load_couple(int i, int j, T value);
  virtual void load_symmetric(int i, int j, T value);
  virtual void load_asymmetric(int r1, int r2, int c1, int c2, T value);

private:
  virtual void set_changed(int, bool j=true) const { untested();
    (void) j;
    // obsolete. use LU_COPY::load_*
  }


protected:
  BSMATRIX_DATA<T> const& data() const { return _data; }

protected: // non-const data xs for friends.
  T& u_(BSMATRIX_DATA<T>& d, int r, int c) { return d.u(r,c); }
  T& l_(BSMATRIX_DATA<T>& d, int r, int c) { return d.l(r,c); }
  T& m_(BSMATRIX_DATA<T>& d, int r, int c) { return d.m(r,c); }
  T& d_(BSMATRIX_DATA<T>& d, int r) { return d.d(r); }
}; // BSMATRIX_SOLVER
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_SOLVER<T>::load_point(int i, int j, T value)
{itested();
  if (i > 0 && j > 0) {itested();
    // BUG only 1 change //
    set_changed(j);
    set_changed(i);
    // set_changed(i, j);
    m(i,j) += value;
  }else{itested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value);
template <class T>
void BSMATRIX_SOLVER<T>::load_diagonal_point(int i, T value)
{
  if (i > 0) {
    set_changed(i, i);
    d(i) += value;
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void BSMATRIX_SOLVER<T>::load_couple(int i, int j, T value)
{
  if (j > 0) {
    if (i > 0) {
      set_changed(i, j);
      m(i,j) -= value;
      set_changed(j, i);
      m(j,i) -= value;
    }else{
    }
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value);	// load_diagonal_point(i, value);
// load_point(j, j, value);	// load_diagonal_point(j, value);
// load_point(i, j, -value);	// load_couple(i, j, value);
// load_point(j, i, -value);
template <class T>
void BSMATRIX_SOLVER<T>::load_symmetric(int i, int j, T value)
{
  if (j > 0) {
    set_changed(j, j);
    d(j) += value;
    if (i > 0) {
      set_changed(i,i);
      set_changed(i,j);
      set_changed(j,i);
      d(i) += value;
      m(i,j) -= value;
      m(j,i) -= value;
    }else{
    }
  }else if (i > 0) {
    set_changed(i,i);
    d(i) += value;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// load_point(r1, c1, value);
// load_point(r2, c2, value);
// load_point(r1, c2, -value);
// load_point(r2, c1, -value);
template <class T>
void BSMATRIX_SOLVER<T>::load_asymmetric(int r1,int r2,int c1,int c2,T value)
{
  if (r1 > 0) {
    if (c1 > 0) {
      m(r1,c1) += value;
      set_changed(r1, c1);
    }else{
    }
    if (c2 > 0) {
      m(r1,c2) -= value;
      set_changed(r1, c2);
    }else{
    }
  }else{
  }
  if (r2 > 0) {
    if (c1 > 0) {
      set_changed(r2, c1);
      m(r2,c1) -= value;
    }else{
    }
    if (c2 > 0) {
      set_changed(r2, c2);
      m(r2,c2) += value;
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
