/*                             -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2023, 2024 Felix Salfelder
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
 * Sparse matrix package
 * Bump and spike - bordered block diagonal pattern
 * -------------------------------------------------
 * To use it (simple method) .....
 * 1. Declare it ...
 *	BSMATRIX<double> a;
 * 2. Then tell it what slots to allocate, in a loop ...
 *	for (all pairs i,j that might exist)
 *	   a.iwant(i,j);
 * 3. Then do the actual allocation ...
 *	a.allocate();
 * 4. If you want, you can add an offset to the diagonal ...
 *	a.dezero(gmin);
 * 5. You probably should set a minimum pivot value ...
 *	a.setminpivot(gmin);
 * 6. Fill in the matrix ...
 *	for (all pairs i,j you want to fill in)
 *	   a.m(i,j) += data;
 * 7. Then do the LU decomposition ...
 *	a.lu_decomp();
 * 8. Then get the solution by applying the right side vector (v)
 * and doing the fwd and back substitution.
 *	a.fbsub(v);
 * The solution is now in v.
 * -------------------------------------------------
 * To reuse it .....
 * Get rid of the old data ...
 *	a.zero().
 * Restore the offset, if you want ...
 *	a.dezero(gmin);
 * Then fill and solve as before. (steps 6-8)
 * -------------------------------------------------
 * In the above case, LU replaced the matrix and the solution replaced
 * the right side, losing the data.
 * To keep the matrix, and keep the right side ...
 * 1. Declare two matrices ...
 *	BSMATRIX<double> a;
 *	BSMATRIX<double> lu;
 * 2. as before
 * 2a. Say the lu has the same structure as a.
 *	lu.clone(a);
 * 3-5. Allocate both.
 * 6. Fill in "a" only.
 * 7. Do the LU decomposition, keeping a, with result in lu ...
 *	lu.lu_decomp(a, false);
 * 8. Do the f&B sub, keeping b (the right side), with result in x ...
 *	lu.fbsub(x, b);
 * -------------------------------------------------
 * To make a change to the matrix and re-solve ...
 * Apply the change to a ...
 *	for (all changes you want to make)
 *	   a.m(i,j) += delta;  a.set_changed(i).set_changed(j)
 * Then solve again .. step 7 above for a full solution,
 * or for an update ...
 *	lu.lu_decomp(a, true);
 * Then do the same step 8 as above.
 * -------------------------------------------------
 * some other functions that might be useful ....
 * reinit(newsize) -- change the size (and lose the contents and structure)
 * size()const -- return the size (# of rows & cols)
 * density()const -- return the matrix density, as a number between 0 and 1
 * sparsity()const -- 1-density
 * -------------------------------------------------
 * individual element access ...
 * 5 access functions are provided.
 * All return lvalues (references to the actual entry).
 * All take 2 args, row and column.  They differ in speed and generality.
 * Since they are used in equations, they all have 1 letter names.
 * s(r,c) -- "safe" -- most general case, with bounds checking
 *	if completely out of bounds, returns trash
 *	if in the matrix but out of band, returns a reference to zero
 *	Changing it will corrupt the matrix.
 *	All others have no bounds checking.
 * m(r,c) -- "matrix" -- known to be within the allocated band
 * u(r,c) -- "upper" -- known to be in the upper triangle. (c>=r)
 * l(r,c) -- "lower" -- known to be in the lower triangle. (r>=c)
 * d(r,c) -- "diagonal" -- known to be on the diagonal. (r==c)
 * Using s() will always work, but will be slower than the other methods.
 * You should use the most restricted function that you know is correct.
 * -------------------------------------------------
 * Other notes ...
 * The numbering starts at 1 (Fortran style).
 * When using "s" access, it is ok to load row and column 0, then ignore it.
 *   This may simplify load functions, at the expense of run speed.
 * "s" will let you change the value of zero,
 *   but you will find out about it later.
 */
//testing=script 2016.09.14
#ifndef M_MATRIX_H
#define M_MATRIX_H
/*--------------------------------------------------------------------------*/
#include "l_stlextra.h"
/*--------------------------------------------------------------------------*/
template <class T>
class BSMATRIX_SOLVER;
template <class T>
class BSMATRIX;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
class BSMATRIX_DATA {
  friend class BSMATRIX_SOLVER<T>;
protected:
  int*	_ulownode{nullptr};	// lowest node connecting to this one
  int*	_llownode{nullptr};	// lowest node connecting to this one
  T*	_space{nullptr};	// ptr to actual memory space used
  T**	_diaptr{nullptr};	// ptrs to diagonal
  int	_nzcount{0};	// count of non-zero elements
  int	_size{0};	// # of rows and columns
  T	_zero{0};	// always 0 but not const
  T	_trash{0};	// depository for row and col 0, write only
public:
  BSMATRIX_DATA(BSMATRIX_DATA const&) = delete;
  explicit	BSMATRIX_DATA();
  ~BSMATRIX_DATA() { uninit(); }
public: // life cycle
  void init(int s);
  void iwant1(int, int);
  void iwant2(int i, int j){ iwant1(i, j); iwant1(j, i); }
  void iwant(int i, int j){ iwant2(i, j); }
  void allocate();
  void unallocate();
  void uninit();
public:
  T const& s(int r, int c)const;
  T const& d(int r, int c)const;

protected:
  T&	   u(int r, int c);
  T&	   l(int r, int c);
  T&	   d(int r);
  T&	   m(int r, int c);
protected: // const xs
  T const& u(int r, int c)const {
    auto& x = const_cast<BSMATRIX_DATA<T>&>(*this);
    return x.u(r, c);
  }
  T const& l(int r, int c)const {
    auto& x = const_cast<BSMATRIX_DATA<T>&>(*this);
    return x.l(r, c);
  }
  T const& d(int r)const {
    auto& x = const_cast<BSMATRIX_DATA<T>&>(*this);
    return x.d(r);
  }
  T const& m(int r, int c) const { untested();
    auto& x = const_cast<BSMATRIX_DATA<T>&>(*this);
    return x.m(r, c);
  }
  int ulownode(int n) const{ assert(_ulownode); return _ulownode[n]; }
  int llownode(int n) const{ assert(_llownode); return _llownode[n]; }
public:
  void zero();
  void dezero(T const& o);
  void fbsub(T* v) const;
  void fbsub(T* x, const T* b, T* c = nullptr) const;
  void fbsubt(T* v) const;
public:
  int		size()const		{return _size;}
  double 	density()const;
}; // BSMATRIX_DATA
/*--------------------------------------------------------------------------*/
class BSMATRIX_BASE {
};
/*--------------------------------------------------------------------------*/
// BUMP/SPIKE MATRIX interface class
template <class T>
class BSMATRIX : private BSMATRIX_BASE {
  friend class BSMATRIX_SOLVER<T>;
  BSMATRIX_DATA<T> _data;
  BSMATRIX_SOLVER<T>* _solver;
public:
  BSMATRIX(const BSMATRIX<T>&) = delete;
  explicit BSMATRIX(int s=0);
  ~BSMATRIX(){}

public:
  int size()const {
    return _data.size();
  }
  void zero() {
    return _data.zero();
  }
  void dezero(double gmin) {
    return _data.dezero(gmin);
  }
  double density()const {
    return _data.density();
  }

public:
  void set_solver(BSMATRIX_SOLVER<T>* S) {
    uninit();
    if(_solver) {
      _solver = nullptr;
    }else{
    }
    if(S) {
      S->uninit();
      _solver = S;
    }else{
    }
    init();
  }
private:
  void init(int s=0);
  void uninit();
public: // loading.
  void load_diagonal_point(int i, T value) {
    assert(_solver); return _solver->load_diagonal_point(i, value);
  }
  void load_point(int i, int j, T value) { itested();
    assert(_solver); return _solver->load_point(i, j, value);
  }
  void load_couple(int i, int j, T value) {
    assert(_solver); return _solver->load_couple(i, j, value);
  }
  void load_symmetric(int i, int j, T value) {
    assert(_solver); return _solver->load_symmetric(i, j, value);
  }
  void load_asymmetric(int r1, int r2, int c1, int c2, T value) {
    assert(_solver); return _solver->load_asymmetric(r1, r2, c1, c2, value);
  }

public:
  BSMATRIX_DATA<T> const& data() const{ untested(); return *this; }
private:
  BSMATRIX_DATA<T>& rw_data(){ return _data; }
public:
  void reinit(int ss=0) { uninit(); init(ss); }
public:
  void unallocate();
  void allocate();
  void reallocate();
  //void	clone(const BSMATRIX<T>&);
  void iwant1(int, int);
  void iwant2(int i, int j){ iwant1(i, j); iwant1(j, i); }
  void iwant(int i, int j){ iwant2(i, j); }
  void set_min_pivot(double x)	{assert(_solver); _solver->set_min_pivot(x);}
  T const& d(int r)const{
    BSMATRIX_SOLVER<T> const* s = _solver;
    assert(s);
    return s->d(r);
  }

public: // "solve?"
  void lu_decomp(bool do_partial) {
    assert(_solver); _solver->lu_decomp(do_partial);
  }
  void lu_decomp() {
    assert(_solver); _solver->lu_decomp(false);
  }
  void fbsub(T* v) const { assert(_solver); _solver->fbsub(v); }
  void fbsub(T* x, const T* b, T* c = nullptr) const {
    assert(_solver); _solver->fbsub(x, b, c);
  }
  void fbsubt(T* v) const {itested(); assert(_solver); _solver->fbsubt(v); }
}; // BSMATRIX
/*--------------------------------------------------------------------------*/
template <class T>
class BSMATRIX_SOLVER {
protected: // private?
  BSMATRIX_DATA<T>& _data; // _aa, _acx and the like
public:
  explicit BSMATRIX_SOLVER(BSMATRIX<T>& m) : _data(m._data) {}
  explicit BSMATRIX_SOLVER(BSMATRIX_SOLVER const&) = delete;
  virtual ~BSMATRIX_SOLVER() { }
public:
  int  size()const { return _data.size(); }

  virtual void init(int ss) = 0;
  virtual void uninit() = 0;
  void iwant(int i, int j){iwant2(i, j);}
  virtual void iwant1(int, int){} // LU_COPY
  virtual void iwant2(int, int){} // LU_COPY
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
// private implementations
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_DATA<T>::init(int s)
{
  assert(!_diaptr);
  assert(!_space);

  _size = s;

  assert(!_ulownode);
  assert(!_llownode);
  _ulownode = new int[size()+1];
  _llownode = new int[size()+1];
  for (int ii = 0;  ii <= size();  ++ii) {
    _ulownode[ii] = _llownode[ii] = ii;
  }

  assert(_zero == 0.);
  _trash = 0.;
  _nzcount = 0;
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_DATA<T>::uninit()
{
  unallocate();
  delete [] _llownode;
  delete [] _ulownode;
  _ulownode = _llownode = nullptr;
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::uninit()
{
  _data.uninit();

  if(_solver) {
    _solver->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::init(int ss)
{
  _data.init(ss);
  if(ss){
    assert(_solver);
  }else{
  }
  if(_solver){
    _solver->init(ss);
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// public implementations
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX_DATA<T>::BSMATRIX_DATA()
 :_ulownode(nullptr),
  _llownode(nullptr),
  _space(nullptr),
  _diaptr(nullptr),
  _nzcount(0),
  _size(0),
  _zero(0.),
  _trash(0.)
{
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>::BSMATRIX(int ss)
  : _data(), _solver(nullptr)
{
  init(ss);
}
/*--------------------------------------------------------------------------*/
#if 0
/* clone: copy to self the structure of another BSMATRIX
 * this does not copy the values stored in the matrix
 */
template <class T>
void BSMATRIX<T>::clone(const BSMATRIX<T> & aa)
{untested();
  reinit(aa.size());
  for (int ii = 0;  ii <= size();  ++ii) {untested();
    _lownode[ii] = aa.lownode(ii);
  }
}
#endif
/*--------------------------------------------------------------------------*/
/* iwant: indicate that "iwant" to allocate this spot in the matrix
 */
template <class T>
void BSMATRIX_DATA<T>::iwant1(int node1, int node2)
{
  assert(_ulownode);
  assert(_llownode);
  assert(node1 <= size());
  assert(node2 <= size());

  if (node1 <= 0  ||  node2 <= 0) {
    // node 0 is ground, and doesn't count as a connection
    // negative is invalid, not used but still may be in a node list
  }else if (node1 < _ulownode[node2]) {
    _ulownode[node2]=node1;
  }else if (node2 < _llownode[node1]) {
    _llownode[node1]=node2;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::iwant1(int node1, int node2)
{
  _data.iwant1(node1, node2);
  assert(_solver);
  _solver->iwant1(node1, node2); // could manage L and U copy
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::reallocate()
{
//  assert(_solver);
//  _solver->unallocate();
  unallocate();
  allocate();
//  _solver->allocate();
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::unallocate()
{
  _data.unallocate();
  assert(_solver);
  return _solver->unallocate();
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX_DATA<T>::unallocate()
{
  assert (_zero == 0.);
  delete [] _diaptr;
  delete [] _space;

  _diaptr = nullptr;
  _space = nullptr;
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::allocate()
{
  _data.allocate();
  assert(_solver);
  return _solver->allocate();
}
/*--------------------------------------------------------------------------*/
/* allocate: really get the space to work
 */
template <class T>
void BSMATRIX_DATA<T>::allocate()
{
  assert(_ulownode);
  assert(_llownode);
  assert(!_diaptr);
  assert(!_space);

  _nzcount = 0;
  for (int ii = 0;   ii <= size();   ++ii) {
    _nzcount += ii - _ulownode[ii];
    _nzcount += ii - _llownode[ii] + 1;
  }

  _diaptr = new T*[size()+1];
  _space  = new T[_nzcount];

  assert(_diaptr);

  zero();

  {
    T* point = _space;
    for (int ii = 0;   ii <= size();   ++ii) {
      _diaptr[ii] = point + ii - _ulownode[ii];
      point = _diaptr[ii] + ii - _llownode[ii] + 1;
    }
  }
}
/*--------------------------------------------------------------------------*/
/* zero: wipe the whole array
 */
template <class T>
void BSMATRIX_DATA<T>::zero()
{
  assert(_space);
  assert(_zero == 0.);
  _trash = 0.;
  std::fill_n(_space, _nzcount, 0.);
}
/*--------------------------------------------------------------------------*/
/* dezero: make sure(?) the diagonal is non-zero
 */
template <class T>
void BSMATRIX_DATA<T>::dezero(T const& offset)
{
  for (int ii = 1;  ii <= size();  ++ii) {
    d(ii) += offset;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
double BSMATRIX_DATA<T>::density() const
{
  if (size() > 0) {
    int nzcount = 0;
    assert(_ulownode);
    assert(_llownode);
    for (int ii = 0;   ii <= size();   ++ii) {
      nzcount += ii - _ulownode[ii];
      nzcount += ii - _llownode[ii] + 1;
    }
    return static_cast<double>(nzcount-1)/(static_cast<double>(size())*size());
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
/* d: fast matrix entry access
 * It is known that the entry is valid and on the diagonal
 */
template <class T>
T& BSMATRIX_DATA<T>::d(int r)
{
  assert(_diaptr);
  assert(0 <= r);
  assert(r <= _size);

  return *(_diaptr[r]);
}
/*--------------------------------------------------------------------------*/
/* u: fast matrix entry access
 * It is known that the entry is valid and in the upper triangle
 */
template <class T>
T& BSMATRIX_DATA<T>::u(int r, int c)
{
  assert(_diaptr);
  assert(_ulownode);
  assert(_llownode);
  assert(0 <= r);
  assert(r <= c);
  assert(c <= _size);
  assert(1 <= _ulownode[c]);
  assert(1 <= _llownode[c]);
  assert(_ulownode[c] <= r);

  return _diaptr[c][r-c];
}
/*--------------------------------------------------------------------------*/
/* l: fast matrix entry access
 * It is known that the entry is valid and in the lower triangle
 */
template <class T>
T& BSMATRIX_DATA<T>::l(int r, int c)
{
  assert(_diaptr);
  assert(_llownode);
  assert(0 < c);
  assert(c < r);
  assert(r <= _size);
  assert(1 <= _llownode[r]);
  assert(_llownode[r] <= c);

  return _diaptr[r][r-c];
}
/*--------------------------------------------------------------------------*/
/* m: semi-fast matrix entry access
 * It is known that the entry refers to a valid location
 * but it is not known whether lower, upper, or diagonal
 */
template <class T>
T& BSMATRIX_DATA<T>::m(int r, int c)
{
  assert(_diaptr);

  assert(_llownode);
  assert(0 < r);
  assert(r <= _size);
  assert(1 <= _llownode[r]);
  assert(_llownode[r] <= c);

  assert(_ulownode);
  assert(0 < c);
  assert(c <= _size);
  assert(1 <= _ulownode[c]);
  assert(_ulownode[c] <= r);

  return _diaptr[(c>=r)?c:r][r-c];
}
/*--------------------------------------------------------------------------*/
/* s: general matrix entry access (read-only)
 * It is known that the location is strictly in bounds,
 *   but it is not known whether the location actually exists.
 * If access is attempted to a non-allocated location,
 *   it returns a reference to a shared zero variable.
 *   Writing to this zero is not prohibited,
 *   but will corrupt the matrix in a known and testable way.
 * If access is attempted to row 0 or column 0,
 *   it returns a reference to a shared trash variable.
 *   Writing to trash is allowed and encouraged,
 *   but reading it gives a number not useful for anything.
 */
template <class T>
T const& BSMATRIX_DATA<T>::s(int row, int col)const
{itested();
  assert(_ulownode);
  assert(_llownode);
  assert(0 <= col);
  assert(col <= size());
  assert(0 <= row);
  assert(row <= size());
  assert(_zero == 0.);

  if (col == row) {itested();
    return d(row);
  }else if (col > row) {itested();    /* above the diagonal */
    if (row == 0) {itested();
      return _trash;
    }else if (row < _ulownode[col]) {itested();
      return _zero;
    }else{itested();
      return u(row, col);
    }
  }else{itested();                    /* below the diagonal */
    assert(col < row);
    if (col == 0) {itested();
      return _trash;
    }else if (col < _llownode[row]) {itested();
      return _zero;
    }else{itested();
      return l(row, col);
    }
  }
  unreachable();
}
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
/* fbsub: forward and back sub, shared storage
 * v = right side vector, changed in place to solution vector
 */
template <class T>
void BSMATRIX_DATA<T>::fbsub(T* v) const
{
  assert(_ulownode);
  assert(_llownode);
  assert(v);

  for (int ii = 1; ii <= size(); ++ii) {	/* forward substitution */
    for (int jj = _llownode[ii]; jj < ii; ++jj) {
      v[ii] -= l(ii,jj) * v[jj];
    }
    v[ii] /= d(ii);
  }

  for (int jj = size(); jj > 1; --jj) {		/* back substitution    */
    for (int ii = _ulownode[jj]; ii < jj; ++ii) {
      v[ii] -= u(ii,jj) * v[jj];
    }
  }
}
/*--------------------------------------------------------------------------*/
/* fbsub: forward and back sub, separate storage
 * b = right side vector
 * c = intermediate vector after fwd sub
 * x = solution vector
 */
template <class T>
void BSMATRIX_DATA<T>::fbsub(T* x, const T* b, T* c) const
{
  assert(_ulownode);
  assert(_llownode);
  assert(x);
  assert(b);
  assert(c);

  {
    int ii = 1;
    for (   ; ii <= size(); ++ii) {
      if (b[ii] != 0.) {
	break;
      }else{
      }
      c[ii] = 0.;
    }

    int first_nz = ii;
    for (   ; ii <= size(); ++ii) {		/* forward substitution */
      int low_node = std::max(_llownode[ii], first_nz);
      c[ii] = b[ii];
      for (int jj = low_node; jj < ii; ++jj) {
	c[ii] -= l(ii,jj) * c[jj];
      }
      c[ii] /= d(ii);
    }
  }

  notstd::copy_n(c, size()+1, x);

  for (int jj = size(); jj > 1; --jj) {		/* back substitution    */
    for (int ii = _ulownode[jj]; ii < jj; ++ii) {
      x[ii] -= u(ii,jj) * x[jj];
    }
  }
  x[0] = 0.;
  // index starts at 1, but node 0 is ground
  // x[0]==0 eliminates a lot of "if" statements
}
/*--------------------------------------------------------------------------*/
/* fbsubt: forward and back substitution with implicitly transposed matrix Ut Lt x = v
 * v = right side vector, changed in place to solution vector
 * GS:
 * this method s used to solve system A_t X = B (_t - transposed)
 * which corresponds to adjoint system
 * (LU)_t then transforms to U_t L_t
 *  added: Gennadiy Serdyuk <gserdyuk@gserdyuk.com>
 */
template <class T>
void BSMATRIX_DATA<T>::fbsubt(T* v) const
{itested();
  assert(_ulownode);
  assert(_llownode);
  assert(v);

  // forward substitution
  for (int ii = 1; ii <= size(); ++ii) {itested();
    for (int jj = _ulownode[ii]; jj < ii; ++jj) {itested();
      v[ii] -= u(jj,ii) * v [jj];
    }
  }

  // back substitution
  for (int jj = size(); jj > 1; --jj) {itested();
    v[jj] /= d(jj);
    for (int ii = _llownode[jj]; ii < jj; ++ii) {itested();
      v[ii] -= l(jj,ii) * v[jj];
    }
  }
  v[1] /= d(1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
