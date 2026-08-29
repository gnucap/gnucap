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
template <class T>
class LU_COPY : public BSMATRIX_SOLVER<T> {
private:
  BSMATRIX_DATA<T> _lu;
  mutable bool* _changed;// flag: this node changed value
  T	_min_pivot;	// minimum pivot value
public:
  explicit LU_COPY(LU_COPY const& aa) = delete;
  explicit LU_COPY(BSMATRIX<T>& aa);
  ~LU_COPY() {}
private:
  using BSMATRIX_SOLVER <T>::size;

  using BSMATRIX_SOLVER<T>::data;
  using BSMATRIX_SOLVER<T>::u_;
  using BSMATRIX_SOLVER<T>::l_;
  using BSMATRIX_SOLVER<T>::m_;
  using BSMATRIX_SOLVER<T>::d_;
  using BSMATRIX_SOLVER<T>::ulownode_;
  using BSMATRIX_SOLVER<T>::llownode_;

private: // LU xs
  int ulownode(int i) const {
    return ulownode_(_lu, i);
  }
  int llownode(int i) const {
    return llownode_(_lu, i);
  }
  T& u(int i, int j){ return u_(_lu,i,j); }
  T& l(int i, int j){ return l_(_lu,i,j); }
  T& m(int i, int j){ return m_(_lu,i,j); }
  T& d(int i       ){ return d_(_lu,i); }

private: // solver overrides
  void set_stamp(MATRIX_STAMP* s)override { untested();
    assert(s);
    BSMATRIX_SOLVER<T>::set_stamp(s);
    _lu.set_stamp(s);
  }
  void init(int ss=0)override;
  void allocate()override { assert(_lu.has_stamp()); _lu.allocate(); }
  void unallocate()override { _lu.unallocate(); }
  void uninit()override;
  void set_min_pivot(double x)override { _min_pivot = x; }
  void lu_decomp(bool do_partial)override;
  void fbsub(T* v)const override {
    return _lu.fbsub(v);
  }
  void fbsub(T* x, const T* b, T* c = NULL)const override {
    return _lu.fbsub(x, b, c);
  }
  void fbsubt(T* v)const override { untested();
    return _lu.fbsubt(v);
  }
private:
  void set_changed(int i, bool j=true) const override {
    _changed[i] = j;
  }
//  void set_changed(int i, int j=1)const { untested();
//    if(i>0 && j>0){ untested();
//      if(i<j){ untested();
//	_changed[i] = j;
//      }else{ untested();
//	_changed[j] = i;
//      }
//    }else{ untested();
//    }
//  }
private:
  bool is_changed(int n)const {
    return _changed[n];
  }
private: // implementation
  T& subtract_dot_product(int r, int c, int d, const T& in);
}; // LU_COPY
/*--------------------------------------------------------------------------*/
template <class T>
class LU_INPLACE : public BSMATRIX_SOLVER<T> {
private:
  T	_min_pivot;	// minimum pivot value
public:
  LU_INPLACE(BSMATRIX<T>& m);
  ~LU_INPLACE() { uninit();}
private:
  using BSMATRIX_SOLVER<T>::_data;
  using BSMATRIX_SOLVER<T>::u_;
  using BSMATRIX_SOLVER<T>::l_;
  using BSMATRIX_SOLVER<T>::d_;
  using BSMATRIX_SOLVER<T>::m_;
  using BSMATRIX_SOLVER<T>::ulownode_;
  using BSMATRIX_SOLVER<T>::llownode_;

private: // data xs
  int ulownode(int i) const {
    return ulownode_(_data, i);
  }
  int llownode(int i) const {
    return llownode_(_data, i);
  }
  int size() const {
    return _data.size();
  }
  T& u(int r, int c) {
    return u_(_data, r, c);
  }
  T& l(int r, int c) {
    return l_(_data, r,c);
  }
  T& d(int r) {
    return d_(_data, r);
  }
  T& d(int r, int) {
    return d_(_data, r);
  }
  T& m(int r, int c) {
    return m_(_data, r, c);
  }
private: // solver overrides
  void init(int)override { }
  void uninit()override { }
  void allocate()override { }
  void unallocate()override { }
  void set_min_pivot(double x)override { _min_pivot = x; }
  void set_changed(int, bool j=true)const override { (void)j; }
  void fbsub(T* v)const override {
    return _data.fbsub(v);
  }
  void fbsub(T* x, const T* b, T* c = NULL)const override { untested();
    return _data.fbsub(x, b, c);
  }
  void fbsubt(T* v)const override {itested();
    return _data.fbsubt(v);
  }
  void lu_decomp(bool do_partial)override;
private: // impl
  T& subtract_dot_product(int r, int c, int d);
}; // LU_INPLACE
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
void LU_COPY<T>::uninit()
{
  BSMATRIX_SOLVER<T>::uninit();

  _lu.uninit();
  unallocate();

  if(_changed){
    delete [] _changed;
    _changed = nullptr;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void LU_COPY<T>::init(int ss)
{
  BSMATRIX_SOLVER<T>::init(ss);
  assert(data().has_stamp());
  _lu.init(ss);

  assert(!_changed);
  _changed = new bool[ss+1];
  assert(_changed);
  for (int ii = 0;  ii <= ss;  ++ii) {
    set_changed(ii, false);
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void LU_COPY<T>::lu_decomp(bool do_partial)
{
  BSMATRIX_DATA<T> const& aa = data();
  int prop = 0;   /* change propagation indicator */
  assert(aa.size() == size());
  for (int mm = 1;   mm <= size();   ++mm) {
    assert(ulownode_(aa,mm) == ulownode(mm));
    assert(llownode_(aa,mm) == llownode(mm));
    int bnu = ulownode(mm);
    int bnl = llownode(mm);
    int bn = std::min(bnu, bnl);
    if (!do_partial  ||  is_changed(mm)  ||  bn <= prop) {
      set_changed(mm, false);
      prop = mm;
      if (bnu < mm) {
	prop = mm;
	u(bnu,mm) = u_(aa,bnu,mm) / d(bnu);
	for (int ii = bnu+1;  ii<mm;  ii++) {
	  /* u(ii,mm) = (aa.u(ii,mm) - dot(ii,mm,ii)) / d(ii,ii); */
	  subtract_dot_product(ii,mm,ii,u_(aa,ii,mm)) /= d(ii);
	}
      }else{
      }
      if (bnl < mm) {
	l(mm,bnl) = l_(aa,mm,bnl);
	for (int jj = bnl+1;  jj<mm;  jj++) {
	  /* l(mm,jj) = aa.l(mm,jj) - dot(mm,jj,jj); */
	  subtract_dot_product(mm,jj,jj,l_(aa,mm,jj));
	}
      }else{
      }
      if (bn < mm) {
	{ /* jj == mm */
	  /* d(mm,mm) = aa.d(mm,mm) - dot(mm,mm,mm); then test */
	  if (subtract_dot_product(mm,mm,mm,d_(aa,mm)) == 0.) {
	    error(bWARNING, "open circuit: internal node %u\n", mm);
	    d(mm) = _min_pivot;
	  }else{
	  }
	}
      }else{    /* bn == mm */
	d(mm) = d_(aa,mm);
	if (d(mm)==0.) {
	  d(mm) = _min_pivot;
	}else{
	}
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
T& LU_INPLACE<T>::subtract_dot_product(int rr, int cc, int dd)
{
  int kk = std::max(llownode(rr), ulownode(cc));
  int len = dd - kk;
  T& in = m(rr, cc);
  typedef typename longer<T>::type longertype;
  longertype dot = 0.;

  if (len > 0) {
    T* row = &(l(rr,kk));	// _diaptr[r][r-c];
    T* col = &(u(kk,cc));	// _diaptr[c][r-c];
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii) {
      dot += row[-ii] * col[ii];
    }
  }else{
  }
  in -= T(dot);
  return in;
}
/*--------------------------------------------------------------------------*/
template <class T>
T& LU_COPY<T>::subtract_dot_product(int rr, int cc, int dd, const T& in)
{
  int kk = std::max(llownode(rr), ulownode(cc));
  int len = dd - kk;
  typedef typename longer<T>::type longertype;
  longertype dot = 0.;
  if (len > 0) {
    T* row = &(l(rr,kk));
    T* col = &(u(kk,cc));
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii) {
      dot += row[-ii] * col[ii];
    }
  }else{
  }
  T& result = m(rr, cc);
  result = T(in - dot);
  return result;
}
/*--------------------------------------------------------------------------*/
template <class T>
void LU_INPLACE<T>::lu_decomp(bool /*ignore partial*/)
{
  BSMATRIX_DATA<T>& acx = _data;
//  assert(_lownode);
  for (int mm = 1;   mm <= size();   ++mm) {
    int bnu = ulownode(mm);
    int bnl = llownode(mm);
    int bn = std::min(bnu, bnl);
    if (bnu < mm) {
      u_(acx,bnu,mm) /= d(bnu,bnu);
      for (int ii =bnu+1;  ii<mm;  ii++) {
	/* (m(ii,mm) -= dot(ii,mm,ii)) /= d(ii,ii); */
	subtract_dot_product(ii,mm,ii) /= d(ii,ii);
      }
    }else{
    }
    if (bnl < mm) {
      for (int jj = bnl+1;  jj<mm;  jj++) {
	/* m(mm,jj) -= dot(mm,jj,jj); */
	subtract_dot_product(mm,jj,jj);
      }
    }else{
    }
    if(bn < mm) {
      { /* jj == mm */
	/* m(mm,mm) -= dot(mm,mm,mm); then test */
	if (subtract_dot_product(mm,mm,mm) == 0.) {itested();
	  error(bWARNING, "open circuit: internal node %u\n", mm);
	  d(mm,mm) = _min_pivot;
	}else{
	}
      }
    }else{    /* bn == mm */
      if (d(mm,mm)==0.) { // fabs?!
	d(mm,mm) = _min_pivot;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
LU_INPLACE<T>::LU_INPLACE(BSMATRIX<T>& m)
   : BSMATRIX_SOLVER<T>(m)
   , _min_pivot(0.)
{
  auto stamp = new BSMATRIX_STAMP();
  BSMATRIX_SOLVER<T>::_data.set_stamp(stamp);

  m.set_stamp(stamp);
}
template LU_INPLACE<COMPLEX>::LU_INPLACE(BSMATRIX<COMPLEX>&);
// template LU_INPLACE<double>::LU_INPLACE(BSMATRIX<double>&);
/*--------------------------------------------------------------------------*/
template <class T>
LU_COPY<T>::LU_COPY(BSMATRIX<T>& aa)
  : BSMATRIX_SOLVER<T>(aa),
  _lu(), // <= store L and U here.
  _changed(nullptr)
{
  auto stamp = new BSMATRIX_STAMP();
  BSMATRIX_SOLVER<T>::_data.set_stamp(stamp);
  _lu.set_stamp(stamp); // here.

  aa.set_stamp(stamp);
}
template LU_COPY<double>::LU_COPY(BSMATRIX<double>&);
// template LU_COPY<COMPLEX>::LU_COPY(BSMATRIX<COMPLEX>&);
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
    if(sim){
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
