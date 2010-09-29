/*$Id: m_matrix.cc,v 22.21 2002/10/06 07:21:50 al Exp $ -*- C++ -*-
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
 */
/*--------------------------------------------------------------------------*/
#include "l_stlextra.h"
#include "u_status.h"
#include "e_node.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
extern NODE* nstat;
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::uninit()
{
  unallocate();
  delete [] _lownode;
  _lownode = NULL;
  delete [] _changed;
  _changed = NULL;
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::init(int ss)
{
  _min_pivot = _trash = _zero = 0.;
  _nzcount = 0;
  _size = ss;
  _rowptr = _colptr = _diaptr = NULL;
  _space = NULL;
  _lownode = new int[_size+1];
  assert(_lownode);
  {for (int ii = 0;  ii <= _size;  ++ii)
    _lownode[ii] = ii;
  }
  _changed = new bool[_size+1];
  assert(_changed);
  for (int ii = 0;  ii <= _size;  ++ii)
    set_changed(ii, false);
  return *this;
}
/*--------------------------------------------------------------------------*/
/* clone: copy to self the structure of another BSMATRIX
 * this does not copy the values stored in the matrix
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::clone(const BSMATRIX<T> & aa)
{
  reinit(aa.size());
  for (int ii = 0;  ii <= _size;  ++ii)
    _lownode[ii] = aa.lownode(ii);  
  return *this;
}
/*--------------------------------------------------------------------------*/
/* iwant: indicate that "iwant" to allocate this spot in the matrix
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::iwant(int node1, int node2)
{
  assert(_lownode);
  assert(node1 <= _size);
  assert(node2 <= _size);
  {if (node1 == 0  ||  node2 == 0){	/* node 0 is ground, and doesn't    */
    ;					/* count as a connection	    */
  }else if (node1 < _lownode[node2]){
    _lownode[node2]=node1;
  }else if (node2 < _lownode[node1]){
    _lownode[node1]=node2;
  }}
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::unallocate()
{
  assert (_zero == 0.);
  delete [] _rowptr;
  delete [] _colptr;
  delete [] _diaptr;
  delete [] _space;
  _rowptr = _colptr = _diaptr = NULL;
  _space = NULL;
  return *this;
}
/*--------------------------------------------------------------------------*/
/* allocate: really get the space to work
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::allocate()
{
  assert(_lownode);
  _nzcount = 0;
  {for (int ii = 0;   ii <= _size;   ++ii)
    _nzcount += 2 * (ii - _lownode[ii]) + 1;
  }
  _colptr = new T*[_size+1];
  _rowptr = new T*[_size+1];
  _diaptr = new T*[_size+1];
  _space  = new T[_nzcount];
  assert(_colptr);
  assert(_rowptr);
  assert(_diaptr);
  zero();

  T* point = _space;
  for (int ii = 0;   ii <= _size;   ++ii){
    _colptr[ii] = point - _lownode[ii];
    _rowptr[ii] = _colptr[ii] + 2*ii;
    _diaptr[ii] = _colptr[ii] + ii;
    point += 2 * (ii - _lownode[ii]) + 1;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
T& BSMATRIX<T>::s(int row, int col)
{
  assert(_lownode);
  assert(0 <= col);
  assert(col <= _size);
  assert(0 <= row);
  assert(row <= _size);
  {if (col == row){
    return d(row,col);
  }else if (col > row){			/* above the diagonal */
    if (row == 0)
      return _trash;
    else if (row < _lownode[col])
      return _zero;
    else
      return u(row,col);
  }else{/* if (col < row) */		/* below the diagonal */
    if (col == 0)
      return _trash;
    else if (col < _lownode[row])
      return _zero;
    else
      return l(row,col);
  }}
}
/*--------------------------------------------------------------------------*/
/* zero: wipe the whole array
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::zero()
{
  assert(_space);
  _trash = _zero = 0.;
  for (int ii = 0;  ii < _nzcount;  ii++){
    _space[ii] = _zero;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* dezero: make sure(?) the diagonal is non-zero
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::dezero(T& offset)
{
  for (int ii = 1;  ii <= _size;  ii++){
    d(ii,ii) += offset;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
double BSMATRIX<T>::density()
{
  {if (_size > 0){
    assert(_lownode);
    _nzcount = 0;
    for (int ii = 0;   ii <= _size;   ++ii)
      _nzcount += 2 * (ii - _lownode[ii]) + 1;
    return static_cast<double>(_nzcount-1)/(_size*_size);
  }else{
    assert(_size == 0);
    return 0;
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
T& BSMATRIX<T>::subtract_dot_product(int rr, int cc, int dd)
{
  assert(_lownode);
  int kk = std::max(_lownode[rr], _lownode[cc]);
  int len = dd - kk;
  T& dot = m(rr, cc);
  if (len > 0){
    T* row = &(l(rr,kk));
    T* col = &(u(kk,cc));
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii){
      dot -= row[-ii] * col[ii];
    }
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
  if (len > 0){
    T* row = &(l(rr,kk));
    T* col = &(u(kk,cc));
    /* for (ii = kk;   ii < dd;   ++ii) */
    for (int ii = 0;   ii < len;   ++ii){
      dot -= row[-ii] * col[ii];
    }
  }
  return dot;
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::lu_partial(const BSMATRIX<T> & aa)
{
  int prop = 0;   /* change propagation indicator */
  assert(_lownode);
  assert(aa._lownode);
  assert(aa.size() == _size);
  STATUS::lud.start();
  for (int mm = 1;   mm <= aa.size();   ++mm){
    assert(aa.lownode(mm) == _lownode[mm]);
    int bn = _lownode[mm];
    if (aa.is_changed(mm)  ||  bn <= prop){
      aa.set_changed(mm, false);
      prop = mm;
      {if (bn < mm){
	prop = mm;
	u(bn,mm) = aa.u(bn,mm) / d(bn,bn);
	for (int ii = bn+1;  ii<mm;  ii++){
	  /* u(ii,mm) = (aa.u(ii,mm) - dot(ii,mm,ii)) / d(ii,ii); */
	  subtract_dot_product(ii,mm,ii,aa.u(ii,mm)) /= d(ii,ii);
	}
	l(mm,bn) = aa.l(mm,bn);
	for (int jj = bn+1;  jj<mm;  jj++){
	  /* l(mm,jj) = aa.l(mm,jj) - dot(mm,jj,jj); */
	  subtract_dot_product(mm,jj,jj,aa.l(mm,jj));
	}
	/* jj == mm */{
	  /* d(mm,mm) = aa.d(mm,mm) - dot(mm,mm,mm); then test */
	  if (subtract_dot_product(mm,mm,mm,aa.d(mm,mm)) == 0.){
	    error(bWARNING, "open circuit: internal node %u\n", mm);
	    d(mm,mm) = _min_pivot;
	  }
	}
      }else{    /* bn == mm */
	d(mm,mm) = aa.d(mm,mm);
	if (d(mm,mm)==0.){
	  d(mm,mm) = _min_pivot;
	}
      }}
    }
  }
  STATUS::lud.stop();
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::lu_full(const BSMATRIX<T> & aa)
{
  assert(_lownode);
  assert(aa._lownode);
  assert(aa.size() == _size);
  STATUS::lud.start();
  for (int mm = 1;   mm <= _size;   ++mm){
    assert(aa.lownode(mm) == _lownode[mm]);
    int bn = _lownode[mm];
    aa.set_changed(mm, false);
    {if (bn < mm){
      u(bn,mm) = aa.u(bn,mm) / d(bn,bn);
      for (int ii = bn+1;  ii<mm;  ii++){
	/* u(ii,mm) = (aa.u(ii,mm) - dot(ii,mm,ii)) / d(ii,ii); */
	subtract_dot_product(ii,mm,ii,aa.u(ii,mm)) /= d(ii,ii);
      }
      l(mm,bn) = aa.l(mm,bn);
      for (int jj = bn+1;  jj<mm;  jj++){
	/* l(mm,jj) = aa.l(mm,jj) - dot(mm,jj,jj); */
	subtract_dot_product(mm,jj,jj,aa.l(mm,jj));
      }
      /* jj == mm */{
	/* d(mm,mm) = aa.d(mm,mm) - dot(mm,mm,mm); then test */
	if (subtract_dot_product(mm,mm,mm,aa.d(mm,mm)) == 0.){
	  error(bWARNING, "open circuit: internal node %u\n", mm);
	  d(mm,mm) = _min_pivot;
	}
      }
    }else{    /* bn == mm */
      d(mm,mm) = aa.d(mm,mm);
      if (d(mm,mm)==0.){
	d(mm,mm) = _min_pivot;
      }
    }}
  }
  STATUS::lud.stop();
  return *this;
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::lu_decomp(const BSMATRIX<T> & aa, bool dopartial)
{
  {if (dopartial){
    return lu_partial(aa);
  }else{
    return lu_full(aa);
  }}
}
/*--------------------------------------------------------------------------*/
template <class T>
BSMATRIX<T>& BSMATRIX<T>::lu_decomp()
{
  assert(_lownode);
  STATUS::lud.start();
  for (int mm = 1;   mm <= _size;   ++mm){
    int bn = _lownode[mm];
    {if (bn < mm){
      u(bn,mm) /= d(bn,bn);
      for (int ii =bn+1;  ii<mm;  ii++){
	/* (m(ii,mm) -= dot(ii,mm,ii)) /= d(ii,ii); */
	subtract_dot_product(ii,mm,ii) /= d(ii,ii);
      }
      for (int jj = bn+1;  jj<mm;  jj++){
	/* m(mm,jj) -= dot(mm,jj,jj); */
	subtract_dot_product(mm,jj,jj);
      }
      /* jj == mm */{
	/* m(mm,mm) -= dot(mm,mm,mm); then test */
	if (subtract_dot_product(mm,mm,mm) == 0.){
	  error(bWARNING, "open circuit: node %u\n", mm);
	  d(mm,mm) = _min_pivot;
	}
      }
    }else{    /* bn == mm */
      if (d(mm,mm)==0.){
	d(mm,mm) = _min_pivot;
      }
    }}
  }
  STATUS::lud.stop();
  return *this;
}
/*--------------------------------------------------------------------------*/
/* fbsub: forward and back sub, shared storage
 * v = right side vector, changed in place to solution vector
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::fbsub(T* v)
{
  assert(_lownode);
  assert(v);
  STATUS::back.start();
  {for (int ii = 1; ii <= _size; ++ii){	    /* forward substitution */
    for (int jj = _lownode[ii]; jj < ii; ++jj){
      v[ii] -= l(ii,jj) * v[jj];
    }
    v[ii] /= d(ii,ii);
  }}
  for (int jj = _size; jj > 1; --jj){	    /* back substitution    */
    for (int ii = _lownode[jj]; ii < jj; ++ii){
      v[ii] -= u(ii,jj) * v[jj];
    }
  }
  STATUS::back.stop();
  return *this;
}
/*--------------------------------------------------------------------------*/
/* fbsub: forward and back sub, separate storage
 * b = right side vector
 * c = intermediate vector after fwd sub
 * x = solution vector
 */
template <class T>
BSMATRIX<T>& BSMATRIX<T>::fbsub(T* x, const T* b, T* c)
{
  assert(_lownode);
  assert(x);
  assert(b);
  assert(c);
  STATUS::back.start();
  if (!c){
    c = x;
    if (!b)
      b = x;
  }
  {
    notstd::copy_n(b, _size+1, c);
    int ii = 1;
    for (   ; ii <= _size; ++ii){
      if (b[ii] != 0.){
	break;
      }
    }
    for (   ; ii <= _size; ++ii){		/* forward substitution */
      for (int jj = _lownode[ii]; jj < ii; ++jj){
	c[ii] -= l(ii,jj) * c[jj];
      }
      c[ii] /= d(ii,ii);
    }
  }
  for (int ii = _size; ii >= 1; --ii){
    x[ii] = c[ii];
    nstat[ii].set_a_iter();
  }
  for (int jj = _size; jj > 1; --jj){		/* back substitution    */
    for (int ii = _lownode[jj]; ii < jj; ++ii){
      x[ii] -= u(ii,jj) * x[jj];
    }
  }
  STATUS::back.stop();
  return *this;
}
/*--------------------------------------------------------------------------*/
#if defined(MANUAL_TEMPLATES)
  template class BSMATRIX<double>;
  template class BSMATRIX<COMPLEX>;
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
