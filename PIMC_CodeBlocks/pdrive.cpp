/**
 * @file pdrive.cpp
 * @brief A dimensionally independent worm algorithm path integral monte carlo code driver.
 * @author Adrian Del Maestro
 * @date 10.14.2008
 */

#include "common.h"
#include "constants.h"
#include "container.h"
#include "path.h"
#include "potential.h"
#include "action.h"
#include "wavefunction.h"
#include "pimc.h"
#include "lookuptable.h"
#include "communicator.h"
#include "setup.h"
#include "cmc.h"
#include "move.h"

/**
 * Main driver.
 * Read in all program options from the user using boost::program_options and setup the simulation
 * cell, initial conditions and both the interaction and external potential. Either equilibrate or
 * restart a simulation, then start measuring. We output all the simulation parameters to disk as a
 * log file so that it can be restart again assigning it a unique PIMCID.
 * @see boost::program_options -- http://www.boost.org/doc/libs/1_43_0/doc/html/program_options.html
 */
int main (int argc, char *argv[]) {

    /* Get initial time */
    time_t start_time = time(NULL);
    time_t current_time; //current time
    bool wallClockReached = false;

	uint32 seed = 139853;	// The seed for the random number generator

	Setup setup;

	/* Attempt to parse the command line options */
    try {
		setup.getOptions(argc,argv);
    }
    catch(exception& ex) {
        cerr << "error: " << ex.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

	/* Parse the setup options and possibly exit */
	if (setup.parseOptions())
		return 1;

	/* The global random number generator, we add the process number to the seed (for
	 * use in parallel simulations.*/
	seed = setup.seed(seed);
	MTRand random(seed);

	/* Get the simulation box */
	Container *boxPtr = NULL;
	boxPtr = setup.cell();

	/* Create the worldlines */
	if (setup.worldlines())
		return 1;

	/* Setup the simulation constants */
	setup.setConstants();

	/* Setup the simulation communicator */
	setup.communicator();

    /* Get number of paths to use */
    int Npaths = constants()->Npaths();

    /* Create and initialize the Nearest Neighbor Lookup Table */
    boost::ptr_vector<LookupTable> lookupPtrVec;
    for(int i=0; i<Npaths; i++){
        lookupPtrVec.push_back(
                new LookupTable(boxPtr,constants()->numTimeSlices(),
                                constants()->initialNumParticles()));
    }

	/* Create and initialize the potential pointers */
	PotentialBase *interactionPotentialPtr = NULL;
	interactionPotentialPtr = setup.interactionPotential();

	PotentialBase *externalPotentialPtr = NULL;
	externalPotentialPtr = setup.externalPotential(boxPtr);

	/* Get the initial conditions associated with the external potential */
	/* Must use the copy constructor as we return a copy */
	Array<dVec,1> initialPos =
		externalPotentialPtr->initialConfig(boxPtr,random,constants()->initialNumParticles());

    externalPotentialPtr->output(9.0);
    exit(-1);

    // dVec pos;
    // pos = 0.0;
    // cout << "Testing Potential = " << externalPotentialPtr->V(pos) << endl;
    // exit(-1);

    /* Perform a classical canonical pre-equilibration to obtain a suitable
     * initial state */
	if (!constants()->restart()) {
       ClassicalMonteCarlo CMC(externalPotentialPtr,interactionPotentialPtr,random,boxPtr,
               initialPos);
       CMC.run(constants()->numEqSteps(),0);
   }

	/* Setup the path data variable */
    vector<Path *> pathPtrVec;
    for(int i=0; i<Npaths; i++){
        pathPtrVec.push_back(
                new Path(boxPtr,lookupPtrVec[i],constants()->numTimeSlices(),
                         initialPos,constants()->numBroken()));
    }

    /* The Trial Wave Function (constant for pimc) */
    WaveFunctionBase *waveFunctionPtr = NULL;
	waveFunctionPtr = setup.waveFunction(*pathPtrVec.front());

	/* Setup the action */
    vector<ActionBase *> actionPtrVec;
    for(int i=0; i<Npaths; i++){
        actionPtrVec.push_back(
                setup.action(*pathPtrVec[i],lookupPtrVec[i],externalPotentialPtr,
                             interactionPotentialPtr,waveFunctionPtr) );
    }

    /* The list of Monte Carlo updates (moves) that will be performed */
    vector< boost::ptr_vector<MoveBase> * > movesPtrVec;
    for(int i=0; i<Npaths;i++){
        movesPtrVec.push_back(
                setup.moves(*pathPtrVec[i],actionPtrVec[i],random).release());
    }

    /* The list of estimators that will be performed */
    /*vector< boost::ptr_vector<EstimatorBase> * > estimatorsPtrVec;
    for(int i=0; i<Npaths;i++){
        estimatorsPtrVec.push_back(
                setup.estimators(*pathPtrVec[i],actionPtrVec[i],random).release());
        if(i > 0) {
            for(uint32 j=0; j<estimatorsPtrVec.back()->size(); j++)
                estimatorsPtrVec.back()->at(j).appendLabel(str(format("%d") % (i+1)));
        }
    }
    */

   /* The list of estimators that will be performed */
    vector< boost::ptr_vector<EstimatorBase> * > estimatorsPtrVec;
    for(int i=0; i<Npaths;i++){
        estimatorsPtrVec.push_back(setup.estimators(*pathPtrVec[i],actionPtrVec[i],random).release());
        if(i>0){
            stringstream tmpSS;
            for(unsigned j=0; j<estimatorsPtrVec.back()->size(); j++){
                tmpSS.str("");
                tmpSS << i+1 ;
                estimatorsPtrVec.back()->at(j).appendLabel(tmpSS.str());
            }
        }
    }

    /* Setup the multi-path estimators */
    if(Npaths>1){
        estimatorsPtrVec.push_back(setup.multiPathEstimators(pathPtrVec,actionPtrVec).release());
    }




	/* Setup the pimc object */
    PathIntegralMonteCarlo pimc(pathPtrVec,random,movesPtrVec,estimatorsPtrVec,
                                !setup.params["start_with_state"].as<string>().empty(),
                                setup.params["bin_size"].as<int>());

	/* If this is a fresh run, we equilibrate and output simulation parameters to disk */
	if (!constants()->restart()) {

		/* Equilibrate */
		cout << format("[PIMCID: %09d] - Equilibration Stage.") % constants()->id() << endl;
		for (uint32 n = 0; n < constants()->numEqSteps(); n++)
			pimc.equilStep(n,setup.params.count("relax"),setup.params.count("relaxmu"));

		/* Output simulation details/parameters */
		setup.outputOptions(argc,argv,seed,boxPtr,lookupPtrVec.front().getNumNNGrid());
	}

	cout << format("[PIMCID: %09d] - Measurement Stage.") % constants()->id() << endl;

	/* Sample */
	int oldNumStored = 0;
	int outNum = 0;
	int numOutput = setup.params["output_config"].as<int>();
	uint32 n = 0;
	do {
		pimc.step();
		if (pimc.numStoredBins > oldNumStored) {
			oldNumStored = pimc.numStoredBins;
			cout << format("[PIMCID: %09d] - Bin #%4d stored to disk.") % constants()->id()
				% oldNumStored << endl;
		}
		n++;

		/* Output configurations to disk */
		if ((numOutput > 0) && ((n % numOutput) == 0)) {
			pathPtrVec.front()->outputConfig(outNum);
			outNum++;
		}

        /* Check if we've reached the wall clock limit*/
        if(constants()->wallClockOn()){
            current_time = time(NULL);
            if ( uint32(current_time)  > (uint32(start_time) + constants()->wallClock()) ){
                wallClockReached = true;
                break;
            }
        }
	} while (pimc.numStoredBins < setup.params["number_bins_stored"].as<int>());
    if (wallClockReached)
        cout << format("[PIMCID: %09d] - Wall clock limit reached.") % constants()->id() << endl;
    else
        cout << format("[PIMCID: %09d] - Measurement complete.") % constants()->id() << endl;

	/* Output Results */
    if (!constants()->saveStateFiles())
        pimc.saveStateFromStr();
	pimc.finalOutput();

	/* Free up memory */
	delete interactionPotentialPtr;
	delete externalPotentialPtr;
	delete boxPtr;
    delete waveFunctionPtr;

	initialPos.free();

	return 1;
}
