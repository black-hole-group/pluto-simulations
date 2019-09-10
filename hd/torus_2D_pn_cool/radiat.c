#include "pluto.h"

/*Extern declaration to get x1 value*/
extern double g_radius;

/* ***************************************************************** */
void Radiat (double *v, double *rhs)
/*!
 *   Provide r.h.s. for tabulated cooling.
 * 
 ******************************************************************* */

{
	
	int klo, khi, kmid, repvar, index;
	static int length_cool, length_n, length_radius, length_T;
	double dn, nlo, nhi, nmid, n;
	double dradius, radiuslo, radiushi, radiusmid, radius;
	double dT, Tlo, Thi, Tmid, scrh, T;
	double c1, c2, c3, c4, c5, c6, c7, prs, mu_e, mu_i, tun_par, mu, bt;
	
	static double *T_tab, *n_tab, *radius_tab, *cool_tab, E_cost;

	FILE *fcool, *ftemp, *fndens, *fradius;

	if (T_tab == NULL || n_tab == NULL || radius_tab == NULL){
    		print1 (" > Reading table from disk...\n");
    		fcool = fopen("coolingtable.dat","r");
		ftemp = fopen("temperature.dat","r");;
		fndens = fopen("eletronicdensity.dat","r");
		fradius = fopen("radius.dat","r");

    		if (fcool == NULL || ftemp == NULL || fndens == NULL || fradius == NULL){
      			print1 ("! Radiat: one or more of the tables could not be found.\n");
     			QUIT_PLUTO(1);
    		}
    		cool_tab = ARRAY_1D(343000, double);
    		T_tab = ARRAY_1D(70, double);
		n_tab = ARRAY_1D(70, double);
		radius_tab = ARRAY_1D(70, double);

    		length_T = 0;
    		while (fscanf(ftemp, "%lf\n", T_tab + length_T)!=EOF) {
     	 		length_T++;
   		}

		length_n = 0;
    		while (fscanf(fndens, "%lf\n", n_tab + length_n)!=EOF) {
     	 		length_n++;
   		}

		length_radius = 0;
    		while (fscanf(fradius, "%lf\n", radius_tab + length_radius)!=EOF) {
     	 		length_radius++;
   		}

		length_cool = 0;
    		while (fscanf(fcool, "%lf\n", cool_tab + length_cool)!=EOF) {
     	 		length_cool++;
   		}
   		E_cost = UNIT_LENGTH/UNIT_DENSITY/pow(UNIT_VELOCITY, 3.0); 	/*Conversion constant of cooling rate (cm3 s / erg)*/
  	}

	/* Wait until simulation reaches steady state*/
	if (g_time < 8709.1 || v[TRC] < 0.99) {
    		rhs[RHOE] = 0.0;
    		return;
  	}


	/* ---------------------------------------------
       	    Get pressure, temperature, number density
	    and distance	 
   	--------------------------------------------- */

	prs = v[RHOE]*(g_gamma-1.0);						/*Eq 7.6 PLUTO*/
  	if (prs < 0.0) {							/*If pressure is < 0 we set a minimum pressure to calculate v[RHOE]*/
    		prs     = g_smallPressure;
    		v[RHOE] = prs/(g_gamma - 1.0);
  	}

	mu_e = 2.0/(1.0 + H_MASS_FRAC);						/*Effective mean molecular weight of electrons for fully ionized gas*/
	mu_i = 4.0/(1.0 + 3.0*H_MASS_FRAC);					/*Effective mean molecular weight of ions for fully ionized gas*/
  	mu  = MeanMolecularWeight(v);
	bt = 10.0;
  	T   = bt/(bt+1.0)*prs/v[RHO]*KELVIN*mu_e;						/*Eq 7.2 PLUTO*/
	

	if (T != T){								/*Verify if temperature is NaN*/
    		printf (" ! Nan found in radiat \n");
    		printf (" ! rho = %12.6e, prs = %12.6e\n",v[RHO], prs);
    		QUIT_PLUTO(1);
  	}

	/* ---------------------------------------------
       	    	    Get electron temperature	 
   	--------------------------------------------- */
	
	
	tun_par = 1.0;
	T = T / (1.0 + mu_e/mu_i * (pow(g_inputParam[RMAX]/g_radius, tun_par) + 2.0));				/*Electron Temperature according to Wu et al. 2016*/

  	if (T < g_minCoolingTemp) { 						/*Verify if it has reached the minimum temperature*/
    		rhs[RHOE] = 0.0;
    		return;
  	}
	
	radius = g_radius * UNIT_LENGTH;
	n = v[RHO] * UNIT_DENSITY / (mu_e * CONST_amu);

	/* ----------------------------------------------
        	Table lookup by binary search  
   	---------------------------------------------- */


	klo = 0;
	khi = length_n - 1;	

	while (klo != (khi - 1)) {						
   		kmid = (klo + khi) / 2;						
   		nmid = n_tab[kmid];						
    		if (n <= nmid) {						
     			khi = kmid;							
    		}else if (n > nmid) {						
 			klo = kmid;
    		 }
  	}

	dn = n_tab[khi] - n_tab[klo];
	nlo = n_tab[klo];
	nhi = n_tab[khi];
	

	repvar = length_cool / length_n;
	index = repvar * klo;

	klo = 0;
	khi = length_radius - 1;
	
	while (klo != (khi - 1)) {						
   		 kmid = (klo + khi) / 2;						
    		 radiusmid = radius_tab[kmid];						
    		 if (radius <= radiusmid) {						
     		 	khi = kmid;							
    		 }else if (radius > radiusmid) {						
 		  	klo = kmid;
    		  }
  	}
	
	dradius = radius_tab[khi] - radius_tab[klo];
	radiuslo = radius_tab[klo];
	radiushi = radius_tab[khi];

	repvar = repvar / length_radius;
	index += repvar * klo;

	klo = 0;
	khi = length_T - 1;

	while (klo != (khi - 1)) {						
   		 kmid = (klo + khi) / 2;						
    		 Tmid = T_tab[kmid];						
    		 if (T <= Tmid) {						
     		 	khi = kmid;							
    		 }else if (T > Tmid) {						
 		  	klo = kmid;
    		  }
  	}

	dT = T_tab[khi] - T_tab[klo];
	Tlo = T_tab[klo];
	Thi = T_tab[khi];

	index += klo;

	/*---------------------------------
	   Weighing the cooling rate value
	   and checking if it is out of
	   range
	  ---------------------------------*/
	c1 = dT * dn * dradius;
	
	if (T <= Tlo){
		c5 = 0.0;
		c2 = dT;	
	}
	else if (T >= Thi){
		c5 = dT;
	 	c2 = 0.0;
 	}
	else {
		c5 = T - Tlo;
		c2 = Thi - T;
	}

	if (radius <= radiuslo){
		c6 = 0.0;
		c3 = dradius;	
	}
	else if (radius >= radiushi){
		c6 = dradius;
	 	c3 = 0.0;
 	}
	else {
		c6 = radius - radiuslo;
		c3 = radiushi - radius;
	}

	if (n <= nlo){
		c7 = 0.0;
		c4 = dn;	
	}
	else if (n >= nhi){
		c7 = dn;
	 	c4 = 0.0;
 	}
	else {
		c7 = n - nlo;
		c4 = nhi - n;
	}

	scrh = cool_tab[index] * c2 * c3 * c4;
	index++;
	scrh += cool_tab[index] * c5 * c3 * c4;
	index += - 1 + length_T;
	scrh += cool_tab[index] * c2 * c6 * c4;
	index++;
	scrh += cool_tab[index] * c5 * c6 * c4;
	index += length_T * (length_radius - 1) - 1;
	scrh += cool_tab[index] * c2 * c3 * c7;
	index++;
	scrh += cool_tab[index] * c5 * c3 * c7;
	index += length_T - 1;
	scrh += cool_tab[index] * c2 * c6 * c7;
	index++;
	scrh += cool_tab[index] * c5 * c6 * c7;
	scrh = scrh / c1;

	/* -----------------------------------------------
    		Compute r.h.s
   	----------------------------------------------- */

	scrh = -scrh*E_cost;
	if (scrh < 0.){										
  		rhs[RHOE] = scrh;							/*Eq 9.1 and converting cooling rate to code units*/
	}										/*checking if cooling rate is not negative (heating)*/
	else{
		rhs[RHOE] = 0.;
	}
}



