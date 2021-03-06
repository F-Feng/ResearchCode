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
#ifndef O2SCL_TENSOR_GRID_H
#define O2SCL_TENSOR_GRID_H

/** \file tensor_grid.h
    \brief File defining \ref o2scl::tensor_grid and rank-specific children
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_ieee_utils.h>

#include <o2scl/err_hnd.h>
#include <o2scl/interp.h>
#include <o2scl/tensor.h>
#include <o2scl/table3d.h>

// Forward definition of the tensor_grid class for HDF I/O
namespace o2scl {
  class tensor_grid;
}

// Forward definition of HDF I/O to extend friendship
namespace o2scl_hdf { 
  class hdf_file; 
  void hdf_input(hdf_file &hf, o2scl::tensor_grid &t, std::string name);
  void hdf_output(hdf_file &hf, o2scl::tensor_grid &t, std::string name);
}

#ifndef DOXYGEN_NO_O2NS
namespace o2scl {
#endif

  /** \brief Tensor class with arbitrary dimensions with a grid

      This tensor class allows one to assign the indexes to numerical
      scales, so that n-dimensional interpolation can be performed. To
      set the grid, use \ref set_grid() or \ref set_grid_packed() and
      then interpolation can be done using \ref interp_linear() or 
      \ref interpolate().
      
      By convention, member functions ending in the _val 
      suffix return the closest grid-point to some user-specified
      values. 

      \comment
      I like how hist_new only allows you to set the
      grid if it's the same size as the old grid or the tensor
      is empty. This same practice could be applied here, to
      force the user to clear the tensor before resetting the grid.
      (10/24/11 - Actually, maybe this is a bad idea, because
      this class is more analogous to ubvector, which doesn't
      behave this way.)
      \endcomment

      \todo Make this a template like the \ref o2scl::tensor class?

      \todo It is possible for the user to create a tensor_grid
      object, upcast it to a tensor object, and then use
      tensor::resize() to resize the tensor, failing to resize the
      grid. This probably needs fixing.

      \future Only allocate space for grid if it is set
      \future Consider creating a new set_grid() function which
      takes grids from an object like uniform_grid. Maybe make a 
      constructor for a tensor_grid object which just takes 
      as input a set of grids?
  */
  class tensor_grid : 
  public tensor<boost::numeric::ublas::vector<double>,
    boost::numeric::ublas::vector<size_t> > {

  public:

    typedef boost::numeric::ublas::vector<double> ubvector;
    typedef boost::numeric::ublas::vector<size_t> ubvector_size_t;
    typedef boost::numeric::ublas::range range;
    typedef boost::numeric::ublas::vector_range<ubvector> ubvector_range;
    typedef boost::numeric::ublas::vector_range<ubvector_size_t> 
      ubvector_size_t_range;

#ifndef DOXYGEN_INTERNAL

  protected:

    /// A rank-sized set of arrays for the grid points
    ubvector grid;

    /// If true, the grid has been set by the user
    bool grid_set;

    /// Interpolation type
    size_t itype;
    
    /// Return a reference to the data (for HDF I/O)
    ubvector &get_data() {
      return data;
    }

#endif

  public:
    
    /// Create an empty tensor with zero rank
  tensor_grid() : tensor<ubvector,ubvector_size_t>() {
      grid_set=false;
      itype=itp_linear;
    }
    
    /** \brief Create a tensor of rank \c rank with sizes given in \c dim
	
	The parameter \c dim must be a vector of sizes with length \c
	rank. If the user requests any of the sizes to be zero, this
	constructor will call the error handler.
    */
    template<class size_vec_t> 
      tensor_grid(size_t rank, const size_vec_t &dim) : 
    tensor<ubvector,ubvector_size_t>(rank,dim) {
      grid_set=false;
      itype=itp_linear;
      // Note that the parent method sets rk to be equal to rank
      for(size_t i=0;i<rk;i++) {
	if (dim[i]==0) {
	  O2SCL_ERR((((std::string)"Requested zero size with non-zero ")+
		     "rank for index "+szttos(i)+" in tensor_grid::"+
		     "tensor_grid(size_t,size_vec_t)").c_str(),
		    exc_einval);
	}
      }
    }

    virtual ~tensor_grid() {
    }

    /// \name Set functions
    //@{
    /// Set the element closest to grid point \c grdp to value \c val
    template<class vec_t> 
      void set_val(const vec_t &grdp, double val) {

      // Find indices
      ubvector_size_t index(rk);
      for(size_t i=0;i<rk;i++) {
	index[i]=lookup_grid(i,grdp[i]);
      }
      
      // Pack
      size_t ix=index[0];
      for(size_t i=1;i<rk;i++) {
	ix*=size[i];
	ix+=index[i];
      }

      // Set value
      data[ix]=val;

      return;
    }

    /** \brief Set the element closest to grid point \c grdp to value 
	\c val
	
	The parameters \c closest and \c grdp may be identical,
	allowing one to update a vector \c grdp with the
	closest grid point.
    */
    template<class vec_t, class vec2_t> 
      void set_val(const vec_t &grdp, double val, vec2_t &closest) {

      // Find indices
      ubvector_size_t index(rk);
      for(size_t i=0;i<rk;i++) {
	index[i]=lookup_grid_val(i,grdp[i],closest[i]);
      }
      
      // Pack
      size_t ix=index[0];
      for(size_t i=1;i<rk;i++) {
	ix*=size[i];
	ix+=index[i];
      }

      // Set value
      data[ix]=val;

      return;
    }
    //@}

    /// \name Get functions
    //@{
    /// Get the element closest to grid point \c grdp 
    template<class vec_t> double get_val(const vec_t &grdp) {
      
      // Find indices
      ubvector_size_t index(rk);
      for(size_t i=0;i<rk;i++) {
	index[i]=lookup_grid(i,grdp[i]);
      }

      // Pack
      size_t ix=index[0];
      for(size_t i=1;i<rk;i++) {
	ix*=size[i];
	ix+=index[i];
      }

      // Set value
      return data[ix];
    }

    /** \brief Get the element closest to grid point \c grdp to 
	value \c val

	The parameters \c grdp and \c closest may refer to the
	same object. 
    */
    template<class vec_t, class vec2_t> 
      double get_val(const vec_t &grdp, vec2_t &closest) {
      
      // Find indices
      ubvector_size_t index(rk);
      for(size_t i=0;i<rk;i++) {
	index[i]=lookup_grid_val(i,grdp[i],closest[i]);
      }
      
      // Pack
      size_t ix=index[0];
      for(size_t i=1;i<rk;i++) {
	ix*=size[i];
	ix+=index[i];
      }

      // Set value
      return data[ix];
    }
    //@}
    
    /// \name Resize method
    //@{
    /** \brief Resize the tensor to rank \c rank with sizes
	given in \c dim
	
	The parameter \c dim must be a vector of sizes with a length
	equal to \c rank. This resize method is always destructive,
	and the grid is always reset. 
	
	If the user requests any of the sizes to be zero, this
	function will call the error handler.
    */
    template<class size_vec_t>
      void resize(size_t rank, const size_vec_t &dim) {
      // Double check that none of the sizes that the user
      // specified are zero
      for(size_t i=0;i<rank;i++) {
	if (dim[i]==0) {
	  O2SCL_ERR((((std::string)"Requested zero size with non-zero ")+
		     "rank for index "+szttos(i)+" in tensor_grid::"+
		     "resize().").c_str(),exc_einval);
	}
      }
      // Set the new rank
      rk=rank;
      // Resize the size vector
      size.resize(rk);
      // Reset the grid
      grid_set=false;
      grid.resize(0);
      // If the new rank is zero, reset the data, otherwise,
      // update the size vector and reset the data vector
      if (rank==0) {
	data.resize(0);
	return;
      } else {
	size_t tot=1;
	for(size_t i=0;i<rk;i++) {
	  size[i]=dim[i];
	  tot*=size[i];
	}
	data.resize(tot);
      }
      return;
    }

    //@}

    /// \name Grid manipulation
    //@{
    /// Return true if the grid has been set
    bool is_grid_set() const { return grid_set; }

    /** \brief Set the grid

	The grid must be specified for all of the dimensions at
	once. Denote \f$ (\mathrm{size})_0 \f$ as the size
	of the first dimension, \f$ (\mathrm{size})_1 \f$ as the
	size of the second dimesion, and so on. Then the 
	first \f$ (\mathrm{size})_0 \f$ entries in \c grid must
	be the grid for the first dimension, the next 
	\f$ (\mathrm{size})_1 \f$ entries must be the grid
	for the second dimension, and so on. Thus \c grid
	must be a vector of size
	\f[
	\sum_{i=0}^{\mathrm{rank}} (\mathrm{size})_i
	\f]

	Note that the grid is copied so the function argument may
	be destroyed by the user after calling set_grid_packed() without
	affecting the tensor grid. 

	\future Define a more generic interface for matrix types
    */
    template<class vec_t>
      void set_grid_packed(const vec_t &grid_vec) {
      if (rk==0) {
	O2SCL_ERR2("Tried to set grid for empty tensor in ",
		   "tensor_grid::set_grid_packed().",exc_einval);
      }
      size_t ngrid=0;
      for(size_t i=0;i<rk;i++) ngrid+=size[i];
      grid.resize(ngrid);
      for(size_t i=0;i<ngrid;i++) {
	grid[i]=grid_vec[i];
      }
      grid_set=true;
      return;
    }

    /** \brief Set grid from a vector of vectors of grid points
     */
    template<class vec_vec_t>
      void set_grid(const vec_vec_t &grid_vecs) {
      if (rk==0) {
	O2SCL_ERR2("Tried to set grid for empty tensor in ",
		   "tensor_grid::set_grid().",exc_einval);
      }
      size_t ngrid=0;
      for(size_t i=0;i<rk;i++) ngrid+=size[i];
      grid.resize(ngrid);
      size_t k=0;
      for(size_t i=0;i<rk;i++) {
	for(size_t j=0;j<size[i];j++) {
	  grid[k]=grid_vecs[i][j];
	  k++;
	}
      }
      grid_set=true;
      return;
    }

    /// Lookup jth value on the ith grid
    double get_grid(size_t i, size_t j) const {
      if (!grid_set) {
	O2SCL_ERR("Grid not set in tensor_grid::get_grid().",
		  exc_einval);
      }
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+
		   " greater than or equal to rank, "+szttos(rk)+
		   ", in tensor_grid::get_grid().").c_str(),
		  exc_einval);
      }
      size_t istart=0;
      for(size_t k=0;k<i;k++) istart+=size[k];
      return grid[istart+j];
    }

    /// Set the jth value on the ith grid
    void set_grid(size_t i, size_t j, double val) {
      if (!grid_set) {
	O2SCL_ERR("Grid not set in tensor_grid::get_grid().",
		  exc_einval);
      }
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+
		   " greater than or equal to rank, "+szttos(rk)+
		   ", in tensor_grid::get_grid().").c_str(),
		  exc_einval);
      }
      size_t istart=0;
      for(size_t k=0;k<i;k++) istart+=size[k];
      grid[istart+j]=val;
      return;
    }

    /// Lookup index for grid closest to \c val
    size_t lookup_grid(size_t i, double val) {
      if (!grid_set) {
	O2SCL_ERR("Grid not set in tensor_grid::lookup_grid().",
		  exc_einval);
      }
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+
		   " greater than or equal to rank, "+szttos(rk)+
		   ", in tensor_grid::lookup_grid().").c_str(),
		  exc_einval);
      }
      size_t istart=0;
      
      for(size_t j=0;j<i;j++) {
	istart+=size[j];
      }
      size_t best=istart;
      double min=fabs(grid[istart]-val);
      for(size_t j=istart;j<istart+size[i];j++) {
	if (fabs(grid[j]-val)<min) {
	  best=j;
	  min=fabs(grid[j]-val);
	}
      }
      return best-istart;
    }

    /** \brief Lookup indices for grid closest point to \c vals

        The values in \c vals are not modified by this function.
	
	\comment
	This function must have a different name than 
	lookup_grid() because the template types cause
	confusion between the two functions.
	\endcomment
    */
    template<class vec_t, class size_vec_t>
      void lookup_grid_vec(const vec_t &vals, size_vec_t &indices) const {
      for(size_t k=0;k<rk;k++) {
	indices[k]=lookup_grid(k,vals[k]);
      }
      return;
    }

    /** \brief Lookup index for grid closest to \c val, returning the 
	grid point

	The parameters \c val and \c val2 may refer to the
	same object. 
    */
    size_t lookup_grid_val(size_t i, const double &val, double &val2) {
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+
		   " greater than or equal to rank, "+szttos(rk)+
		   ", in tensor_grid::lookup_grid_val().").c_str(),
		  exc_einval);
      }
      if (grid_set==false) {
	O2SCL_ERR("Grid not set in tensor_grid::lookup_grid_val().",
		  exc_einval);
      }
      size_t istart=0;
      for(size_t j=0;j<i;j++) istart+=size[j];
      size_t best=istart;
      double min=fabs(grid[istart]-val);
      val2=grid[istart];
      for(size_t j=istart;j<istart+size[i];j++) {
	if (fabs(grid[j]-val)<min) {
	  best=j;
	  min=fabs(grid[j]-val);
	  val2=grid[j];
	}
      }
      return best-istart;
    }

    /// Lookup index for grid closest to \c val
    size_t lookup_grid_packed(size_t i, double val) {
      if (!grid_set) {
	O2SCL_ERR("Grid not set in tensor_grid::lookup_grid_packed().",
		  exc_einval);
      }
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+" greater than rank, "+
		   szttos(rk)+
		   ", in tensor_grid::lookup_grid_packed().").c_str(),
		  exc_einval);
      }
      size_t istart=0;
      for(size_t j=0;j<i;j++) istart+=size[j];
      size_t best=istart;
      double min=fabs(grid[istart]-val);
      for(size_t j=istart;j<istart+size[i];j++) {
	if (fabs(grid[j]-val)<min) {
	  best=j;
	  min=fabs(grid[j]-val);
	}
      }
      return best;
    }

    /// Lookup index for grid closest to \c val
    size_t lookup_grid_packed_val(size_t i, double val, double &val2) {
      if (!grid_set) {
	O2SCL_ERR("Grid not set in tensor_grid::lookup_grid_packed().",
		  exc_einval);
      }
      if (i>=rk) {
	O2SCL_ERR((((std::string)"Index ")+szttos(i)+" greater than rank, "+
		   szttos(rk)+
		   ", in tensor_grid::lookup_grid_packed().").c_str(),
		  exc_einval);
      }
      size_t istart=0;
      for(size_t j=0;j<i;j++) istart+=size[j];
      size_t best=istart;
      double min=fabs(grid[istart]-val);
      val2=grid[istart];
      for(size_t j=istart;j<istart+size[i];j++) {
	if (fabs(grid[j]-val)<min) {
	  best=j;
	  min=fabs(grid[j]-val);
	  val2=grid[j];
	}
      }
      return best;
    }
    //@}

    /// \name Slicing
    //@{
    /** \brief Create a slice in a table3d object with an aligned
	grid

	This function uses the grid associated with indices \c ix_x
	and \c ix_y, and the tensor interpolation function to copy to
	the slice named \c slice_name in the table3d object \c tab .

	If the table3d object does not currently have a grid set, then
	the grid is automatically set to be the same as that stored in
	the tensor_grid object associated with ranks \c ix_x and \c
	iy_y. If the \ref o2scl::table3d object does have a grid set,
	then the values returned by \ref o2scl::table3d::get_nx() and
	\ref o2scl::table3d::get_ny() must be equal to the size of the
	tensor in indices \c ix_x and ix_y, respectively.

	This currently requires a copy of all the tensor data
	into the table3d object.
     */
    template<class size_vec_t> 
      void copy_slice_align(size_t ix_x, size_t ix_y, size_vec_t &index, 
			    table3d &tab, std::string slice_name) {
      
      if (ix_x>=rk || ix_y>=rk || ix_x==ix_y) {
	O2SCL_ERR2("Either indices greater than rank or x and y ind",
		   "ices equal in tensor_grid::copy_slice_align().",
		   exc_efailed);
      }

      // Get current table3d grid
      size_t nx, ny;
      tab.get_size(nx,ny);

      if (nx==0 && ny==0) {

	// If there's no grid, just create it
	std::vector<double> gx, gy;
	for(size_t i=0;i<size[ix_x];i++) {
	  gx.push_back(this->get_grid(ix_x,i));
	}
	for(size_t i=0;i<size[ix_y];i++) {
	  gy.push_back(this->get_grid(ix_y,i));
	}
	nx=gx.size();
	ny=gy.size();
	tab.set_xy("x",nx,gx,"y",ny,gy);
      }

      // Check that the grids are commensurate
      if (nx!=size[ix_x] || ny!=size[ix_y]) {
	O2SCL_ERR2("Grids not commensurate in ",
		   "tensor_grid::copy_slice_align().",exc_einval);
      }

      // Create slice if not already present
      size_t is;
      if (!tab.is_slice(slice_name,is)) tab.new_slice(slice_name);
      
      // Copy over data
      for(size_t i=0;i<nx;i++) {
	for(size_t j=0;j<ny;j++) {
	  index[ix_x]=i;
	  index[ix_y]=j;
	  double val=this->get(index);
	  tab.set(i,j,slice_name,val);
	}
      }
      
      return;
    }

    /** \brief Copy to a slice in a table3d object using interpolation

	This function uses the grid associated with indices \c ix_x
	and \c ix_y, and the tensor interpolation function to copy the
	tensor information to the slice named \c slice_name in the
	table3d object \c tab .
	
	\note This function uses the \ref tensor_grid::interp_linear() 
	for the interpolation.
     */
    template<class size_vec_t> 
      void copy_slice_interp(size_t ix_x, size_t ix_y, size_vec_t &index, 
			       table3d &tab, std::string slice_name) {

      if (ix_x>=rk || ix_y>=rk || ix_x==ix_y) {
	O2SCL_ERR2("Either indices greater than rank or x and y ",
		   "indices equal in tensor_grid::copy_slice_interp().",
		   exc_efailed);
      }

      // Get current table3d grid
      size_t nx, ny;
      tab.get_size(nx,ny);

      if (nx==0 && ny==0) {
	// If there's no grid, then just use the aligned version
	return copy_slice_align(ix_x,ix_y,index,tab,slice_name);
      }

      // Create vector of values to interpolate with
      std::vector<double> vals(rk);
      for(size_t i=0;i<rk;i++) {
	if (i!=ix_x && i!=ix_y) vals[i]=this->get_grid(i,index[i]);
      }

      // Create slice if not already present
      size_t is;
      if (!tab.is_slice(slice_name,is)) tab.new_slice(slice_name);

      // Loop through the table grid to perform the interpolation
      for(size_t i=0;i<nx;i++) {
	for(size_t j=0;j<ny;j++) {
	  vals[ix_x]=tab.get_grid_x(i);
	  vals[ix_y]=tab.get_grid_y(j);
	  tab.set(i,j,slice_name,this->interp_linear(vals));
	}
      }

      return;
    }
    //@}

    /// \name Interpolation
    //@{
    /// Set interpolation type
    void set_interp_type(size_t interp_type) {
      itype=interp_type;
      return;
    }

    /** \brief Interpolate values \c vals into the tensor, 
	returning the result

	\warning This is being deprecated and may be removed
	or completely rewritten in later versions. 
      
	This is a quick and dirty implementation of n-dimensional
	interpolation by recursive application of the 1-dimensional
	routine from \ref interp_vec, using the base
	interpolation object specified in the template parameter \c
	base_interp_t. This will be slow for sufficiently large data
	sets.

	\future Maybe make this a template as well?

	\future It should be straightforward to improve the scaling of
	this algorithm significantly by creating a "window" of local
	points around the point of interest. This could be done easily
	by constructing an initial subtensor. However, this should
	probably be superceded by a more generic alternative which
	avoids explicit use of the 1-d interpolation types.
    */
    double interpolate(double *vals) {

      typedef interp_vec<ubvector> interp_t;
      
      if (rk==1) {
	
	interp_t si(size[0],grid,data,itype);
	return si.eval(vals[0]);

      } else {
	
	// Get total number of interpolations at this level
	size_t ss=1;
	for(size_t i=1;i<rk;i++) ss*=size[i];

	// Create space for y vectors and interpolators
	std::vector<ubvector> yvec(ss);
	std::vector<interp_t *> si(ss);
	for(size_t i=0;i<ss;i++) {
	  yvec[i].resize(size[0]);
	}
	
	// Create space for interpolation results
	tensor_grid tdat;
	ubvector_size_t_range size_new(size,range(1,rk));
	tdat.resize(rk-1,size_new);

	// Set grid for temporary tensor
	ubvector_range grid_new(grid,range(size[0],grid.size()));
	tdat.set_grid_packed(grid_new);
	
	// Create starting coordinate and counter
	ubvector_size_t co(rk);
	for(size_t i=0;i<rk;i++) co[i]=0;
	size_t cnt=0;

	// Loop over every interpolation
	bool done=false;
	while(done==false) {

	  // Fill yvector with the appropriate data
	  for(size_t i=0;i<size[0];i++) {
	    co[0]=i;
	    yvec[cnt][i]=get(co);
	  }
	  
	  si[cnt]=new interp_t(size[0],grid,yvec[cnt],itype);
	  
	  ubvector_size_t_range co2(co,range(1,rk));
	  tdat.set(co2,si[cnt]->eval(vals[0]));

	  // Go to next interpolation
	  cnt++;
	  co[rk-1]++;
	  // carry if necessary
	  for(int j=((int)rk)-1;j>0;j--) {
	    if (co[j]>=size[j]) {
	      co[j]=0;
	      co[j-1]++;
	    }
	  }

	  // Test if done
	  if (cnt==ss) done=true;

	  // End of while loop
	}
      
	// Now call the next level of interpolation
	double res=tdat.interpolate(vals+1);
	
	for(size_t i=0;i<ss;i++) {
	  delete si[i];
	}

	return res;
      }
    }

    /** \brief Perform a linear interpolation of \c v into the 
	function implied by the tensor and grid
	
	This performs multi-dimensional linear interpolation (or
	extrapolation) It works by first using \ref o2scl::search_vec
	to find the interval containing (or closest to) the specified
	point in each direction and constructing the corresponding
	hypercube of size \f$ 2^{\mathrm{rank}} \f$ containing \c v.
	It then calls \ref interp_linear_power_two() to perform the
	interpolation in that hypercube.
    */
    template<class vec2_t> double interp_linear(vec2_t &v) {

      // Find the the corner of the hypercube containing v
      size_t rgs=0;
      std::vector<size_t> loc(rk);
      std::vector<double> gnew;
      for(size_t i=0;i<rk;i++) {
	std::vector<double> grid_unpacked(size[i]);
	for(size_t j=0;j<size[i];j++) {
	  grid_unpacked[j]=grid[j+rgs];
	}
	search_vec<std::vector<double> > sv(size[i],grid_unpacked);
	loc[i]=sv.find(v[i]);
	gnew.push_back(grid_unpacked[loc[i]]);
	gnew.push_back(grid_unpacked[loc[i]+1]);
	rgs+=size[i];
      }

      // Now construct a 2^{rk}-sized tensor containing only that 
      // hypercube
      std::vector<size_t> snew(rk);
      for(size_t i=0;i<rk;i++) {
	snew[i]=2;
      }
      tensor_grid tnew(rk,snew);
      tnew.set_grid_packed(gnew);
      
      // Copy over the relevant data
      for(size_t i=0;i<tnew.total_size();i++) {
	std::vector<size_t> index_new(rk), index_old(rk);
	tnew.unpack_indices(i,index_new);
	for(size_t j=0;j<rk;j++) index_old[j]=index_new[j]+loc[j];
	tnew.set(index_new,get(index_old));
      }
      
      // Now use interp_power_two()
      return tnew.interp_linear_power_two(v);
    }
    
    /** \brief Perform linear interpolation assuming that all
	indices can take only two values

	This function works by recursively slicing the hypercube of
	size \f$ 2^{\mathrm{rank}} \f$ into a hypercube of size \f$
	2^{\mathrm{rank-1}} \f$ performing linear interpolation for
	each pair of points.
    */
    template<class vec2_t> double interp_linear_power_two(vec2_t &v) {

      if (rk==1) {
	return data[0]+(data[1]-data[0])/(grid[1]-grid[0])*(v[0]-grid[0]);
      }

      size_t last=rk-1;
      double frac=(v[last]-get_grid(last,0))/
	(get_grid(last,1)-get_grid(last,0));

      // Create new size vector and grid
      tensor_grid tnew(rk-1,size);
      tnew.set_grid_packed(grid);

      // Create data in new tensor, removing the last index through
      // linear interpolation
      for(size_t i=0;i<tnew.total_size();i++) {
	std::vector<size_t> index(rk);
	tnew.unpack_indices(i,index);
	index[rk-1]=0;
	double val_lo=get(index);
	index[rk-1]=1;
	double val_hi=get(index);
	tnew.set(index,val_lo+frac*(val_hi-val_lo));
      }
      
      // Recursively interpolate the smaller tensor
      return tnew.interp_linear_power_two(v);
    }
    //@}

    friend void o2scl_hdf::hdf_output(o2scl_hdf::hdf_file &hf, tensor_grid &t, 
				      std::string name);
    
    friend void o2scl_hdf::hdf_input(o2scl_hdf::hdf_file &hf, tensor_grid &t, 
				     std::string name);

  };

  /** \brief Rank 1 tensor with a grid
      
      \future Make rank-specific get_val and set_val functions?
   */
  class tensor_grid1 : public tensor_grid {
     
  public:
     
    /// Create an empty tensor
  tensor_grid1() : tensor_grid() {}
      
    /// Create a rank 2 tensor of size \c (sz,sz2,sz3)
  tensor_grid1(size_t sz) : tensor_grid() {
      this->rk=1;
      this->size.resize(1);
      this->size[0]=sz;
      this->data.resize(sz);
      this->grid_set=false;
    }
    
    virtual ~tensor_grid1() {
    }
   
    /// Get the element indexed by \c (ix1)
    double &get(size_t ix1) { 
      size_t sz[1]={ix1};
      return tensor_grid::get(sz); 
    }

    /// Get the element indexed by \c (ix1)
    const double &get(size_t ix1) const { 
      size_t sz[1]={ix1};
      return tensor_grid::get(sz); 
    }
 
    /// Set the element indexed by \c (ix1) to value \c val
    void set(size_t ix1, double val) {
      size_t sz[1]={ix1};
      tensor_grid::set(sz,val); 
    }

    /// Interpolate \c x and return the results
    double interp(double x) {
      return interpolate(&x);
    }

    /// Interpolate \c x and return the results
    double interp_linear(double x) {
      double arr[1]={x};
      return tensor_grid::interp_linear(arr);
    }
  };
  
  /** \brief Rank 2 tensor with a grid
   */
  class tensor_grid2 : public tensor_grid {
     
  public:
     
    /// Create an empty tensor
  tensor_grid2() : tensor_grid() {}

    /// Create a rank 2 tensor of size \c (sz,sz2)
  tensor_grid2(size_t sz, size_t sz2) : tensor_grid() {
      this->rk=2;
      this->size.resize(2);
      this->size[0]=sz;
      this->size[1]=sz2;
      size_t tot=sz*sz2;
      this->data.resize(tot);
      this->grid_set=false;
    }
   
    virtual ~tensor_grid2() {
    }
   
    /// Get the element indexed by \c (ix1,ix2)
    double &get(size_t ix1, size_t ix2) { 
      size_t sz[2]={ix1,ix2};
      return tensor_grid::get(sz); 
    }

    /// Get the element indexed by \c (ix1,ix2)
    const double &get(size_t ix1, size_t ix2) const { 
      size_t sz[2]={ix1,ix2};
      return tensor_grid::get(sz); 
    }
 
    /// Set the element indexed by \c (ix1,ix2) to value \c val
    void set(size_t ix1, size_t ix2, double val) {
      size_t sz[2]={ix1,ix2};
      tensor_grid::set(sz,val); 
      return;
    }

    /// Interpolate \c (x,y) and return the results
    double interp(double x, double y) {
      double arr[2]={x,y};
      return interpolate(arr);
    }

    /// Interpolate \c (x,y) and return the results
    double interp_linear(double x, double y) {
      double arr[2]={x,y};
      return tensor_grid::interp_linear(arr);
    }
  };
  
  /** \brief Rank 3 tensor with a grid
   */
  class tensor_grid3 : public tensor_grid {
     
  public:
     
    /// Create an empty tensor
  tensor_grid3() : tensor_grid () {}

    /// Create a rank 3 tensor of size \c (sz,sz2,sz3)
  tensor_grid3(size_t sz, size_t sz2, size_t sz3) : tensor_grid () {
      this->rk=3;
      this->size.resize(3);
      this->size[0]=sz;
      this->size[1]=sz2;
      this->size[2]=sz3;
      size_t tot=sz*sz2*sz3;
      this->data.resize(tot);
      this->grid_set=false;
    }
   
    virtual ~tensor_grid3() {
    }
   
    /// Get the element indexed by \c (ix1,ix2,ix3)
    double &get(size_t ix1, size_t ix2, size_t ix3) { 
      size_t sz[3]={ix1,ix2,ix3};
      return tensor_grid::get(sz); 
    }
 
    /// Get the element indexed by \c (ix1,ix2,ix3)
    const double &get(size_t ix1, size_t ix2, size_t ix3) const { 
      size_t sz[3]={ix1,ix2,ix3};
      return tensor_grid::get(sz); 
    }
 
    /// Set the element indexed by \c (ix1,ix2,ix3) to value \c val
    void set(size_t ix1, size_t ix2, size_t ix3, double val) {
      size_t sz[3]={ix1,ix2, ix3};
      tensor_grid::set(sz,val); 
      return;
    }

    /// Interpolate \c (x,y,z) and return the results
    double interp(double x, double y, double z) {
      double arr[3]={x,y,z};
      return interpolate(arr);
    }

    /// Interpolate \c (x,y,z) and return the results
    double interp_linear(double x, double y, double z) {
      double arr[3]={x,y,z};
      return tensor_grid::interp_linear(arr);
    }
  };
  
  /** \brief Rank 4 tensor with a grid
   */
  class tensor_grid4 : public tensor_grid {
     
  public:
     
    /// Create an empty tensor
  tensor_grid4() : tensor_grid () {}

    /// Create a rank 4 tensor of size \c (sz,sz2,sz3,sz4)
  tensor_grid4(size_t sz, size_t sz2, size_t sz3,
	       size_t sz4) : tensor_grid () {
      this->rk=4;
      this->size.resize(4);
      this->size[0]=sz;
      this->size[1]=sz2;
      this->size[2]=sz3;
      this->size[3]=sz4;
      size_t tot=sz*sz2*sz3*sz4;
      this->data.resize(tot);
      this->grid_set=false;
    }
   
    virtual ~tensor_grid4() {
    }
   
    /// Get the element indexed by \c (ix1,ix2,ix3,ix4)
    double &get(size_t ix1, size_t ix2, size_t ix3, size_t ix4) { 
      size_t sz[4]={ix1,ix2,ix3,ix4};
      return tensor_grid::get(sz); 
    }
    
    /// Get the element indexed by \c (ix1,ix2,ix3,ix4)
    const double &get(size_t ix1, size_t ix2, size_t ix3,
		      size_t ix4) const { 
      size_t sz[4]={ix1,ix2,ix3,ix4};
      return tensor_grid::get(sz); 
    }
    
    /// Set the element indexed by \c (ix1,ix2,ix3,ix4) to value \c val
    void set(size_t ix1, size_t ix2, size_t ix3, size_t ix4,
	    double val) {
      size_t sz[4]={ix1,ix2,ix3,ix4};
      tensor_grid::set(sz,val); 
      return;
    }

    /// Interpolate \c (x,y,z,a) and return the results
    double interp(double x, double y, double z, double a) {
      double arr[4]={x,y,z,a};
      return interpolate(arr);
    }

    /// Interpolate \c (x,y,z,a) and return the results
    double interp_linear(double x, double y, double z, double a) {
      double arr[4]={x,y,z,a};
      return tensor_grid::interp_linear(arr);
    }
  };
  
#ifndef DOXYGEN_NO_O2NS
}
#endif

#endif



