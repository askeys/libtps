/*
LibTPS -- A Transition Path Sampling Library
(LibTPS) Open Source Software License Copyright 2012 The Regents of the 
University of Michigan All rights reserved.

LibTPS may contain modifications ("Contributions") provided, and to which
copyright is held, by various Contributors who have granted The Regents of the
University of Michigan the right to modify and/or distribute such Contributions.

You may redistribute, use, and create derivate works of LibTPS, in source
and binary forms, provided you abide by the following conditions:

* Redistributions of source code must retain the above copyright notice, this
list of conditions, and the following disclaimer both in the code and
prominently in any materials provided with the distribution.

* Redistributions in binary form must reproduce the above copyright notice, this
list of conditions, and the following disclaimer in the documentation and/or
other materials provided with the distribution.

* Apart from the above required attributions, neither the name of the copyright
holder nor the names of LibTPS's contributors may be used to endorse or
promote products derived from this software without specific prior written
permission.

Disclaimer

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND/OR ANY
WARRANTIES THAT THIS SOFTWARE IS FREE OF INFRINGEMENT ARE DISCLAIMED.

IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <glotzapi/glotzapi.h>
#include <vmdstream/vmdstream.h>
#include <tps/tps.h>
#include <iostream>
#include <ctime>
using namespace std;

class OrderParamVapor : public TpsOrderParameter
{
	bool hA(TpsTrajectory& traj)
	{
		double r = evaluate(traj, 0);
		std::cerr << "HA " << r << "\n";
		if (r < 1.85) {
			return true;
		}
		else {
			return false;
		}
	}

	bool hB(TpsTrajectory& traj)
	{
		int last_timeslice = traj.getNumberOfTimeslices()-1;
		double r = evaluate(traj, last_timeslice);
		std::cerr << "HB " << r << "\n";
		if (r > 2.5) {
			return true;
		}
		else {
			return false;
		}
	}

	double evaluate(
		TpsTrajectory& traj, int timeslice_number)
	{
		TpsSimulationPluginAtomic& sim = 
			traj.getSimulationPlugin().safeDowncastToAtomicPlugin();
		
		sim.readRestart(traj.getTimeslice(timeslice_number).getFilename().c_str());
		std::vector< std::vector<double> > x;
		sim.copyPositionsTo(x);
		BoxInfo box;
		sim.copyBoxTo(box);

		int dim = sim.getDimensions();
		double rsqmax = 0.0;
		
		for (int i=0; i<x.size(); i++) {
			double rsqmin = 1000.0;
			for (int j=0; j<x.size(); j++) {
				if (i==j) {
					continue;
				}
				double rsq = 0.0;
				for (int k=0; k<dim; k++) {
					double dx = x[i][k] - x[j][k];
					if (box.periodic[k]) {
						dx -= box.period[k]*rint(dx/box.period[k]);
					}
					rsq += dx*dx;
				}
				rsqmin = std::min(rsqmin, rsq);
			}
			rsqmax = std::max(rsqmax, rsqmin);
		}
		return sqrt(rsqmax);
	}		
};

class CustomTps : public TpsAlgorithmTPS
{
public:
	CustomTps(TpsTrajectoryEnsemble& tpe, TpsRNG& rng, TpsOrderParameter& op, 
		TpsInitializer& init) : 
		TpsAlgorithmTPS(tpe, rng, op, init),
		_tpe(tpe),
		_writer_acc("accepted_trajectory%.xyz", 50),
		_writer_rej("rejected_trajectory%.xyz", 50)
	{
		time_t s1, s2, s3;
		s1 = time(NULL)%13; s2 = time(NULL)%7; s3 = time(NULL)%30;
		cout << s1 <<"\t" << s2 <<"\t" << s3 << endl;
		unsigned short newseed[3] = {s1, s2, s3};
		seed48(newseed);
		
		//vmd connect on random port
		_vmdsock = newvmdsock("vmd", 5000 + int(drand48()*1000));
		vmdstream* vmdp = new vmdstream(_vmdsock);
		_vmd = vmdp;
		*_vmd << "display projection Orthographic\n";
		*_vmd << "axes location off\n";
		*_vmd << "color Display Background iceblue\n";
		_vmd->flush();
	}
	
	~CustomTps()
	{
		closevmdsock(_vmdsock);
		delete _vmd;
	}
	
	void callbackOnTrialMoveAccepted()
	{
		std::cerr << "ACCEPTED!\n";
		_writer_acc.analyze(_tpe.getLastTrajectory());
		*_vmd << "mol off top\n";
		*_vmd << "mol new accepted_trajectory" << _tpe.getLastTrajectory().getID() << ".xyz\n";
		*_vmd << "mol modstyle 0 top VDW 0.500000 15.000000\n";
		_vmd->flush();
	}

	void callbackOnTrialMoveRejected()
	{
		std::cerr << "REJECTED!\n";
		_writer_rej.analyze(_tpe.getTrajectory(-1));
		*_vmd << "mol off top\n";
		*_vmd << "mol new rejected_trajectory-1.xyz\n";
		*_vmd << "mol modstyle 0 top VDW 0.500000 15.000000\n";
		*_vmd << "mol modcolor 0 top ResID\n";
		_vmd->flush();
	}
	
	vmdsock_t _vmdsock;
	vmdstream* _vmd;
	TpsAnalysisWriteXYZAtomic _writer_acc, _writer_rej;
	TpsTrajectoryEnsemble& _tpe;
	
};

int main()
{
	cout << "Initializing simulation\n";
	//2=2d
	IAPISimulation *simulation = new GlotzillaSimulation(2);
	IAPIBox *box = simulation->getBox(0);
	// Set simulation 
	IAPIBoundary *boundary = new GlotzillaBoundaryPeriodic(simulation);
	boundary->setDimensions(new GlotzillaVector2D(25, 25));
	box->setBoundary(boundary);
	// Create atom type. Create species
	
	IAPIAtomType *atomType = new GlotzillaAtomTypeSphere(1);
	GlotzillaSpeciesSpheresMono *species = 
		new GlotzillaSpeciesSpheresMono(simulation, atomType);
	species->addChildType(atomType);
	// Add species to species manager
	simulation->getSpeciesManager()->addSpecies(species);
	// Add a molecule to the box
	box->setNMolecules(species, 100);
	GlotzillaIntegratorVelocityVerlet *integrator = 
		new GlotzillaIntegratorVelocityVerlet(simulation);
	
	integrator->setTimestep(0.0025);
	
	IAPIAtomList *al = box->getLeafList();
		
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; j++) {
			
			int ii = i-5; 
			int jj = j-5;
			
			IAPIAtomKinetic *atom = 
				dynamic_cast<IAPIAtomKinetic*>(al->getAtom(i*10+j));
			GlotzillaVector2D x(ii*1.2,jj*1.2);
			atom->getPosition()->E(&x);
			GlotzillaVector2D v(0.1*(2*drand48()-1), 0.1*(2*drand48()-1));
			atom->getVelocity()->E(&v);
		}
	}
	
	GlotzillaPotentialMaster* pmaster = new GlotzillaPotentialMasterBruteForce(simulation);
	
	GlotzillaPotentialTwelveSixLj* pot = new GlotzillaPotentialTwelveSixLj;
	pmaster->addPotential(pot, atomType, atomType);
		
	//rescale the velocities
	for (int t=0; t<10; t++) {
		for (int t2=0; t2<100; t2++) {
			integrator->doStep();
		}
		IAPIAtomList *al = simulation->getBox(0)->getLeafList();
		int natom = al->getAtomCount();
		int dim = 2;
			
		for (int i=0; i<natom; i++) {
			IAPIAtomKinetic *atom = dynamic_cast<IAPIAtomKinetic*>(al->getAtom(i));
			IAPIVector* v = atom->getVelocity();
			double vnew[dim];
			for (int k=0; k<dim; k++) {
				vnew[k] = v->x(k)*.9;
			}
			atom->getVelocity()->E(vnew);
		}
	}

	for (int t=0; t<1000; t++) {
		integrator->doStep();
	}

	
	//simulation is set up, now start the path sampling:
	
	TpsSimulationPluginAtomicMolSimAPI sim(simulation, integrator, pmaster, 2);
	//sim.setRestartMode(TpsSimulationPluginAtomicMolSimAPI::DISK | TpsSimulationPluginAtomicMolSimAPI::ASCII);

	IAPIIntegrator* integ = integrator;	
	TpsRNG48 rng(19, 7, 13);
	OrderParamVapor op;	
	TpsInitializerBruteForce init;
	TpsTrajectoryUniformStep trj(sim, 5000, 250);
	TpsTrajectoryEnsemble tpe(trj);

	trj.start();
	trj.extend();
		
	cerr << "Initializing path\n";
	CustomTps tps(tpe, rng, op, init);

	cerr << "Starting path sampling\n";
	TpsTimesliceModifierPerturbVelocities perturb(0.1, 1.0, true, true);
	TpsTrialMoveShot shot(perturb);
	tps.addTrialMove(shot, 0.5);
	TpsTrialMoveShift shift;
	tps.addTrialMove(shift, 0.5);
	
	int nstep = 0;
	while(1) {
		cout << "step " << nstep++ << endl;
		tps.doStep();
		cout << "\n\n************************************\n\n";
		cout << "Press return key to do a step, ctrl+c to quit\n";
		cout << "\n\n************************************\n\n";
		cin.get();
	}

	cerr << "Success!" << endl;
	return 0;

}