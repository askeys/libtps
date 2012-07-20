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
 *  TpsTestRandomWalkOrderParam.cpp
 *  tpsapi
 *
 *  Created by askeys on 6/11/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestRandomWalkOrderParam.h"
#include "TpsSimulationPluginAtomic.h"
#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include <cmath>
#include <iostream>


TpsTestRandomWalkOrderParam::TpsTestRandomWalkOrderParam(double min_distance)
	: _min_distance(min_distance)
{
}

void TpsTestRandomWalkOrderParam::setDistanceCutoff(double min_distance)
{
	_min_distance = min_distance;
}

bool TpsTestRandomWalkOrderParam::hA(TpsTrajectory& traj)
{
	//this is ignored for full function of path
	return true;
}

bool TpsTestRandomWalkOrderParam::hB(TpsTrajectory& traj)
{
	double r = evaluate(traj, -1);
	//std::cerr << "HB " << r << "\n";
	
	if (r > _min_distance) {
		return true;
	}
	else {
		return false;
	}
}

double TpsTestRandomWalkOrderParam::evaluate(
	TpsTrajectory& traj, int timeslice_number)
{
	TpsSimulationPluginAtomic& sim = 
		traj.getSimulationPlugin().safeDowncastToAtomicPlugin();
	
	std::vector< std::vector<double> > x, x0;
	sim.readRestart(traj.getTimeslice(0).getFilename().c_str());
	sim.copyPositionsTo(x0);
	int last_timeslice = traj.getNumberOfTimeslices()-1;
	sim.readRestart(traj.getTimeslice(last_timeslice).getFilename().c_str());
	sim.copyPositionsTo(x);
	
	double rsq = 0.0;
	int dim = sim.getDimensions();
	
	for (int k=0; k<dim; k++) {
		double dx = x[0][k] - x0[0][k];
		rsq += dx*dx;
	}
	return sqrt(rsq);
}		
