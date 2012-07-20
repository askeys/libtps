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
//Simple demo of using TPS to detect density fluctions

#include <tps/tps.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class CountParticlesOrderParam : public TpsOrderParameter
{
public:
	double R2;
	double NA, NB, deltaN;
	
	CountParticlesOrderParam( double inR, double inNA, double inNB )
		: R2( inR*inR ), NA( inNA ), NB( inNB ), deltaN(2)
		{}

	bool hA(TpsTrajectory& traj) {
		double N = evaluate(traj, 0);
		cout << "startpoint\t" << N << endl;
		return N <= 10;
	}
                
	bool hB(TpsTrajectory& traj) {
		int last_timeslice = traj.getNumberOfTimeslices()-1;
		double N = evaluate(traj, last_timeslice);
		cout << "endpoint:\t" << N << endl;
		return N >= 15;
	}

	double evaluate(TpsTrajectory& traj, int timeslice_number) {

		TpsSimulationPluginAtomic& sim = 
			traj.getSimulationPlugin().safeDowncastToAtomicPlugin();
		sim.readRestart(
			traj.getTimeslice(timeslice_number).getFilename().c_str());
		
		vector< vector<double> > x;
		sim.copyPositionsTo(x);
		BoxInfo box;
		sim.copyBoxTo(box);

		vector<double> center(2);
		center[0] = 0.5*(box.boxhi[0]+box.boxlo[0]);
		center[1] = 0.5*(box.boxhi[1]+box.boxlo[1]);

		int N = 0;
		for (int i=0; i<x.size(); i++) {
			double dx, dy;
			dx = x[i][0] - center[0];
			dy = x[i][1] - center[1];

			double r2 = dx*dx + dy*dy;
			if( r2 < R2 ) N++;
		}
		return (double)N;
	}               
};

class MySimulation : public TpsSimulationPluginLAMMPS {
public:
	
	//This method is called by the "writeTrajectory" analyzer at each write
	//point along the trajectory.  The analyzer is invoked every time a 
	//trajectory is accepted (see callbackOnTrialMoveAccepted below).
	
	void writeData(const char* filename, int pos) {
		if (pos == -1) {
			ofstream os(filename);
			os.close();
		}
		if (pos == 0) {
			ofstream os(filename, ios::app);
			vector< vector<double> > x;
			copyPositionsTo(x);
			BoxInfo box;
			copyBoxTo(box);

			vector<double> center(2);
			center[0] = 0.5*(box.boxhi[0]+box.boxlo[0]);
			center[1] = 0.5*(box.boxhi[1]+box.boxlo[1]);
			
			vector<int> red_list, yellow_list, white_list;
			
			int max_red = 40, max_white = 650;

			os << max_red + max_white << "\n";
			os << "Header" << endl;
			for( int i = 0; i < x.size(); i++ ) {
				double dx, dy;
				dx = x[i][0] - center[0];
				dy = x[i][1] - center[1];
				double r2 = dx*dx + dy*dy;

				if( r2 < 4.0*4.0 ) {
					red_list.push_back(i);
				}
				else {
					white_list.push_back(i);
				}
			}
			
			for (int i=0; i<max_red; i++) {
				if (i < red_list.size()) {
					os << "O\t" << x[red_list[i]][0]<<"\t";
					os << x[red_list[i]][1] << "\t0.0\n";
				}
				else {
					os << "O 0 0 0\n";
				}
			}
			for (int i=0; i<max_white; i++) {
				if (i < white_list.size()) {
					os << "H\t" << x[white_list[i]][0]<<"\t";
					os << x[white_list[i]][1] << "\t0.0\n";
				}
				else {
					os << "H 0 0 0\n";
				}
			}
			
			os.close();
		}
	}
};

class MyTPS : public TpsAlgorithmTPS
{
public:
	MyTPS( TpsTrajectoryEnsemble& t,
		   TpsRNG& r,
		   TpsOrderParameter& f,
		   TpsInitializer& i )
		: TpsAlgorithmTPS( t, r, f, i ), writer("traj%.xyz", 5)
		{
			
		}
	
	void callbackOnTrialMoveAccepted()
		{
			cout << "Move Accepted!" << endl;
			writer.analyze(_trajectory_factory.getLastTrajectory());
		}
	//Write an XYZ file whenever a new trajectory is created (% = trajectory #)
	TpsAnalysisWriteTrajectory writer;

};

int main( int argc, char **argv )
{
	//Initialize a LAMMPS simulation of the double-well system
	MySimulation lmp;
	lmp.readInputScript("deltaN.in");
	//lmp.useRAM(true);

	//Use a unix rand48 random number generator, seed 10, 7, 13
	TpsRNG48 rng(10, 7, 13);

	//Use random-spaced trajectories of length 1000 step size rand [0,50]
	TpsTrajectoryUniformStep trj(lmp, 1000, 50);
	TpsTrajectoryEnsemble trj_fac(trj);
                        
	//Use a brute force scheme to create the initial trajectory
	TpsInitializerBruteForce init;

	//Use our special class derived above as an order parameter
	CountParticlesOrderParam particles_in_vol( 4, 15, 20 );

	//Initialize the TPS algorithm
	MyTPS tps(trj_fac, rng, particles_in_vol, init);

	//Add a shooting move with velocity perturbations
	TpsTimesliceModifierPerturbVelocities dv(0.0001, 1.0, true, false);
	TpsTrialMoveShot shot(dv);
	tps.addTrialMove(shot);
        
	//Add a shifting move with min shift 50, max shift 400
	TpsTrialMoveShift shift(50, 500);
	tps.addTrialMove(shift);

	//Do 100 path sampling MC steps
	cout << "Ready" << endl;
	for (int i=0; i<100; i++) {
		tps.doStep();
		cout << "Done step " << i << endl;
	}
}
