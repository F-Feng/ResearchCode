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
/** \file eos_tov.h
    \brief File defining \ref o2scl::eos_tov
*/
#ifndef O2SCL_TOV_EOS_H
#define O2SCL_TOV_EOS_H

#include <cmath>
#include <iostream>
#include <fstream>

#include <boost/numeric/ublas/vector.hpp>

#include <o2scl/constants.h>
#include <o2scl/lib_settings.h>
#include <o2scl/interp.h>
#include <o2scl/table_units.h>
#include <o2scl/vector_derint.h>

#ifndef DOXYGEN_NO_O2NS
namespace o2scl {
#endif

  /** \brief A EOS base class for the TOV solver
  */
  class eos_tov {

  protected:
    
    /** \brief Set to true if the baryon density is provided in the
	EOS (default false)
    */
    bool baryon_column;
    
    friend class tov_solve;

  public:
    
    eos_tov() {
      baryon_column=false;
      verbose=1;
    }

    virtual ~eos_tov() {}
    
    /// Control for output (default 1)
    int verbose;

    /** \brief Given the pressure, produce the energy and number densities

	The arguments \c P and \c e should always be in \f$
	M_{\odot}/\mathrm{km}^3 \f$ . The argument for \c nb should be
	in \f$ \mathrm{fm}^{-3} \f$ .
	
	If \ref baryon_column is false, then \c nb is unmodified.
    */
    virtual void get_eden(double P, double &e, double &nb)=0;
    
    /** \brief Given the pressure, produce all the remaining quantities 
	
	The argument \c P should always be in \f$
	M_{\odot}/\mathrm{km}^3 \f$ .
    */
    virtual void get_aux(double P, size_t &np, std::vector<double> &auxp) {
      np=0;
      return;
    }
    
    /** \brief Fill a list with strings for the names of the remaining 
	quanities
    */
    virtual void get_names_units(size_t &np, 
				std::vector<std::string> &pnames,
				std::vector<std::string> &punits) {
      np=0;
      return;
    }

    /** \brief Check that the baryon density is consistent 
	with the EOS
     */
    void check_nb(double &avg_abs_dev, double &max_abs_dev) {
      if (!baryon_column) {
	O2SCL_ERR2("Variable 'baryon_column' false in",
		   "eos_tov::check_nb().",exc_einval);
      }
      std::vector<double> edv, prv, nbv, dedn;
      for (double pres=0.1;pres<3.0;pres*=1.001) {
	double eps, nb;
	get_eden(pres,eps,nb);
	edv.push_back(eps);
	prv.push_back(pres);
	nbv.push_back(nb);
      }
      dedn.resize(edv.size());
      vector_deriv_interp(edv.size(),nbv,edv,dedn,itp_linear);
      avg_abs_dev=0.0;
      max_abs_dev=0.0;
      for(size_t i=0;i<edv.size();i++) {
	double abs_dev=(fabs(edv[i]+prv[i]-dedn[i]*nbv[i])/
		      fabs(dedn[i]*nbv[i]));
	if (abs_dev>max_abs_dev) max_abs_dev=abs_dev;
	avg_abs_dev+=abs_dev;
      }
      avg_abs_dev/=((double)edv.size());
      return;
    }

  };

  /** \brief The Buchdahl EOS for the TOV solver

      Given the EOS
      \f[
      \varepsilon = 12 \sqrt{p_{*} P}- 5 P
      \f]
      the TOV equation has an analytical solution
      \f[
      R=(1-\beta) \sqrt{\frac{\pi}{288 p_{*} G (1-2 \beta)}}
      \f]
      where \f$ \beta = GM/R \f$.

      The baryon chemical potential is
      \f[
      \mu = \mu_1 \left[ \frac{\left(9 p_{*}-P_1\right) \left(3+t_1\right)
      \left(3-t_2\right)}{\left(9 p_{*}-P\right)\left(3-t_1\right)
      \left(3+t_2\right)}\right]^{1/4}
      \f]
      where \f$ t_1 = \sqrt{P}/\sqrt{p_{*}} \f$ and \f$ t_2 =
      \sqrt{P_1/p_{*}} \f$ . The baryon density can then be obtained
      directly from the thermodynamic identity. In the case that one
      assumes \f$ \mu_1 = m_n \f$ and \f$ P_1 = 0 \f$, the baryon
      density can be simplified to
      \f[
      n m_n = 12 \sqrt{P p_{*}} \left( 1-\frac{1}{3} \sqrt{P/p_{*}} 
      \right)^{3/2}
      \f]
      c.f. Eq. 10 in \ref Lattimer01. 

      The central pressure and energy density are
      \f[
      P_c = 36 p_{*} \beta^2 
      \f]
      \f[
      {\varepsilon}_c = 72 p_{*} \beta (1-5 \beta/2) 
      \f]

      Physical solutions are obtained only for \f$ P< 25 p_{*}/144 \f$
      (ensuring that the argument to the square root is positive)
      and \f$ \beta<1/6 \f$ (ensuring that the EOS is not acausal). 

      Based on \ref Lattimer01 .

      \future Figure out what to do with the buchfun() function
  */
  class eos_tov_buchdahl : public eos_tov {
    
  protected:

    /** \brief The baryon density at \c ed1
     */
    double nb1;

    /** \brief The energy density for which the baryon density is known
     */
    double ed1;

    /** \brief The pressure at \c ed1
     */
    double pr1;

  public:

    eos_tov_buchdahl() {
      Pstar=3.2e-5;
    }

    virtual ~eos_tov_buchdahl() {}

    /** \brief The parameter with units of pressure in units of solar
	masses per km cubed (default value \f$ 3.2 \times 10^{-5} \f$
	)
    */
    double Pstar;
    
    /** \brief Set the baryon density
     */
    void set_baryon_density(double nb, double ed) {
      if (nb<0.0 || ed<0.0) {
	O2SCL_ERR2("Negative densities not supported in ",
		   "eos_tov_polytrope::set_coeff_index().",exc_einval);
      }
      baryon_column=true;
      nb1=nb;
      ed1=ed;
      if (36.0*Pstar*Pstar-5.0*Pstar*ed<0.0) {
	O2SCL_ERR2("Values of 'Pstar' and 'ed' incommensurate in ",
		   "eos_tov_buchdahl::set_baryon_density().",exc_einval);
      }
      pr1=0.04*(72.0*Pstar-5.0*ed+
		12.0*sqrt(36.0*Pstar*Pstar-5.0*Pstar*ed));
      return;
    }

    /** \brief Given the pressure, produce the energy and number densities
	
	If the baryon density is not specified, it should be set to
	zero or \ref baryon_column should be set to false
    */
    virtual void get_eden(double P, double &e, double &nb) {
      e=12.0*sqrt(Pstar*P)-5.0*P;
      if (baryon_column) {
	double mu1=(pr1+ed1)/nb1;
	//double t1=P/sqrt(P*Pstar);
	//double t2=pr1/sqrt(pr1*Pstar);
	double t1=sqrt(P/Pstar);
	double t2=sqrt(pr1/Pstar);
	double mu=mu1*pow((-pr1+9.0*Pstar)*(3.0+t1)*(3.0-t2)/
			  (-P+9.0*Pstar)/(3.0-t1)/(3.0+t2),0.25);
	nb=(P+e)/mu;
      } else {
	nb=0.0;
      }
      return;
    }
    
    /// Given the pressure, produce all the remaining quantities 
    virtual void get_aux(double P, size_t &np, std::vector<double> &auxp) {
      np=0;
      return;
    }
    
    /** \brief Fill a list with strings for the names of the remaining 
	quanities
    */
    virtual void get_names(size_t &np, std::vector<std::string> &pnames) {
      np=0;
      return;
    }
    
  protected:
    
    /** \brief Solve to compute profiles

	After solving the two equations
	\f{eqnarray*}
	r^{\prime} &=& r \left(1-\beta+u\right)^{-1} 
	\left(1 - 2 \beta\right) \nonumber \\
	A^2 &=& 288 \pi p_{*} G \left( 1 - 2 \beta \right)^{-1}
	\f}
	for \f$ u = \beta/(A r^{\prime}) \sin A r^{\prime} \f$ 
	and \f$ r^{\prime} \f$,
	one can compute the pressure and energy density 
	profiles
	\f{eqnarray*}
	8 \pi P &=& A^2 u^2 \left(1 - 2 \beta \right)
	\left(1 - \beta + u \right)^{-2}
	\nonumber \\
	8 \pi \varepsilon &=& 2 A^2 u \left(1 - 2 \beta\right)
	\left(1 - \beta - 3 u/2\right) \left(1 - \beta + u \right)^{-2}
	\nonumber \\
	\f}
	
    */
    int solve_u_rp_fun(size_t bv, const std::vector<double> &bx, 
		       std::vector<double> &by) {
      double u, rp;
      u=bx[0];
      rp=bx[1];
      //by[0]=rp*(1.0-beta+u)/(1.0-2.0*beta)-buchrad;
      //by[1]=beta/biga/rp*sin(biga*rp);
      return 0;
    }

  };

  /** \brief Standard polytropic EOS \f$ P = K \varepsilon^{1+1/n} \f$

      The quantity \f$ K \f$ must be in units of 
      \f$ \left(M_{\odot}/km^3\right)^{-1/n} \f$ .
  */
  class eos_tov_polytrope : public eos_tov {
    
  protected:

    /** \brief The baryon density at \c ed1
     */
    double nb1;

    /** \brief The energy density for which the baryon density is known
     */
    double ed1;

    /** \brief The pressure at \c ed1
     */
    double pr1;

    /** \brief Coefficient (default 1.0)
    */
    double K;

    /// Index (default 3.0)
    double n;

  public:

    eos_tov_polytrope() {
      K=1.0;
      n=3.0;
    }

    virtual ~eos_tov_polytrope() {}

    /** \brief Desc
     */
    void set_coeff_index(double coeff, double index) {
      if (coeff<0.0 || index<0.0) {
	O2SCL_ERR2("Negative coefficients and indices not supported in ",
		   "eos_tov_polytrope::set_coeff_index().",exc_einval);
      }
      K=coeff;
      n=index;
      if (baryon_column) {
	pr1=K*pow(ed1,1.0+1.0/n);
      }
    }
    
    /** \brief Set the baryon density
     */
    void set_baryon_density(double nb, double ed) {
      if (nb<0.0 || ed<0.0) {
	O2SCL_ERR2("Negative densities not supported in ",
		   "eos_tov_polytrope::set_coeff_index().",exc_einval);
      }
      baryon_column=true;
      nb1=nb;
      ed1=ed;
      pr1=K*pow(ed1,1.0+1.0/n);
      return;
    }

    /** \brief Given the pressure, produce the energy and number densities
     */
    virtual void get_eden(double P, double &e, double &nb) {
      e=pow(P/K,n/(1.0+n));
      if (baryon_column) {
	nb=nb1*pow(e/ed1,1.0+n)/pow((e+P)/(ed1+pr1),n);
      } else {
	nb=0.0;
      }
      return;
    }
    
  };

  /** \brief Linear EOS \f$ P = c_s^2 (\varepsilon-\varepsilon_0) \f$

      \note Experimental
   */
  class eos_tov_linear : public eos_tov {

  protected:

    /** \brief The baryon density at \c ed1
     */
    double nb1;

    /** \brief The energy density for which the baryon density is known
     */
    double ed1;

    /** \brief The pressure at \c ed1
     */
    double pr1;

    /** \brief Coefficient (default 1.0)
    */
    double cs2;

    /// Index (default 0.0)
    double eps0;

  public:

    eos_tov_linear() {
      cs2=1.0;
      eps0=0.0;
    }

    virtual ~eos_tov_linear() {}

    /** \brief Desc
     */
    void set_cs2_eps0(double cs2_, double eps0_) {
      eps0=eps0_;
      cs2=cs2_;
      return;
    }

    /** \brief Set the baryon density
     */
    void set_baryon_density(double nb, double ed) {
      if (nb<0.0 || ed<0.0) {
	O2SCL_ERR2("Negative densities not supported in ",
		   "eos_tov_polytrope::set_coeff_index().",exc_einval);
      }
      baryon_column=true;
      nb1=nb;
      ed1=ed;
      pr1=cs2*(ed-eps0);
      return;
    }

    /** \brief Given the pressure, produce the energy and number densities
    */
    virtual void get_eden(double P, double &e, double &nb) {
      e=P/cs2+eps0;
      if (baryon_column) {
	nb=nb1*pow(e+cs2*e-cs2*eps0,1.0/(1.0+cs2))*
	  pow(ed1+cs2*(-eps0+ed1),-1.0/(1.0+cs2));
      } else {
	nb=0.0;
      }
      return;
    }
    
    /// Given the pressure, produce all the remaining quantities 
    virtual void get_aux(double P, size_t &np, std::vector<double> &auxp) {
      np=0;
      return;
    }
    
    /** \brief Fill a list with strings for the names of the remaining 
	quanities
    */
    virtual void get_names(size_t &np, std::vector<std::string> &pnames) {
      np=0;
      return;
    }
    
  };

  /** \brief An EOS for the TOV solver using simple linear
      interpolation and an optional crust EOS

      The simplest usage of this class is simply to use \ref
      read_table() to read a tabulated EOS stored in a \ref
      table_units object and optionally specify a separate crust EOS.

      \note This stores a pointer to the user-specified table,
      so if that pointer becomes invalid, the interpolation will
      fail.

      Alternatively, the user can simply specify objects
      of type <tt>std::vector<double></tt> which store the energy
      density, pressure, and baryon density. 

      There are two methods to handle the crust-core interface. The
      default, <tt>smooth_trans</tt> uses the crust below pressure \f$
      P_1 \f$ (equal to the value of \ref trans_pres divided by \ref
      trans_width) and the core above pressure \f$ P_2 \f$ (the value
      of \ref trans_pres times \ref trans_width) and then in between
      uses
      \f[
      \varepsilon(P) = [1-\chi(P)] \varepsilon_{\mathrm{crust}}(P) + 
      \chi(P) \varepsilon_{\mathrm{core}}(P)
      \f]
      where the value \f$ \chi(P) \f$ is determined by
      \f[
      \chi(P) = (P-P_1)/(P_2-P_1) \, .
      \f]
      This method is a bit more faithful to the original EOS tables,
      but the matching can result in pressures which decrease with
      increasing energy density. Alternatively the <tt>match_line</tt>
      method uses
      \f$ \varepsilon_1=\varepsilon_{\mathrm{crust}}(P_1) \f$ and 
      \f$ \varepsilon_2=\varepsilon_{\mathrm{core}}(P_2) \f$ and
      \f[
      \varepsilon(P) = (\varepsilon_2 - \varepsilon_1) \chi 
      + \varepsilon_1 \, .
      \f]
      (using the same expression for \f$ \chi \f$ ). This method less
      frequently results in decreasing pressures, but can deviate
      further from the original tables.

      Internally, energy and pressure are stored in units of \f$
      \mathrm{M}_{\odot}/\mathrm{km}^3 \f$ and baryon density is
      stored in units of \f$ \mathrm{fm}^{-3} \f$ . The user-specified
      EOS table is left as is, and unit conversion is performed as
      needed in get_eden() and other functions from the units
      specified in the input \ref table_units object.

      \todo It might be useful to exit more gracefully when non-finite
      values are obtained in interpolation, analogous to the
      err_nonconv mechanism elsewhere.

      \todo Create a sanity check where core_auxp is nonzero only if
      core_table is also nonzero. Alternatively, this complication is
      due to the fact that this class works in two ways: one where it
      reads a table (and adds a crust), and one where it reads in
      vectors (with no crust). Maybe these two modes of operation
      should be separated into two classes.

      \todo Read different vector types than just std::vector<>. 
  */
  class eos_tov_interp : public eos_tov {
    
  public:
    
    eos_tov_interp();

    virtual ~eos_tov_interp();

    /// \name Mode of transitioning between crust and core EOS
    //@{
    int transition_mode;
    static const int smooth_trans=0;
    static const int match_line=1;
    //@}

    /// \name Basic usage
    //@{
    /// Specify the EOS through a table
    void read_table(table_units<> &eosat, std::string s_cole, 
		    std::string s_colp, std::string s_colnb="");
    
    /** \brief Read the EOS from a set of equal length
	vectors for energy density, pressure, and baryon density
     */
    void read_vectors(size_t n_core, std::vector<double> &core_ed, 
		      std::vector<double> &core_pr, 
		      std::vector<double> &core_nb);
    
    /** \brief Read the EOS from a pair of equal length
	vectors for energy density and pressure
    */
    void read_vectors(size_t n_core, std::vector<double> &core_ed, 
		      std::vector<double> &core_pr);
    //@}
    
    /// \name Crust EOS functions
    //@{
    /// Default crust EOS from \ref Negele73 and \ref Baym71
    void default_low_dens_eos();

    /// Crust EOS from \ref Shen11b
    void sho11_low_dens_eos();

    /** \brief Crust EOS from \ref Steiner12

	Current acceptable values for \c model are <tt>APR</tt>,
	<tt>Gs</tt>, <tt>Rs</tt> and <tt>SLy4</tt>.
    */
    void s12_low_dens_eos(std::string model="SLy4",
			      bool external=false);

    /** \brief Crust EOS from Goriely, Chamel, and Pearson
	
	From \ref Goriely10, \ref Pearson11, and \ref Pearson12 .
     */
    void gcp10_low_dens_eos(std::string model="BSk20",
			  bool external=false);

    /** \brief Crust EOS from \ref Newton13 given L in MeV

	Current acceptable values for \c model are <tt>PNM</tt>
	and <tt>J35</tt>. 

     */
    void ngl13_low_dens_eos(double L, std::string model="PNM",
			     bool external=false);
    
    /** \brief Crust EOS from \ref Newton13 given S and L in MeV
	and a transition density

	Note that this function works only if \f$ 28 < S < 38 \f$ MeV,
	\f$ 25 < L < 115 \f$ MeV, \f$ 0.01 < n_t < 0.15 \f$, 
	and \f$ L > 5 S-65~\mathrm{MeV} \f$
	. If \c fname is a string of length 0 (the default),
	then this function looks for a file named \c newton_SL.o2
	in the \o2 data directory specified by
	\ref o2scl::lib_settings_class::get_data_dir() .
    */
    void ngl13_low_dens_eos2(double S, double L, double nt,
			     std::string fname="");
    
    /// Compute with no crust EOS
    void no_low_dens_eos() {
      use_crust=false;
      return;
    }
    //@}

    /// \name Functions used by the tov_solve class
    //@{
    /** \brief Given the pressure, produce the energy and number densities

	The arguments \c P and \c e should always be in \f$
	M_{\odot}/\mathrm{km}^3 \f$ . The argument for \c nb should be
	in \f$ \mathrm{fm}^{-3} \f$ .
	
	If the baryon density is not specified, it should be set to
	zero or \ref baryon_column should be set to false
    */
    virtual void get_eden(double pres, double &ed, double &nb);

    /** \brief Given the pressure, produce all the remaining quantities 
	
	The argument \c P should always be in
	\f$ M_{\odot}/\mathrm{km}^3 \f$ .
    */
    virtual void get_aux(double P, size_t &nv, std::vector<double> &auxp);
    
    /** \brief Fill a list with strings for the names of the remaining 
	quanities
    */
    virtual void get_names_units(size_t &np, 
				 std::vector<std::string> &pnames,
				 std::vector<std::string> &punits);
    //@}

    /// \name Other functions
    //@{
    /** \brief Get the energy density from the pressure in the 
	user-specified unit system
    */
    virtual void get_eden_user(double pres, double &ed, double &nb);

    /** \brief Get the transition pressure (in the user-specified
	unit system) and width
    */
    void get_transition(double &ptrans, double &pwidth);
    
    /** \brief Set the transition pressure and "width"

	Sets the transition pressure and the width (specified as a
	number greater than unity in \c pw) of the transition between
	the two EOSs. The transition should be in the same units of
	the user-specified EOS. The transition is done smoothly using
	linear interpolation between \f$ P=\mathrm{ptrans}/pmathrm{pw}
	\f$ and \f$ P=\mathrm{ptrans} \times pmathrm{pw} \f$.
     */
    void set_transition(double ptrans, double pw);
    //@}

    /// \name User EOS
    //@{
    /// Energy densities from full EOS
    std::vector<double> full_vece;
    /// Pressures from full EOS
    std::vector<double> full_vecp;
    /// Baryon densities from full EOS
    std::vector<double> full_vecnb;
    /// Number of lines in full EOS
    size_t full_nlines;
    //@}

#ifndef DOXYGEN_INTERNAL

  protected:

    void internal_read();

    /// \name Crust EOS variables
    //@{
    /// Set to true if we are using a crust EOS (default false)
    bool use_crust;

    /// Energy densities
    std::vector<double> crust_vece;
    /// Pressures
    std::vector<double> crust_vecp;
    /// Baryon densities
    std::vector<double> crust_vecnb;
    /// Number of EOS entries
    size_t crust_nlines;
    //@}
    
    /// \name User EOS
    //@{
    /// True if core table has been specified
    bool core_set;
    /// Full user EOS table
    table_units<> *core_table;
    /// Column for energy density in EOS file
    size_t cole;
    /// Column for pressure in EOS file
    size_t colp;
    /// Column for baryon density in EOS file
    size_t colnb;
    /// True if an EOS has been specified
    bool eos_read;
    /// Number of additional columns in the core EOS
    size_t core_auxp;
    //@}

    /// \name Interpolation objects
    //@{
    interp_vec<std::vector<double> > pe_int;
    interp_vec<std::vector<double> > pnb_int;
    interp<std::vector<double> > gen_int;
    //@}

    /// \name Unit conversion factors for core EOS
    //@{
    /// Unit conversion factor for energy density (default 1.0)
    double efactor;
    /// Unit conversion factor for pressure (default 1.0)
    double pfactor;
    /// Unit conversion factor for baryon density (default 1.0)
    double nfactor;
    //@}

    /// \name Properties of transition
    //@{
    /** \brief Transition pressure (in \f$ M_{\odot}/\mathrm{km}^3 \f$)
     */
    double trans_pres;
    /// Transition width (unitless)
    double trans_width;
    //@}

#endif

  };

#ifndef DOXYGEN_NO_O2NS
}
#endif

#endif


