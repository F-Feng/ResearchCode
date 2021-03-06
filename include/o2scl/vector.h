/*
  -------------------------------------------------------------------
  
  Copyright (C) 2006-2014, Andrew W. Steiner
  
  This file is part of O2scl.
  
  O2scl is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  O2scl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with O2scl. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/
#ifndef O2SCL_VECTOR_H
#define O2SCL_VECTOR_H

/** \file vector.h
    \brief Assorted generic vector functions

    This file contains a set of template functions which can be
    applied to almost any vector or matrix type which allow element
    access through <tt>operator[]</tt>. Detailed requirements
    on the template parameters are given in the functions below. 

    For a general discussion of vectors and matrices in \o2, see the
    \ref vecmat_section of the User's Guide.

    For statistics operations not included here, see \ref vec_stats.h
    in the directory \c src/other . Also related are the matrix output
    functions, \ref o2scl::matrix_out(), which is defined in \ref
    columnify.h because they utilize the class \ref o2scl::columnify to
    format the output.

    For functions which search for a value in an ordered (either
    increasing or decreasing) vector, see the class \ref
    o2scl::search_vec .

    \future Create a matrix transpose copy function?
    \future Create matrix swap row and column functions
*/
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

#include <o2scl/misc.h>
#include <o2scl/uniform_grid.h>

namespace o2scl {

  /// \name Copying vectors and matrices
  //@{
  /** \brief Simple generic vector copy

      Copy \c src to \c dest, resizing \c dest if it is too small
      to hold <tt>src.size()</tt> elements.

      This function will work for any classes \c vec_t and
      \c vec2_t which have suitably defined <tt>operator[]</tt>,
      <tt>size()</tt>, and <tt>resize()</tt> methods.
  */
  template<class vec_t, class vec2_t> 
    void vector_copy(vec_t &src, vec2_t &dest) {
    size_t N=src.size();
    if (dest.size()<N) dest.resize(N);
    size_t i, m=N%4;
    for(i=0;i<m;i++) {
      dest[i]=src[i];
    }
    for(i=m;i+3<N;i+=4) {
      dest[i]=src[i];
      dest[i+1]=src[i+1];
      dest[i+2]=src[i+2];
      dest[i+3]=src[i+3];
    }
    return;
  }

  /** \brief Simple generic vector copy of the first N elements

      Copy the first \c N elements of \c src to \c dest.
      It is assumed that the memory allocation for \c dest
      has already been performed.

      This function will work for any class <tt>vec2_t</tt> which has
      an operator[] which returns a reference to the corresponding
      element and class <tt>vec_t</tt> with an operator[] which
      returns either a reference or the value of the corresponding
      element.
  */
  template<class vec_t, class vec2_t> 
    void vector_copy(size_t N, vec_t &src, vec2_t &dest) {
    size_t i, m=N%4;
    for(i=0;i<m;i++) {
      dest[i]=src[i];
    }
    for(i=m;i+3<N;i+=4) {
      dest[i]=src[i];
      dest[i+1]=src[i+1];
      dest[i+2]=src[i+2];
      dest[i+3]=src[i+3];
    }
    return;
  }

  /** \brief Simple generic matrix copy
      
      Copy \c src to \c dest, resizing \c dest if it is too small.
      
      This function will work for any classes \c mat_t and
      \c mat2_t which have suitably defined <tt>operator()</tt>,
      <tt>size()</tt>, and <tt>resize()</tt> methods.
  */
  template<class mat_t, class mat2_t> 
    void matrix_copy(mat_t &src, mat2_t &dest) {
    size_t m=src.size1();
    size_t n=src.size2();
    if (dest.size1()<m || dest.size2()<n) dest.resize(m,n);
    for(size_t i=0;i<m;i++) {
      for(size_t j=0;j<n;j++) {
	dest(i,j)=src(i,j);
      }
    }
  }

  /** \brief Simple generic matrix copy of the first \f$ (M,N) \f$ 
      matrix elements

      Copy the first <tt>(M,N)</tt> elements of \c src to \c dest. It
      is assumed that the memory allocation for \c dest has already
      been performed.

      This function will work for any class <tt>vec2_t</tt> which has
      an operator[][] which returns a reference to the corresponding
      element and class <tt>vec_t</tt> with an operator[][] which
      returns either a reference or the value of the corresponding
      element.
  */
  template<class mat_t, class mat2_t> 
    void matrix_copy(size_t M, size_t N, mat_t &src, mat2_t &dest) {
    for(size_t i=0;i<M;i++) {
      for(size_t j=0;j<N;j++) {
	dest(i,j)=src(i,j);
      }
    }
  }
  //@}

  /// \name Swapping parts of vectors and matrices
  //@{
  /** \brief Swap the first N elements of two vectors

      This function swaps the elements of \c v1 and \c v2, one element
      at a time.
   */
  template<class vec_t, class vec2_t, class data_t> 
    void vector_swap(size_t N, vec_t &v1, vec2_t &v2) {
    data_t temp;
    size_t i, m=N%4;
    for(i=0;i<m;i++) {
      temp=v1[i];
      v1[i]=v2[i];
      v2[i]=temp;
    }
    for(i=m;i+3<N;i+=4) {
      temp=v1[i];
      v1[i]=v2[i];
      v2[i]=temp;
      temp=v1[i+1];
      v1[i+1]=v2[i+1];
      v2[i+1]=temp;
      temp=v1[i+2];
      v1[i+2]=v2[i+2];
      v2[i+2]=temp;
      temp=v1[i+3];
      v1[i+3]=v2[i+3];
      v2[i+3]=temp;
    }
    return;
  }

  /** \brief Generic swap of of the first N elements of two
      double-precision vectors

      This function swaps the elements of \c v1 and \c v2, one element
      at a time.
   */
  template<class vec_t, class vec2_t>
    void vector_swap_double(size_t N, vec_t &v1, vec2_t &v2) {
    return vector_swap<vec_t,vec2_t,double>(N,v1,v2);
  }

  /** \brief Generic swap of two matrices
      
      This function swaps the elements of \c v1 and \c v2, one element
      at a time.
  */
  template<class mat_t, class mat2_t, class data_t> 
    void matrix_swap(size_t M, size_t N, mat_t &v1, mat2_t &v2) {
    data_t temp;
    for(size_t i=0;i<M;i++) {
      for(size_t j=0;j<N;j++) {
	temp=v1[i][j];
	v1[i][j]=v2[i][j];
	v2[i][j]=temp;
      }
    }
    return;
  }

  /** \brief Generic swap of two matrices
      
      This function swaps the elements of \c m1 and \c m2, one element
      at a time.
  */
  template<class mat_t, class mat2_t, class data_t> 
    void matrix_swap_double(size_t M, size_t N, mat_t &m1, mat2_t &m2) {
    return matrix_swap<mat_t,mat2_t,double>(M,N,m1,m2);
  }

  /** \brief Generic swap two elements in a vector

      This function swaps the element \c i and element \c j of vector
      \c v1. 
   */
  template<class vec_t, class data_t> 
    void vector_swap(vec_t &v, size_t i, size_t j) {
    data_t temp=v[i];
    v[i]=v[j];
    v[j]=temp;
    return;
  }
  
  /** \brief Generic swap two elements in a vector
      
      This function swaps the element \c i and element \c j of vector
      \c v1. 
  */
  template<class vec_t>
    void vector_swap_double(vec_t &v, size_t i, size_t j) {
    return vector_swap<vec_t,double>(v,i,j);
  }
  
  /** \brief Generic swap two elements in a matrix

      This function swaps the element <tt>(i1,j1)</tt> and 
      element <tt>(i2,j2)</tt> of matrix \c m1. 
  */
  template<class mat_t, class data_t> 
    void matrix_swap(mat_t &m, size_t i1, size_t j1, size_t i2, size_t j2) {
    data_t temp=m(i1,j1);
    m(i1,j1)=m(i2,j2);
    m(i2,j2)=temp;
    return;
  }
  
  /** \brief Generic swap two elements in a matrix
      
      This function swaps the element \c i and element \c j of matrix
      \c v1. 
  */
  template<class mat_t>
    void matrix_swap_double(mat_t &m, size_t i1, size_t j1, 
			    size_t i2, size_t j2) {
    return matrix_swap<mat_t,double>(m,i1,j1,i2,j2);
  }
  
  /** \brief Generic swap two columns in a matrix

      This function swaps the element <tt>(i1,j1)</tt> and 
      element <tt>(i2,j2)</tt> of matrix \c m1. 
  */
  template<class mat_t, class data_t> 
    void matrix_swap_cols(size_t M, mat_t &m, size_t j1, size_t j2) {
    data_t temp;
    for(size_t i=0;i<M;i++) {
      temp=m(i,j1);
      m(i,j1)=m(i,j2);
      m(i,j2)=temp;
    }
    return;
  }
  
  /** \brief Generic swap two elements in a matrix
      
      This function swaps the element \c i and element \c j of matrix
      \c v1. 
  */
  template<class mat_t>
    void matrix_swap_cols_double(size_t M, mat_t &m, size_t j1, size_t j2) {
    return matrix_swap_cols<mat_t,double>(M,m,j1,j2);
  }
  
  /** \brief Generic swap two columns in a matrix

      This function swaps the element <tt>(i1,j1)</tt> and 
      element <tt>(i2,j2)</tt> of matrix \c m1. 
  */
  template<class mat_t, class data_t> 
    void matrix_swap_rows(size_t N, mat_t &m, size_t i1, size_t i2) {
    data_t temp;
    for(size_t j=0;j<N;j++) {
      temp=m(i1,j);
      m(i1,j)=m(i2,j);
      m(i2,j)=temp;
    }
    return;
  }
  
  /** \brief Generic swap two elements in a matrix
      
      This function swaps the element \c i and element \c j of matrix
      \c v1. 
  */
  template<class mat_t>
    void matrix_swap_rows_double(size_t N, mat_t &m, size_t i1, size_t i2) {
    return matrix_swap_rows<mat_t,double>(N,m,i1,i2);
  }
  //@}

  /// \name Sorting vectors
  //@{
  /** \brief Provide a downheap() function for vector_sort()
  */
  template<class vec_t, class data_t>
    void sort_downheap(vec_t &data, size_t n, size_t k) {
    
    data_t v=data[k];
    
    while (k<=n/2) {
      size_t j=2*k;
      
      if (j<n && data[j] < data[j+1]) j++;
      if (!(v < data[j])) break;
      data[k]=data[j];
      k=j;
    }
    
    data[k]=v;
  }

  /** \brief Sort a vector (in increasing order)

      This is a generic sorting template function using a heapsort
      algorithm. It will work for any types \c data_t and \c vec_t for
      which
      - \c data_t has a non-const version of <tt>operator=</tt>
      - \c data_t has a less than operator to compare elements
      - <tt>vec_t::operator[]</tt> returns a non-const reference
      to an object of type \c data_t 
      
      In particular, it will work with the STL template class
      <tt>std::vector</tt>, and arrays and pointers of numeric,
      character, and string objects.

      For example,
      \code
      std::string list[3]={"dog","cat","fox"};
      vector_sort<std::string[3],std::string>(3,list);
      \endcode

      \note With this function template alone, the user cannot avoid
      explicitly specifying the template types for this function
      because there is no parameter of type \c data_t, and function
      templates cannot handle default template types. For this
      reason, the function template \ref o2scl::vector_sort_double() was
      also created which provides the convenience of not requiring
      the user to specify the vector template type.

      \note This sorting routine is not stable, i.e. equal elements
      have arbtrary final ordering

      \note If \c n is zero, this function will do nothing and will
      not call the error handler.

      This works similarly to the GSL function <tt>gsl_sort_vector()</tt>.
  */
  template<class vec_t, class data_t>
    void vector_sort(size_t n, vec_t &data) {
    
    size_t N;
    size_t k;
    
    if (n==0) return;
    
    N=n-1;
    k=N/2;
    k++;
    do {
      k--;
      sort_downheap<vec_t,data_t>(data,N,k);
    } while (k > 0);
    
    while (N > 0) {
      data_t tmp=data[0];
      data[0]=data[N];
      data[N]=tmp;
      N--;
      sort_downheap<vec_t,data_t>(data,N,0);
    }
    
    return;
  }

  /** \brief Provide a downheap() function for vector_sort_index()
   */
  template<class vec_t, class vec_size_t> 
    void sort_index_downheap(size_t N, const vec_t &data, vec_size_t &order,
			     size_t k) {

    const size_t pki = order[k];
    
    while (k <= N / 2) {
      size_t j = 2 * k;
      
      if (j < N && data[order[j]] < data[order[j + 1]]) {
	j++;
      }
      
      // [GSL] Avoid infinite loop if nan
      if (!(data[pki] < data[order[j]])) {
	break;
      }
      
      order[k] = order[j];
      
      k = j;
    }
    
    order[k] = pki;
    
    return;
  }

  /** \brief Create a permutation which sorts a vector (in increasing order)

      This function takes a vector \c data and arranges a list of
      indices in \c order, which give a sorted version of the vector.
      The value <tt>order[i]</tt> gives the index of entry in in \c
      data which corresponds to the <tt>i</tt>th value in the sorted
      vector. The vector \c data is unchanged by this function, and
      the initial values in \c order are ignored. Before calling this
      function, \c order must already be allocated as a vector of size
      \c n.

      For example, after calling this function, a sorted version the
      vector can be output with
      \code
      size_t n=5;
      double data[5]={3.1,4.1,5.9,2.6,3.5};
      permutation order(n);
      vector_sort_index(n,data,order);
      for(size_t i=0;i<n;i++) {
        cout << data[order[i]] << endl;
      }
      \endcode

      To create a permutation which stores as its <tt>i</tt>th element,
      the index of <tt>data[i]</tt> in the sorted vector, you can
      invert the permutation created by this function.

      This is a generic sorting template function. It will work for
      any types \c vec_t and \c vec_size_t for which
      - \c vec_t has an <tt>operator[]</tt>, and
      - \c vec_size_t has an <tt>operator[]</tt> which returns 
      a \c size_t .
      One possible type for \c vec_size_t is \ref o2scl::permutation.

      This works similarly to the GSL function <tt>gsl_sort_index()</tt>.
  */
  template<class vec_t, class vec_size_t> 
    void vector_sort_index(size_t n, const vec_t &data, vec_size_t &order) {
    size_t N;
    size_t i, k;
    
    if (n == 0) return;

    // [GSL] Set permutation to identity

    for (i = 0 ; i < n ; i++) {
      order[i] = i;
    }
    
    /* [GSL] We have n_data elements, last element is at 'n_data-1',
       first at '0' Set N to the last element number.
    */
    N = n - 1;
    
    k = N / 2;
    // [GSL] Compensate the first use of 'k--'
    k++;                          
    do {
      k--;
      sort_index_downheap<vec_t,vec_size_t>(N,data,order,k);
    } while (k > 0);
    
    while (N > 0) {
      
      // [GSL] First swap the elements 
      size_t tmp = order[0];
      order[0] = order[N];
      order[N] = tmp;
      
      // [GSL] Then process the heap
      N--;
      
      sort_index_downheap<vec_t,vec_size_t>(N,data,order,0);
    }

    return;
  }

  /** \brief Sort a vector of doubles (in increasing order)

      This function is just a wrapper for
      \code
      vector_sort<vec_t,double>(n,data);
      \endcode
      See the documentation of \ref o2scl::vector_sort() for more 
      details.
  */
  template<class vec_t>
    void vector_sort_double(size_t n, vec_t &data) {
    return vector_sort<vec_t,double>(n,data);
  }
  //@}
  
  /// \name Smallest or largest subset functions
  //@{
  /** \brief Find the k smallest entries of a vector

      Given a vector \c data of size \c n this sets the first \c k
      entries of the vector \c smallest to the k smallest entries from
      vector \c data in ascending order. The vector \c smallest must
      be allocated beforehand to hold at least \c k elements.

      This works similarly to the GSL function <tt>gsl_sort_smallest()</tt>.

      \note This \f$ {\cal O}(k N) \f$ algorithm is useful only when 
      \f$ k << N \f$.

      If \c k is zero, then this function does nothing and
      returns \ref o2scl::success .
   */
  template<class vec_t, class data_t>
    void vector_smallest(size_t n, vec_t &data, size_t k, vec_t &smallest) {
    if (k>n) {
      O2SCL_ERR2("Subset length greater than size in ",
		 "function vector_smallest().",exc_einval);
    }
    if (k==0 || n==0) {
      O2SCL_ERR2("Vector size zero or k zero in ",
		 "function vector_smallest().",exc_einval);
    }

    // Take the first element
    size_t j=1;
    data_t xbound=data[0];
    smallest[0]=xbound;

    // Examine the remaining elements
    for(size_t i=1;i<n;i++) {
      data_t xi=data[i];
      if (j<k) {
	j++;
      } else if (xi>=xbound) {
	continue;
      }
      size_t i1;
      for(i1=j-1;i1>0;i1--) {
	if (xi>smallest[i1-1]) break;
	smallest[i1]=smallest[i1-1];
      }
      smallest[i1]=xi;
      xbound=smallest[j-1];
    }
    return;
  }

  /** \brief Find the k largest entries of a vector

      Given a vector \c data of size \c n this sets the first \c k
      entries of the vector \c largest to the k largest entries from
      vector \c data in descending order. The vector \c largest must
      be allocated beforehand to hold at least \c k elements.

      This works similarly to the GSL function <tt>gsl_sort_largest()</tt>.

      \note This \f$ {\cal O}(k N) \f$ algorithm is useful only when 
      \f$ k << N \f$.

      If \c k is zero, then this function does nothing and
      returns \ref o2scl::success .
   */
  template<class vec_t, class data_t>
    void vector_largest(size_t n, vec_t &data, size_t k, vec_t &largest) {
    if (k>n) {
      O2SCL_ERR2("Subset length greater than size in ",
		 "function vector_largest().",exc_einval);
    }
    if (k==0 || n==0) {
      O2SCL_ERR2("Vector size zero or k zero in ",
		 "function vector_largest().",exc_einval);
    }

    // Take the first element
    size_t j=1;
    data_t xbound=data[0];
    largest[0]=xbound;

    // Examine the remaining elements
    for(size_t i=1;i<n;i++) {
      data_t xi=data[i];
      if (j<k) {
	j++;
      } else if (xi<=xbound) {
	continue;
      }
      size_t i1;
      for(i1=j-1;i1>0;i1--) {
	if (xi<largest[i1-1]) break;
	largest[i1]=largest[i1-1];
      }
      largest[i1]=xi;
      xbound=largest[j-1];
    }
    return;
  }
  //@}

  /// \name Vector minimum and maximum functions
  //@{
  /** \brief Compute the maximum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    data_t vector_max_value(size_t n, const vec_t &data) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_max_value().",exc_efailed);
    }
    data_t max=data[0];
    for(size_t i=1;i<n;i++) {
      if (data[i]>max) {
	max=data[i];
      }
    }
    return max;
  }

  /** \brief Compute the index which holds the 
      maximum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    size_t vector_max_index(size_t n, const vec_t &data) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_max_index().",exc_efailed);
    }
    data_t max=data[0];
    size_t ix=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]>max) {
	max=data[i];
	ix=i;
      }
    }
    return ix;
  }

  /** \brief Compute the maximum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    void vector_max(size_t n, const vec_t &data, size_t &index, 
		    data_t &val) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_max().",exc_efailed);
    }
    val=data[0];
    index=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]>val) {
	val=data[i];
	index=i;
      }
    }
    return;
  }

  /** \brief Compute the minimum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    data_t vector_min_value(size_t n, const vec_t &data) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min_value().",exc_efailed);
    }
    data_t min=data[0];
    for(size_t i=1;i<n;i++) {
      if (data[i]<min) {
	min=data[i];
      }
    }
    return min;
  }

  /** \brief Compute the index which holds the 
      minimum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    size_t vector_min_index(size_t n, const vec_t &data) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min_index().",exc_efailed);
    }
    data_t min=data[0];
    size_t ix=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]<min) {
	min=data[i];
	ix=i;
      }
    }
    return ix;
  }

  /** \brief Compute the minimum of the first \c n elements of a vector
   */
  template<class vec_t, class data_t>
    void vector_min(size_t n, const vec_t &data, size_t &index, 
		    data_t &val) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min().",exc_efailed);
    }
    val=data[0];
    index=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]<val) {
	val=data[i];
	index=i;
      }
    }
    return;
  }

  /** \brief Compute the minimum and maximum of the first 
      \c n elements of a vector
  */
  template<class vec_t, class data_t>
    void vector_minmax_value(size_t n, vec_t &data, 
			    data_t &min, data_t &max) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min().",exc_efailed);
    }
    min=data[0];
    max=min;
    for(size_t i=1;i<n;i++) {
      if (data[i]<min) {
	min=data[i];
      }
      if (data[i]>max) {
	max=data[i];
      }
    }
    return;
  }

  /** \brief Compute the minimum and maximum of the first 
      \c n elements of a vector
  */
  template<class vec_t, class data_t>
    void vector_minmax_index(size_t n, vec_t &data, 
			     size_t &ix_min, size_t &ix_max) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min().",exc_efailed);
    }
    data_t min=data[0];
    data_t max=min;
    ix_min=0;
    ix_max=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]<min) {
	min=data[i];
	ix_min=i;
      }
      if (data[i]>max) {
	max=data[i];
	ix_max=i;
      }
    }
    return;
  }

  /** \brief Compute the minimum and maximum of the first 
      \c n elements of a vector
  */
  template<class vec_t, class data_t>
    void vector_minmax(size_t n, vec_t &data, 
		      size_t &ix_min, data_t &min, 
		      size_t &ix_max, data_t &max) {
    
    if (n==0) {
      O2SCL_ERR("Sent size=0 to vector_min().",exc_efailed);
    }
    min=data[0];
    max=min;
    ix_min=0;
    ix_max=0;
    for(size_t i=1;i<n;i++) {
      if (data[i]<min) {
	min=data[i];
	ix_min=i;
      }
      if (data[i]>max) {
	max=data[i];
	ix_max=i;
      }
    }
    return;
  }
  //@}
  
  /// \name Minima and maxima of vectors through quadratic fit
  //@{
  /** \brief Maximum of vector by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_max_quad(size_t n, const vec_t &data) {
    size_t ix=vector_max_index<vec_t,data_t>(n,data);
    if (ix==0) {
      return quadratic_extremum_y<data_t>(0,1,2,data[0],data[1],data[2]);
    } else if (ix==n-1) {
      return quadratic_extremum_y<data_t>
	(n-3,n-2,n-1,data[n-3],data[n-2],data[n-1]);
    } 
    return quadratic_extremum_y<data_t>
      (ix-1,ix,ix+1,data[ix-1],data[ix],data[ix+1]);
  }

  /** \brief Maximum of vector by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_max_quad(size_t n, const vec_t &x, const vec_t &y) {
    size_t ix=vector_max_index<vec_t,data_t>(n,y);
    if (ix==0 || ix==n-1) return y[ix];
    return quadratic_extremum_y<data_t>(x[ix-1],x[ix],x[ix+1],
				y[ix-1],y[ix],y[ix+1]);
  }

  /** \brief Location of vector maximum by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_max_quad_loc(size_t n, const vec_t &x, const vec_t &y) {
    size_t ix=vector_max_index<vec_t,data_t>(n,y);
    if (ix==0 || ix==n-1) return y[ix];
    return quadratic_extremum_x<data_t>(x[ix-1],x[ix],x[ix+1],
				y[ix-1],y[ix],y[ix+1]);
  }

  /** \brief Minimum of vector by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_min_quad(size_t n, const vec_t &data) {
    size_t ix=vector_min_index<vec_t,data_t>(n,data);
    if (ix==0) {
      return quadratic_extremum_y<data_t>(0,1,2,data[0],data[1],data[2]);
    } else if (ix==n-1) {
      return quadratic_extremum_y<data_t>
	(n-3,n-2,n-1,data[n-3],data[n-2],data[n-1]);
    } 
    return quadratic_extremum_y<data_t>
      (ix-1,ix,ix+1,data[ix-1],data[ix],data[ix+1]);
  }

  /** \brief Minimum of vector by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_min_quad(size_t n, const vec_t &x, const vec_t &y) {
    size_t ix=vector_min_index<vec_t,data_t>(n,y);
    if (ix==0 || ix==n-1) return y[ix];
    return quadratic_extremum_y<data_t>(x[ix-1],x[ix],x[ix+1],
				y[ix-1],y[ix],y[ix+1]);
  }

  /** \brief Location of vector minimum by quadratic fit
   */
  template<class vec_t, class data_t>
    data_t vector_min_quad_loc(size_t n, const vec_t &x, const vec_t &y) {
    size_t ix=vector_min_index<vec_t,data_t>(n,y);
    if (ix==0 || ix==n-1) return y[ix];
    return quadratic_extremum_x<data_t>(x[ix-1],x[ix],x[ix+1],
				y[ix-1],y[ix],y[ix+1]);
  }
  //@}

  /// \name Matrix minimum and maximum functions
  //@{
  /** \brief Compute the maximum of the lower-left part of a matrix
   */
  template<class mat_t, class data_t>
    data_t matrix_max(size_t m, const size_t n, const mat_t &data) {
    
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_max().",exc_efailed);
    }
    data_t max=data(0,0);
    for(size_t i=0;i<m;i++) {
      for(size_t j=0;j<n;j++) {
	if (data(i,j)>max) {
	  max=data(i,j);
	}
      }
    }
    return max;
  }

  /** \brief Compute the maximum of a matrix
   */
  template<class mat_t, class data_t> data_t matrix_max(const mat_t &data) {
    size_t m=data.size1();
    size_t n=data.size2();
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_max().",exc_efailed);
    }
    data_t max=data(0,0);
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)>max) {
	  max=data(i,j);
	}
      }
    }
    return max;
  }

  /** \brief Compute the maximum of a matrix and return 
      the indices of the maximum element
   */
  template<class mat_t, class data_t>
    void matrix_max_index(size_t n, const size_t m, const mat_t &data,
			  size_t &i_max, size_t &j_max, data_t &max) {
    
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_max().",exc_efailed);
    }
    max=data(0,0);
    i_max=0;
    j_max=0;
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)>max) {
	  max=data(i,j);
	  i_max=i;
	  j_max=j;
	}
      }
    }
    return;
  }

  /** \brief Compute the minimum of a matrix
   */
  template<class mat_t, class data_t>
    data_t matrix_min(size_t n, const size_t m, const mat_t &data) {
    
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_min().",exc_efailed);
    }
    data_t min=data(0,0);
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)<min) {
	  min=data(i,j);
	}
      }
    }
    return min;
  }

  /** \brief Compute the minimum of a matrix and return 
      the indices of the minimum element
  */
  template<class mat_t, class data_t>
    void matrix_min_index(size_t n, const size_t m, const mat_t &data,
			  size_t &i_min, size_t &j_min, data_t &min) {
    
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_min().",exc_efailed);
    }
    min=data(0,0);
    i_min=0;
    j_min=0;
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)<min) {
	  min=data(i,j);
	  i_min=i;
	  j_min=j;
	}
      }
    }
    return;
  }

  /** \brief Compute the minimum and maximum of a matrix
   */
  template<class mat_t, class data_t>
    void matrix_minmax(size_t n, const size_t m, const mat_t &data,
		      data_t &min, data_t &max) {
    
    if (n==0 || m==0) {
      O2SCL_ERR("Sent size=0 to matrix_min().",exc_efailed);
    }
    min=data(0,0);
    max=data(0,0);
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)<min) {
	  min=data(i,j);
	} else if (data(i,j)>max) {
	  max=data(i,j);
	}
      }
    }
    return;
  }

  /** \brief Compute the minimum and maximum of a matrix and
      return their locations
   */
  template<class mat_t, class data_t>
    void matrix_minmax_index(size_t n, const size_t m, const mat_t &data,
			     size_t &i_min, size_t &j_min, data_t &min, 
			     size_t &i_max, size_t &j_max, data_t &max) {
    
    if (n==0 || m==0) {
      O2SCL_ERR2("Sent size=0 to function ",
		 "matrix_minmax_index().",exc_efailed);
    }
    min=data(0,0);
    i_min=0;
    j_min=0;
    max=data(0,0);
    i_max=0;
    j_max=0;
    for(size_t i=0;i<n;i++) {
      for(size_t j=0;j<m;j++) {
	if (data(i,j)<min) {
	  min=data(i,j);
	  i_min=i;
	  j_min=j;
	} else if (data(i,j)>max) {
	  max=data(i,j);
	  i_max=i;
	  j_max=j;
	}
      }
    }
    return;
  }
  //@}

  /// \name Searching vectors and matrices
  //@{
  /** \brief Lookup element \c x0 in vector \c x of length \c n

      This function finds the element in vector \c x which is closest
      to \c x0. It ignores all elements in \c x which are not finite.
      If the vector is empty (i.e. \c n is zero), or if all of the
      elements in \c x are not finite, then the error handler will be
      called.

      This function works for all classes \c vec_t where an operator[]
      is defined which returns a double (either as a value or a
      reference).
  */
  template<class vec_t>
    size_t vector_lookup(size_t n, const vec_t &x, double x0) {
    if (n==0) {
      O2SCL_ERR("Empty vector in function vector_lookup().",
		exc_einval);
      return 0;
    }
    size_t row=0, i=0;
    while(!o2scl::is_finite(x[i]) && i<n-1) i++;
    if (i==n-1) {
      O2SCL_ERR2("Entire vector not finite in ",
		 "function vector_lookup()",exc_einval);
      return 0;
    }
    double best=x[i], bdiff=fabs(x[i]-x0);
    for(;i<n;i++) {
      if (o2scl::is_finite(x[i]) && fabs(x[i]-x0)<bdiff) {
	row=i;
	best=x[i];
	bdiff=fabs(x[i]-x0);
      }
    }
    return row;
  }

  /** \brief Lookup an element in a matrix

      Return the location <tt>(i,j)</tt> of the element closest to 
      \c x0. 
   */
  template<class mat_t>
    void matrix_lookup(size_t m, size_t n, const mat_t &A, 
		       double x0, size_t &i, size_t &j) {
    if (m==0 || n==0) {
      O2SCL_ERR("Empty matrix in matrix_lookup().",
		exc_einval);
    }
    double dist=0.0;
    bool found_one=false;
    for(size_t i2=0;i2<m;i2++) {
      for(size_t j2=0;j2<n;j2++) {
	if (o2scl::is_finite(A(i,j))) {
	  if (found_one==false) {
	    dist=fabs(A(i,j)-x0);
	    found_one=true;
	    i=i2;
	    j=j2;
	  } else {
	    if (fabs(A(i,j)-x0)<dist) {
	      dist=fabs(A(i,j)-x0);
	      i=i2;
	      j=j2;
	    }
	  }
	}
      }
    }
    if (found_one==false) {
      O2SCL_ERR2("Entire matrix not finite in ",
		 "function matrix_lookup()",exc_einval);
    }
    return;
  }

  /** \brief Binary search a part of an increasing vector for
      <tt>x0</tt>.
      
      This function performs a binary search of between
      <tt>x[lo]</tt> and <tt>x[hi]</tt>. It returns 
      - \c lo if \c x0 < <tt>x[lo]</tt>
      - \c i if <tt>x[i]</tt> <= \c x0 < <tt>x[i+2]</tt> 
      for \c lo <= \c i < \c hi
      - \c hi-1 if \c x0 >= \c <tt>x[hi-1]</tt>

      This function is designed to find the interval containing \c x0,
      not the index of the element closest to x0. To perform the
      latter operation, you can use \ref vector_lookup().

      The element at <tt>x[hi]</tt> is never referenced by this
      function. The parameter \c hi can be either the index of the
      last element (e.g. <tt>n-1</tt> for a vector of size <tt>n</tt>
      with starting index <tt>0</tt>), or the index of one element
      (e.g. <tt>n</tt> for a vector of size <tt>n</tt> and a starting
      index <tt>0</tt>) for a depending on whether or not the user
      wants to allow the function to return the index of the last
      element.

      This function operates in the same way as
      <tt>gsl_interp_bsearch()</tt>.

      The operation of this function is undefined if the data is
      not strictly monotonic, i.e. if some of the data elements are 
      equal.

      This function will call the error handler if \c lo is
      greater than \c hi.
  */
  template<class vec_t, class data_t> 
    size_t vector_bsearch_inc(const data_t x0, const vec_t &x,
			      size_t lo, size_t hi) {
    if (lo>hi) {
      O2SCL_ERR2("Low and high indexes backwards in ",
		 "function vector_bsearch_inc().",exc_einval);
    }
    while (hi>lo+1) {
      size_t i=(hi+lo)/2;
      if (x[i]>x0) {
	hi=i;
      } else {
	lo=i;
      }
    }
    
    return lo;
  }

  /** \brief Binary search a part of an decreasing vector for
      <tt>x0</tt>.
      
      This function performs a binary search of between
      <tt>x[lo]</tt> and <tt>x[hi]</tt> (inclusive). It returns 
      - \c lo if \c x0 > <tt>x[lo]</tt>
      - \c i if <tt>x[i]</tt> >= \c x0 > <tt>x[i+1]</tt> 
      for \c lo <= \c i < \c hi
      - \c hi-1 if \c x0 <= \c <tt>x[hi-1]</tt>
    
      The element at <tt>x[hi]</tt> is never referenced by this
      function. The parameter \c hi can be either the index of the
      last element (e.g. <tt>n-1</tt> for a vector of size <tt>n</tt>
      with starting index <tt>0</tt>), or the index of one element
      (e.g. <tt>n</tt> for a vector of size <tt>n</tt> and a starting
      index <tt>0</tt>) for a depending on whether or not the user
      wants to allow the function to return the index of the last
      element.

      The operation of this function is undefined if the data is
      not strictly monotonic, i.e. if some of the data elements are 
      equal.
      
      This function will call the error handler if \c lo is
      greater than \c hi.
  */
  template<class vec_t, class data_t> 
    size_t vector_bsearch_dec(const data_t x0, const vec_t &x,
			      size_t lo, size_t hi) {
    if (lo>hi) {
      O2SCL_ERR2("Low and high indexes backwards in ",
		 "function vector_bsearch_dec().",exc_einval);
    }
    while (hi>lo+1) {
      size_t i=(hi+lo)/2;
      if (x[i]<x0) {
	hi=i;
      } else {
	lo=i;
      }
    }
    
    return lo;
  }

  /** \brief Binary search a part of a monotonic vector for
      <tt>x0</tt>.

      This wrapper just calls \ref o2scl::vector_bsearch_inc() or
      \ref o2scl::vector_bsearch_dec() depending on the ordering
      of \c x. 
  */
  template<class vec_t, class data_t> 
    size_t vector_bsearch(const data_t x0, const vec_t &x,
			  size_t lo, size_t hi) {
    if (x[lo]<x[hi-1]) {
      return vector_bsearch_inc<vec_t,data_t>(x0,x,lo,hi);
    }
    return vector_bsearch_dec<vec_t,data_t>(x0,x,lo,hi);
  }
  //@}

  /// \name Miscellaneous mathematical functions
  //@{
  /** \brief Compute the sum of the first \c n elements of a vector

      If \c n is zero, this will return 0 without throwing
      an exception.
  */
  template<class vec_t, class data_t>
    data_t vector_sum(size_t n, vec_t &data) {
    
    data_t sum=0.0;
    for(size_t i=0;i<n;i++) {
      sum+=data[i];
    }
    return sum;
  }

  /** \brief Compute the sum of all the elements of a vector

      If the vector has zero size, this will return 0 without
      throwing an exception.
  */
  template<class vec_t, class data_t> data_t vector_sum(vec_t &data) {
    data_t sum=0.0;
    for(size_t i=0;i<data.size();i++) {
      sum+=data[i];
    }
    return sum;
  }

  /** \brief Compute the norm of a vector of floating-point 
      (single or double precision) numbers

      This function is a more generic version of 
      \ref o2scl_cblas::dnrm2 . 
  */
  template<class vec_t, class data_t>
    data_t vector_norm(size_t n, const vec_t &x) {
    
    data_t scale = 0.0;
    data_t ssq = 1.0;
    
    if (n <= 0) {
      return 0.0;
    } else if (n == 1) {
      return fabs(x[0]);
    }
      
    for (size_t i = 0; i < n; i++) {
      const data_t xx = x[i];

      if (xx != 0.0) {
	const data_t ax = fabs(xx);
          
	if (scale < ax) {
	  ssq = 1.0 + ssq * (scale / ax) * (scale / ax);
	  scale = ax;
	} else {
	  ssq += (ax / scale) * (ax / scale);
	}
      }

    }
      
    return scale * sqrt(ssq);
  }
  //@}

  /// \name Other vector and matrix functions
  //@{
  /** \brief "Rotate" a vector so that the kth element is now the beginning

      This is a generic template function which will work for
      any types \c data_t and \c vec_t for which
      - \c data_t has an <tt>operator=</tt>
      - <tt>vec_t::operator[]</tt> returns a reference
      to an object of type \c data_t 

      This function is used, for example, in \ref o2scl::pinside.

      \note This function is not the same as a Givens rotation,
      which is typically referred to in BLAS routines as <tt>drot()</tt>.
  */
  template<class vec_t, class data_t>
    void vector_rotate(size_t n, vec_t &data, size_t k) {

    data_t *tmp=new data_t[n];
    for(size_t i=0;i<n;i++) {
      tmp[i]=data[(i+k)%n];
    }
    for(size_t i=0;i<n;i++) {
      data[i]=tmp[i];
    }
    delete[] tmp;
    
    return;
  }

  /** \brief Reverse a vector

      If \c n is zero, this function will silently do nothing.
  */
  template<class vec_t, class data_t>
    void vector_reverse(size_t n, vec_t &data) {
    data_t tmp;

    for(size_t i=0;i<n/2;i++) {
      tmp=data[n-1-i];
      data[n-1-i]=data[i];
      data[i]=tmp;
    }
    return;
  }

  /** \brief Reverse a vector

      If \c n is zero, this function will silently do nothing.
  */
  template<class vec_t, class data_t>
    void vector_reverse(vec_t &data) {
    data_t tmp;
    size_t n=data.size();

    for(size_t i=0;i<n/2;i++) {
      tmp=data[n-1-i];
      data[n-1-i]=data[i];
      data[i]=tmp;
    }
    return;
  }

  /** \brief Construct a row of a matrix

      This class template works with combinations of ublas
      <tt>matrix</tt> and <tt>matrix_row</tt> objects,
      <tt>arma::mat</tt> and <tt>arma::rowvec</tt>, and
      <tt>Eigen::MatrixXd</tt> and <tt>Eigen::VectorXd</tt>.

      \note When calling this function with ublas objects, the
      namespace prefix <tt>"o2scl::"</tt> must often be specified,
      otherwise some compilers will use argument dependent lookup and
      get (justifiably) confused with matrix_row in the ublas
      namespace.

      \note The template parameters must be explicitly specified
      when calling this template function. 
   */
  template<class mat_t, class mat_row_t>
    mat_row_t matrix_row(mat_t &M, size_t row) {
    return mat_row_t(M,row);
  }

  /** \brief Generic object which represents a row of a matrix

      \note This class is experimental.
   */
  template<class mat_t> class matrix_row_gen {
  public:
    mat_t &m_;
    size_t row_;
  matrix_row_gen(mat_t &m, size_t row) : m_(m), row_(row) {
    }
    double &operator[](size_t i) {
      return m_(row_,i);
    }
    const double &operator[](size_t i) const {
      return m_(row_,i);
    }
  };

  /** \brief Construct a column of a matrix

      This class template works with combinations of ublas
      <tt>matrix</tt> and <tt>matrix_column</tt> objects,
      <tt>arma::mat</tt> and <tt>arma::colvec</tt>, and
      <tt>Eigen::MatrixXd</tt> and <tt>Eigen::VectorXd</tt>.

      \note When calling this function with ublas objects, the
      namespace prefix <tt>"o2scl::"</tt> must often be specified,
      otherwise some compilers will use argument dependent lookup and
      get (justifiably) confused with matrix_column in the ublas
      namespace.

      \note The template parameters must be explicitly specified
      when calling this template function.
   */
  template<class mat_t, class mat_column_t>
    mat_column_t matrix_column(mat_t &M, size_t column) {
    return mat_column_t(M,column);
  }

  /** \brief Generic object which represents a column of a matrix

      \note This class is experimental.
   */
  template<class mat_t> class matrix_column_gen {
  public:
    mat_t &m_;
    size_t column_;
  matrix_column_gen(mat_t &m, size_t column) : m_(m), column_(column) {
    }
    double &operator[](size_t i) {
      return m_(i,column_);
    }
    const double &operator[](size_t i) const {
      return m_(i,column_);
    }
  };

  /** \brief Output a vector to a stream
      
      No trailing space is output after the last element, and an
      endline is output only if \c endline is set to \c true.  If the
      parameter \c n is zero, this function silently does nothing.

      Note that the \o2 vector classes also have their own
      \c operator<<() defined for them.

      This works with any class <tt>vec_t</tt> which has an operator[]
      which returns either the value of or a reference to the ith
      element and the element type has its own output operator which
      has been defined.
  */
  template<class vec_t> 
    void vector_out(std::ostream &os, size_t n, const vec_t &v, 
		    bool endline=false) {
    
    // This next line is important since n-1 is not well-defined if n=0
    if (n==0) return;

    for(size_t i=0;i<n-1;i++) os << v[i] << " ";
    os << v[n-1];
    if (endline) os << std::endl;
    return;
  }

  /** \brief Output a vector to a stream
      
      No trailing space is output after the last element, and an
      endline is output only if \c endline is set to \c true.  If the
      parameter \c n is zero, this function silently does nothing.

      Note that the \o2 vector classes also have their own
      \c operator<<() defined for them.

      This works with any class <tt>vec_t</tt> which has an operator[]
      which returns either the value of or a reference to the ith
      element and the element type has its own output operator which
      has been defined.
  */
  template<class vec_t> 
    void vector_out(std::ostream &os, const vec_t &v, bool endline=false) {
    
    size_t n=v.size();
    
    // This next line is important since n-1 is not well-defined if n=0
    if (n==0) return;

    for(size_t i=0;i<n-1;i++) os << v[i] << " ";
    os << v[n-1];
    if (endline) os << std::endl;
    return;
  }
  
  /** \brief Fill a vector with a specified grid
   */
  template<class vec_t, class data_t>
    void vector_grid(uniform_grid<data_t> g, vec_t &v) {
    g.template vector<vec_t>(v);
    return;
  }

  /// Set a matrix to unity on the diagonal and zero otherwise
  template<class mat_t> 
    void matrix_set_identity(size_t M, size_t N, mat_t &m) {
    for(size_t i=0;i<M;i++) {
      for(size_t j=0;j<N;j++) {
	if (i==j) m(i,j)=1.0;
	else m(i,j)=0.0;
      }
    }
  }
  //@}
  
}

#if defined (O2SCL_COND_FLAG) || defined (DOXYGEN)

#if defined (O2SCL_ARMA) || defined (DOXYGEN)
#include <armadillo>
namespace o2scl {

  /// \name Armadillo specializations
  //@{
  /// Armadillo version of \ref matrix_max()
  double matrix_max(const arma::mat &data);

  /// Armadillo version of \ref matrix_row()
  template<> arma::subview_row<double>  
    matrix_row<arma::mat,arma::subview_row<double> >
    (arma::mat &M, size_t row);

  /// Armadillo version of \ref matrix_column()
  template<> arma::subview_col<double>  
    matrix_column<arma::mat,arma::subview_col<double> >
    (arma::mat &M, size_t column);
  //@}

}

#endif

#if defined (O2SCL_EIGEN) || defined (DOXYGEN)
#include <Eigen/Dense>

namespace o2scl {

  /// \name Eigen specializations
  //@{
  /// Eigen version of \ref matrix_max()
  double matrix_max(const Eigen::MatrixXd &data);
  
  /// Eigen version of \ref matrix_row()
  template<> Eigen::MatrixXd::RowXpr 
    matrix_row<Eigen::MatrixXd,Eigen::MatrixXd::RowXpr>
    (Eigen::MatrixXd &M, size_t row);

  /// Eigen version of \ref matrix_column()
  template<> Eigen::MatrixXd::ColXpr 
    matrix_column<Eigen::MatrixXd,Eigen::MatrixXd::ColXpr>
    (Eigen::MatrixXd &M, size_t column);
  //@}

}

#endif

#else

#include <o2scl/vector_special.h>

// End of "#if defined (O2SCL_COND_FLAG) || defined (DOXYGEN)"
#endif

#ifdef DOXYGEN
/** \brief Placeholder documentation of some related Boost objects
*/
namespace boost {
  /** \brief Documentation of Boost::numeric objects
   */
  namespace numeric {
    /** \brief Documentation of uBlas objects
     */
    namespace ublas {
      /** \brief The default vector type from uBlas 

	  The uBlas types aren't documented here, but the full documentation 
	  is available at
	  http://www.boost.org/doc/libs/release/libs/numeric/ublas/doc/index.htm

	  Internally in \o2, this is often typedef'd using
	  \code
	  typedef boost::numeric::ublas::vector<double> ubvector;
	  typedef boost::numeric::ublas::vector<size_t> ubvector_size_t;
	  typedef boost::numeric::ublas::vector<int> ubvector_int;
	  \endcode

	  This is documented in \ref vector.h .
       */
      template<class T, class A> class vector {
      };
      /** \brief The default matrix type from uBlas 
	  
	  The uBlas types aren't documented here, but the full documentation 
	  is available at
	  http://www.boost.org/doc/libs/release/libs/numeric/ublas/doc/index.htm

	  Internally in \o2, this is often typedef'd using
	  \code
	  typedef boost::numeric::ublas::matrix<double> ubmatrix;
	  typedef boost::numeric::ublas::matrix<size_t> ubmatrix_size_t;
	  typedef boost::numeric::ublas::matrix<int> ubmatrix_int;
	  \endcode

	  This is documented in \ref vector.h .
       */
      template<class T, class F, class A> class matrix {
      };
    }
  }
}
// End of "#ifdef DOXYGEN"
#endif

// End of "#ifndef O2SCL_VECTOR_H"
#endif
