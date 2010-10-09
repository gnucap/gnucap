/*$Id: m_matrix.h,v 26.131 2009/11/20 08:22:10 al Exp $ -*- C++ -*-
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
//testing=script 2008.09.19
#ifndef M_MATRIX_H
#define M_MATRIX_H
/*--------------------------------------------------------------------------*/
#include "l_stlextra.h"
/*--------------------------------------------------------------------------*/
template <class T>
class BSMATRIX {
private:
  mutable bool* _changed;// flag: this node changed value
  int*	_lownode;	// lowest node connecting to this one
  T*	_space;		// ptr to actual memory space used
  T**	_rowptr;	// ptrs to col 0 of every row
  T**	_colptr;	// ptrs to row 0 of every col
  T**	_diaptr;	// ptrs to diagonal
  int	_nzcount;	// count of non-zero elements
  int	_size;		// # of rows and columns
  T	_zero;		// always 0 but not const
  T	_trash;		// depository for row and col 0, write only
  T	_min_pivot;	// minimum pivot value
private:
  explicit	BSMATRIX(const BSMATRIX<T>&) {incomplete();unreachable();}
  void		uninit();
  void		init(int s=0);
  T&		subtract_dot_product(int r, int c, int d);
  T&		subtract_dot_product(int r, int c, int d, const T& in);
  int		lownode(int i)const	{return _lownode[i];}
  bool		is_changed(int n)const	{return _changed[n];}
  void		set_changed(int n, bool x = true)const {_changed[n] = x;}
public:
  explicit	BSMATRIX(int ss=0);
  		~BSMATRIX()		{uninit();}
  void		reinit(int ss=0)	{uninit(); init(ss);}
  //void	clone(const BSMATRIX<T>&);
  void		iwant(int, int);
  void		unallocate();
  void		allocate();
  void		reallocate()		{unallocate(); allocate();}
  void		set_min_pivot(double x)	{_min_pivot = x;}
  void		zero();
  void		dezero(T& o);
  int		size()const		{return _size;}
  double 	density();
  T 	d(int r, int  )const	{return *(_diaptr[r]);}
private:
  T 	u(int r, int c)const	{return _colptr[c][r];}
  T 	l(int r, int c)const	{return _rowptr[r][-c];}
  T&	d(int r, int c);
  T&	u(int r, int c);
  T&	l(int r, int c);
  T&	m(int r, int c);
  //T&	s(int r, int c);
public:
  void		load_diagonal_point(int i, T value);
  void		load_point(int i, int j, T value);
  void		load_couple(int i, int j, T value);
  void		load_symmetric(int i, int j, T value);
  void		load_asymmetric(int r1, int r2, int c1, int c2, T value);
  
  void		lu_decomp(const BSMATRIX<T>&, bool do_partial);
  void		lu_decomp();
  void		fbsub(T* v) const;
  void		fbsub(T* x, const T* b, T* c = NULL) const;
};
/*--------------------------------------------------------------------------*/
// private implementations
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::uninit()
{
  unallocate();
  delete [] _lownode;
  _lownode = NULL;
  delete [] _changed;
  _changed = NULL;
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::init(int ss)
{
  assert(!_lownode);
  assert(!_colptr);
  assert(!_rowptr);
  assert(!_diaptr);
  assert(!_space);

  assert(_zero == 0.);
  _min_pivot = _trash = 0.;
  _nzcount = 0;
  _size = ss;
  _lownode = new int[size()+1];
  assert(_lownode);
  for (int ii = 0;  ii <= size();  ++ii) {
    _lownode[ii] = ii;
  }
  _changed = new bool[size()+1];
  assert(_changed);
  for (int ii = 0;  ii <= size();  ++ii) {
    set_changed(ii, false);
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
T& BSMATRIX<T>::subtract_dot_product(int rr, int cc, int dd)
{
  assert(_lownode);
  int kk = std::max(_lownode[rr], _lownode[cc]);
  int len = dd - kk;
  T& dot = m(rr, cc);
  if (len > 0) {
    T* row = &(l(rr,kk));
    T* col = &(u(kk,cc));
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii) {
      dot -= row[-ii] * col[ii];
    }
  }else{
  }
  return dot;
}
/*--------------------------------------------------------------------------*/
template <class T>
T& BSMATRIX<T>::subtract_dot_product(int rr, int cc, int dd, const T& in)
{
  assert(_lownode);
  int kk = std::max(_lownode[rr], _lownode[cc]);
  int len = dd - kk;
  T& dot = m(rr, cc);
  dot = in;
  if (len > 0) {
    T* row = &(l(rr,kk));
    T* col = &(u(kk,cc));
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii) {
      dot -= row[-ii] * col[ii];
    }
  }else{
  }
  return dot;
}
/*--------------------------------------------------------------------------*/
// public implementations
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>::BSMATRIX(int ss)
  :_changed(NULL),
   _lownode(NULL),
   _space(NULL),
   _rowptr(NULL),
   _colptr(NULL),
   _diaptr(NULL),
   _nzcount(0),
   _size(ss),
   _zero(0.),
   _trash(0.),
   _min_pivot(0.)
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
void BSMATRIX<T>::iwant(int node1, int node2)
{
  assert(_lownode);
  assert(node1 <= size());
  assert(node2 <= size());

  if (node1 <= 0  ||  node2 <= 0) {
    // node 0 is ground, and doesn't count as a connection
    // negative is invalid, not used but still may be in a node list
  }else if (node1 < _lownode[node2]) {
    _lownode[node2]=node1;
  }else if (node2 < _lownode[node1]) {
    _lownode[node1]=node2;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::unallocate()
{
  assert (_zero == 0.);
  delete [] _rowptr;
  delete [] _colptr;
  delete [] _diaptr;
  delete [] _space;

  _rowptr = _colptr = _diaptr = NULL;
  _space = NULL;
}
/*--------------------------------------------------------------------------*/
/* allocate: really get the space to work
 */
template <class T>
void BSMATRIX<T>::allocate()
{
  assert(_lownode);
  assert(!_colptr);
  assert(!_rowptr);
  assert(!_diaptr);
  assert(!_space);

  _nzcount = 0;
  for (int ii = 0;   ii <= size();   ++ii) {
    _nzcount += 2 * (ii - _lownode[ii]) + 1;
  }

  _colptr = new T*[size()+1];
  _rowptr = new T*[size()+1];
  _diaptr = new T*[size()+1];
  _space  = new T[_nzcount];

  assert(_colptr);
  assert(_rowptr);
  assert(_diaptr);

  zero();

  {
    T* point = _space;
    for (int ii = 0;   ii <= size();   ++ii) {
      _colptr[ii] = point - _lownode[ii];
      _rowptr[ii] = _colptr[ii] + 2*ii;
      _diaptr[ii] = _colptr[ii] + ii;
      point += 2 * (ii - _lownode[ii]) + 1;
    }
  }
}
/*--------------------------------------------------------------------------*/
/* zero: wipe the whole array
 */
template <class T>
void BSMATRIX<T>::zero()
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
void BSMATRIX<T>::dezero(T& offset)
{
  for (int ii = 1;  ii <= size();  ++ii) {
    d(ii,ii) += offset;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
double BSMATRIX<T>::density()
{
  if (size() > 0) {
    assert(_lownode);
    _nzcount = 0;
    for (int ii = 0;   ii <= size();   ++ii) {
      _nzcount += 2 * (ii - _lownode[ii]) + 1;
    }
    return static_cast<double>(_nzcount-1)/(static_cast<double>(size())*size());
  }else{
    return 0;
  }
}
/*--------------------------------------------------------------------------*/
/* d: fast matrix entry access, lvalue
 * It is known that the entry is valid and on the diagonal
 */
template <class T>
T& BSMATRIX<T>::d(int r, int c)
{
  assert(_diaptr);
  assert(r == c);
  assert(0 <= r);
  assert(r <= _size);

  return *(_diaptr[r]);
}
/*--------------------------------------------------------------------------*/
/* u: fast matrix entry access
 * It is known that the entry is valid and in the upper triangle
 */
template <class T>
T& BSMATRIX<T>::u(int r, int c)
{
  assert(_colptr);
  assert(_lownode);
  assert(0 < r);
  assert(r <= c);
  assert(c <= _size);
  assert(1 <= _lownode[c]);
  assert(_lownode[c] <= r);

  return _colptr[c][r];
}
/*--------------------------------------------------------------------------*/
/* l: fast matrix entry access
 * It is known that the entry is valid and in the lower triangle
 */
template <class T>
T& BSMATRIX<T>::l(int r, int c)
{
  assert(_rowptr);
  assert(_lownode);
  assert(0 < c);
  assert(c <= r);
  assert(r <= _size);
  assert(1 <= _lownode[r]);
  assert(_lownode[r] <= c);

  return _rowptr[r][-c];
}
/*--------------------------------------------------------------------------*/
/* m: semi-fast matrix entry access
 * It is known that the entry refers to a valid location
 * but it is not known whether lower, upper, or diagonal
 */
template <class T>
T& BSMATRIX<T>::m(int r, int c)
{
  return (c>=r) ? u(r,c) : l(r,c);
}
/*--------------------------------------------------------------------------*/
/* s: general matrix entry access.
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
#if 0
template <class T>
T& BSMATRIX<T>::s(int row, int col)
{untested();
  assert(_lownode);
  assert(0 <= col);
  assert(col <= size());
  assert(0 <= row);
  assert(row <= size());
  assert(_zero == 0.);

  if (col == row) {untested();
    return d(row,col);
  }else if (col > row) {untested();	/* above the diagonal */
    if (row == 0) {untested();
      return _trash;
    }else if (row < _lownode[col]) {untested();
      return _zero;
    }else{untested();
      return u(row,col);
    }
  }else{untested();			/* below the diagonal */
    assert(col < row);
    if (col == 0) {untested();
      return _trash;
    }else if (col < _lownode[row]) {untested();
      return _zero;
    }else{untested();
      return l(row,col);
    }
  }
  unreachable();
}
#endif
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::load_point(int i, int j, T value)
{
  if (i > 0 && j > 0) {
    set_changed(j);
    set_changed(i);
    m(i,j) += value;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value);
template <class T>
void BSMATRIX<T>::load_diagonal_point(int i, T value)
{
  if (i > 0) {
    set_changed(i);
    d(i,i) += value;
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void BSMATRIX<T>::load_couple(int i, int j, T value)
{
  if (j > 0) {
    set_changed(j);
    if (i > 0) {
      set_changed(i);
      m(i,j) -= value;
      m(j,i) -= value;
    }else{
    }
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value); or load_diagonal_point(i, value);
// load_point(j, j, value); or load_diagonal_point(j, value);
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void BSMATRIX<T>::load_symmetric(int i, int j, T value)
{
  if (j > 0) {
    set_changed(j);
    d(j,j) += value;
    if (i > 0) {
      set_changed(i);
      d(i,i) += value;
      m(i,j) -= value;
      m(j,i) -= value;
    }else{
    }
  }else if (i > 0) {
    set_changed(i);
    d(i,i) += value;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// load_point(r1, c1, value);
// load_point(r2, c2, value);
// load_point(r1, c2, -value);
// load_point(r2, c1, -value);
template <class T>
void BSMATRIX<T>::load_asymmetric(int r1,int r2,int c1,int c2,T value)
{
  set_changed(c1);
  set_changed(c2);
  if (r1 > 0) {
    set_changed(r1);
    if (c1 > 0) {
      m(r1,c1) += value;
    }else{
    }
    if (c2 > 0) {
      m(r1,c2) -= value;
    }else{
    }
  }else{
  }
  if (r2 > 0) {
    set_changed(r2);
    if (c1 > 0) {
      m(r2,c1) -= value;
    }else{
    }
    if (c2 > 0) {
      m(r2,c2) += value;
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::lu_decomp(const BSMATRIX<T>& aa, bool do_partial)
{
  int prop = 0;   /* change propagation indicator */
  assert(_lownode);
  assert(aa._lownode);
  assert(aa.size() == size());
  for (int mm = 1;   mm <= size();   ++mm) {
    assert(aa.lownode(mm) == _lownode[mm]);
    int bn = _lownode[mm];
    if (!do_partial  ||  aa.is_changed(mm)  ||  bn <= prop) {
      aa.set_changed(mm, false);
      prop = mm;
      if (bn < mm) {
	prop = mm;
	u(bn,mm) = aa.u(bn,mm) / d(bn,bn);
	for (int ii = bn+1;  ii<mm;  ii++) {
	  /* u(ii,mm) = (aa.u(ii,mm) - dot(ii,mm,ii)) / d(ii,ii); */
	  subtract_dot_product(ii,mm,ii,aa.u(ii,mm)) /= d(ii,ii);
	}
	l(mm,bn) = aa.l(mm,bn);
	for (int jj = bn+1;  jj<mm;  jj++) {
	  /* l(mm,jj) = aa.l(mm,jj) - dot(mm,jj,jj); */
	  subtract_dot_product(mm,jj,jj,aa.l(mm,jj));
	}
	{ /* jj == mm */
	  /* d(mm,mm) = aa.d(mm,mm) - dot(mm,mm,mm); then test */
	  if (subtract_dot_product(mm,mm,mm,aa.d(mm,mm)) == 0.) {untested();
	    error(bWARNING, "open circuit: internal node %u\n", mm);
	    d(mm,mm) = _min_pivot;
	  }else{
	  }
	}
      }else{    /* bn == mm */
	d(mm,mm) = aa.d(mm,mm);
	if (d(mm,mm)==0.) {untested();
	  d(mm,mm) = _min_pivot;
	}else{
	}
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSMATRIX<T>::lu_decomp()
{
  assert(_lownode);
  for (int mm = 1;   mm <= size();   ++mm) {
    int bn = _lownode[mm];
    if (bn < mm) {
      u(bn,mm) /= d(bn,bn);
      for (int ii =bn+1;  ii<mm;  ii++) {
	/* (m(ii,mm) -= dot(ii,mm,ii)) /= d(ii,ii); */
	subtract_dot_product(ii,mm,ii) /= d(ii,ii);
      }
      for (int jj = bn+1;  jj<mm;  jj++) {
	/* m(mm,jj) -= dot(mm,jj,jj); */
	subtract_dot_product(mm,jj,jj);
      }
      { /* jj == mm */
	/* m(mm,mm) -= dot(mm,mm,mm); then test */
	if (subtract_dot_product(mm,mm,mm) == 0.) {untested();
	  error(bWARNING, "open circuit: internal node %u\n", mm);
	  d(mm,mm) = _min_pivot;
	}else{
	}
      }
    }else{    /* bn == mm */
      if (d(mm,mm)==0.) {
	d(mm,mm) = _min_pivot;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/* fbsub: forward and back sub, shared storage
 * v = right side vector, changed in place to solution vector
 */
template <class T>
void BSMATRIX<T>::fbsub(T* v) const
{
  assert(_lownode);
  assert(v);

  for (int ii = 1; ii <= size(); ++ii) {	/* forward substitution */
    for (int jj = _lownode[ii]; jj < ii; ++jj) {
      v[ii] -= l(ii,jj) * v[jj];
    }
    v[ii] /= d(ii,ii);
  }

  for (int jj = size(); jj > 1; --jj) {		/* back substitution    */
    for (int ii = _lownode[jj]; ii < jj; ++ii) {
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
void BSMATRIX<T>::fbsub(T* x, const T* b, T* c) const
{
  assert(_lownode);
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
      int low_node = std::max(_lownode[ii], first_nz);
      c[ii] = b[ii];
      for (int jj = low_node; jj < ii; ++jj) {
	c[ii] -= l(ii,jj) * c[jj];
      }
      c[ii] /= d(ii,ii);
    }
  }

  notstd::copy_n(c, size()+1, x);

  for (int jj = size(); jj > 1; --jj) {		/* back substitution    */
    for (int ii = _lownode[jj]; ii < jj; ++ii) {
      x[ii] -= u(ii,jj) * x[jj];
    }
  }
  x[0] = 0.;
  //BUG// some things don't work unless there is a zero here.
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif

