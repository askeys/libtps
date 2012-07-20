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
/******************************************************************************
  Compile command (example):
    mpic++ ../plugin/TpsSimulationPluginLAMMPS.cpp lammps_nve_test.cpp -D_USE_LAMMPS_PATCH -ltps -llammps -o nve_test
  Run command:
    ./nve_test -screen none
*******************************************************************************/

#include <tps/tps.h>
#include "../plugin/TpsSimulationPluginLAMMPS.h"

#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

void makepts(double *x, double *y, double *z, int N, double box)
{
	int i = 0;
	double cut = 0.82*0.82;
	
	while (i < N) {

		double xi = drand48() * box - 0.5 * box;
		double yi = drand48() * box - 0.5 * box;
		double zi = drand48() * box - 0.5 * box;
		
		int overlap = 0;
		for(int j=0; j<i; j++) {
			double dx = xi - x[j];
			dx -= box * rint ( dx / box);
			double dy = yi - y[j];
			dy -= box * rint ( dy / box);
			double dz = zi - z[j];
			dz -= box * rint ( dz / box);
			double rsq = dx*dx + dy*dy + dz*dz;
			if(rsq < cut) {
				overlap = 1;
				break;
			}
		}
		
		if(!overlap) {
			x[i] = xi;
			y[i] = yi;
			z[i] = zi;
			i++;
		}
	}
}

void writedatafile(int N)
{
	int Na = 0.8*N;
	double rho = 1.2;
	double L = pow(N/rho, 1.0/3.0);

	ofstream f("data.txt");
	
	f << "TPSglass data file\n";
	f << "\t" << N << " atoms\n";
	f << "\n";
	f << "\t2 atom types\n";
	f << "\n";
	f << -L/2.0 << "\t" << L/2.0 << " xlo xhi\n";
	f << -L/2.0 << "\t" << L/2.0 << " ylo yhi\n";
	f << -L/2.0 << "\t" << L/2.0 << " zlo zhi\n";
	
	f << "Masses\n\n";
	f << "\t1\t"<<1.0 << "\n";
	f << "\t2\t"<<1.0 << "\n";
	
	f << "Atoms\n\n";
	
	double *x = new double[N];
	double *y = new double[N];
	double *z = new double[N];
	
	makepts(x, y, z, N, L);
	
	for (int i=0; i<N; i++) {
		int type;
		if (i < Na) {
            type = 1;
        }
		else {
            type = 2;
        }
		f << "\t" << i+1 << "\t"<< type << "\t" << x[i] << "\t" << y[i] << "\t" << z[i] << "\n";
	}
	
	delete x;
	delete y;
	delete z;
}

void writeinputfile()
{
    ofstream f("input.txt");
    assert(f.good());
    
    f << "#Kob-Andersen binary LJ supercooled liquid\n";
    f << "echo none\n";
    f << "log none\n";
    f << "units           lj\n";
    f << "atom_style      atomic\n";
    f << "atom_modify     map array\n";
    f << "dimension       3\n";

    f << "read_data		data.txt\n";

    f << "pair_style		lj/cut 2.5\n";
    f << "pair_coeff		1 1 1.0 1.0 2.5\n";
    f << "pair_coeff		2 2 0.5 0.88 2.5\n";
    f << "pair_coeff		1 2 1.5 0.8 2.5\n";

    f << "neighbor		0.3 nsq\n";
    f << "neigh_modify    check yes\n";

    f << "timestep		0.0025\n";

    f << "fix				2 all momentum 1 linear 1 1 1\n";
    f << "fix				1 all nve\n";

    f << "fix				3 all temp/rescale 1 0.5 0.5 0.05 1.0\n";
    f << "run               5000\n";
    f << "unfix				3\n";
    
    f.close();
}

double displacement(
    vector< vector<double> > &x1, vector< vector<double> >& x2, BoxInfo& box)
{
    double diff = 0.0;
    for (int i=0; i<x1.size(); i++) {
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

double checkdivergence(double error, double tol, const char* testinfo)
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
	cout << "Testing the LAMMPS plugin..." << endl;
	cout << "Equilibrating the system (this will take ~2 minutes)..." << endl;
	
    int N=500;
    writeinputfile();
    writedatafile(N);

	TpsSimulationPluginLAMMPS lmp(argc, argv);
	lmp.readInputScript("input.txt");

    vector< vector<double> > x1, x2;
    BoxInfo box;
    lmp.copyBoxTo(box);
    double diff;

	cout << "********************************" << endl;
	cout << "Testing run command: " << endl;

    lmp.copyPositionsTo(x1);
    lmp.run(1000);
    lmp.copyPositionsTo(x2);

    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    cout << "Test = 1000 steps forward" << endl;

	cout << "Success!" << endl;

	cout << "********************************" << endl;
	cout << "Testing reversibility: " << endl;


    lmp.copyPositionsTo(x1);
    lmp.run(1);
    lmp.invertVelocities();
    lmp.run(1);
    lmp.copyPositionsTo(x2);
   
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-17, "1 step forward, 1 step backward");
    
    lmp.copyPositionsTo(x1);
    lmp.run(10);
    lmp.invertVelocities();
    lmp.run(10);
    lmp.copyPositionsTo(x2);
   
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-16, "10 steps forward, 10 steps backward");
    
    lmp.copyPositionsTo(x1);
    lmp.run(100);
    lmp.invertVelocities();
    lmp.run(100);
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-14, "100 steps forward, 100 steps backward");

    lmp.copyPositionsTo(x1);
    lmp.run(1000);
    lmp.invertVelocities();
    lmp.run(1000);
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-11, "1000 steps forward, 1000 steps backward");

	cout << "********************************" << endl;
	cout << "Testing restart : " << endl;
    
    lmp.useRAM(false);
    lmp.writeRestart("temp.restart");
    lmp.copyPositionsTo(x1);
    lmp.readRestart("temp.restart");
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-18, "before restart == after restart");

    lmp.writeRestart("temp.restart");
    lmp.run(100);
    lmp.copyPositionsTo(x1);
    lmp.readRestart("temp.restart");
    lmp.run(100);
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-12, "100 steps (restart vs. non-restart");

    lmp.useRAM(true);
    lmp.writeRestart("temp.memory.restart");
    lmp.copyPositionsTo(x1);
    lmp.readRestart("temp.memory.restart");
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-18, "before restart == after restart (RAM)");

    lmp.writeRestart("temp.memory.restart");
    lmp.run(100);
    lmp.copyPositionsTo(x1);
    lmp.readRestart("temp.memory.restart");
    lmp.run(100);
    lmp.copyPositionsTo(x2);
    diff = displacement(x1, x2, box);
    cout << "difference (per atom): " << diff/(double)N << endl;
    checkdivergence(diff/(double)N, 1e-12, "100 steps (restart vs. non-restart) (RAM)");
    
	return 0;
}