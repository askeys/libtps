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
/*
 *  TpsTestPeSurf2dMD.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestPeSurf2dOrderParam.h"
#include "TpsTimeslice.h"

#include <iostream>
#include <cmath>
#include <vector>

using std::vector;


TpsTestPeSurf2dOrderParamX::TpsTestPeSurf2dOrderParamX(double nstdev)
: _nstdev(nstdev)
{
	_xA = -1.03528;
	_stdev_xA = 0.177083;

	_xB = 1.0026;
	_stdev_xB = 0.191586;
}

void TpsTestPeSurf2dOrderParamX::sampleA(
	TpsTestPeSurf2d& sim, int nstep, int stepsize)
{
	double sumx = 0.0;
	double sumxsq = 0.0;

	for (int i=0; i<nstep; i++) {
		sim.run(stepsize);
		TpsTestPeSurf2dTimeslice s = sim.getCurrentTimeslice();
		sumx += s.x[0];
		sumxsq += s.x[0]*s.x[0];
	}
	int N = nstep;
	_stdev_xA = 1.0/N * sqrt(N * sumxsq - sumx*sumx);
	_xA = sumx / N;
}

void TpsTestPeSurf2dOrderParamX::sampleB(
	TpsTestPeSurf2d& sim, int nstep, int stepsize)
{
	double sumx = 0.0;
	double sumxsq = 0.0;

	for (int i=0; i<nstep; i++) {
		sim.run(stepsize);
		TpsTestPeSurf2dTimeslice s = sim.getCurrentTimeslice();
		sumx += s.x[0];
		sumxsq += s.x[0]*s.x[0];
	}
	int N = nstep;
	_stdev_xB = 1.0/N * sqrt(N * sumxsq - sumx*sumx);
	_xB = sumx / N;
}

bool TpsTestPeSurf2dOrderParamX::hA(TpsTrajectory& traj)
{
	double x = evaluate(traj, 0);
	
	if (x > _xA - _nstdev*_stdev_xA && x < _xA + _nstdev*_stdev_xA) {
		return true;
	}
	else {
		return false;
	}
}

bool TpsTestPeSurf2dOrderParamX::hB(TpsTrajectory& traj)
{
	int last_timeslice = traj.getNumberOfTimeslices()-1;
	double x = evaluate(traj, last_timeslice);

	if (x > _xB - _nstdev*_stdev_xB && x < _xB + _nstdev*_stdev_xB) {
		return true;
	}
	else {
		return false;
	}
}

double TpsTestPeSurf2dOrderParamX::evaluate(TpsTrajectory& traj, int timeslice_number)
{
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();
	TpsSimulationPluginAtomic& sim_atomic = sim.safeDowncastToAtomicPlugin();
	sim.readRestart(traj.getTimeslice(timeslice_number).getFilename().c_str());
	
	vector< vector<double> > x_array;
	sim_atomic.copyPositionsTo(x_array);	
	return x_array[0][0];
}		

TpsTestPeSurf2dOrderParamY::TpsTestPeSurf2dOrderParamY(double nstdev)
: _nstdev(nstdev)
{
	_yA = -0.0417531;
	_stdev_yA = 0.149455;
	_yB = 0.118493;
	_stdev_yB = 0.167281;
}

void TpsTestPeSurf2dOrderParamY::sampleA(
	TpsTestPeSurf2d& sim, int nstep, int stepsize)
{
	double sumy = 0.0;
	double sumysq = 0.0;
	for (int i=0; i<nstep; i++) {
		sim.run(stepsize);
		TpsTestPeSurf2dTimeslice s = sim.getCurrentTimeslice();
		sumy += s.x[1];
		sumysq += s.x[1]*s.x[1];
	}
	int N = nstep;
	_stdev_yA = 1.0/N * sqrt(N * sumysq - sumy*sumy);
	_yA = sumy / N;
}

void TpsTestPeSurf2dOrderParamY::sampleB(
	TpsTestPeSurf2d& sim, int nstep, int stepsize)
{
	double sumy = 0.0;
	double sumysq = 0.0;
	for (int i=0; i<nstep; i++) {
		sim.run(stepsize);
		TpsTestPeSurf2dTimeslice s = sim.getCurrentTimeslice();
		sumy += s.x[1];
		sumysq += s.x[1]*s.x[1];
	}
	int N = nstep;
	_stdev_yB = 1.0/N * sqrt(N * sumysq - sumy*sumy);
	_yB = sumy / N;
}

bool TpsTestPeSurf2dOrderParamY::hA(TpsTrajectory& traj)
{
	double y = evaluate(traj, 0);

	if (y > _yA - _nstdev*_stdev_yA && y < _yA + _nstdev*_stdev_yA) {
		return true;
	}
	else {
		return false;
	}
}

bool TpsTestPeSurf2dOrderParamY::hB(TpsTrajectory& traj)
{
	int last_timeslice = traj.getNumberOfTimeslices()-1;
	double y = evaluate(traj, last_timeslice);
		
	if (y > _yB - _nstdev*_stdev_yB && y < _yB + _nstdev*_stdev_yB) {
		return true;
	}
	else {
		return false;
	}
}

double TpsTestPeSurf2dOrderParamY::evaluate(TpsTrajectory& traj, int timeslice_number)
{
	TpsSimulationPlugin& sim = traj.getSimulationPlugin();
	TpsSimulationPluginAtomic& sim_atomic = sim.safeDowncastToAtomicPlugin();
	sim.readRestart(traj.getTimeslice(timeslice_number).getFilename().c_str());

	vector< vector<double> > x_array;
	sim_atomic.copyPositionsTo(x_array);	
	return x_array[0][1];
}		
