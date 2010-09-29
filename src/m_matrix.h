/*$Id: m_matrix.h,v 21.14 2002/03/26 09:20:25 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@ieee.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
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
#ifndef M_MATRIX_H
#define M_MATRIX_H
/*--------------------------------------------------------------------------*/
//#include "md.h"
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
public:
  explicit	BSMATRIX(int ss=0)	{init(ss);}
  		~BSMATRIX()		{uninit();}
  BSMATRIX<T>&	reinit(int ss=0)	{return uninit().init(ss);}
  BSMATRIX<T>&	clone(const BSMATRIX<T>&);
  BSMATRIX<T>&	iwant(int, int);
  BSMATRIX<T>&	unallocate();
  BSMATRIX<T>&	allocate();
  void		set_changed(int n, bool x = true)const {_changed[n] = x;}
  BSMATRIX<T>&	set_min_pivot(double x)	{_min_pivot = x; return *this;}
  BSMATRIX<T>&	zero();
  BSMATRIX<T>&	dezero(T& o);
  int		size()const{return _size;}
  double 	density();
  double 	sparsity(){return 1.-density();}
private:
  explicit	BSMATRIX(const BSMATRIX<T>&) {incomplete();unreachable();}
  BSMATRIX<T>&	uninit();
  BSMATRIX<T>&	init(int s=0);
  const T& 	d(int r, int  )const	{return *(_diaptr[r]);}
  const T& 	u(int r, int c)const	{return _colptr[c][r];}
  const T& 	l(int r, int c)const	{return _rowptr[r][-c];}
  T&		subtract_dot_product(int r, int c, int d);
  T&		subtract_dot_product(int r, int c, int d, const T& in);
  BSMATRIX<T>&	lu_partial(const BSMATRIX<T>&);
  BSMATRIX<T>&	lu_full(const BSMATRIX<T>&);
  int		lownode(int i)const	{return _lownode[i];}
  bool		is_changed(int n)const	{return _changed[n];}
public:
#if !defined(NO_MATRIX_DEBUG) && !defined(NDEBUG)
  T&	d(int r, int c){
	assert(_diaptr);
  	assert(r == c);
	assert(0 <= r);
	assert(r <= _size);
	return *(_diaptr[r]);
  }
  T&	u(int r, int c){
	assert(_colptr);
	assert(_lownode);
	assert(1 <= _lownode[c]);
	assert(_lownode[c] <= r);
  	assert(r <= c);
	assert(c <= _size);
	return _colptr[c][r];
  }
  T&	l(int r, int c){
	assert(_rowptr);
	assert(_lownode);
	assert(1 <= _lownode[r]);
	assert(_lownode[r] <= c);
  	assert(c <= r);
	assert(r <= _size);
	return _rowptr[r][-c];
  }
#else
  T&	d(int r, int  ){return *(_diaptr[r]);}
  T&	u(int r, int c){return _colptr[c][r];}
  T&	l(int r, int c){return _rowptr[r][-c];}
#endif
  T&	m(int r, int c){return (c>=r) ? u(r,c) : l(r,c);}
  T&	s(int r, int c);

  BSMATRIX<T>&	load_symmetric(int i, int j, T value){
    {if (j != 0){
      set_changed(j);
      d(j,j) += value;
      if (i != 0){
	set_changed(i);
	d(i,i) += value;
	m(i,j) -= value;
	m(j,i) -= value;
      }
    }else if (i != 0){
      set_changed(i);
      d(i,i) += value;
    }}
    return *this;
  }

  BSMATRIX<T>&	load_asymmetric(int r1, int r2, int c1, int c2, T value){
    set_changed(r1);
    set_changed(c1);
    set_changed(r2);
    set_changed(c2);
    s(r1,c1) += value;
    s(r2,c2) += value;
    s(r1,c2) -= value;
    s(r2,c1) -= value;
    return *this;
  }
  
  BSMATRIX<T>&	lu_decomp(const BSMATRIX<T>&, bool dopartial);
  BSMATRIX<T>&	lu_decomp();
  BSMATRIX<T>&	fbsub(T* v);
  BSMATRIX<T>&	fbsub(T* x, const T* b, T* c = NULL);
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
