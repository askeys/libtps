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
 *  TpsTrialMove.cpp
 *  tpsapi
 *
 *  Created by askeys on 10/19/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#include "TpsTrialMove.h"
#include "TpsAlgorithmTPS.h"
#include <cassert>
#include <iostream>

int TpsTrialMove::_global_id = 0;

TpsTrialMove::TpsTrialMove() : 
	_n_attemps(0), 
	_n_accepts(0), 
	_id(_global_id++),
	_n_attempts_per_cycle(1.0),
	_optimize(false)
{

}

TpsTrialMove::~TpsTrialMove()
{
}

void TpsTrialMove::attempt(TpsAlgorithmTPS& ctrl)
{
	_n_attemps++;
	if (doMove(ctrl) == true) {
		_n_accepts++;
		callbackOnAccepted();
		ctrl.callbackOnTrialMoveAccepted();
	}
	else {
		callbackOnRejected();
		ctrl.callbackOnTrialMoveRejected();
	}
	updateStepSize();
	callbackOnAttempt();
	ctrl.callbackOnTrialMove();
}
	

bool TpsTrialMove::doMove(TpsAlgorithmTPS&)
{
	int this_virtual_function_was_overridden = 0;
	assert(this_virtual_function_was_overridden);
	return false;
}

void TpsTrialMove::setAttemptsPerCycle(double n)
{
	_n_attempts_per_cycle = n;
}

double TpsTrialMove::getAttemptsPerCycle()
{
	return _n_attempts_per_cycle;
}

int TpsTrialMove::getID()
{
	return _id;
}

/**
\brief resets the counters for acceptance probability to zero
*/
void TpsTrialMove::resetAcceptanceProbability()
{
    _n_attemps = _n_accepts = 0.0;
}

/**
\brief gets the acceptance probabilty for the given move
\return the acceptance probability
*/
double TpsTrialMove::getAcceptanceProbability()
{
	if (_n_attemps == 0) {
		return 0.0;
	}
	else {
		return (_n_accepts/(double)_n_attemps);
	}
}

double TpsTrialMove::getStepSize()
{
	return 0.0;
}

void TpsTrialMove::setStepSize(double)
{
}

void TpsTrialMove::optimizeStepSize(bool do_optimize, double target, int every)
{
	_optimize = do_optimize;
	_target_acceptance_probability = target;
	_optimize_every = every;
}

/**
\brief Called every time the trial move is attempted.  You can override the 
	callback to do something special.
*/
void TpsTrialMove::callbackOnAccepted()
{
}

/**
\brief Called every time the trial move is rejected.  You can override the 
	callback to do something special.
*/
void TpsTrialMove::callbackOnRejected()
{
}

/**
\brief Called every time the trial move is attempted.  You can override the 
	callback to do something special.
*/
void TpsTrialMove::callbackOnAttempt()
{
}

void TpsTrialMove::updateStepSize()
{
	if (_optimize && _n_attemps%_optimize_every == 0) {
		double acceptance_probability = getAcceptanceProbability();
		if (acceptance_probability < 0.05) {
			acceptance_probability = 0.05;
		}
		double step_size = this->getStepSize();
		step_size *= acceptance_probability / _target_acceptance_probability;
		this->setStepSize(step_size);
	}
}
