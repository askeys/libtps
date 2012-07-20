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
 *  TpsInitializerMidpointTS.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/24/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsInitializerMidpointTS.h"
#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include "TpsOrderParameter.h"
#include "TpsAnalysisCommitter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrialMoveShift.h"
#include <iostream>
#include <cmath>

TpsInitializerMidpointTS::TpsInitializerMidpointTS(
	TpsOrderParameter& order_param,
	TpsAnalysisCommitter& committer, 
	TpsInitializer& initializer, 
	int ntraj, 
	double tol,
	int max_tries)
:	
	_order_param(order_param),
	_committer(committer),
	_base_initializer(initializer),
	_n_committer_trajectories(ntraj),
	_tolerance(tol),
	_max_tries(max_tries)
{
}

TpsInitializerMidpointTS::~TpsInitializerMidpointTS()
{
}

void TpsInitializerMidpointTS::initializeTrajectory(
	TpsTrajectory& traj, TpsOrderParameter& path_function)
{	
	TpsTrajectoryEnsemble temp_trj_fac(traj);
	TpsRNG48 rng(11, 12, 1);
	TpsAlgorithmTPS tps(temp_trj_fac, rng, _order_param, _base_initializer);
	TpsTrialMoveShift shift(1, traj.getLength()/2);
	tps.addTrialMove(shift);
	_committer.setUseFullPath(true);
	double old_p = 0.0;
	double new_p = 0.0;
	
	for (int i=0; i<_max_tries; i++) {
		while(!(new_p > old_p)) {
			tps.doStep();
			old_p = new_p;
			new_p = shift.getAcceptanceProbability();
			std::cerr << old_p <<"\t" << new_p << std::endl;
		}
		
		std::cerr << "computing pb:\n";
		TpsTrajectory& trj = temp_trj_fac.getLastTrajectory();
		double pb = _committer.compute(
			trj, trj.getNumberOfTimeslices()/2, _n_committer_trajectories);
		
		std::cerr << pb << std::endl;
		
		if (fabs(pb - 0.5) <= _tolerance) {
			break;
		}
		if (i == _max_tries) {
			std::cerr << "ERROR: initialization to TS failed\n";
		}
	}
	
	traj.copy(temp_trj_fac.getLastTrajectory());
	temp_trj_fac.eraseTrajectories();
}
