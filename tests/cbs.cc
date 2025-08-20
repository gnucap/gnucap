/*                             -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2023, 2024 Felix Salfelder
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
 * compressed bump & spike
 * -------------------------------------------------
 */
#include "l_stlextra.h"
#include "l_smallset.h"
#include <stack>
#include "m_matrix.h"
#include "e_base.h"
#include "u_sim_data.h"
#include "c_comand.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
template<class T>
int count_nz(T const& t)
{
  size_t nz = 0;
  for(auto const& i : t){
    nz += i.size();
  }
  return int(nz);
}
/*--------------------------------------------------------------------------*/
bool is_in(int const* idx, int what, int dir)
{
  while(*idx){
    if(*idx == what){
      trace2("isin", *idx, what);
      return true;
    }else{
      trace2("notin", *idx, what);
    }
    idx += dir;
  }
  return false;

}
// serialize BS adjacency list. row oriented:
// row idx are at diag[i]   .. diag[i]+rowsize-1
// col idx are at diag[i]-1 .. diag[i]-colsize
// ordered ascending from the diagonal
//
// swap args for col orientation
template<class container_t>
int serialize_bs_adj(container_t const& rows_, container_t const& cols_,
    int*& _idx, int*& _didx)
{
  int unz = count_nz(cols_)
          + count_nz(rows_);

  _idx = new int[unz + cols_.size() + 1];
  _didx = new int[cols_.size()];
  int sep = 0;
  _idx[sep] = 0;

  for(int i = 0; i < int(cols_.size()); ++i ){
    _didx[i] = sep + 1 + cols_[i].size();
    int s = 0;
    for(int x : cols_[i]){
      _idx[_didx[i] - s - 1] = x;
      ++s;
    }
    s = 0;
    for(int x : rows_[i]){
      _idx[_didx[i] + s] = x;
      ++s;
    }
    sep = _didx[i] + s;
    _idx[sep] = 0;
  }

  assert(sep == unz + int(cols_.size()));
  return unz;
}
namespace {
/*--------------------------------------------------------------------------*/
class FOOTPRINT {
  typedef int value_type;
  typedef SMALL_SET<value_type> set_t;
  typedef std::vector<set_t> container_t;
  typedef container_t::const_iterator const_iterator;

private:
  int _nreq{0};
  int _nstamp{0};
  int* _raw_idx{nullptr};
  int* _raw_didx{nullptr};
public: // BUG
  container_t _rows;
  container_t _cols;
  set_t _inodes;
public:
  explicit FOOTPRINT(int s){
    init(s);
  }

  void iwant_point(int, int);
  void iwant_quad(int i, int j){ iwant_point(i,j), iwant_point(j,i);}
  void iwant_inode(int i, int j){
    if(i){
      iwant_quad(i,j);
    }else{
    }
    assert(j);
    _inodes.insert(j);
  }

  int size()const {
    int i = 0;
    for(auto d : _rows){
      i += int(d.size());
    }
    for(auto d : _cols){
      i += int(d.size());
    }
    return i;
  }
  int nreq()const {return _nreq;}
  int nstamp()const {return _nstamp;}
  bool is_inode(int i)const { return _inodes.count(i); }
  int ninode()const { return _inodes.size(); }

  void init(int s) {
    assert(_cols.empty());
    assert(_rows.empty());
    _cols.resize(s);
    _rows.resize(s);
    assert(!size());
  }
  void allocate() {
    if(_raw_idx){
      // keep it
    }else{
      _nstamp = serialize_bs_adj(cols(), rows(), _raw_idx, _raw_didx);
    }
  }
  void unallocate() {
  }
  void uninit(){
    delete _raw_idx;
    delete _raw_didx;
    _raw_idx = _raw_didx = nullptr;

    _cols.clear();
    _rows.clear();
  }

  void dumpgr(){ untested();
    int s = 0;
    for(auto& i : _rows){ untested();
      s += int(i.size());
    }
    std::cout << "p tw " << to_string(size()) << " " << to_string(s) << "\n";
    auto ii = 0;
    for(auto& i : _rows){ untested();
      ++ii;
      for(auto const& j : i){ untested();
	std::cout << ii << " " << j << "\n";
      }
    }
  }

  void sort(){ }

  container_t const& cols()const {return _cols;}
  container_t const& rows()const {return _rows;}
public: // used in compute_lu_fill
  set_t& cols(int i) {return _cols[i];}
  set_t& rows(int i) {return _rows[i];}

  bool is_req(int r, int c)const {
    assert(r<int(_cols.size()));
    assert(c<int(_cols.size()));
    assert(_raw_idx);
    if(r<c){
      // upper
      return is_in(&_raw_idx[_raw_didx[r]-1], c+1, -1);
    }else{
      // lower
      return is_in(&_raw_idx[_raw_didx[c]], r+1, 1);
    }
  }
};
/*--------------------------------------------------------------------------*/
inline void FOOTPRINT::iwant_point(int i, int j)
{
  assert(i);
  assert(j);
  if(i<j){
    // upper
    _rows[i-1].insert(j);
  }else if(j<i){
    // lower
    _cols[j-1].insert(i);
  }else{
  }
  ++_nreq;
}
/*--------------------------------------------------------------------------*/
template<class T>
class CBS;
/*--------------------------------------------------------------------------*/
// asymmetric BSMATRIX
template<class T>
class BSSMATRIX {
  friend class CBS<T>;
  int*	_lownode_row;	// lowest node in this row
  int*	_lownode_col;	// lowest node in this col
  T*	_space;		// ptr to actual memory space used
  T**	_colptr;	// ptrs to col 0 of every row
  T**	_diaptr;	// ptrs to diagonal
  int	_memsize;	// count of allocated Ts
  int	_size;		// # of rows and columns
  T	_zero;		// always 0 but not const
  T	_trash;		// depository for row and col 0, write only
public:
  explicit BSSMATRIX(int s=0);
  ~BSSMATRIX(){ untested();
    uninit();
  }

private: // impl.
  T const* rowptr(int i)const {return _colptr[i+1] - 1; }
  T const* colptr(int i)const {return _colptr[i]; }
  T const* diaptr(int i)const {return _diaptr[i]; }
  T* rowptr(int i) {return _colptr[i+1] - 1; }
  T* colptr(int i) {return _colptr[i]; }
  T* diaptr(int i) {return _diaptr[i]; }
//  void set_changed(int n, bool x = true)const { untested();_changed[n] = x;}
public: // lifecycle
  void init(int size);
  void iwant(int i, int j);
  void allocate();
  void unallocate();
  void uninit();

public: // internals for CBS
  int lownode_col(int i){ untested(); return _lownode_col[i];}
  int lownode_row(int i){ return _lownode_row[i];}

public:
  void zero();
  void dezero(T& offset);
  int size()const { return _size;}

private:
  T&	   u(int r, int c);
  T&	   l(int r, int c);
  T&	   m(int r, int c);
  T&	   d(int r) { untested(); return *diaptr(r); }
  T const& u(int r, int c)const;
  T const& l(int r, int c)const;
  T const& m(int r, int c)const;
  T const& d(int r)const { return *diaptr(r); }
//  int lownode(int n) const{ untested();return _lownode[n]; }

public:
  void fbsub(T* x, const T* b, T* c = nullptr) const;
  void fbsubt(T* x) const;
public: // lu subroutines
  T subdot(int i, int j, int dd, T const& in) const;
  T subdot(T* l, int j, T* u, int i, int dd, T const& in) const;
}; // BSSMARIX
/*--------------------------------------------------------------------------*/
template<class T>
class CBS : public BSMATRIX_SOLVER<double> {
  BSMATRIX_DATA<T>& _aa;
  BSSMATRIX<T> _lu;
  FOOTPRINT _fp;
  T _min_pivot;
  mutable unsigned* _changed{nullptr};// flag: this node changed value

  int*	_lownode_row{nullptr};	// lowest node in this row
  int*	_lownode_col{nullptr};	// lowest node in this col
  int* _idx{nullptr};
  int* _didx{nullptr};

  int _nzcount{0};
public:
  explicit CBS(BSMATRIX<double>& aa)
   :BSMATRIX_SOLVER(aa)
   ,_aa(BSMATRIX_SOLVER<T>::rw_data_(aa))
   ,_lu(aa.size())
   ,_fp(aa.size())
   ,_min_pivot(0.)
   ,_changed(nullptr) {
  }
private: // SOLVER
  void iwant_point(int i, int j)override {
    if(i && j){
      _fp.iwant_point(i, j);
    }else{
    }
  }
  void iwant_quad(int i, int j)override {
    if(i && j){
      _fp.iwant_point(i, j);
      _fp.iwant_point(j, i);
    }else{
    }
  }
  void iwant_inode(int i, int j)override {
    if(i && j){
      _fp.iwant_inode(i, j);
    }else if(j){
      _fp.iwant_inode(i, j);
    }else{
    }
  }
  void allocate()override;
  void check_consistency(int m);
  void unallocate()override;

private: // CBS
  void init(int ss) override;
  void uninit()override;
  void set_min_pivot(double x)override {_min_pivot = x;}
//  void zero() override;
  void lu_decomp(bool do_partial) override;
//  void lu_decomp() override{ untested();unreachable();}
  void load_diagonal_point(int i, T value)override;
  void load_point(int i, int j, T value)override;
  void load_couple(int i, int j, T value)override;
  void load_symmetric(int i, int j, T value)override;
  void load_asymmetric(int r1, int r2, int c1, int c2, T value)override;

  void set_changed(int, bool)const override{ untested();
    unreachable(); // obsolete
  }
  void set_changed(int i, int j)const {

    assert(i>=0);
    assert(j>=0);
    if(i==0 || j==0){
      // reset.
      _changed[j] = i;
      _changed[i] = j;
    }else if(i<j){
      // upper
      _changed[j] = std::min(unsigned(i-1), _changed[j]-1)+1;
      _changed[i] = std::min(unsigned(i-1), _changed[i]-1)+1; // diag
    }else if(i>j){
      // lower
      _changed[i] = std::min(unsigned(j-1), _changed[i]-1)+1;
      _changed[j] = std::min(unsigned(j-1), _changed[j]-1)+1; // diag
    }else if(i==j){
      _changed[i] = std::min(unsigned(j-1), _changed[i]-1)+1;
    }else{ untested();
    }

    assert(i==0 || int(_changed[i]) <= i );
    assert(j==0 || int(_changed[j]) <= j );

  }

private:
  void lu_iwant(int i, int j);

private: // aa data xs
//  BSMATRIX<T>& aa() { untested();return data_();}
 // int aalownode(int i) const{ return lownode_(data(), i); }
  int aalownode_u(int i) const{ return ulownode_(data(), i); }
  int aalownode_l(int i) const{ return llownode_(data(), i); }

  T const & aau(int i, int j)const { untested(); return u_(data(), i, j); }
  T const & aal(int i, int j)const { untested(); return l_(data(), i, j); }
  T const & aad(int i       )const { untested(); return d_(data(), i); }

  T& aam(int i, int j){ return BSMATRIX_SOLVER<T>::m_(_aa, i, j); }
  T& aal(int i, int j){ return BSMATRIX_SOLVER<T>::l_(_aa, i, j); }
  T& aau(int i, int j){ return BSMATRIX_SOLVER<T>::u_(_aa, i, j); }
  T& aad(int i       ){ return BSMATRIX_SOLVER<T>::d_(_aa, i); }

private: // lu data xs
  // T& m(int r, int c) { untested(); return _lu.m(r, c); }

private:
  unsigned is_changed(int n)const {
    return _changed[n];
  }

private: // implementation
  void fbsub(T* v) const override{itested();
    return _lu.fbsub(v, v, v);
  }
  void fbsub(T* x, const T* b, T* c = nullptr) const override {
    return _lu.fbsub(x, b, c);
  }
  void fbsubt(T*)const override { untested(); incomplete(); }

private: // memory
  void tag_wanted();
  bool nonzero_lu(int r, int c, int d);
  void compute_lu_fill();
  //  void compute_lu_fill2();
  void want_lu_fill();
  void lu_set_tags(int m);
  void dump_lu();

private:
  T lu_tag() const{
    return -1.;
  }
  union one{
    one(){
      t = 0.;
      x[0] = 1;
    }
    T t;
    char x[sizeof(T)];

    operator T() const{return t;}
  };
  // T const& one() const;
  bool is_one(T const&) const;
  bool nz(T const& t) const{ return bool(t); }
  void propagate(int m);

public:
  bool is_inode(int i)const { return fp().is_inode(i); }
  int fpsize()const { return fp().size() + size() - ninode(); }
  int fpnreq()const { return fp().nreq(); }
  int nload()const { return fp().nstamp() + size() - ninode(); }
  int nnz()const { return _nzcount; }
  int ninode()const { return fp().ninode(); }
  FOOTPRINT const& fp()const {return _fp;}
  bool is_req(int r, int c)const {
    return fp().is_req(r, c);
  }
private:
  FOOTPRINT& fp() {return _fp;}
}; // CBS
/*--------------------------------------------------------------------------*/
template<class T>
void CBS<T>::init(int ss)
{
  _lu.init(ss);
  _fp.init(ss);
  _changed = new unsigned[size()+1];
  assert(_changed);
  std::fill_n(_changed, size()+1, 0);
//  assert(_zero == 0.);

  assert(!_lownode_col);
  assert(!_lownode_row);

  _lownode_row = new int[size()+1];
  _lownode_col = new int[size()+1];
  assert(_lownode_row);
  assert(_lownode_col);
  for (int ii = 0;  ii <= size();  ++ii) {
    _lownode_row[ii] = ii;
    _lownode_col[ii] = ii;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::allocate()
{
  fp().allocate();
  tag_wanted();
  compute_lu_fill();
  want_lu_fill();
  _lu.allocate();

  for (int m=1; m<=size(); ++m) {
    lu_set_tags(m);
    check_consistency(m);
  }

  serialize_bs_adj(fp().cols(), fp().rows(), _idx, _didx);

  _aa.zero();
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::unallocate()
{
  _lu.unallocate();
  fp().unallocate();
}
/*--------------------------------------------------------------------------*/
/* u: as above, but lvalue */
template <class T>
T& BSSMATRIX<T>::u(int r, int c)
{ untested();
  assert(_colptr);
  assert(_lownode_col);
  assert(0 < r);
  assert(r <= c); // !
  assert(c <= _size);
  assert(0 < _lownode_col[c]);
  assert(_lownode_col[c] <= r);

  return _colptr[c][r];
}
/*--------------------------------------------------------------------------*/
/* l: as above, but lvalue */
template <class T>
T& BSSMATRIX<T>::l(int r, int c)
{ untested();
  assert(_lownode_row);
  assert(0 < c);
  assert(c <= r); // !
  assert(r <= _size);
  assert(0 < _lownode_row[r]);
  assert(_lownode_row[r] <= c);

  return rowptr(r)[-c];
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::uninit()
{
  unallocate();

  _fp.uninit();
  _lu.uninit();

  delete [] _changed;
  _changed = nullptr;
  delete [] _lownode_row;
  _lownode_row = nullptr;
  delete [] _lownode_col;
  _lownode_col = nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
void BSSMATRIX<T>::uninit()
{
  unallocate();
  delete [] _lownode_row;
  _lownode_row = nullptr;
  delete [] _lownode_col;
  _lownode_col = nullptr;
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSSMATRIX<T>::init(int ss)
{
  assert(!_lownode_col);
  assert(!_lownode_row);
  assert(!_colptr);
  assert(!_diaptr);
  assert(!_space);

  assert(_zero == 0.);
  // _min_pivot =
  _trash = 0.;
  _memsize = 0;
  _size = ss;
  _lownode_row = new int[size()+1];
  _lownode_col = new int[size()+1];
  assert(_lownode_row);
  assert(_lownode_col);
  for (int ii = 0;  ii <= size();  ++ii) {
    _lownode_row[ii] = ii;
    _lownode_col[ii] = ii;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class T>
union idx_ {
  idx_(const T& t) : _t(t){ }
  idx_(int i = 1) {
   x[0] = i;
   x[1] = -1;
  }
  int value(){
    if(x[1] == -1){
      return x[0];
    }else{
      return 0;
    }
  }
  T _t;
  int x[sizeof(T)/sizeof(int)];
};
/*--------------------------------------------------------------------------*/
template <class T>
int idx(T* const&t)
{
#ifndef NDEBUG
  T a = 0.; (void) a;
#endif
  return idx_<T>(*t).value();
}
template <class T>
int idx(T const&t)
{
#ifndef NDEBUG
  T a; a = 0.; (void) a;
#endif
  return idx_<T>(t).value();
}
/*--------------------------------------------------------------------------*/
template <class T>
T /* const& */ makeidx(int const& i)
{
  assert(i);
  return idx_<T>(i)._t;
}
/*--------------------------------------------------------------------------*/
struct index{
  index(int const& i) : _i(i) {
  }
  template<class T>
  operator T() const{
    return makeidx<T>(_i);
  }

  int _i;
};
/*--------------------------------------------------------------------------*/
template<class T, int i>
class int_ {
  T& _t;
  union bits {
    bits(T const& t) : _t(t) {}
    T _t;
    int _x[sizeof(T)/sizeof(int)];
  };
public:
  int_(T& t) : _t(t){ }

  int_& operator=(int const& x) {
    bits t(_t);
    t._x[i] = x;
    _t = t._t;
    // t._x[i];
    return *this;
  }

  //int& row(){ untested();return x[0];}
  //int& col(){ untested();return x[1];}
};
/*--------------------------------------------------------------------------*/
template <class T>
int_<T,0> row(T&t)
{
  return int_<T,0>(t);
}
/*--------------------------------------------------------------------------*/
template <class T>
int_<T,1> col(T&t)
{
  return int_<T,1>(t);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template <class T>
bool CBS<T>::nonzero_lu(int rr, int cc, int dd)
{
  assert(!idx(aam(rr, cc)));
  assert(!nz(aam(rr, cc)));
  int kk = std::max(aalownode_l(rr), aalownode_u(cc));
  int len = dd - kk;
  if (len > 0) {
    T* row = &(aal(rr,kk));
    T* col = &(aau(kk,cc));
    for (int ii = len-1;   ii >= 0; ) {
      int m = std::max(idx(col[ii]), idx(row[-ii]));
      if(m){
	ii -= m;
      }else if(nz(col[ii]) && nz(row[-ii])){
	return true;
      }else{
	--ii;
      }
    }
  }else{
  }
  return false;
}
/*--------------------------------------------------------------------------*/
template<class T>
int skip(T*&t, int& i, int dir=1)
{
  if(idx(t)){
    i += idx(t);
    t += dir;
    assert(!idx(t));
  }else{
  }
  return i;
}
/*--------------------------------------------------------------------------*/
template<class T>
int piks(T*&t, int& i, int dir=1)
{
  if(idx(t)){
    i -= idx(t);
    t -= dir;
    // assert(!idx(t));
  }else{
  }
  return i;
}
/*--------------------------------------------------------------------------*/
template <class T>
T BSSMATRIX<T>::subdot(T* lj, int i, T* ui, int j, int dd, T const& in) const
{
  typename longer<T>::type dot = in;
  assert(!std::isnan(dot));
  assert(!std::isinf(dot));

  T const* last_u = colptr(j);
  T const* last_l = rowptr(i);

  int ll = dd-1;
  ++lj;

  int uu = dd-1;
  --ui;

  if(last_l<lj){
    assert(dot == subdot(i,j,dd,in));
    return T(dot);
  }else if(ui<last_u){ untested();
    assert(dot == subdot(i,j,dd,in));
    return T(dot);
  }

  piks(lj, ll, -1);
  piks(ui, uu);

  if(last_l<lj){
    assert(dot == subdot(i,j,dd,in));
    return T(dot);
  }else if(ui<last_u){
    assert(dot == subdot(i,j,dd,in));
    return T(dot);
  }

  for( ; last_l>=lj && ui>=last_u ;){
    if(uu == ll){
      assert(!idx(ui));
      assert(!idx(lj));
      assert(!std::isnan(*ui));
      assert(!std::isnan(*lj));

      dot -= *ui * *lj;
      --uu;
      --ui;
      --ll;
      ++lj;
      piks(ui, uu);
      piks(lj, ll, -1);
      assert(!std::isnan(dot));
    }else if(uu>ll){
      --uu;
      --ui;
      piks(ui, uu);
    }else{
      --ll;
      ++lj;
      piks(lj, ll, -1);
    }
  }

  assert(!std::isnan(dot));
  return T(dot);
}
/*--------------------------------------------------------------------------*/
template <class T>
T BSSMATRIX<T>::subdot(int i, int j, int dd, T const& in) const
{
  // dot = m(i, j);
  T dot = in;
  assert(!std::isnan(dot));

  T const* u = colptr(j);
  T const* dj = _diaptr[j];

  T const* l = rowptr(i);
  T const* di = _diaptr[i];

  assert(u<=dj && di<=l);

  assert(!idx(di));
  assert(!idx(dj));

  int ll = 1;
  int uu = 1;
  skip(u, uu);
  skip(l, ll, -1);
  for( ; uu<dd && ll<dd ;){ untested();
    if(uu == ll){ untested();
      assert( u<dj && di<l);
      assert(!idx(u));
      assert(!idx(l));
      assert(!std::isnan(*u));
      assert(!std::isnan(*l));

      dot -= *u * *l;
      ++uu;
      ++u;
      ++ll;
      --l;
      skip(u, uu);
      skip(l, ll, -1);
      assert(!std::isnan(dot));
    }else if(uu<ll){ untested();
      ++uu;
      ++u;
      skip(u, uu);
    }else{ untested();
      ++ll;
      --l;
      skip(l, ll, -1);
    }
  }

  assert(!std::isnan(dot));
  return dot;
}
/*--------------------------------------------------------------------------*/
// public implementations
/*--------------------------------------------------------------------------*/
template <class T>
BSSMATRIX<T>::BSSMATRIX(int s)
  :_lownode_row(nullptr),
   _lownode_col(nullptr),
   _space(nullptr),
   _colptr(nullptr),
   _diaptr(nullptr),
   _memsize(0),
   _size(0),
   _zero(0.),
   _trash(0.)
{
  init(s);
}
/*--------------------------------------------------------------------------*/
/* iwant: indicate that "iwant" to allocate this spot in the matrix
 */
template <class T>
void BSSMATRIX<T>::iwant(int i, int j)
{
  assert(_lownode_row);
  assert(_lownode_col);
  assert(i <= size());
  assert(j <= size());
  assert(i);
  assert(j);

  if (i <= 0  ||  j <= 0) { untested();
    // ...
  }else if (i < _lownode_col[j]) {
    _lownode_col[j] = i;
  }else if (j < _lownode_row[i]) {
    _lownode_row[i] = j;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::lu_iwant(int i, int j)
{
  _lu.iwant(i, j);
  assert(_lownode_row);
  assert(_lownode_col);
  assert(i <= size());
  assert(j <= size());
  assert(i);
  assert(j);

  if (i <= 0  ||  j <= 0) { untested();
    // start tags?
  }else if (i < _lownode_col[j]) {
    _lownode_col[j] = i;
  }else if (j < _lownode_row[i]) {
    _lownode_row[i] = j;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void BSSMATRIX<T>::unallocate()
{
  assert (_zero == 0.);
  delete [] _colptr;
  _colptr = nullptr;
  delete [] _diaptr;
  _diaptr = nullptr;
  delete [] _space;
  _space = nullptr;
}
/*--------------------------------------------------------------------------*/
/* allocate: really get the space to work
 */
template <class T>
void BSSMATRIX<T>::allocate()
{
  assert(_lownode_row);
  assert(_lownode_col);
  assert(!_colptr);
  assert(!_diaptr);
  assert(!_space);

  _memsize = 1;
  for (int ii = 0;   ii <= size();   ++ii) {
    _memsize += 1; // diag.
    _memsize += ii - _lownode_col[ii];
    _memsize += ii - _lownode_row[ii];
  }

  error(bNOERROR,"BSS allocate memsize " + to_string(_memsize) + "\n");

  _colptr = new T*[size()+2];
  _diaptr = new T*[size()+1];
  _space  = new T[_memsize];

  assert(_colptr);
  assert(_diaptr);
  assert(_space);

  std::fill_n(_space, _memsize, 0.);

  {
    _colptr[0] = _space;
    for (int ii = 0;   ii <= size();   ++ii) {
      _diaptr[ii] = _colptr[ii] + ii - _lownode_col[ii];
      _colptr[ii+1] = _diaptr[ii] + 1;
      _colptr[ii+1] += ii - _lownode_row[ii];
    }
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::tag_wanted()
{
  CBS<T>::_data.zero();
  int nz = 0;

  _fp.sort();
  int mm = 1;
  for(auto const& d : _fp.rows()) {
    T* prev = &aad(mm);
    nz += int(d.size());
    for(auto i : d){
      assert(mm<i);
      col(*prev) = i;
      prev = &aau(mm, i);
    }
    col(*prev) = -1;
    ++mm;
  }

  mm = 1;
  for(auto const& d : fp().cols()) {
    T* prev = &aad(mm);
    nz += int(d.size());
    for(auto j : d){
      // lower(i, mm)
      assert(mm<j);
      row(*prev) = j;
      prev = &aal(j, mm);
    }
//    trace3("tag row", mm, prev, -1);
    row(*prev) = -1;
    ++mm;
  }
  error(bNOERROR, "wanted nz tagged " + to_string(nz) + "\n");
}
/*--------------------------------------------------------------------------*/
/* zero: wipe the whole array
 */
template <class T>
void BSSMATRIX<T>::zero()
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::propagate(int m)
{
  int diag = _didx[m-1];
  int const* it = &_idx[diag];
  while(*(--it)){
    set_changed(m, *it);
  }
  it = &_idx[diag];
  while(*it){
    set_changed(*it, m);
    ++it;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::lu_decomp(bool do_partial)
{
  int saved = 0;
  int dsaved = 0;
  int unch = 0;
  int ch = 0;
  int unn = 0;
  int use = 0;
  assert(_lu.size() == size());

   for (int mm=1; mm<=size(); ++mm) {
     if(is_changed(mm)){
       ++ch;
     }
   }
  for (int mm=1; mm<=size(); ++mm) {
    int ln = aalownode_u(mm);
    T* ui = _lu.colptr(mm);
    int ii = 1;
    skip(ui, ii);
    bool pp = false;

    if (!do_partial || is_changed(mm) ) {
      assert(!idx(ui));

      for (; ii<mm; ) {
	T* dd = _lu.diaptr(ii);
	assert(ii >= aalownode_u(mm));
	assert(!idx(ui));

	// assert(ui!=_lu.colptr(10));
	assert(!idx(ui));
	assert(!idx(aau(ii,mm)));

	assert(*dd == *dd);
	assert(*dd);
	/* *ui = u(ii,mm) = (aa.u(ii,mm) - dot(ii,mm,ii)) / d(ii,ii); */
	T* lj = _lu._diaptr[ii];

	if( ( ii > int(is_changed(mm)-1u) )|| !do_partial) {
	  T x = _lu.subdot(lj, ii, ui, mm, ii, aau(ii,mm)) / *dd;

	  if(*ui == x){
	    ++unn;
	  }else{
	    assert(is_changed(mm)|| !do_partial);
	    pp = true;
	    ++use;
	  }
	  *ui = x;
	}else{
	  ++saved;
	  // assert(!is_changed(ii));
	  assert(fabs(*ui - _lu.subdot(lj, ii, ui, mm, ii, aau(ii,mm)) / *dd) < 1e-5);
	}

	assert(!idx(ui));
	assert(!std::isnan(*ui));

	++ui;
	++ii;
	skip(ui, ii);
      }

      T* lj = _lu.rowptr(mm);
      int jj = 1;
      skip(lj, jj, -1);
      // first one? l(mm,bn) = aa.l(mm,bn);
      for (; jj<mm; ){
	assert(!idx(lj));
	/* l(mm,jj) = aa.l(mm,jj) - dot(mm,jj,jj); */
	T* uix = _lu._diaptr[jj];

	if( ( jj > int(is_changed(mm)-1u) )|| !do_partial) {
	  T l =  aal(mm,jj);
	  auto x = _lu.subdot(lj, mm, uix, jj, jj, aal(mm,jj));
	  if(x)
	    trace5("ludecomp L", mm, jj, l, x, ln);

	  if(*lj == x){
	    ++unn;
	  }else{
	    assert(is_changed(mm)|| !do_partial);
	    ++use;
	    pp = true;
	  }
	  *lj = x;
	}else{
	  ++saved;
	  // assert(!is_changed(jj));
	  assert(fabs ( *lj - _lu.subdot(lj, mm, uix, jj, jj, aal(mm,jj))) < 1e-5);
	}

	--lj;
	++jj;
	skip(lj, jj, -1);
      }

      { /* jj == mm */
	T* dd = _lu.diaptr(mm);
	/* d(mm,mm) = aa.d(mm,mm) - dot(mm,mm,mm); then test */
	T diag = _lu.subdot(dd,mm,dd,mm,mm,aad(mm));
	if(!do_partial){
	}else if(diag != *dd){
	  propagate(mm);
	}else if(pp){
	  propagate(mm);
	  ++dsaved;
	}else{
	}
	*dd = diag;
        if(fabs(*dd) < _min_pivot) {itested();
	  error(bWARNING, "open circuit: internal node %u\n", mm);
	  *dd = _min_pivot;
	}else{
	}
      }
    }else if(!is_changed(mm)){
      ++unch;
    }
  }

  for (int mm=1; mm<=size(); ++mm) {
    set_changed(mm, 0);
    assert(!is_changed(mm));
  }

#ifdef ACCT
   if(do_partial){ untested();
   std::cerr << "ch " << ch <<  " use " << use << " ds " << dsaved
             << " unnec " << unn << " unch " << unch <<"\n";
   if(saved){ untested();
   std::cerr << "saved " << saved << "\n";
   }
   }
#endif
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::lu_set_tags(int mm)
{
  {
    int bn = aalownode_u(mm);
    int zcnt = 0;
    assert(_lownode_col[mm] >= _lu._lownode_col[mm]);
    assert(_lownode_col[mm] <= _lu._lownode_col[mm]); // for now.
    T* seek = _lu.diaptr(mm)-1; // - _lownode_col[mm] + _lu.lownode_col(mm);
    for (int ii=mm-1; ii>=bn; --ii) {
      if(idx(aau(ii,mm))){
	++zcnt;
      }else if(nz(aau(ii,mm))){
	if(zcnt){
	  *seek = makeidx<T>(zcnt);
	  --seek;
	}else{
	}
	--seek;
	zcnt = 0;
      }else{
	++zcnt;
      }
    }

    if(zcnt){ untested();
      *seek = makeidx<T>(zcnt); // oops? + gap?
    }else if(bn==1){
    }else if(bn==mm){
      assert(seek == _lu.diaptr(mm)-1); // - _lownode_col[mm] + _lu.lownode_col(mm);
      *seek = makeidx<T>(mm-1);
    }else{
      assert(zcnt + bn - 1);
      assert(seek == _lu.colptr(mm));
      *seek = makeidx<T>(zcnt + bn - 1);
    }

    //trace2("lu_set_tags scan === L ", mm, bn);
    bn = aalownode_l(mm);
    zcnt = 0;
    assert(_lownode_row[mm] >= _lu.lownode_row(mm));
    assert(_lownode_row[mm] <= _lu.lownode_row(mm)); // for now.
    seek = _lu.diaptr(mm) + 1;
    for (int jj=mm-1; jj>=bn; --jj) {
      if(idx(aal(mm,jj))){
	++zcnt;
      }else if(nz(aal(mm,jj))){
	if(zcnt){
	  *seek = index(zcnt);
	  ++seek;
	}else{
	}
	++seek;
	zcnt = 0;
      }else{ untested();
	++zcnt;
      }
    }
    if(zcnt){ untested();
      *seek = makeidx<T>(zcnt); // oops? + gap?
    }else if(bn==1){
    }else if(bn==mm){
      assert(seek == _lu.diaptr(mm)+1); // - _lownode_col[mm] + _lu.lownode_col(mm);
      *seek = makeidx<T>(mm-1);
    }else{
      *seek = makeidx<T>(zcnt + bn - 1);
    }

    check_consistency(mm);
  }
} // lu_set_tags
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::dump_lu()
{ untested();
  trace1("========= DUMP D", size());
  for (int mm=1; mm <= size(); ++mm) { untested();
    T* dd = _lu.diaptr(mm);
    trace2("D", mm, *dd);
  }
  trace1("========= DUMP U", size());
  for (int mm=1; mm <= size(); ++mm) { untested();
    T* ui = _lu.colptr(mm);
    int ii = 1;
    if(idx(ui)){ untested();
      ii += idx(ui++);
    }else{ untested();
    }

    for (; ii<mm; ){ untested();
      trace3("U", ii, mm, *ui);
      ++ui;
      ++ii;
      if(idx(ui)){ untested();
	ii += idx(ui++);
      }else{ untested();
      }
    }
  }
  trace1("========= DUMP L", size());
  for (int mm=1; mm <= size(); ++mm) { untested();
    T* lj = _lu.rowptr(mm);
    int jj = 1;
    if(idx(lj)) { untested();
      jj += idx(lj--);
    }else{ untested();
    }
      // first one? l(mm,bn) = aa.l(mm,bn);
    for (; jj<mm; ){ untested();
      trace3("L", mm, jj, *lj);
      --lj;
      ++jj;
      if(idx(lj)){ untested();
	jj += idx(lj--);
      }else{ untested();
      }
    }

  }
}
/*--------------------------------------------------------------------------*/
// TODO: faster scan (skip idx)
template <class T>
void CBS<T>::want_lu_fill()
{
  _nzcount = 0;
  for (int mm = 1;   mm <= size();   ++mm) {
    int bn = aalownode_u(mm);
    bool zeros = false;
    int nzcount = 0;
    for (int ii=mm-1; ii>=bn; --ii) {
      if(idx(aau(ii,mm))){
	zeros = true;
      }else if(nz(aau(ii,mm))){
	nzcount += 1 + zeros;
	zeros = false;
      }else{
	zeros = true;
      }
    }
    if(nzcount==mm-1){
    }else if(bn==1 && !idx(aau(1,mm)) && nz(aau(1,mm))){
    }else{
      ++nzcount;
    }

    lu_iwant(mm-nzcount,mm);

    bn = aalownode_l(mm);
    zeros = false;
    _nzcount += nzcount;
    nzcount = 0;
    for (int jj=mm-1; jj>=bn; --jj) {
      if(idx(aal(mm,jj))){
	zeros = true;
      }else if(nz(aal(mm,jj))){
	nzcount += 1 + zeros;
	zeros = false;
      }else{ untested();
	zeros = true;
      }
    }
    if(mm-nzcount == 1){
    }else if(bn==1 && !idx(aal(mm,1)) && nz(aal(mm,1))){
    }else{
      ++nzcount;
    }
    lu_iwant(mm, mm-nzcount);
    _nzcount += nzcount;
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::compute_lu_fill()
{
  assert(nz(one()));
  int f = 0;
  int z = 0;
  int lnz = 0;
  int lnf = 0;
  int lnzsum = 0;
  int gap = 0;
  for (int mm = 1;   mm <= size();   ++mm) {
    int bn = std::max(aalownode_u(mm), aalownode_l(mm));
    assert (bn <= mm);
    if (bn < mm) {
      if(nz(aad(bn))){
	++f;
      }else{ untested();
      }
    }else{
    }

    bn = aalownode_u(mm);
    if (bn < mm) {
      /// ============= U ================
      gap = 0;
      assert(!idx(aau(bn,mm)));

      if(nz(aau(bn,mm))){
	++f;
      }else{ untested();
	aau(bn,mm) = index(++gap);
	assert(!nz(aau(bn,mm)));
	++z;
      }
      for (int ii =bn+1;  ii<mm;  ii++) {
	/* (m(ii,mm) -= dot(ii,mm,ii)) /= d(ii,ii); */
	T& dot = aau(ii,mm);
	assert(!idx(dot));
	if(nz(dot)){
	  ++f;
	  ++lnf;
	  gap = 0;
	}else if(nonzero_lu(ii,mm,ii)){
	  ++lnz;
	  dot = lu_tag();
	  assert(ii<mm);
	  fp().rows(ii-1).insert(mm);
	  ++f;
	  gap = 0;
	}else{
	  dot = index(++gap);
	  ++z;
	}
      }
    }else{
    }
    
    bn = aalownode_l(mm);
    if (bn < mm) {
      /// ============= L ================
      gap=0;
      if(nz(aal(mm, bn))){
	++f;
      }else{ untested();
	aal(mm, bn) = index(++gap);
	++z;
      }
      for (int jj = bn+1;  jj<mm;  jj++) {
	/* m(mm,jj) -= dot(mm,jj,jj); */
	T& dot = aal(mm,jj);
	assert(!idx(dot));
	if(nz(dot)){
	  ++f;
	  ++lnf;
	  gap = 0;
	}else if(nonzero_lu(mm,jj,jj)){
	  dot = lu_tag();
	  ++lnz;
	  fp().cols(jj-1).insert(mm);
	  ++f;
	  gap = 0;
	}else{
	  dot = index(++gap);
	  ++z;
	}
      }
    }else{    /* bn == mm */
      assert (bn == mm);
    }
    aad(mm) = lu_tag();

    error(bTRACE, "FILL m " + to_string(mm) + " lnz " + to_string(lnz) + " lnf " + to_string(lnf) + "\n");
    lnzsum += lnz;
    lnz = lnf = 0;
  }

  error(bLOG, "FILL lu nz " + to_string(f) + " z " + to_string(z) + " lu new " + to_string(lnzsum) + "\n");
}
/*--------------------------------------------------------------------------*/
/* fbsub: forward and back sub, separate storage
 * b = right side vector
 * c = intermediate vector after fwd sub
 * x = solution vector
 */
template <class T>
void BSSMATRIX<T>::fbsub(T* x, const T* b, T* c) const
{
  trace0("FBSUB=broken======================");
    for ( int ii=0  ; ii <= size(); ++ii) {
      trace2("bb", ii, b[ii]);
    }
  assert(_lownode_row);
  assert(_lownode_col);
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
    ii = 1;

    for (   ; ii <= size(); ++ii) {		/* forward substitution */
      T const* lj = rowptr(ii);
      int jj = 1;
      if(idx(lj)){
	jj+=idx(lj--);
      }else{
      }
      assert(jj);
      int low_node = jj; // std::max(jj, first_nz);
      assert(!idx(lj));
      while(jj<low_node){ untested();
	++jj;
	if(idx(--lj)){ untested();
	  jj += idx(lj--);
	}else{ untested();
	}
      }

      c[ii] = b[ii];
      // for (int jj = low_node; jj < ii; ++jj)
      for (; jj<ii; ) {
	if(std::isnan(c[jj])){itested();
	}else{itested();
	}
	assert(!idx(lj));
	assert(!std::isnan(*lj));
	T l__ = *lj;
	if(c[jj] && l__)
	  trace5("fw", ii, jj, c[ii], c[jj], l__);
	c[ii] -= *lj * c[jj];
	++jj;
	if(idx(--lj)){
	  jj += idx(lj--);
	}else{
	}
      }
      c[ii] /= d(ii);
    }
  }

  notstd::copy_n(c, size()+1, x);

  for (int jj = size(); jj > 1; --jj) {		/* back substitution    */
    T const* ui = colptr(jj);
    int ii = 1;
    skip(ui, ii);
    assert(!idx(ui));

    // for (int ii = _lownode[jj]; ii < jj; ++ii)
    for (; ii<jj; ) {
      // x[ii] -= u(ii,jj) * x[jj];
      assert(!idx(ui));
      if(std::isnan(x[jj])){itested();
      }else{itested();
      }
      assert(!std::isnan(*ui));
      x[ii] -= *ui * x[jj];
      if(*ui)
      trace5("bs", ii, jj, x[ii], *ui, x[jj]);
      ++ii;
      ++ui;
      skip(ui, ii);
    }
  }
  x[0] = 0.;
}
/*--------------------------------------------------------------------------*/
/* fbsubt: forward and back substitution with implicitly transposed matrix Ut Lt x = v
 * v = right side vector, changed in place to solution vector
 */
template <class T>
void BSSMATRIX<T>::fbsubt(T*) const
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::check_consistency(int m)
{
  (void) m;
#ifndef NDEBUG
  {
    T* u = _lu.colptr(m);
    T* l = _lu.rowptr(m);

    assert(aalownode_u(m));
    if(aalownode_u(m)!=1){
      assert(idx(u));
      trace3("dbg", idx(u), aalownode_u(m), m);
      assert(idx(u)+1 == aalownode_u(m));
    }else{
      trace3("dbg", idx(u), aalownode_u(m), m);
      assert(!idx(u));
    }

    int i = 1;
    skip(u, i);
    trace2("skip?", idx(u), i);

    for(; u!=_lu.diaptr(m);){
      assert(!idx(u));
      assert(nz(aam(i, m)));
      ++i;
      ++u;
      int ii = i;
      skip(u, i);
      trace2("skip2", idx(u), i);

      for(;ii<i; ++ii){
	trace2("idx?", ii, m);
	assert(idx(aam(ii, m)));
      }

      if (i>=m){
	assert(u==_lu.diaptr(m));
	break;
      }
      assert(u!=_lu.diaptr(m));
    }
    assert(i==m);
    assert(u==_lu.diaptr(m));

    if(aalownode_l(m)!=1){
      assert(idx(*l));
      assert(idx(l));
    }else{
    }

    i = 1;
    for(; l!=_lu.diaptr(m);){

      if(idx(*l)){
	i+=idx(*l);
      }else{
	++i;
      }
      --l;
    }
    assert(i==m);
  }
#endif
}
/*--------------------------------------------------------------------------*/
template <class T>
void CBS<T>::load_point(int i, int j, T value)
{ untested();
  if (i > 0 && j > 0) { untested();
    set_changed(i, j);
    aam(i,j) += value;
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value);
template <class T>
void CBS<T>::load_diagonal_point(int i, T value)
{
  if (i > 0) {
    set_changed(i, i);
    aad(i) += value;
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void CBS<T>::load_couple(int i, int j, T value)
{itested();
  if (j > 0) {itested();
    if (i > 0) {itested();
      set_changed(i, j);
      aam(i,j) -= value;
      set_changed(j, i);
      aam(j,i) -= value;
    }else{itested();
    }
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value); or load_diagonal_point(i, value);
// load_point(j, j, value); or load_diagonal_point(j, value);
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void CBS<T>::load_symmetric(int i, int j, T value)
{
  if (j > 0) {
    set_changed(j, j);
    aad(j) += value;
    if (i > 0) {
      set_changed(i,i);
      set_changed(i,j);
      set_changed(j,i);
      aad(i) += value;
      aam(i,j) -= value;
      aam(j,i) -= value;
    }else{
    }
  }else if (i > 0) {
    set_changed(i,i);
    aad(i) += value;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
// load_point(r1, c1, value);
// load_point(r2, c2, value);
// load_point(r1, c2, -value);
// load_point(r2, c1, -value);
template <class T>
void CBS<T>::load_asymmetric(int r1,int r2,int c1,int c2,T value)
{
  if (r1 > 0) {
    if (c1 > 0) {
      aam(r1,c1) += value;
      set_changed(r1, c1);
    }else{itested();
    }
    if (c2 > 0) {
      aam(r1,c2) -= value;
      set_changed(r1, c2);
    }else{
    }
  }else{itested();
  }
  if (r2 > 0) {
    if (c1 > 0) {
      set_changed(r2, c1);
      aam(r2,c1) -= value;
    }else{itested();
    }
    if (c2 > 0) {
      set_changed(r2, c2);
      aam(r2,c2) += value;
    }else{itested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
CBS<double>* m;
struct set{
  set(){
    m = new CBS<double>(CKT_BASE::_sim->_aa);
    CKT_BASE::_sim->_aa.set_solver(m);
  }
  ~set(){
    CKT_BASE::_sim->_aa.set_solver(nullptr);
  }
} s;
/*--------------------------------------------------------------------------*/
class CMD_STAT : public CMD {
public:
  void print(OMSTREAM, const CARD_LIST*);
  void do_it(CS&, CARD_LIST* )override {
    OMSTREAM o = IO::mstdout;
    assert(m);
    int iwant_points = m->fpnreq();
    int load_size    = m->nload();
    int footprint    = m->fpsize();
    int nnz          = m->nnz();
    int fill_size    = m->fpsize() - m->nload();
    double denom = m->size() * m->size();

    oo(o, "iwant points", iwant_points, denom);
    oo(o, "load size   ", load_size,    denom);
    oo(o, "footprint   ", footprint,    denom);
    oo(o, "nnz         ", nnz,          denom);
    oo(o, "fill size   ", fill_size,    denom);
  }
private:
  void oo(OMSTREAM& o, char const* label, int n, double denom)const
  {
    o.form("%s: %5.1f%% (%d)\n", label, 100.*n/denom, n);
  }
} p5;
DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "cbsstat", &p5);
/*--------------------------------------------------------------------------*/
class CMD_DUMP : public CMD {
public:
  void print(OMSTREAM, const CARD_LIST*);
  void do_it(CS& cmd, CARD_LIST* )override {
    OMSTREAM o = IO::mstdout;
    int max = INT_MAX;
    if(cmd.more()){
      cmd >> max;
    }else{
    }
    assert(m);
    CBS<double> const* cm = m;
    int i=0;
    for(auto p : cm->fp().rows()) {
      if(i>=max){
	break;
      }else{
      }
      int k=0;
      while(k<i){
	if(!cm->fp().cols()[k].count(i+1)){
	  o << ".";
	}else if(cm->is_req(i, k)) {
	  o << "*";
	}else{
	  o << "o";
	}
	++k;
      }
      if(m->is_inode(i)){
	o << " ";
      }else{
	o << "\\";
      }
      ++k;
      if(k>max){
	k = INT_MAX;
      }else{
      }
      ++k;
      ++i;
      for(auto q : p) {
	while(k<q && k<=max){
	  o << ".";
	  ++k;
	}

	if(k>max){
	  break;
	}else if(cm->is_req(i-1, k-1)) {
	  o << '*';
	}else{
	  o << 'o';
	}
	++k;
      }

      while(k<=int(cm->fp().rows().size()) && k <= max){
	o<<".";
	++k;
      }
      o<<"\n";
    }
  }
} p6;
DISPATCHER<CMD>::INSTALL d6(&command_dispatcher, "cbsdump", &p6);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
