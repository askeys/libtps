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
 *  TpsTrialMoveShotAimless.cpp
 *  tpsapi
 *
 *  Created by askeys on 6/9/09.
 *  Copyright 2009 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrialMoveShotAimless.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsTimeslice.h"
#include "TpsSimulationPlugin.h"
#include <iostream>
#include <sstream>

TpsTrialMoveShotAimless::TpsTrialMoveShotAimless(int delta)
: _delta_timeslices(delta), _n_shooting_pts(0), _old_shooting_pt(-1)
{	
}

TpsTrialMoveShotAimless::TpsTrialMoveShotAimless(
	TpsTimesliceModifier& p, int delta)
:	TpsTrialMoveShot(p), 
	_delta_timeslices(delta), 
	_n_shooting_pts(0), 
	_old_shooting_pt(-1)
{	
}

void TpsTrialMoveShotAimless::setDeltaTimeslices(int delta)
{
	_delta_timeslices = delta;
}

std::vector<std::string>& TpsTrialMoveShotAimless::getShootingPoints()
{
	return _shooting_points;
}

void TpsTrialMoveShotAimless::clearShootingPoints(TpsSimulationPlugin& sim)
{
	for (unsigned int i=0; i<_shooting_points.size(); i++) {
		sim.freeRestart(_shooting_points[i].c_str());
	}
	_shooting_points.clear();
}


bool TpsTrialMoveShotAimless::doMove(TpsAlgorithmTPS& controller)
{	
	int num = controller.getPathNumber();
	TpsTrajectory& old_traj = 
		controller.getTrajectoryFactory().getTrajectory(num);
	TpsTrajectory& new_traj = 
		controller.getTrajectoryFactory().getTrajectory(-1);
	TpsRNG& rng = controller.getRNG();
	TpsOrderParameter& path_function = controller.getPathFunction();
	
	if (_old_shooting_pt < 0) {
		_old_shooting_pt = old_traj.getNumberOfTimeslices()/2;
	}
	
	//choose a new shooting point at old point or at +/- delta_timeslices
	//int k = (rng.randInt()%3 - 1)*_delta_timeslices;
	//question: can we do this:
	int k = rng.randInt()%(2*_delta_timeslices) - _delta_timeslices;
	
	int new_shooting_pt = _old_shooting_pt + k;		
	new_traj.clear();
		
	//make sure that the shooting point is not off the trajectory (unlikely)
	if (new_shooting_pt >= old_traj.getNumberOfTimeslices()) {
		new_shooting_pt = old_traj.getNumberOfTimeslices()-1;
	}
	else if (new_shooting_pt < 0) {
		new_shooting_pt = 0;
	}
		
	new_traj.shootFrom(old_traj, new_shooting_pt, _perturbation, 
		_n_tiny_timestep, _size_tiny_timestep);

	TpsTimeslice shooting_timeslice = new_traj.getTimeslice(0);
	int t_shot = old_traj.getTimeslice(new_shooting_pt).getTimesteps();
	int t_half_traj = old_traj.getTargetLength()/2;
	
	new_traj.setTimeWindow(t_shot-t_half_traj, t_shot+t_half_traj);
	new_traj.extendBackward();
	//trajectory should now end/start in one of the stable states
	if (!(path_function.hA(new_traj) || path_function.hB(new_traj))) {
		old_traj.incrementWeight();
		return false;
	}
	new_traj.extendForward();

	//allow fw and bw paths
	TpsTrajectory& inv_traj = 
		controller.getTrajectoryFactory().getTrajectory(-2);
	inv_traj.clear();
	inv_traj.copy(new_traj);
	inv_traj.invert();
	
	if (path_function.isTransitionPath(new_traj)
		|| path_function.isTransitionPath(inv_traj)) {
		_old_shooting_pt = new_traj.find(shooting_timeslice);
		
		TpsSimulationPlugin& sim = new_traj.getSimulationPlugin();
		std::ostringstream os;
		os << "shooting_point_" << _n_shooting_pts++; 
		sim.copyRestart(shooting_timeslice.getFilename().c_str(), os.str().c_str());
		_shooting_points.push_back(os.str());		
		controller.getTrajectoryFactory().moveTrajectory(-1, num+1);
		controller.incrementPathNumber();
		return true;
	}
	else {
		old_traj.incrementWeight();
	}

	inv_traj.clear();
	return false;
}
