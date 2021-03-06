/**
 * @file potential.cpp
 * @author Adrian Del Maestro
 *
 * @brief Implementation of all potential types.
 */

#include "potential.h"
#include "path.h"
#include "lookuptable.h"
#include "communicator.h"
#include <fstream>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// POTENTIAL BASE CLASS ------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
******************************************************************************/
PotentialBase::PotentialBase () : tailV(0.0) {

}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
PotentialBase::~PotentialBase () {
}

/**************************************************************************//**
 * Return an initial particle configuration.
 *
 * The default version creates a list of particle positions in an equally
 * spaced grid.
******************************************************************************/
Array<dVec,1> PotentialBase::initialConfig(const Container *boxPtr, MTRand &random,
		const int numParticles) {

	/* The particle configuration */
	Array<dVec,1> initialPos(numParticles);
	initialPos = 0.0;

	/* Get the linear size per particle, and the number of particles */
	double initSide = pow((1.0*numParticles/boxPtr->volume),-1.0/(1.0*NDIM));

	/* We determine the number of initial grid boxes there are in
	 * in each dimension and compute their size */
	int totNumGridBoxes = 1;
	iVec numNNGrid;
	dVec sizeNNGrid;

	for (int i = 0; i < NDIM; i++) {
		numNNGrid[i] = static_cast<int>(ceil((boxPtr->side[i] / initSide) - EPS));

		/* Make sure we have at least one grid box */
		if (numNNGrid[i] < 1)
			numNNGrid[i] = 1;

		/* Compute the actual size of the grid */
		sizeNNGrid[i] = boxPtr->side[i] / (1.0 * numNNGrid[i]);

		/* Determine the total number of grid boxes */
		totNumGridBoxes *= numNNGrid[i];
	}

	/* Now, we place the particles at the middle of each box */
	PIMC_ASSERT(totNumGridBoxes>=numParticles);
	dVec pos;
	for (int n = 0; n < totNumGridBoxes; n++) {

		iVec gridIndex;
		for (int i = 0; i < NDIM; i++) {
			int scale = 1;
			for (int j = i+1; j < NDIM; j++)
				scale *= numNNGrid[j];
			gridIndex[i] = (n/scale) % numNNGrid[i];
		}

		for (int i = 0; i < NDIM; i++)
			pos[i] = (gridIndex[i]+0.5)*sizeNNGrid[i] - 0.5*boxPtr->side[i];

		boxPtr->putInside(pos);

		if (n < numParticles)
			initialPos(n) = pos;
		else
			break;
	}

	return initialPos;
}

/**************************************************************************//**
 * Ouptut the potential.
 *
 * For use during comparison and debugging, we output the potential out to
 * a supplied separation.
 *
 * @param maxSep the maximum separation
******************************************************************************/
void PotentialBase::output(const double maxSep) {
	dVec sep;
	sep = 0.0;
    cout << "Hello this is potential base";
	for (double d = 0; d < maxSep; d+= (maxSep/1000.0)) {
		sep[0] = d;
		communicate()->file("debug")->stream()
            << format("%10.4E\t%16.8E\n") % d % V(sep);
	}
    communicate()->file("debug")->stream().flush();
}

/**************************************************************************//**
 * Initialize getExcLen method.
 *
 * This is only used for Gasparini potential, could probably be better.
******************************************************************************/
Array<double,1> PotentialBase::getExcLen() {
	/* The particle configuration */
	Array<double,1> excLens(0);
	return excLens;
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// TABULATED POTENTIAL CLASS -------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
******************************************************************************/
TabulatedPotential::TabulatedPotential() {
	extV = 0.0;
	extdVdr = 0.0;
    extd2Vdr2 = 0.0;
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
TabulatedPotential::~TabulatedPotential() {
	lookupV.free();
	lookupdVdr.free();
    lookupd2Vdr2.free();
}

/**************************************************************************//**
 *  Given a discretization factor and the system size, create and fill
 *  the 1D lookup tables for a 1D potential and its derivative.
******************************************************************************/
void TabulatedPotential::initLookupTable(const double _dr, const double maxSep) {

	/* We now calculate the lookup tables for the interaction potential and
	 * its first and second derivatives of the 1D case. */
	dr = _dr;
	tableLength = int(maxSep/dr);
	lookupV.resize(tableLength);
	lookupdVdr.resize(tableLength);
    lookupd2Vdr2.resize(tableLength);
	lookupV = 0.0;
	lookupdVdr = 0.0;
    lookupd2Vdr2 = 0.0;

	double r = 0;

	for (int n = 0; n < tableLength; n++) {
		lookupV(n)    = valueV(r);
		lookupdVdr(n) = valuedVdr(r);
        lookupd2Vdr2(n) = valued2Vdr2(r);
		r += dr;
	}

//	double rc = constants()->rc();
//	for (int n = 0; n < tableLength; n++) {
//		r += dr;
//		if (r <= rc) {
//			lookupV(n) = valueV(r) - valueV(rc) - valuedVdr(rc)*(r-rc);
//			lookupdVdr(n) = valuedVdr(r) - valuedVdr(rc);
//		}
//		else {
//			lookupV(n) = 0.0;
//			lookupdVdr(n) = 0.0;
//		}
//		cout << format("%16.8E%16.8E%16.8E%16.8E%16.8E%16.8E%16.8E\n") % r % lookupV(n) % valueV(r) %
//			lookupdVdr(n) % valuedVdr(r) % (lookupV(n) - valueV(r)) % (lookupdVdr(n) - valuedVdr(r));
//	}

}

void TabulatedPotential::initLookupTable(const double _dx, const double _dy, const double X, const double Y) {

	/* We now calculate the lookup tables for the interaction potential and
	 * its first and second derivatives of the 2D case. X is the length in the 1st dimension,
       Y is the length in the second. Notice this lookup table does not have to be square, and is
       only square in the special case where (X/dx) = (Y/dy). */
	dx = _dx;
	dy = _dy;
	double xLength = int(X/dx);
	double yLength = int(Y/dy);
	twoDlookupV.resize(xLength, yLength);
	twoDlookupdVdr.resize(xLength, yLength);
    twoDlookupd2Vdr2.resize(xLength, yLength);
	twoDlookupV = 0.0;
	twoDlookupdVdr = 0.0;
    twoDlookupd2Vdr2 = 0.0;

	double x = 0.0;
	double y = 0.0;

	for (int n = 0; n < xLength; n++)
    {
        for (int m = 0; m < yLength; m++)
        {
            dVec rVec;
            rVec[0] = x;
            rVec[1] = y;
            twoDlookupV(n,m) = valueV(rVec);
            twoDlookupdVdr(n,m) = valuedVdr(rVec);
            twoDlookupd2Vdr2(n,m) = valued2Vdr2(rVec);
            y += dy;
        }
        x += dx;
	}
}

/**************************************************************************//**
 *  Use the Newton-Gregory forward difference method to do a 2-point lookup
 *  on the potential table.
 *
 *  @see M.P. Allen and D.J. Tildesley, "Computer Simulation of Liquids"
 *  (Oxford Press, London, England) p 144 (2004).
******************************************************************************/
double TabulatedPotential::newtonGregory(const Array<double,1> &VTable,
		const TinyVector<double,2> &extVal, const double r) {

	double rdr = r/dr;
	int k = int(rdr);

	if (k <= 0)
		return extVal[0];

	if (k >= tableLength)
		return extVal[1];

	double xi = rdr - 1.0*k;
	double vkm1 = VTable(k-1);
	double vk = VTable(k);
	double vkp1 = VTable(k+1);

	double T1 = vkm1 + (vk - vkm1) * xi;
	double T2 = vk + (vkp1 - vk) * (xi - 1.0);

	return (T1 + 0.5 * (T2 - T1) * xi);
}

/**************************************************************************//**
 *  Use the bilinear interpolation method to do a 4-point lookup
 *  on the potential table.
 *  See Wikipedia: Bilinear Interpolation
******************************************************************************/
double TabulatedPotential::bilinearInterp(const Array<double,2> &VTable,
		const TinyVector<double,2> &extVal, const double x, const double y) {

    cout << "Hello this is base bilinear interpolation" << endl;
    double x1, x2, y1, y2, fx1y1, fx1y2, fx2y1, fx2y2;
    int xindex1, yindex1, xindex2, yindex2;

    // Get this indices for the interpolation points
    xindex1 = int(x/dx);
    xindex2 = xindex1 + 1;
    yindex1 = int(y/dy);
    yindex2 = yindex1 + 1;

    if ((xindex1 >= xLength) or (yindex1 >= yLength)){
        return extV[1];
        }
    else{
        // Get the x and y position values that exist in the lookup table
        x1 = xindex1*dx;
        x2 = xindex2*dx;
        y1 = yindex1*dy;
        y2 = yindex2*dy;

        // Get the lookup table values
        fx1y1 = VTable(xindex1, yindex1);
        fx1y2 = VTable(xindex1, yindex2);
        fx2y1 = VTable(xindex2, yindex1);
        fx2y2 = VTable(xindex2, yindex2);

        // Do the interpolation
        double interpval = (1.0/((x2-x1)*(y2-y1)))*(fx1y1*(x2-x)*(y2-y) + fx2y1*(x-x1)*(y2-y) + fx1y2*(x2-x)*(y-y1) + fx2y2*(x-x1)*(y-y1));
        return interpval;
        }
    }

/**************************************************************************//**
 *  Use a direct lookup for the potential table.
 *
 *  This is faster than Newton-Gregory and may give similar results for a fine
 *  enough mesh.
******************************************************************************/
double TabulatedPotential::direct(const Array<double,1> &VTable,
		const TinyVector<double,2> &extVal, const double r) {

	int k = int(r/dr);
	if (k <= 0)
		return extVal[0];

	if (k >= tableLength)
		return extVal[1];

    return VTable(k);
}

/**************************************************************************//**
 *  Use a direct lookup for the 2D potential table.
 *
 *  This is faster than Newton-Gregory and may give similar results for a fine
 *  enough mesh.
******************************************************************************/
double TabulatedPotential::direct(const Array<double,1> &VTable,
		const TinyVector<double,2> &extVal, const double x, const double y) {

	int xindex = int(x/dx);
	int yindex = int(y/dy);

    if ((xindex >= xLength) or (yindex >= yLength))
        {return extV[1];}
    else
        {return twoDlookupV(xindex, yindex);}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// FREE POTENTIAL CLASS ------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
******************************************************************************/
FreePotential::FreePotential() : PotentialBase() {
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
FreePotential::~FreePotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// SINGLE WELL POTENTIAL CLASS -----------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
******************************************************************************/
SingleWellPotential::SingleWellPotential() : PotentialBase() {
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
SingleWellPotential::~SingleWellPotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// HARMONIC POTENTIAL CLASS --------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
******************************************************************************/
HarmonicPotential::HarmonicPotential(double _omega) : PotentialBase() {
    omega2 = _omega*_omega;
}

HarmonicPotential::HarmonicPotential() : PotentialBase() {
    omega2 = 1.0;
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
HarmonicPotential::~HarmonicPotential() {
}

/**************************************************************************//**
 * Return an initial particle configuration.
 *
 * We create particles at random locations close to the origin.
******************************************************************************/
Array<dVec,1> HarmonicPotential::initialConfig(const Container *boxPtr, MTRand &random,
		const int numParticles) {

	/* The particle configuration */
	Array<dVec,1> initialPos(numParticles);
	initialPos = 0.0;

	for (int n = 0; n < numParticles; n++) {
		for (int i = 0; i < NDIM; i++)
			initialPos(n)[i] = 0.1*boxPtr->side[i]*(-1.0 + 2.0*random.rand());
	}

	return initialPos;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// HARMONIC TUBE POTENTIAL CLASS ---------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 * Constructor.
 *
 * Using a supplied tube radius, setup the soft harmonic tube potential.
******************************************************************************/
HarmonicCylinderPotential::HarmonicCylinderPotential(const double radius) : PotentialBase()
{
	/* c is a dimensionless constant */
	c = 1.20272;

	/* We have to determine the frequency of the oscillator from it's length.
	 * w = \hbar / (m R^2).  It is measured in THz */
	w = 6.35077 / (radius*radius*constants()->m());
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
HarmonicCylinderPotential::~HarmonicCylinderPotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// DELTA POTENTIAL CLASS -----------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *
 *  Setup the delta function strength and normalization constant.
 *  @param _a The width^2 of the Gaussian
 *  @param _c The integrated strength of the Gaussian
******************************************************************************/
DeltaPotential::DeltaPotential(double _a, double _c) : PotentialBase()
{
	/* Define the parameters of the delta function potential. */
	a = _a;
	c = _c;
	norm = c/sqrt(a*M_PI);
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
DeltaPotential::~DeltaPotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// LORENTZIAN POTENTIAL CLASS ------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *
 *  Setup the delta function strength and normalization constant.
 *  @param _a The width^2 of the Lorentzian
 *  @param _c The integrated strength of the Lorentzian
******************************************************************************/
LorentzianPotential::LorentzianPotential(double _a, double _c) : PotentialBase()
{
	/* Define the parameters of the Lorentzian delta function potential. */
	a = _a;
	c = _c;
	norm = 2.0 * c * a / M_PI;
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
LorentzianPotential::~LorentzianPotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// FIXED AZIZ POTENTIAL CLASS ------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *
 *  We load the positions of fixed but interacting particles from disk and
 *  create a new lookup table which will be used to speed up the computation.
 *  The interactions with the fixed particles are assumed to be Aziz.
******************************************************************************/
FixedAzizPotential::FixedAzizPotential(const Container *_boxPtr) :
	aziz(_boxPtr->side) {

	char state;				// Fixed or updateable?
	dVec pos;				// The loaded position

	/* Initialize the cutoff^2 */
	rc2 = constants()->rc2();

	/* We start with an array of size 500 */
	fixedParticles.resize(500);

	/* Here we load both the number and location of fixed helium atoms from disk. */
	numFixedParticles = 0;
	int n = 0;
	while (!communicate()->file("fixed")->stream().eof()) {
		if (communicate()->file("fixed")->stream().peek() == '#') {
			communicate()->file("fixed")->stream().ignore(512,'\n');
		}
		else {
			communicate()->file("fixed")->stream() >> state;
			for (int i = 0; i < NDIM; i++)
				communicate()->file("fixed")->stream() >> pos[i];

			/* If the particle is labelled with an 'F' it is fixed and should
			 * be included here */
			if (state == 'F') {
				numFixedParticles++;
				if (numFixedParticles >= int(fixedParticles.size()))
					fixedParticles.resizeAndPreserve(numFixedParticles);

				/* Put the initial position in the container */
				_boxPtr->putInside(pos);
				fixedParticles(n) = pos;
				n++;
			}
			communicate()->file("fixed")->stream().ignore();
		}
	}

	fixedParticles.resizeAndPreserve(numFixedParticles);

	/* Now that we have the particle positions, create a new lookup table pointer
	 * and initialize it */
	lookupPtr = new LookupTable(_boxPtr,1,numFixedParticles);
	lookupPtr->updateGrid(fixedParticles);

	/* Resize and initialize our local grid box arrays */
	fixedBeadsInGrid.resize(lookupPtr->getTotNumGridBoxes(),numFixedParticles);
	numFixedBeadsInGrid.resize(lookupPtr->getTotNumGridBoxes());
	fixedBeadsInGrid = XXX;
	numFixedBeadsInGrid = 0;

	/* Create a local copy of all beads in each grid box plus nearest neighbors.
	 * This will drastically speed up the computing of potential energies. */
	for (n = 0; n < lookupPtr->getTotNumGridBoxes(); n++) {
		lookupPtr->updateFullInteractionList(n,0);
		numFixedBeadsInGrid(n) = lookupPtr->fullNumBeads;
		for (int m = 0; m < lookupPtr->fullNumBeads; m++)
			fixedBeadsInGrid(n,m) = lookupPtr->fullBeadList(m)[1];
	}

}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
FixedAzizPotential::~FixedAzizPotential() {
	delete lookupPtr;
	fixedParticles.free();
	fixedBeadsInGrid.free();
	numFixedBeadsInGrid.free();
}

/**************************************************************************//**
 *  The total potential coming from the interaction of a particle with all
 *  fixed particles.
******************************************************************************/
double FixedAzizPotential::V(const dVec &pos) {

	double totV = 0.0;

	/* We first find the grid box number that the particle resides in */
	int gridNumber = lookupPtr->gridNumber(pos);

	/* We now loop over all fixed particles in this grid box, only computing
	 * interactions when the separation is less than the cutoff */
	dVec sep;
	for (int n = 0; n < numFixedBeadsInGrid(gridNumber); n++) {
		sep = fixedParticles(fixedBeadsInGrid(gridNumber,n)) - pos;
		lookupPtr->boxPtr->putInBC(sep);
		if (dot(sep,sep) < rc2)
			totV += aziz.V(sep);
	}

	return totV;
}

/**************************************************************************//**
 *  The gradient of the total potential coming from the interaction of a
 *  particle with all fixed particles.
******************************************************************************/
dVec FixedAzizPotential::gradV(const dVec &pos) {

	dVec totGradV;
	totGradV = 0.0;

	/* We first find the grid box number that the particle resides in */
	int gridNumber = lookupPtr->gridNumber(pos);

	/* We now loop over all fixed particles in this grid box, only computing
	 * the gradient of interactions when the separation is less than the cutoff */
	dVec sep;
	for (int n = 0; n < numFixedBeadsInGrid(gridNumber); n++) {
		sep = fixedParticles(fixedBeadsInGrid(gridNumber,n)) - pos;
		lookupPtr->boxPtr->putInBC(sep);
		if (dot(sep,sep) < rc2)
			totGradV += aziz.gradV(sep);
	}

	return totGradV;
}

/**************************************************************************//**
 * Return an initial particle configuration.
 *
 * We load an initial configuration from disk, which consists of a number of
 * updateable positions.  These positions are stored as NDIM vectors and
 * proceeded by a letter 'U'.
******************************************************************************/
Array<dVec,1> FixedAzizPotential::initialConfig(const Container *boxPtr, MTRand &random,
		const int numParticles) {

	/* The particle configuration */
	Array<dVec,1> initialPos(1);
	initialPos = 0.0;

	int locNumParticles = 0;	// Number of updateable particles
	char state;					// Update or Fix
	dVec pos;					// The current position

	/* We go through all lines in the fixed input file, discarding any comments
	 * and assign the initial positions of the particles */
	int n = 0;
	while (!communicate()->file("fixed")->stream().eof()) {
		if (communicate()->file("fixed")->stream().peek() == '#') {
			communicate()->file("fixed")->stream().ignore(512,'\n');
		}
		else {
			communicate()->file("fixed")->stream() >> state;
			for (int i = 0; i < NDIM; i++)
				communicate()->file("fixed")->stream() >> pos[i];

			/* If the particle is labelled with an 'U' it is updatable and should
			 * be included */
			if (state == 'U') {
				locNumParticles++;
				initialPos.resizeAndPreserve(locNumParticles);

				/* Put the initial position in the box */
				boxPtr->putInside(pos);

				/* Assign the position to all time slices*/
				initialPos(n) = pos;
				n++;
			}
			communicate()->file("fixed")->stream().ignore();
		}
	}

	/* Reset the file pointer */
	communicate()->file("fixed")->stream().seekg(0, ios::beg);

	/* Return the initial Positions */
	return initialPos;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// HARD CYLINDER POTENTIAL CLASS ---------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *  @param radius The radius of the cylinder
******************************************************************************/
HardCylinderPotential::HardCylinderPotential(const double radius) :
	PotentialBase(),
	R(radius) {
}

/**************************************************************************//**
 *  Destructor.
******************************************************************************/
HardCylinderPotential::~HardCylinderPotential() {
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// LJ CYLINDER POTENTIAL CLASS -----------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *
 *  We create a Lennard-Jones cylinder, which uses a lookup table to hold the
 *  value of the integrated 6-12 potential for helium atoms interacting
 *  with a silicon nitride cylinder.
 *  @see C. Chakravarty J. Phys. Chem. B,  101, 1878 (1997).
 *  @param radius The radius of the cylinder
******************************************************************************/
LJCylinderPotential::LJCylinderPotential(const double radius) :
	PotentialBase(),
	TabulatedPotential()
{

	/* The radius of the tube */
	R = radius;

	/* The density of nitrogen in silicon nitride */
	density = 0.078; // atoms / angstrom^3
//	density = 0.008; // atoms / angstrom^3

	/* We define the values of epsilon and sigma for N and He */
//	double epsilonHe = 10.216; 	// Kelvin
//	double sigmaHe   = 2.556; 	// angstroms
//	double sigmaN    = 3.299; 	// angstroms
//	double epsilonN  = 36.2;	// Kelvin
//	epsilon = sqrt(epsilonHe*epsilonN);
//	sigma = 0.5*(sigmaHe + sigmaN);

	/* We operate under the assumption that the silicon can be neglected in the
	 * silicon-nitride, and thus only consider the Nitrogen.  We use a
	 * Kiselov type model to extract the actual parameters.  We assume that
	 * silicate and silicon-nitride are roughly equivalent. */
    epsilon = 10.22; 	// Kelvin
	sigma   = 2.628;	// angstroms

//	epsilon = 32; 	// Kelvin
//	sigma   = 3.08;	// angstroms

    /* Debugging Params */
//    sigma = epsilon = density = 1.0;

	/* We choose a mesh consisting of 10^6 points, and create the lookup table */
	dR = (1.0E-6)*R;

	initLookupTable(dR,R);

	/* Find the minimun of the potential */
	minV = 1.0E5;
	for (int n = 0; n < tableLength; n++) {
		if (lookupV(n) < minV)
			minV = lookupV(n);
	}

	/* The extremal values for the lookup table */
	extV = valueV(0.0),valueV(R);
	extdVdr = valuedVdr(0.0),valuedVdr(R);
}

/**************************************************************************//**
 *  Destructor.
******************************************************************************/
LJCylinderPotential::~LJCylinderPotential() {
}

#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
/**************************************************************************//**
 *  Return the actual value of the LJ Cylinder potential, for a distance r
 *  from the surface of the wall.
 *
 *  Checked and working with Mathematica.
******************************************************************************/
double LJCylinderPotential::valueV(const double r) {
	double x = r / R;

	if (x >= 1.0)
		x = 1.0 - EPS;

	double x2 = x*x;
	double x4 = x2*x2;
	double x6 = x2*x4;
	double x8 = x4*x4;
	double f1 = 1.0 / (1.0 - x2);
	double sigoR3 = pow(sigma/R,3.0);
	double sigoR9 = sigoR3*sigoR3*sigoR3;

	double Kx2 = boost::math::ellint_1(x);
	double Ex2 = boost::math::ellint_2(x);

	double v9 = (1.0*pow(f1,9.0)/(240.0)) * (
			(1091.0 + 11156*x2 + 16434*x4 + 4052*x6 + 35*x8)*Ex2 -
			8.0*(1.0 - x2)*(1.0 + 7*x2)*(97.0 + 134*x2 + 25*x4)*Kx2);
	double v3 = 2.0*pow(f1,3.0) * ((7.0 + x2)*Ex2 - 4.0*(1.0-x2)*Kx2);

	return ((M_PI*epsilon*sigma*sigma*sigma*density/3.0)*(sigoR9*v9 - sigoR3*v3));
}

/**************************************************************************//**
 *  Return the r-derivative of the LJ Cylinder potential for separation r.
 *
 *  Checked and working with Mathematica.
******************************************************************************/
double LJCylinderPotential::valuedVdr(const double r) {

	double x = r / R;

	if (x >= 1.0)
		x = 1.0 - EPS;

	/* dV/dr */
	if (x < EPS)
		return (1.28121E8/pow(R,11.0) - 102245.0/pow(R,5.0))*x;
	else {
		double x2 = x*x;
		double x4 = x2*x2;
		double x6 = x2*x4;
		double x8 = x4*x4;
		double f1 = 1.0 / (1.0 - x2);
		double sigoR3 = pow(sigma/R,3.0);
		double sigoR9 = sigoR3*sigoR3*sigoR3;

		double Kx2 = boost::math::ellint_1(x);
		double Ex2 = boost::math::ellint_2(x);

		double dv9dx =(3.0*pow(f1,10.0)/(80.0*x)) *
			( (1.0 + x2)*(35.0 + 5108*x2 + 22482*x4 + 5108*x6 + 35*x8)*Ex2 -
			  (1.0 - x2)*(35.0 + 3428*x2 + 15234*x4 + 12356*x6 +1715*x8)*Kx2 );
		double dv3dx = (6.0*pow(f1,4.0)/x) *
			( (1.0 + 14*x2 + x4)*Ex2 - (1.0 + 6*x2 - 7*x4)*Kx2 );
		return ((M_PI*epsilon*sigma*sigma*sigma*density/(3.0*R))*(sigoR9*dv9dx - sigoR3*dv3dx));
	}
}

/**************************************************************************//**
 *  Return the second r-derivative of the LJ Cylinder potential for separation r.
 *
 * This has been checked with Mathematica --MTG.
******************************************************************************/
double LJCylinderPotential::valued2Vdr2(const double r) {

    double x = r / R;

    if (x >= 1.0)
        x = 1.0 - EPS;

    /* d2V/dr2 */
    /*if (x < EPS){
    // related to hard core limit, this will likely need to be implemented.
    return (1.28121E8/pow(R,11.0) - 102245.0/pow(R,5.0))*x;
    }
    else {*/
    double x2 = x*x;
    double x4 = x2*x2;
    double x6 = x2*x4;
    double x8 = x4*x4;
    double x10 = x8*x2;
    double f1 = 1.0 / (1.0 - x2);
    double sigoR3 = pow(sigma/R,3.0);
    double sigoR9 = sigoR3*sigoR3*sigoR3;

    double Kx2 = boost::math::ellint_1(x);
    double Ex2 = boost::math::ellint_2(x);

    double d2v9dx2 = (2.0/240)*pow(f1,11)*((1925.0*x10 + 319451.0*x8 + 2079074.0*x6 + 2711942.0*x4
                + 764873.0*x2 + 20975.0)*Ex2
            - (729400.0*x10 + 430024.0*x8 + 344752.0*x6 + 767248.0*x4 + 386200.0*x2 + 12712.0)*Kx2); // checked --MTG
    double d2v3dx2 = 8.0*pow(f1,5)*((11.0 + 80.0*x2  + 5.0*x4)*Ex2 + 4.0*(5.0*x4 - 4.0*x2 - 1.0)*Kx2); // checked --MTG

    return ((M_PI*epsilon*sigma*sigma*sigma*density/(3.0*R*R))*(sigoR9*d2v9dx2 - sigoR3*d2v3dx2));
    //}
}

/**************************************************************************//**
 * Return an initial particle configuration.
 *
 * Return a set of initial positions inside the cylinder.
******************************************************************************/
Array<dVec,1> LJCylinderPotential::initialConfig(const Container *boxPtr, MTRand &random,
		const int numParticles) {

	/* The particle configuration */
	Array<dVec,1> initialPos(numParticles);
	initialPos = 0.0;

	/* We shift the radius inward to account for the excluded volume from the
	 * hard wall.  This represents the largest prism that can be put
	 * inside a cylinder. */
	dVec lside;
	lside[0] = lside[1] = sqrt(2.0)*(R-1.0);
	lside[2] = boxPtr->side[NDIM-1];

	/* Get the linear size per particle */
	double initSide = pow((1.0*numParticles/product(lside)),-1.0/(1.0*NDIM));

	/* We determine the number of initial grid boxes there are in
	 * in each dimension and compute their size */
	int totNumGridBoxes = 1;
	iVec numNNGrid;
	dVec sizeNNGrid;

	for (int i = 0; i < NDIM; i++) {
		numNNGrid[i] = static_cast<int>(ceil((lside[i] / initSide) - EPS));

		/* Make sure we have at least one grid box */
		if (numNNGrid[i] < 1)
			numNNGrid[i] = 1;

		/* Compute the actual size of the grid */
		sizeNNGrid[i] = lside[i] / (1.0 * numNNGrid[i]);

		/* Determine the total number of grid boxes */
		totNumGridBoxes *= numNNGrid[i];
	}

	/* Now, we place the particles at the middle of each box */
	PIMC_ASSERT(totNumGridBoxes>=numParticles);
	dVec pos;
	for (int n = 0; n < totNumGridBoxes; n++) {

		iVec gridIndex;
		for (int i = 0; i < NDIM; i++) {
			int scale = 1;
			for (int j = i+1; j < NDIM; j++)
				scale *= numNNGrid[j];
			gridIndex[i] = (n/scale) % numNNGrid[i];
		}

		for (int i = 0; i < NDIM; i++)
			pos[i] = (gridIndex[i]+0.5)*sizeNNGrid[i] - 0.5*lside[i];

		boxPtr->putInside(pos);

		if (n < numParticles)
			initialPos(n) = pos;
		else
			break;
	}

	return initialPos;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// LJ HEXAGONAL NANOPORE POTENTIAL CLASS -------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
*  Constructor.
*
*  We create a Lennard-Jones hexagonal pore, which uses a lookup table
*  to hold the value of the integrated 6-12 potential for helium atoms interacting
*  with a silicate hexagonal pore.
*  @see C. Chakravarty J. Phys. Chem. B,  101, 1878 (1997).
*  @param sidelength The side length of a regular hexagon
******************************************************************************/

LJHexPorePotential::LJHexPorePotential(const double sidelength) :
    PotentialBase(),
    TabulatedPotential()
{
	/* The side length of the hexagon */
    t = sidelength;

	/* We define the values of epsilon and sigma for N, O, and He */

//	double sigmaN    = 3.299; 	// angstroms
//	double epsilonN  = 36.2;	// Kelvin
//  double sigmaHe = 2.28;      // angstroms
//  double epsilonHe = 10.22;   // Kelvin
//  double sigmaO = 2.708;      // angstroms
//  double epsilonO = 185.0;    // Kelvin

    /* Mixing Rules */
//	epsilon = sqrt(epsilonHe*epsilonO);
//	sigma = 0.5*(sigmaHe + sigmaO);

	/* We operate under the assumption that the silicon can be neglected in the
	 * silicate, and thus only consider the Oxygen. */

    /* Mixed Parameters for Oxygen and Helium */
     sigma = 2.494; //Angstroms
     epsilon = 43.48; //Kelvin

    /* Number density of oxygen in silicate */
     density = .05982; // atoms / angstrom^3

    /*Silica nitride parameters. Used for testing*/

    /* The density of nitrogen in silicon nitride */
    //	density = 0.078; // atoms / angstrom^3
    //	density = 0.008; // atoms / angstrom^3

    /* Mixed Nitrogen-Helium Parameters */

     // sigma = 2.79; // Angstroms
     // epsilon = 19.23; // Kelvin

     /* Debugging Params */
//     sigma = epsilon = density = 1.0;

    /* Choose a square mesh of points in R and theta*/
    double theta = M_PI/3.0;
    dR = t/3534;
    dTheta = theta/3534;
    rLength = int(t/dR);
    thetaLength = int(theta/dTheta);
    extV[0] = computePotential(0.0,0.0);
    extV[1] = computePotential(t-EPS, 0.0);
    initLookupTable(dR, dTheta, t, theta);
}

/**************************************************************************//**
*  Destructor.
******************************************************************************/
LJHexPorePotential::~LJHexPorePotential() {

}

/**************************************************************************//**
*  Return the actual value of the LJ Hexagonal Pore potential, for a point (x,y)
*  within the pore.
*
*  Checked and working with Mathematica and Python.
******************************************************************************/
double LJHexPorePotential::valueV(const dVec &rhat)
{

    double x, y;
    x = rhat[0]*cos(rhat[1]);
    y = rhat[0]*sin(rhat[1]);
    double TotalPotential = 0.0;

    if ( (-t/2.0 <= x) && (x <= t/2.0) && (-(sqrt(3.0)/2.0)*t < y ) && (y < (sqrt(3.0)/2.0)*t) )
    {
        TotalPotential = computePotential(x, y);
    }
    else if ( (-t < x) && (x < -t/2.0) && (-sqrt(3.0)*(x+t) < y) && (y < sqrt(3.0)*(x+t)) )
    {
        TotalPotential = computePotential(x, y);
    }
    else if ( (t/2.0 < x) && (x < t) && (sqrt(3.0)*(x-t) < y) && (y < -sqrt(3.0)*(x-t)) )
    {
        TotalPotential = computePotential(x, y);
    }
    else
    {
        TotalPotential = extV[1];
    }
    return TotalPotential;
}

/**************************************************************************//**
*  The above if statements go here. This function actually computes the
*  potential
*  Checked and working with Mathematica and Python.
******************************************************************************/
double LJHexPorePotential::computePotential(double x, double y)

{
    double denom3, denom9, denom33, denom39, denom43, denom49, Potential, xdim, ydim, tdim;
    getBounds(x, y);
    xdim = x/sigma;
    ydim = y/sigma;
    tdim = t/sigma;
    denom3 = pow(sqrt(3)*xdim+ydim-sqrt(3)*tdim,3);
    denom9 = pow(sqrt(3)*xdim+ydim-sqrt(3)*tdim,9);

    sectionOne = M_PI*(((-441*cos(boundList[0]))/(128*denom9)) + ((3*cos(boundList[0]))/(8*denom3)) - ((49*cos(3*boundList[0]))/(32*denom9))
                 +(cos(3*boundList[0])/(12*denom3)) - ((63*cos(5*boundList[0]))/(320*denom9)) + ((9*cos(7*boundList[0]))/(256*denom9))
                 +((7*cos(9*boundList[0]))/(1152*denom9)) + ((441*cos(boundList[1]))/(128*denom9)) - ((3*cos(boundList[1]))/(8*denom3))
                 +((49*cos(3*boundList[1]))/(32*denom9)) - (cos(3*boundList[1])/(12*denom3)) + ((63*cos(5*boundList[1]))/(320*denom9))
                 -((9*cos(7*boundList[1]))/(256*denom9)) - ((7*cos(9*boundList[1]))/(1152*denom9)) + ((441*sqrt(3)*sin(boundList[0]))/(128*denom9))
                 -((3*sqrt(3)*sin(boundList[0]))/(8*denom3)) - ((63*sqrt(3)*sin(5*boundList[0]))/(320*denom9)) - ((9*sqrt(3)*sin(7*boundList[0]))/(256*denom9))
                 -((441*sqrt(3)*sin(boundList[1]))/(128*denom9))+((3*sqrt(3)*sin(boundList[1]))/(8*denom3)) + ((63*sqrt(3)*sin(5*boundList[1]))/(320*denom9))
                 +((9*sqrt(3)*sin(7*boundList[1]))/(256*denom9)));

    sectionTwo = M_PI*(((9*cos(boundList[1])-cos(3*boundList[1])-9*cos(boundList[2])+cos(3*boundList[2]))/(12*pow(2*ydim-sqrt(3)*tdim,3))) - ((39690*cos(boundList[1])
                 -8820*cos(3*boundList[1]) + 2268*cos(5*boundList[1]) - 405*cos(7*boundList[1]) + 35*cos(9*boundList[1]) - 39690*cos(boundList[2])
                 +8820*cos(3*boundList[2]) - 2268*cos(5*boundList[2]) + 405*cos(7*boundList[2]) - 35*cos(9*boundList[2]))/(5760*pow(2*ydim-sqrt(3)*tdim,9))));

    denom33 = pow(sqrt(3)*(xdim+tdim)-ydim, 3);
    denom39 = pow(sqrt(3)*(xdim+tdim)-ydim, 9);

    sectionThree = M_PI*(((-3*cos(boundList[2]))/(8*denom33)) + ((441*cos(boundList[2]))/(128*denom39)) - (cos(3*boundList[2])/(12*denom33))
                   + ((49*cos(3*boundList[2]))/(32*denom39)) + ((63*cos(5*boundList[2]))/(320*denom39)) - ((9*cos(7*boundList[2]))/(256*denom39))
                   - ((7*cos(9*boundList[2]))/(1152*denom39)) + ((3*cos(boundList[3]))/(8*denom33)) - ((441*cos(boundList[3]))/(128*denom39))
                   + (cos(3*boundList[3])/(12*denom33)) - ((49*cos(3*boundList[3]))/(32*denom39)) - ((63*cos(5*boundList[3]))/(320*denom39))
                   + ((9*cos(7*boundList[3]))/(256*denom39)) + ((7*cos(9*boundList[3]))/(1152*denom39)) - ((3*sqrt(3)*sin(boundList[2]))/(8*denom33))
                   + ((441*sqrt(3)*sin(boundList[2]))/(128*denom39)) - ((63*sqrt(3)*sin(5*boundList[2]))/(320*denom39))
                   - ((9*sqrt(3)*sin(7*boundList[2]))/(256*denom39)) + ((3*sqrt(3)*sin(boundList[3]))/(8*denom33))- ((441*sqrt(3)*sin(boundList[3]))/(128*denom39))
                   + ((63*sqrt(3)*sin(5*boundList[3]))/(320*denom39)) + ((9*sqrt(3)*sin(7*boundList[3]))/(256*denom39)));

    denom43 = pow(sqrt(3)*(xdim+tdim)+ydim,3);
    denom49 = pow(sqrt(3)*(xdim+tdim)+ydim,9);

    sectionFour = M_PI*(((3*cos(boundList[3]))/(8*denom43)) - ((441*cos(boundList[3]))/(128*denom49)) + (cos(3*boundList[3])/(12*denom43))
                  - ((49*cos(3*boundList[3]))/(32*denom49)) - ((63*cos(5*boundList[3]))/(320*denom49)) + ((9*cos(7*boundList[3]))/(256*denom49))
                  + ((7*cos(9*boundList[3]))/(1152*denom49)) - ((3*cos(boundList[4]))/(8*denom43)) + ((441*cos(boundList[4]))/(128*denom49))
                  - (cos(3*boundList[4])/(12*denom43)) + ((49*cos(3*boundList[4]))/(32*denom49)) + ((63*cos(5*boundList[4]))/(320*denom49))
                  - ((9*cos(7*boundList[4]))/(256*denom49)) - ((7*cos(9*boundList[4]))/(1152*denom49)) - ((3*sqrt(3)*sin(boundList[3]))/(8*denom43))
                  + ((441*sqrt(3)*sin(boundList[3]))/(128*denom49)) - ((63*sqrt(3)*sin(5*boundList[3]))/(320*denom49))
                  - ((9*sqrt(3)*sin(7*boundList[3]))/(256*denom49)) + ((3*sqrt(3)*sin(boundList[4]))/(8*denom43)) - ((441*sqrt(3)*sin(boundList[4]))/(128*denom49))
                  + ((63*sqrt(3)*sin(5*boundList[4]))/(320*denom49)) + ((9*sqrt(3)*sin(7*boundList[4]))/(256*denom49)));

    sectionFive = M_PI*(((9*cos(boundList[4])-cos(3*boundList[4])-9*cos(boundList[5])+cos(3*boundList[5]))/(12*pow(2*ydim+sqrt(3)*tdim,3))) - ((39690*cos(boundList[4])
                  -8820*cos(3*boundList[4]) + 2268*cos(5*boundList[4]) - 405*cos(7*boundList[4]) + 35*cos(9*boundList[4]) - 39690*cos(boundList[5])
                  +8820*cos(3*boundList[5]) - 2268*cos(5*boundList[5]) + 405*cos(7*boundList[5]) - 35*cos(9*boundList[5]))/(5760*pow(2*ydim+sqrt(3)*tdim,9))));

    sectionSix = (M_PI/11520)*(((480*(9*cos(boundList[0]) + 2*cos(3*boundList[0]) - 9*cos(boundList[5]) - 2*cos(3*boundList[5]) + 9*sqrt(3)*(sin(boundList[0])
                  - sin(boundList[5]))))/pow(sqrt(3)*xdim-ydim-sqrt(3)*tdim,3)) + (1/pow(sqrt(3)*tdim+ydim-sqrt(3)*xdim,9))*(39690*cos(boundList[0]) + 17640*cos(3*boundList[0])
                  + 2268*cos(5*boundList[0]) - 405*cos(7*boundList[0]) - 70*cos(9*boundList[0]) + 70*cos(9*boundList[5]) - 9*(4410*cos(boundList[5])
                  + 1960*cos(3*boundList[5]) + 9*(28*cos(5*boundList[5]) - 5*cos(7*boundList[5]) + sqrt(3)*(28*sin(5*boundList[0])-490*sin(boundList[0])
                  + 5*sin(7*boundList[0]) + 490*sin(boundList[5]) - 28*sin(5*boundList[5]) - 5*sin(7*boundList[5]))))));

    // This is the fitting parameter given by fitting the isolated pore potential to the array potential.
    double fitparam = 0.999667333333;
    Potential = 4*epsilon*fitparam*pow(sigma,3)*density*(sectionOne + sectionTwo + sectionThree + sectionFour + sectionFive + sectionSix);
    return Potential;
}
/**************************************************************************//**
*  Calculate the six necessary bounds on theta and add them to the bounds
*  list
*  Checked and working with Mathematica and Python.
******************************************************************************/

void LJHexPorePotential::getBounds(double x, double y)
{
    boundList[0] = atan2(-y,(t-x));
    boundList[1] = atan2(((sqrt(3)/2)*t-y),((t/2)-x));
    boundList[2] = atan2(((sqrt(3)/2)*t-y),((-t/2)-x));
    boundList[3] = atan2(-y,(-(t+x)));
    boundList[4] = 2*M_PI + atan2(((-sqrt(3)/2)*t-y),((-t/2)-x));
    boundList[5] = 2*M_PI + atan2(((-sqrt(3)/2)*t-y),((t/2)-x));

};

/**************************************************************************//**
*  Calculate the six necessary bounds on theta and add them to the bounds
*  list
*  Checked and working with Mathematica and Python.
******************************************************************************/
void LJHexPorePotential::initLookupTable(const double _dr, const double _dtheta, const double R, const double theta) {

	/* We now calculate the lookup tables for the interaction potential and
	 * its first and second derivatives. */
	dr = _dr;
	double dtheta = _dtheta;
//    rLength = int(R/dr);
//    thetaLength = int(theta/dtheta);
	twoDlookupV.resize(thetaLength, rLength);
	//lookupdVdr.resize(tableLength);
    //lookupd2Vdr2.resize(tableLength);
	twoDlookupV = 0.0;
	//lookupdVdr = 0.0;
    //lookupd2Vdr2 = 0.0;

	double r = 0;
	double thetaVal = 0;
	for (int n = 0; n < rLength; n++)
    {
        for (int m = 0; m < thetaLength; m++)
        {
            dVec rhat;
            rhat[0] = r;
            rhat[1] = thetaVal;
            twoDlookupV(m, n) = valueV(rhat);
            //lookupdVdr(n) = valuedVdr(r);
            //lookupd2Vdr2(n) = valued2Vdr2(r);
            thetaVal += dtheta;
        }
        r += dr;
	}
}

/**************************************************************************//**
*  Pull the value of the LJ Hex Pore potential from the lookup table by rotating
*  points outside of the top right wedge into the top right wedge.
*
******************************************************************************/
double LJHexPorePotential::V(const dVec &r)
{
    double x, y, theta, rval;
    int section, rindex;
    x = r[0];
    y = r[1];
    theta = atan2(y, x);
    theta += (theta < 0)*(2.0*M_PI);
    section = trunc(theta/(M_PI/3.0));
    theta -= (M_PI/3.0)*section;
    rval = sqrt(x*x + y*y);

    // Check rindex. If the first rindex is near the edge, just return edge value because the potential is enormous out there anyway.
    rindex = trunc(rval/dR);
    if (rindex >= rLength-1){
        return extV[1];
    }
    else{
        return bilinearInterp(twoDlookupV, extV, rval, theta);
    }
//    rindex = static_cast<int>(sqrt(x*x + y*y)/dR);
//    thetaindex = static_cast<int>(theta/dTheta);
//    if ((rindex >= rLength) or (thetaindex >= thetaLength)){return extV[1];}
//    else{return twoDlookupV(thetaindex, rindex);}

}

/**************************************************************************//**
*  Override the base bilinearInterp to deal with r and theta
*
*
******************************************************************************/
double LJHexPorePotential::bilinearInterp(const Array<double,2> &VTable,
                                          const TinyVector<double,2> &extVal, const double r, const double theta) {

    double r1, r2, theta1, theta2, fr1theta1, fr1theta2, fr2theta1, fr2theta2;
    int rindex1, thetaindex1, rindex2, thetaindex2;

    // Get the indices for the interpolation points
    rindex1 = trunc(r/dR);
    rindex2 = rindex1 + 1;
    thetaindex1 = trunc(theta/dTheta);
    thetaindex2 = thetaindex1 + 1;

    // Get the REAL r and theta position values, BEFORE we shift indices so the interpolation remains accurate.
    r1 = rindex1*dR;
    r2 = rindex2*dR;
    theta1 = thetaindex1*dTheta;
    theta2 = thetaindex2*dTheta;

    /* THERE SHOULD BE A WAY TO ELIMINATE THIS IF STATEMENT */
    // Could copy the first row and attach it to the end of the array. However this would require ignoring this last row in all checks i.e rLength-1 >> rLength-2 etc.

    // Shift thetaindex2 to grab potential values from the beginning of the array, effectively using periodic boundary conditions
    thetaindex2 -= (thetaindex2 >= thetaLength)*thetaLength;

    // Get the lookup table values
    fr1theta1 = twoDlookupV(thetaindex1, rindex1);
    fr1theta2 = twoDlookupV(thetaindex2, rindex1);
    fr2theta1 = twoDlookupV(thetaindex1, rindex2);
    fr2theta2 = twoDlookupV(thetaindex2, rindex2);

    // Do the interpolation
    double interpval = (1.0/((r2-r1)*(theta2-theta1)))*(fr1theta1*(r2-r)*(theta2-theta) + fr2theta1*(r-r1)*(theta2-theta) + fr1theta2*(r2-r)*(theta-theta1) + fr2theta2*(r-r1)*(theta-theta1));
    return interpval;
}


/**************************************************************************//**
*  Return the gradient of the LJ Hex Pore potential. Returns 0 for now.
*
******************************************************************************/
dVec LJHexPorePotential::gradV(const dVec &r)
{
    dVec null;
    null = 0.0;
    return null;
}

/**************************************************************************//**
*  Return the Laplacian of the LJ Hex Pore potential. Returns 0
*  for now.
******************************************************************************/
double LJHexPorePotential::grad2V(const dVec &thing)
{
    return 0.0;
}

/**************************************************************************
 ** Return an initial particle configuration.
 ** Return a set of initial positions inside the cylinder.
******************************************************************************/
Array<dVec,1> LJHexPorePotential::initialConfig(const Container *boxPtr, MTRand &random,
                                                 const int numParticles) {
    Array<dVec,1> initialPos(numParticles);
	initialPos = 0.0;

	/* We shift the radius inward to account for the excluded volume from the
	 * hard wall.  This represents the largest rectanglular prism that can be put
	 * inside a hexagonal pore. A more appropiate implementation may
     * need to be applied in the future */
	dVec lside;
	lside[0] = lside[1] = sqrt(2.0)*(t-1.0);
	lside[2] = boxPtr->side[NDIM-1];

	/* Get the linear size per particle */
	double initSide = pow((1.0*numParticles/product(lside)),-1.0/(1.0*NDIM));

	/* We determine the number of initial grid boxes there are in
	 * in each dimension and compute their size */
	int totNumGridBoxes = 1;
	iVec numNNGrid;
	dVec sizeNNGrid;

	for (int i = 0; i < NDIM; i++) {
		numNNGrid[i] = static_cast<int>(ceil((lside[i] / initSide) - EPS));

		/* Make sure we have at least one grid box */
		if (numNNGrid[i] < 1)
			numNNGrid[i] = 1;

		/* Compute the actual size of the grid */
		sizeNNGrid[i] = lside[i] / (1.0 * numNNGrid[i]);

		/* Determine the total number of grid boxes */
		totNumGridBoxes *= numNNGrid[i];
	}

	/* Now, we place the particles at the middle of each box */
	PIMC_ASSERT(totNumGridBoxes>=numParticles);
	dVec pos;
	for (int n = 0; n < totNumGridBoxes; n++) {

		iVec gridIndex;
		for (int i = 0; i < NDIM; i++) {
			int scale = 1;
			for (int j = i+1; j < NDIM; j++)
				scale *= numNNGrid[j];
			gridIndex[i] = (n/scale) % numNNGrid[i];
		}

		for (int i = 0; i < NDIM; i++)
			pos[i] = (gridIndex[i]+0.5)*sizeNNGrid[i] - 0.5*lside[i];

		boxPtr->putInside(pos);

		if (n < numParticles)
			initialPos(n) = pos;
		else
			break;
	}

	return initialPos;

}

/**************************************************************************//**
 * Ouptut the potential.
 *
 * For use during comparison and debugging, we output the potential out to
 * a supplied separation.
 *
 * @param maxSep the maximum separation
******************************************************************************/

void LJHexPorePotential::output(const double maxSep) {
    dVec position;
    cout << "Hello this is the hex pore output method \n";
    cout << "Value at edge is: " << computePotential(maxSep, 0.0) << endl;
    cout << "extV[0] = " << extV[0] << endl;
    cout << "extV[1] = " << extV[1] << endl;
    position[1] = position[2] = 0.0;
//    /* Plot along line to corner */
//    double dx = maxSep/1000;
//    double x = 0.0;
//    double y = 0.0;
//    double radialdist = 0.0;
//    for (int n = 0; n <= 1000; n += 1) {
//        if (radialdist <= maxSep) {
//            position[0] = x;
//            y = sqrt(3)*x - .000001;
//            position[1] = y;
//            radialdist = sqrt(x*x + y*y);
//            communicate()->file("debug1")->stream()
//            << format("%10.4E\t%16.8E\n") % radialdist % V(position);
//            communicate()->file("debug1")->stream().flush();
//            x += dx;
//        }
//    }
//    communicate()->file("debug1")->stream().flush();

//        /* Plot along line to corner */
//        double dx = maxSep/1000;
//        double x = 0.0;
//        for (int n = 0; n <= 1000; n += 1) {
//            if (x <= maxSep) {
//                position[0] = x;
//                communicate()->file("debug1")->stream()
//                << format("%10.4E\t%16.8E\n") % x % V(position);
//                communicate()->file("debug1")->stream().flush();
//                x -= dx;
//            }
//        }
//        communicate()->file("debug1")->stream().flush();
    
        /* Plot along line to center of edge */
        double dy = maxSep/1000;
        double y = 0.0;
        position[0] = 0.0;
        for (int n = 0; n <= 1000; n += 1) {
            if (abs(y) <= (maxSep)) {
                position[1] = y;
                V(position);
                communicate()->file("debug2")->stream()
                << format("%10.4E\t%16.8E\n") % y % V(position);
                communicate()->file("debug2")->stream().flush();
                y -= dy;
            }
        }
        communicate()->file("debug2")->stream().flush();

    //    /* Plot along theta for r halfway between center and wall */
    //    double th = 0.0;
    //    for (int n = 0; n < thetaLength; n += 1) {
    //        communicate()->file("debug3")->stream()
    //        << format("%10.4E\t%16.8E\n") % th % twoDlookupV(n, int(rLength/2));
    //        communicate()->file("debug3")->stream().flush();
    //        th += dTheta;
    //    }
    //    communicate()->file("debug3")->stream().flush();
    //
    //    /* Plot along theta for r close to wall */
    //    double th2 = 0.0;
    //    for (int n = 0; n < thetaLength; n += 1) {
    //        communicate()->file("debug4")->stream()
    //        << format("%10.4E\t%16.8E\n") % th2 % twoDlookupV(n, int((5*rLength)/6));
    //        communicate()->file("debug4")->stream().flush();
    //        th2 += dTheta;
    //    }
    //    communicate()->file("debug3")->stream().flush();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// AZIZ POTENTIAL CLASS ------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *
 *  Create the Aziz interaction potential.  We use the standard 1979 values.
 *  @see R.A. Aziz et al. J. Chem. Phys. 70, 4330 (1979).
******************************************************************************/
AzizPotential::AzizPotential(const dVec &side) : PotentialBase(), TabulatedPotential()
{
	/* Define all variables for the Aziz potential */
	/* R.A. Aziz et al. J. Chem. Phys. 70, 4330 (1979) */
	rm      = 2.9673;  	// A
	A       = 0.5449E6;
	epsilon = 10.8; 	// K
	alpha   = 13.353;
	D       = 1.2413;
	C6      = 1.3732;
	C8      = 0.42538;
	C10     = 0.1781;

	/* The extremal values are all zero here */
	extV = 0.0;
	extdVdr = 0.0;
    extd2Vdr2 = 0.0;

	/* We take the maximum possible separation */
	double L = max(side);

	/* Create the potential lookup tables */
	// initLookupTable(0.00005*rm,L);
	initLookupTable((1.0E-6)*rm,L);

	/* Now we compute the tail correction */
	double rmoL = rm / L;
	double rm3 = rm*rm*rm;
	double t1 = A*exp(-alpha*L/(2.0*rm))*rm*(8.0*rm*rm + 4.0*L*rm * alpha + L*L*alpha*alpha)
		/ (4.0*alpha*alpha*alpha);
	double t2 = 8.0*C6*pow(rmoL,3.0)/3.0;
	double t3 = 32.0*C8*pow(rmoL,5.0)/5.0;
	double t4 = 128.0*C10*pow(rmoL,7.0)/7.0;

	tailV = 2.0*M_PI*epsilon*(t1 - rm3*(t2+t3+t4));
}

/**************************************************************************//**
 *  Destructor.
******************************************************************************/
AzizPotential::~AzizPotential() {
}

/**************************************************************************//**
 *  Return the actual value of the Aziz potential, used to construct a lookup
 *  table.
 *
 *  Checked and working with Mathematica.
******************************************************************************/
double AzizPotential::valueV(const double r) {
	double x = r / rm;

	double Urep = A * exp(-alpha*x);

	/* No self interactions */
	if (x < EPS)
		return 0.0;
	/* Hard core limit */
	else if (x < 0.01)
		return (epsilon * Urep);
	else {
		double ix2 = 1.0 / (x * x);
		double ix6 = ix2 * ix2 * ix2;
		double ix8 = ix6 * ix2;
		double ix10 = ix8 * ix2;
		double Uatt = -( C6*ix6 + C8*ix8 + C10*ix10 ) * F(x);
		return ( epsilon * (Urep + Uatt) );
	}
}

/**************************************************************************//**
 *  Return the r-derivative of the Aziz potential for separation r.
 *
 *  Checked and working with Mathematica.
******************************************************************************/
double AzizPotential::valuedVdr(const double r) {
	double x = r / rm;

	double T1 = -A * alpha * exp(-alpha*x);

	/* dV/dR */
	/* No self interactions */
	if (x < EPS)
		return 0.0;
	/* Hard core limit */
	else if (x < 0.01)
		return ( ( epsilon / rm ) * T1 );
	else {
		/* The various inverse powers of x */
		double ix = 1.0 / x;
		double ix2 = ix*ix;
		double ix6 = ix2 * ix2 * ix2;
		double ix7 = ix6 * ix;
		double ix8 = ix6 * ix2;
		double ix9 = ix8 * ix;
		double ix10 = ix8 * ix2;
		double ix11 = ix10 * ix;
		double T2 = ( 6.0*C6*ix7 + 8.0*C8*ix9 + 10.0*C10*ix11 ) * F(x);
		double T3 = -( C6*ix6 + C8*ix8 + C10*ix10 ) * dF(x);
		return ( ( epsilon / rm ) * (T1 + T2 + T3) );
	}
}

/**************************************************************************//**
 *  Return the second r-derivative of the Aziz potential for separation r.
 *
 *  Double checked and working with Mathematica. -MTG
******************************************************************************/
double AzizPotential::valued2Vdr2(const double r) {
	double x = r / rm;

	double T1 = A * alpha * alpha * exp(-alpha*x);

	/* d^2V/dR^2 */
	/* No self interactions */
	if (x < EPS)
		return 0.0;
	/* Hard core limit */
	else if (x < 0.01)
		return ( ( epsilon / rm ) * T1 );
	else {
		/* The various inverse powers of x */
		double ix = 1.0 / x;
		double ix2 = ix*ix;
		double ix6 = ix2 * ix2 * ix2;
		double ix7 = ix6 * ix;
		double ix8 = ix6 * ix2;
		double ix9 = ix8 * ix;
		double ix10 = ix8 * ix2;
		double ix11 = ix10 * ix;
		double ix12 = ix11 * ix;
		double T2 = - ( 42.0*C6*ix8 + 72.0*C8*ix10 + 110.0*C10*ix12 ) * F(x);
		double T3 = 2.0*( 6.0*C6*ix7 + 8.0*C8*ix9 + 10.0*C10*ix11 ) * dF(x);
		double T4 = - ( C6*ix6 + C8*ix8 + C10*ix10 ) * d2F(x);
		return ( ( epsilon / (rm*rm) ) * (T1 + T2 + T3 + T4) );
	}
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// EXCLUDED VOLUME CLASS -----------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
/**************************************************************************//**
 * Constructor.
******************************************************************************/
Gasparini_1_Potential::Gasparini_1_Potential(double _az, double _ay, const Container *_boxPtr) :
    PotentialBase(),
    excZ(0.5*_boxPtr->side[2]-_az),
    excY(0.5*_boxPtr->side[1]-_ay),
    V0(1.0E6)
{
    // Empty Constructor
}

/**************************************************************************//**
 * Destructor.
******************************************************************************/
Gasparini_1_Potential::~Gasparini_1_Potential() {
    // Empty Destructor
}

/**************************************************************************//**
 * Return initial particle configuration.
 *
 * Return initial positions to exclude volume in the simulation cell.
******************************************************************************/
Array<dVec,1> Gasparini_1_Potential::initialConfig(const Container *boxPtr,
        MTRand &random, const int numParticles) {

	/* label the lengths of the sides of the simulation cell */
    dVec lside;
    lside[0] = boxPtr->side[0];
    lside[1] = boxPtr->side[1];
    lside[2] = boxPtr->side[NDIM-1];

    /* calculate actual volume */
    double volTot = product(lside);

    /* calculate density */
    double density = (1.0*numParticles/volTot);

    /* calculate excluded volume */
    double volExc = lside[0]*(2.0*excY)*(2.0*excZ);

    /* calculate actual number of particles */
    int correctNum = int(numParticles-density*volExc);

    /* The particle configuration */
	Array<dVec,1> initialPos(correctNum);

    /* get linear size per particle  */
    double initSide = pow((1.0*correctNum/(volTot-volExc)),-1.0/(1.0*NDIM));

    /* For accessible volume, determine the number of
     * initial grid boxes there are in each dimension and compute
     * their size. */
    int totNumGridBoxes1 = 1;
    int totNumGridBoxes2 = 1;
    iVec numNNGrid1;
    iVec numNNGrid2;
    dVec sizeNNGrid1;
    dVec sizeNNGrid2;

    /* divide space into two regions, insert particles appropriately */
    double V1 = (lside[1]-2.0*excY)*(2.0*excZ)*lside[0];
    double V2 = (lside[2]-2.0*excZ)*lside[1]*lside[0];

    double fracV1 = V1/(V1+V2);

    int numIn1 = int(correctNum*fracV1);
    int numIn2 = (correctNum-numIn1);

    /* grid space in volume 1 */
    /* x */
    numNNGrid1[0] = static_cast<int>(ceil((1.0*lside[0]/initSide)-EPS));
    if (numNNGrid1[0] < 1)
        numNNGrid1[0] = 1;
    sizeNNGrid1[0] = 1.0*lside[0]/(1.0*numNNGrid1[0]);
    totNumGridBoxes1 *= numNNGrid1[0];
    /* y */
    numNNGrid1[1] = static_cast<int>(ceil(((lside[1]-2.0*excY)/initSide)-EPS));
    if (numNNGrid1[1] < 1)
        numNNGrid1[1] = 1;
    sizeNNGrid1[1] = (lside[1]-2.0*excY)/(1.0*numNNGrid1[1]);
    totNumGridBoxes1 *= numNNGrid1[1];
    /* z */
    numNNGrid1[2] = static_cast<int>(ceil(((2.0*excZ)/initSide)-EPS));
    if (numNNGrid1[2] < 1)
        numNNGrid1[2] = 1;
    sizeNNGrid1[2] = (2.0*excZ)/(1.0*numNNGrid1[2]);
    totNumGridBoxes1 *= numNNGrid1[2];

    /* grid space in volume 2 */
    /* x */
    numNNGrid2[0] = static_cast<int>(ceil((1.0*lside[0]/initSide)-EPS));
    if (numNNGrid2[0] < 1)
        numNNGrid2[0] = 1;
    sizeNNGrid2[0] = 1.0*lside[0]/(1.0*numNNGrid2[0]);
    totNumGridBoxes2 *= numNNGrid2[0];
    /* y */
    numNNGrid2[1] = static_cast<int>(ceil((1.0*lside[1]/initSide)-EPS));
    if (numNNGrid2[1] < 1)
        numNNGrid2[1] = 1;
    sizeNNGrid2[1] = 1.0*lside[1]/(1.0*numNNGrid2[1]);
    totNumGridBoxes2 *= numNNGrid2[1];
    /* z */
    numNNGrid2[2] = static_cast<int>(ceil(((lside[2]-2.0*excZ)/initSide)-EPS));
    if (numNNGrid2[2] < 1)
        numNNGrid2[2] = 1;
    sizeNNGrid2[2] = (lside[2]-2.0*excZ)/(1.0*numNNGrid2[2]);
    totNumGridBoxes2 *= numNNGrid2[2];

    /* Place particles in the middle of the boxes -- volume 1 */
    PIMC_ASSERT(totNumGridBoxes1>=numIn1);
    dVec pos1;

    for (int n = 0; n < totNumGridBoxes1; n++) {
        iVec gridIndex1;
        /* update grid index */
        for (int i = 0; i < NDIM; i++) {
            int scale = 1;
            for (int j = i+1; j < NDIM; j++)
                scale *= numNNGrid1[j];
            gridIndex1[i] = (n/scale) % numNNGrid1[i];
        }
        /* place particle in position vector, skipping over excluded volume */
        pos1[0] = (gridIndex1[0]+0.5)*sizeNNGrid1[0] + +0.5*lside[0] + 2.0*EPS;
        pos1[1] = (gridIndex1[1]+0.5)*sizeNNGrid1[1] + excY + 2.0*EPS;
        pos1[2] = (gridIndex1[2]+0.5)*sizeNNGrid1[2] - excZ + 2.0*EPS;

        if ((pos1[1]<-excY || pos1[1]>excY) || (pos1[2]<-excZ || pos1[2]>excZ))
            boxPtr->putInside(pos1);

        if (n < numIn1){
			initialPos(n) = pos1;
        }
		else
			break;
    }

    /* Place particles in the middle of the boxes -- volume 2 */
    PIMC_ASSERT(totNumGridBoxes2>=numIn2);
    dVec pos2;

    for (int n = 0; n < totNumGridBoxes2; n++) {
        iVec gridIndex2;
        /* update grid index */
        for (int i = 0; i < NDIM; i++) {
            int scale = 1;
            for (int j = i+1; j < NDIM; j++)
                scale *= numNNGrid2[j];
            gridIndex2[i] = (n/scale) % numNNGrid2[i];
        }
        /* place particles in position vectors */
        pos2[0] = (gridIndex2[0]+0.5)*sizeNNGrid2[0] + 0.5*lside[0] + 2.0*EPS;
        pos2[1] = (gridIndex2[1]+0.5)*sizeNNGrid2[1] + 0.5*lside[1] + 2.0*EPS;
        pos2[2] = (gridIndex2[2]+0.5)*sizeNNGrid2[2] + excZ + 2.0*EPS;

        if ((pos2[1]<-excY || pos2[1]>excY) || (pos2[2]<-excZ || pos2[2]>excZ))
            boxPtr->putInside(pos2);

        if (n < numIn2){
			initialPos(n+numIn1) = pos2;
        }
		else
			break;
    }
    /* do we want to output the initial config to disk? */
    bool outToDisk = 1;
    ofstream OF;
    if (outToDisk){
        OF.open("./OUTPUT/initialConfig.dat");
        OF<<"# Cartesian Coordinates of initial Positions (X-Y-Z)"<<endl;
        OF<<"# "<<lside[0]<<"\t"<<lside[1]<<"\t"<<lside[2]<<endl;
        OF<<"# "<< excY <<"\t"<< excZ <<endl;
        for (int i=0; i< int(initialPos.size()); i++)
            OF<<initialPos(i)(0)<< "\t"<<initialPos(i)(1)<<"\t"<<initialPos(i)(2)<<endl;
        OF.close();
    }

    return initialPos;
}
/*************************************************************************//**
 *  Returns the exclusion lengths ay and az
******************************************************************************/
Array<double,1> Gasparini_1_Potential::getExcLen(){

    Array<double, 1> excLens(2);
    excLens(0) = excY; // was ay
    excLens(1) = excZ; // was az

    return (excLens);
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// HARD SPHERE POTENTIAL CLASS -----------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *  @param _a The radius of the hard sphere (also the scattering length)
******************************************************************************/
HardSpherePotential::HardSpherePotential(double _a) :
	PotentialBase(),
    a(_a) {

}

/**************************************************************************//**
 *  Destructor.
******************************************************************************/
HardSpherePotential::~HardSpherePotential() {
// empty deconstructor
}

/**************************************************************************//**
 *  The effective potential.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-12.
 *
 *  @param sep1 The first separation
 *  @param sep2 The second separation
 *  @param lambdaTau The product of \lambda = \hbar^2/2m and \tau
 *  @return the two-body effective pair potential
******************************************************************************/
double HardSpherePotential::V(const dVec &sep1, const dVec &sep2,
        double lambdaTau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    if ((r1 <= a ) || (r2 <= a))
        return LBIG;

    double cosTheta = dot(sep1,sep2)/(r1*r2);

    double t1 = -(r1*r2 + a*a - a*(r1 + r2)) * (1.0 + cosTheta);
    t1 /= (4.0*lambdaTau);

    double t2 = (a*(r1+r2) - a*a)/(r1*r2);
    double t3 = 1.0 - t2*exp(t1);

    return -log(t3);
}

/**************************************************************************//**
 *  The derivative of the effective potential with respect to lambda.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-12.
 *
 *  @param sep1 the first separation
 *  @param sep2 the second separation
 *  @param lambda \lambda = \hbar^2/2m
 *  @param tau the imaginary timestep tau
 *  @return the derivative of the effective potential with respect to lambda
******************************************************************************/
double HardSpherePotential::dVdlambda(const dVec &sep1, const dVec &sep2,
        double lambda, double tau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    double cosTheta = dot(sep1,sep2)/(r1*r2);

    double t1 = -(r1*r2 + a*a - a*(r1 + r2)) * (1.0 + cosTheta);
    t1 /= (4.0*lambda*tau);

    double t2 = (a*(r1+r2) - a*a)/(r1*r2);
    double t3 = 1.0 - t2*exp(t1);

    double t4 = (1.0/t3)*(t2*exp(t1))*(t1/lambda);

    return -t4;
}

/**************************************************************************//**
 *  The derivative of the effective potential with respect to tau.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-12.
 *
 *  @param sep1 the first separation
 *  @param sep2 the second separation
 *  @param lambda \lambda = \hbar^2/2m
 *  @param tau the imaginary timestep tau
 *  @return the derivative of the effective potential with respect to tau
******************************************************************************/
double HardSpherePotential::dVdtau(const dVec &sep1, const dVec &sep2,
        double lambda, double tau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    double cosTheta = dot(sep1,sep2)/(r1*r2);

    double t1 = -(r1*r2 + a*a - a*(r1 + r2)) * (1.0 + cosTheta);
    t1 /= (4.0*lambda*tau);

    double t2 = (a*(r1+r2) - a*a)/(r1*r2);
    double t3 = 1.0 - t2*exp(t1);

    double t4 = (1.0/t3)*(t2*exp(t1))*(t1/tau);

    return -t4;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 1D Delta Potential Class -----------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
*  Constructor.
*  @param g The strength of delta interaction
******************************************************************************/
Delta1DPotential::Delta1DPotential(double _g) :
PotentialBase(),
g(_g) {

}

/**************************************************************************//**
*  Destructor.
******************************************************************************/
Delta1DPotential::~Delta1DPotential() {
    // empty deconstructor
}

/**************************************************************************//**
*  The effective potential.
*
*  Computes the non-local two-body effective pair potential.
*
*  @param sep1 The first separation
*  @param sep2 The second separation
*  @param lambda \lambda = \hbar^2/2m
*  @param tau the imaginary timestep tau*
*  @return the two-body effective pair potential
******************************************************************************/
double Delta1DPotential::V(const dVec &sep1, const dVec &sep2,
                              double lambda, double tau) {

    double r1 = sep1[0];
    double r2 = sep2[0];

    double s = g/(4.0*lambda)*sqrt(2.0*lambda*tau);
    double z = 1.0/sqrt(8.0*lambda*tau);
    double r = z*(abs(r1)+abs(r2));

    double expArg = s*(s-2*r)+z*z*(r1-r2)*(r1-r2);
    double erfVal = erfc(r-s);

    double rho = 1.0+sqrt(M_PI)*s*exp(expArg)*erfVal;

    if ((rho < exp(-5.0))||(rho > exp(5.0)))
        cout << rho << endl;

    return -log(rho);
}

/**************************************************************************//**
*  The derivative of the effective potential with respect to lambda.
*
*  Computes the non-local two-body effective pair potential.
*
*  @param sep1 the first separation
*  @param sep2 the second separation
*  @param lambda \lambda = \hbar^2/2m
*  @param tau the imaginary timestep tau
*  @return the derivative of the effective potential with respect to lambda
******************************************************************************/
double Delta1DPotential::dVdlambda(const dVec &sep1, const dVec &sep2,
                                      double lambda, double tau) {

    double r1 = sep1[0];
    double r2 = sep2[0];

    double s = g/(4.0*lambda)*sqrt(2.0*lambda*tau);
    double z = 1.0/sqrt(8.0*lambda*tau);
    double r = z*(abs(r1)+abs(r2));
    double expArg = s*(s-2*r)+z*z*(r1-r2)*(r1-r2);

    double expVal = exp(expArg);
    double erfVal = erfc(r-s);
    double rho = 1.0+sqrt(M_PI)*s*expVal*erfVal;

    double dsdl = (-1.0)*g*sqrt(tau)/(4.0*sqrt(2.0)*lambda*sqrt(lambda));
    double dzdl = (-1.0)/(4.0*sqrt(2.0)*lambda*sqrt(lambda*tau));
    double drdl = (-1.0)*(abs(r1)+abs(r2))/(4.0*sqrt(2.0)*sqrt(lambda*tau)*lambda);
    double dexpArgdl = dsdl*(s-2.0*r)+s*(dsdl-2.0*drdl)+2.0*z*dzdl*(r1-r2)*(r1-r2);

    double drhodl = sqrt(M_PI)*dsdl*expVal*erfVal+sqrt(M_PI)*s*expVal*dexpArgdl*erfVal-(2.0)*s*expVal*exp((-1.0)*(r-s)*(r-s))*(drdl-dsdl);

    return (-1.0)/rho*drhodl;
}

/**************************************************************************//**
*  The derivative of the effective potential with respect to tau.
*
*  Computes the non-local two-body effective pair potential.
*
*  @param sep1 the first separation
*  @param sep2 the second separation
*  @param lambda \lambda = \hbar^2/2m
*  @param tau the imaginary timestep tau
*  @return the derivative of the effective potential with respect to tau
******************************************************************************/
double Delta1DPotential::dVdtau(const dVec &sep1, const dVec &sep2,
                                   double lambda, double tau) {

    double r1 = sep1[0];
    double r2 = sep2[0];

    double s = g/(4.0*lambda)*sqrt(2.0*lambda*tau);
    double z = 1.0/sqrt(8.0*lambda*tau);
    double r = z*(abs(r1)+abs(r2));
    double expArg = s*(s-2*r)+z*z*(r1-r2)*(r1-r2);

    double expVal = exp(expArg);
    double erfVal = erfc(r-s);
    double rho = 1.0+sqrt(M_PI)*s*expVal*erfVal;

    double dsdt = g/(4.0*sqrt(2.0)*sqrt(lambda)*sqrt(tau));
    double dzdt = (-1.0)/(4.0*sqrt(2.0)*tau*sqrt(lambda*tau));
    double drdt = (-1.0)*(abs(r1)+abs(r2))/(4.0*sqrt(2.0)*sqrt(lambda*tau)*tau);
    double dexpArgdt = dsdt*(s-2.0*r)+s*(dsdt-2.0*drdt)+2.0*z*dzdt*(r1-r2)*(r1-r2);

    double drhodt = sqrt(M_PI)*dsdt*expVal*erfVal+sqrt(M_PI)*s*expVal*dexpArgdt*erfVal-(2.0)*s*expVal*exp((-1.0)*(r-s)*(r-s))*(drdt-dsdt);

    double dVdtau = (-1.0)/rho*drhodt;
    if ( dVdtau != dVdtau )
        cout << r1 << '\t' << r2 << '\t' <<  expArg << '\t' <<  expVal << '\t' << erfVal << '\t' << rho << '\t' << drhodt << '\t' << dVdtau << endl;

    return dVdtau;
}



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// HARD ROD POTENTIAL CLASS --------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

/**************************************************************************//**
 *  Constructor.
 *  @param _a The radius of the hard rod (also the scattering length)
******************************************************************************/
HardRodPotential::HardRodPotential(double _a) :
	PotentialBase(),
    a(_a) {

}

/**************************************************************************//**
 *  Destructor.
******************************************************************************/
HardRodPotential::~HardRodPotential() {
// empty deconstructor
}

/**************************************************************************//**
 *  The effective potential.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-17.
 *
 *  @param sep1 The first separation
 *  @param sep2 The second separation
 *  @param lambdaTau The product of \f$\lambda = \hbar^2/2m\f$ and \f$\tau\f$
 *  @return the two-body effective pair potential
******************************************************************************/
double HardRodPotential::V(const dVec &sep1, const dVec &sep2,
        double lambdaTau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    /* We need to enforce the distinguishable particle constraint at short
     * imaginary times */
    if ( (sep1[0]*sep2[0] < 0.0) || (r1 <= a ) || (r2 <= a) )
        return LBIG;

    double t1 = -(r1-a)*(r2-a)/(2.0*lambdaTau);

    return (-log(1.0 - exp(t1)));
}

/**************************************************************************//**
 *  The derivative of the effective potential with respect to lambda.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-17.
 *
 *  @param sep1 the first separation
 *  @param sep2 the second separation
 *  @param lambda \f$\lambda = \hbar^2/2m\f$
 *  @param tau the imaginary timestep tau
 *  @return the derivative of the effective potential with respect to lambda
******************************************************************************/
double HardRodPotential::dVdlambda(const dVec &sep1, const dVec &sep2,
        double lambda, double tau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    double t1 = (r1-a)*(r2-a);
    double t2 = t1/(2.0*lambda*tau);

    return ((0.5*t1/(exp(t2)-1.0))/(lambda*lambda*tau));
}

/**************************************************************************//**
 *  The derivative of the effective potential with respect to tau.
 *
 *  Computes the non-local two-body effective pair potential.
 *
 *  Tested and working with Mathematica on 2013-06-17.
 *
 *  @param sep1 the first separation
 *  @param sep2 the second separation
 *  @param lambda \f$\lambda = \hbar^2/2m\f$
 *  @param tau the imaginary timestep tau
 *  @return the derivative of the effective potential with respect to tau
******************************************************************************/
double HardRodPotential::dVdtau(const dVec &sep1, const dVec &sep2,
        double lambda, double tau) {

    double r1 = sqrt(dot(sep1,sep1));
    double r2 = sqrt(dot(sep2,sep2));

    double t1 = (r1-a)*(r2-a);
    double t2 = t1/(2.0*lambda*tau);

    return ((0.5*t1/(exp(t2)-1.0))/(lambda*tau*tau));
}
