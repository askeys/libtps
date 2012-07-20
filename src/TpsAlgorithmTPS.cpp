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
 *  TpsAlgorithmTPS.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 The Regents of the University of Michigan. 
 *  All rights reserved.
 *
 */

#include "TpsAlgorithmTPS.h"
#include "TpsRNG.h"
#include "TpsTrialMove.h"
#include "TpsTrajectory.h"
#include "TpsTrajectoryEnsemble.h"
#include "TpsInitializer.h"

#include <iostream>

TpsAlgorithmTPS::TpsAlgorithmTPS(TpsTrajectoryEnsemble& t, 
	TpsRNG& r, TpsOrderParameter& f, TpsInitializer& i)
:	TpsAlgorithm(t, r, f),
	_initializer(i),
	_n_attempts_per_cycle(0.0),
	_path_number(0)
{
	TpsTrajectory& traj = _trajectory_factory.getTrajectory(-1);
	_initializer.initializeTrajectory(traj, _path_function);
	_trajectory_factory.moveTrajectory(-1, _path_number);
}

TpsAlgorithmTPS::TpsAlgorithmTPS(TpsTrajectoryEnsemble& t, 
	TpsRNG& r, TpsOrderParameter& f)
:	TpsAlgorithm(t, r, f),
	_initializer(_default_initializer),
	_n_attempts_per_cycle(0.0),
	_path_number(0)
{
}

TpsAlgorithmTPS::~TpsAlgorithmTPS()
{
}

void TpsAlgorithmTPS::initialize(TpsInitializer& initializer)
{
	TpsTrajectory& traj = _trajectory_factory.getTrajectory(_path_number);
	initializer.initializeTrajectory(traj, _path_function);
}

void TpsAlgorithmTPS::addTrialMove(	
	TpsTrialMove& move, double attempts_per_cycle)
{
	if (attempts_per_cycle > 0.0) {
		move.setAttemptsPerCycle(attempts_per_cycle);
	}
	_trial_move.push_back(&move);
	updateTrialMoves();
}

void TpsAlgorithmTPS::removeTrialMove(TpsTrialMove& move)
{
	for (unsigned int i=0; i<_trial_move.size(); i++) {
		if (_trial_move[i]->getID() == move.getID()) {
			_trial_move[i] = _trial_move[_trial_move.size()-1];
			_trial_move.pop_back();
			break;
		}
	}
	updateTrialMoves();
}

void TpsAlgorithmTPS::updateTrialMoves()
{
	_n_attempts_per_cycle = 0.0;
	_trial_move_rn_cut.clear();
	
	for (unsigned int i=0; i<_trial_move.size(); i++)
	{
		_n_attempts_per_cycle += _trial_move[i]->getAttemptsPerCycle();
		_trial_move_rn_cut.push_back(_n_attempts_per_cycle);
	}
}

void TpsAlgorithmTPS::doStep()
{
	int attempts = (int)(_n_attempts_per_cycle);

	if (_rng.randUniform() < _n_attempts_per_cycle - attempts) {
		attempts++;
	}

	for (int i=0; i<attempts; i++) {
		double r = _rng.randUniform(0.0, _n_attempts_per_cycle);			
		for (unsigned int j=0; j<_trial_move.size(); j++) {
			if (r < _trial_move_rn_cut[j]) {
				_trial_move[j]->attempt(*this);
				break;
			}
		}
	}
	//allow the user to insert custom code after cycle:
	callbackOnStepCompleted();
}

int TpsAlgorithmTPS::getPathNumber()
{
	return _path_number;
}

void TpsAlgorithmTPS::incrementPathNumber()
{
	_path_number++;
}

void TpsAlgorithmTPS::setPathNumber(int path_number)
{
	_path_number = path_number;
}
