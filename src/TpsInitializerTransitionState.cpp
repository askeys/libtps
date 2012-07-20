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
 *  TpsInitializerTransitionState.cpp
 *  tpsapi
 *
 *  Created by askeys on 3/6/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsInitializerTransitionState.h"

/*
 *  TpsInitializerTransitionState.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/24/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsInitializerTransitionState.h"
#include "TpsTrajectory.h"
#include "TpsTimeslice.h"
#include "TpsOrderParameter.h"
#include <iostream>

TpsInitializerTransitionState::TpsInitializerTransitionState(
	const char* transition_state,
	TpsTimesliceModifier& velocity_distribution)
:	
	_transition_state(transition_state),
	_velocity_distribution(velocity_distribution),
	_n_tiny_timestep(0),
	_step_size(-1)
{
}

TpsInitializerTransitionState::~TpsInitializerTransitionState()
{
}

void TpsInitializerTransitionState::setTinyTimesteps(int nstep, double stepsize)
{
	_n_tiny_timestep = nstep;
	_step_size = stepsize;
}

void TpsInitializerTransitionState::initializeTrajectory(
	TpsTrajectory& traj, TpsOrderParameter& path_function)
{		
	TpsTrajectory temp_traj(traj);
	TpsTimeslice temp_timeslice(_transition_state.c_str(), 0);
	temp_traj.pushBack(temp_timeslice);
	
	int tau = traj.getLength();
	traj.setTimeWindow(-tau/2, tau/2);
	
	do {
		traj.clear();
		traj.shootFrom(
			temp_traj, 0, _velocity_distribution, _n_tiny_timestep, _step_size);		
		traj.extend();
	}
	while (!(path_function.isTransitionPath(traj)));
}
