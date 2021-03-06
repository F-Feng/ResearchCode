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
#include <o2scl/eos_nse.h>

using namespace std;
using namespace o2scl;
using namespace o2scl_const;

eos_nse::eos_nse() {
  root=&def_root;
}

void eos_nse::calc_mu(double mun, double mup, double T,
		     double &nb, double &Ye, thermo &th, vector<nucleus> &nd) {

  nb=0.0;
  Ye=0.0;

  for (vector<nucleus>::iterator ndi=nd.begin();ndi!=nd.end();ndi++) {
    ndi->mu=mun*ndi->N+mup*ndi->Z-ndi->be;
    ndi->non_interacting=true;
    ndi->inc_rest_mass=false;
    cla.calc_mu(*ndi,T);
    nb+=ndi->n*((double)ndi->A);
    Ye+=ndi->n*((double)ndi->Z);
    th.ed+=ndi->ed;
    th.pr+=ndi->pr;
    th.en+=ndi->en;
  }

  Ye/=nb;

  return;
}

int eos_nse::calc_density(double nb, double Ye, double T, 
			  double &mun, double &mup, thermo &th, 
			  vector<nucleus> &nd) {

  ubvector x(2);
  x[0]=mun/T;
  x[1]=mup/T;

  solve_parms sp={nb,Ye,T,&nd};
  mm_funct11 mfm=std::bind
    (std::mem_fn<int(size_t,const ubvector &,ubvector &,solve_parms &)>
     (&eos_nse::solve_fun),
     this,std::placeholders::_1,std::placeholders::_2,
     std::placeholders::_3,std::ref(sp));

  int ret=root->msolve(2,x,mfm);
  // If the solver doesn't throw an exception because err_nonconv is
  // false, then just return the error value
  if (ret!=success) return ret;

  mun=x[0]*T;
  mup=x[1]*T;
  
  calc_mu(mun,mup,T,nb,Ye,th,nd);

  return success;
}

int eos_nse::solve_fun(size_t nv, const ubvector &x, ubvector &y, 
		       solve_parms &sp) {

  double mun=x[0]*sp.T;
  double mup=x[1]*sp.T;

  double nb, Ye;
  thermo th;
  
  calc_mu(mun,mup,sp.T,nb,Ye,th,*sp.ndp);
  
  y[0]=(nb-sp.nb)/sp.nb;
  y[1]=Ye-sp.Ye;

  if (nb<=0.0 || y[0]==-1.0) {
    return exc_ebadfunc;
  }
  if (!o2scl::is_finite(y[0]) || !o2scl::is_finite(y[1])) {
    O2SCL_ERR("Function values not finite in eos_nse::solve_fun().",
	      exc_efailed);
  }

  return success;
}

