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
 *  TpsTrialMoveShotFlexibleV2.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrialMoveShotFlexibleV2.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsTimeslice.h"
#include "TpsSimulationPlugin.h"
#include <iostream>

TpsTrialMoveShotFlexibleV2::TpsTrialMoveShotFlexibleV2(int max_path_length) 
: _max_path_length(max_path_length)
{	
}

TpsTrialMoveShotFlexibleV2::TpsTrialMoveShotFlexibleV2(
	int max_path_length, TpsTimesliceModifier& p)
:	_max_path_length(max_path_length),
	TpsTrialMoveShot(p)
{	
}

void TpsTrialMoveShotFlexibleV2::setMaxPathLength(int length)
{
	_max_path_length = length;
}

bool TpsTrialMoveShotFlexibleV2::doMove(TpsAlgorithmTPS& controller)
{	
	int num = controller.getPathNumber();
	TpsTrajectory& old_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(num);
	TpsTrajectory& new_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(-1);
	TpsRNG& rng = controller.getRNG();
	TpsOrderParameter& path_function = controller.getPathFunction();
	
	int L_old = old_trajectory.getLength();

	new_trajectory.clear();	
	new_trajectory.shootFrom(old_trajectory, rng, _perturbation,
		_n_tiny_timestep, _size_tiny_timestep);	
	
	int i = 0;
	while (!path_function.isTransitionPath(new_trajectory) && 
			new_trajectory.getLength() < _max_path_length) {
		if (i++%2 == 0) {//(rng.randDouble() < 0.5) {
			new_trajectory.extendBackward(1);
		}
		else {
			new_trajectory.extendForward(1);
		}
	}
	
	if (new_trajectory.getLength() < _max_path_length) {
		int L_new = new_trajectory.getLength();
		if (rng.randDouble() < (L_old + 1.0)/(double)(L_new + 1.0)) {
			controller.getTrajectoryFactory().moveTrajectory(-1, num+1);
			controller.incrementPathNumber();
			return true;
		}
		else {
			old_trajectory.incrementWeight();	
			return false;
		}		
	}
	else {
		old_trajectory.incrementWeight();	
		return false;
	}
}
