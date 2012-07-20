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
//Typical compile command:
//c++ ../plugin/TpsSimulationPluginHOOMD.cpp helloHOOMD.cpp $HOOMD_FLAGS -ltps

#undef ENABLE_CUDA

#include <tps/tps.h>
#include "TpsSimulationPluginHOOMD.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <hoomd/LJForceCompute.h>
#include <hoomd/HOOMDInitializer.h>
#include <hoomd/HOOMDBinaryInitializer.h>
#include <hoomd/BinnedNeighborList.h>
#include <hoomd/Initializers.h>
#include <hoomd/HOOMDDumpWriter.h>
#include <hoomd/TempCompute.h>
#include <hoomd/System.h>
#include <hoomd/SFCPackUpdater.h>
#include <hoomd/TwoStepNVE.h>
#include <hoomd/IntegratorTwoStep.h>

#ifdef ENABLE_CUDA
#include <hoomd/BinnedNeighborListGPU.h>
#include <hoomd/TwoStepNVEGPU.h>
#include <hoomd/LJForceComputeGPU.h>
#endif

#include <cmath>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>

int N=1000;

using namespace std;
using namespace boost;

void randpoints3d(double *x, double *y, double *z, int N, double box)
{
	int i = 0;
	double cut = 0.82*0.82;
	
	while(i < N)
	{
		//cout << i << "\n";
		double xi = drand48() * box - 0.5 * box;
		double yi = drand48() * box - 0.5 * box;
		double zi = drand48() * box - 0.5 * box;
		
		int overlap = 0;
		for(int j=0; j<i; j++)
		{
			double dx = xi - x[j];
			dx -= box * rint ( dx / box);
			double dy = yi - y[j];
			dy -= box * rint ( dy / box);
			double dz = zi - z[j];
			dz -= box * rint ( dz / box);
			double rsq = dx*dx + dy*dy + dz*dz;
			if(rsq < cut)
			{
				overlap = 1;
				break;
			}
		}
		
		if(!overlap)
		{
			x[i] = xi;
			y[i] = yi;
			z[i] = zi;
			i++;
		}
	}
}

void writeinitfile(
	const char* filename="init_file.xml", int N=1000, int rho=1.0)
{
	double L = pow(N/rho, 1.0/3.0);
			
	std::ofstream fout("init_file.xml");
	
	fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	fout << "<hoomd_xml>\n";
	fout << "<configuration time_step=\"0\">\n";
	fout << "<box units=\"sigma\"  Lx=\"" << L << "\" Ly=\"" << L << "\" Lz=\"" << L << "\"/>\n";
	fout << "<!-- Setup the initial condition to place all particles in a line -->\n";
	
	fout << "<position units=\"sigma\">\n";

	double *x = new double[N];
	double *y = new double[N];
	double *z = new double[N];
			
	randpoints3d(x, y, z, N, L);
					
	for(int i=0; i<N; i++) {
			fout << x[i] << "\t" << y[i] << "\t" << z[i] << "\n";
	}
					
	delete x;
	delete y;
	delete z;

	fout << "</position>\n";

	fout << "<type>\n";
	for(int i=0; i<N; i++) {
		fout << "A\n";
	}
	fout << "</type>\n";
	fout << "</configuration>\n</hoomd_xml>\n";
}

double displacement(
    vector< vector<double> > &x1, vector< vector<double> >& x2, BoxInfo& box)
{
    double diff = 0.0;
    for (unsigned int i=0; i<x1.size(); i++) {
        double dx = x1[i][0]-x2[i][0];
        dx -= box.period[0]*rint(dx/box.period[0]);
        double dy = x1[i][1]-x2[i][1];
        dy -= box.period[1]*rint(dy/box.period[1]);
        double dz = x1[i][2]-x2[i][2];
        dz -= box.period[1]*rint(dz/box.period[1]);
        diff += sqrt(dx*dx + dy*dy + dz*dz);
    }
    return diff;
}

void checkdivergence(double error, double tol, const char* testinfo)
{
    cout << "Test = " << testinfo << endl;
    if (error > tol) {
        cout << "Error: tolerance exceeded for fix_nve reversibility" << endl;
    }
    else {
	cout << "Success!" << endl;    
    }
}

int main(int argc, char** argv)
{
	cout << "Testing the HOOMD plugin..." << endl;
	writeinitfile("init_file.xml", N);
    HOOMDInitializer init("init_file.xml");
    //HOOMDBinaryInitializer init("restart_file.bin");
    
    #ifdef ENABLE_CUDA
    shared_ptr<SystemDefinition> sysdef(new SystemDefinition(init, ExecutionConfiguration(ExecutionConfiguration::GPU)));
    #else
    shared_ptr<SystemDefinition> sysdef(new SystemDefinition(init, ExecutionConfiguration(ExecutionConfiguration::CPU)));
    #endif
        
    #ifdef ENABLE_CUDA
    shared_ptr<BinnedNeighborListGPU> nlist(new BinnedNeighborListGPU(sysdef, Scalar(2.5), Scalar(0.3)));
    nlist->setStorageMode(NeighborList::full);
    #else
    shared_ptr<BinnedNeighborList> nlist(new BinnedNeighborList(sysdef, Scalar(2.5), Scalar(0.3)));
    nlist->setStorageMode(NeighborList::half);
    #endif

    #ifdef ENABLE_CUDA
    shared_ptr<LJForceComputeGPU> fc(new LJForceComputeGPU(sysdef, nlist, Scalar(2.5)));
    #else
    shared_ptr<LJForceCompute> fc(new LJForceCompute(sysdef, nlist, Scalar(2.5)));
    #endif

    // setup some values for alpha and sigma
    Scalar epsilon = Scalar(1.0);
    Scalar sigma = Scalar(1.0);
    Scalar alpha = Scalar(1.0);
    Scalar lj1 = Scalar(4.0) * epsilon * pow(sigma,Scalar(12.0));
    Scalar lj2 = alpha * Scalar(4.0) * epsilon * pow(sigma,Scalar(6.0));

    // specify the force parameters
    fc->setParams(0,0,lj1,lj2);
    fc->setShiftMode(LJForceCompute::shift);
    
    shared_ptr<ParticleSelector> selector_all(new ParticleSelectorTag(sysdef, 0, N-1));
    shared_ptr<ParticleGroup> group_all(new ParticleGroup(sysdef, selector_all));

    #ifdef ENABLE_CUDA
    shared_ptr<TwoStepNVEGPU> two_step_nve(new TwoStepNVEGPU(sysdef, group_all));
    //shared_ptr<NVEUpdaterGPU> integrator(new NVEUpdaterGPU(sysdef, deltaT));
    #else
    shared_ptr<TwoStepNVE> two_step_nve(new TwoStepNVE(sysdef, group_all));
    //shared_ptr<NVEUpdater> integrator(new NVEUpdater(sysdef, deltaT));
    #endif

    Scalar deltaT = Scalar(0.0025);
    shared_ptr<IntegratorTwoStep> integrator(new IntegratorTwoStep(sysdef, deltaT));
    integrator->addIntegrationMethod(two_step_nve);
    integrator->addForceCompute(fc);
    shared_ptr<SFCPackUpdater> sfcpack(new SFCPackUpdater(sysdef, 1.0));

    int t0 = 0;
    shared_ptr<System> hoomd_system(new System(sysdef, t0));
    hoomd_system->setIntegrator(integrator);
    hoomd_system->addUpdater(sfcpack, "sfcpack", 500);
    
	cout << "Success!" << endl;

	TpsSimulationPluginHOOMD hoomd(hoomd_system);
    vector< vector<double> > x1, x2;
    BoxInfo box;
    hoomd.copyBoxTo(box);
    double diff;

	cout << "********************************" << endl;
	cout << "Testing run command: " << endl;

    hoomd.copyPositionsTo(x1);
    hoomd.run(1);
    hoomd.copyPositionsTo(x2);

    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    cout << "Test = 1 step forward" << endl;
	cout << "Success!" << endl;

    hoomd.copyPositionsTo(x1);
    hoomd.run(10);
    hoomd.copyPositionsTo(x2);

    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    cout << "Test = 10 step forward" << endl;
	cout << "Success!" << endl;

    hoomd.copyPositionsTo(x1);
    hoomd.run(100);
    hoomd.copyPositionsTo(x2);

    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    cout << "Test = 100 step forward" << endl;
	cout << "Success!" << endl;

    hoomd.copyPositionsTo(x1);
    hoomd.run(1000);
    hoomd.copyPositionsTo(x2);

    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    cout << "Test = 1000 steps forward" << endl;

	cout << "Success!" << endl;

	cout << "********************************" << endl;
	cout << "Testing reversibility: " << endl;

    hoomd.copyPositionsTo(x1);
    hoomd.run(1);
    hoomd.invertVelocities();
    hoomd.run(1);
    hoomd.copyPositionsTo(x2);
   
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-17, "1 step forward, 1 step backward");
    
    hoomd.copyPositionsTo(x1);
    hoomd.run(10);
    hoomd.invertVelocities();
    hoomd.run(10);
    hoomd.copyPositionsTo(x2);
   
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-16, "10 steps forward, 10 steps backward");
    
    hoomd.copyPositionsTo(x1);
    hoomd.run(100);
    hoomd.invertVelocities();
    hoomd.run(100);
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-14, "100 steps forward, 100 steps backward");

    hoomd.copyPositionsTo(x1);
    hoomd.run(1000);
    hoomd.invertVelocities();
    hoomd.run(1000);
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-11, "1000 steps forward, 1000 steps backward");

	cout << "********************************" << endl;
	cout << "Testing restart : " << endl;
    
    hoomd.useRAM(false);
    hoomd.writeRestart("temp.restart");
    hoomd.copyPositionsTo(x1);
    hoomd.readRestart("temp.restart");
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-18, "before restart == after restart");

    hoomd.writeRestart("temp.restart");
    hoomd.run(100);
    hoomd.copyPositionsTo(x1);
    hoomd.readRestart("temp.restart");
    hoomd.run(100);
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-12, "100 steps (restart vs. non-restart");

    hoomd.useRAM(true);
    hoomd.writeRestart("temp.memory.restart");
    hoomd.copyPositionsTo(x1);
    hoomd.readRestart("temp.memory.restart");
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-18, "before restart == after restart (RAM)");

    hoomd.writeRestart("temp.memory.restart");
    hoomd.run(100);
    hoomd.copyPositionsTo(x1);
    hoomd.readRestart("temp.memory.restart");
    hoomd.run(100);
    hoomd.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-12, "100 steps (restart vs. non-restart) (RAM)");

	return 0;
}
