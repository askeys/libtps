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
 *  TpsTrialMoveShot.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrialMoveShot.h"
#include "TpsAlgorithmTPS.h"
#include "TpsTrajectory.h"
#include "TpsOrderParameter.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsTimeslice.h"
#include <iostream>

TpsTimesliceModifier TpsTrialMoveShot::_default_perturbation;

TpsTrialMoveShot::TpsTrialMoveShot()
:	_perturbation(_default_perturbation),
	_n_tiny_timestep(0),
	_size_tiny_timestep(-1)
{
	
}

TpsTrialMoveShot::TpsTrialMoveShot(TpsTimesliceModifier& p)
:	_perturbation(p),
	_n_tiny_timestep(0),
	_size_tiny_timestep(-1)
{
	
}

TpsTrialMoveShot::~TpsTrialMoveShot()
{
}

void TpsTrialMoveShot::setStepSize(double step)
{
	//_perturbation.setMaxPerturbation(step);
}

double TpsTrialMoveShot::getStepSize()
{
	//return _perturbation.getMaxPerturbation();
	return 0.0;
}

/**
\param nstep is the number of steps to perform
\param size is the size of the step (default is normal timestep/10)
\warning this should only be done for atomic or molecular simulations

In simulations of dense systems, perturbing the velocities may occasionally
cause particles to overlap more than usual if the timestep is large.  We
can avoid this by taking a few very small timesteps at the point of the 
perturbation.  Specifically we do some small timesteps forward and then the 
same number of small timesteps backward in time.  This tends to get rid of
overlapping particles while leaving the trajectory mostly unchanged.
*/
void TpsTrialMoveShot::setTinyTimesteps(int nstep, double size)
{       
     _n_tiny_timestep = nstep;
     _size_tiny_timestep = size;
}

bool TpsTrialMoveShot::doMove(TpsAlgorithmTPS& controller)
{	
	int num = controller.getPathNumber();
	TpsTrajectory& old_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(num);
	TpsTrajectory& new_trajectory = 
		controller.getTrajectoryFactory().getTrajectory(-1);
	TpsRNG& rng = controller.getRNG();
	TpsOrderParameter& path_function = controller.getPathFunction();
	
	new_trajectory.clear();	
	new_trajectory.shootFrom(old_trajectory, rng, _perturbation,
		_n_tiny_timestep, _size_tiny_timestep);	
	new_trajectory.extendBackward();
	if(path_function.hA(new_trajectory)) {
		new_trajectory.extendForward();
		//depending on what we are doing, we may want to count
		//paths that end in B (hb) or paths that visit B (HB)
		if(path_function.hBorHB(new_trajectory)) {
			controller.getTrajectoryFactory().moveTrajectory(-1, num+1);
			controller.incrementPathNumber();
			return true;
		}
	}
	old_trajectory.incrementWeight();	
	return false;
}
