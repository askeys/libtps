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
 *  TpsTestPeSurf2dMC.cpp
 *  tpsapi
 *
 *  Created by askeys on 2/22/09.
 *  Copyright 2009 The Regents of the University of Michigan. All rights reserved.
 *
 */

#include "TpsTestPeSurf2dMC.h"
#include <cmath>
#include <iostream>

TpsTestPeSurf2dMC::TpsTestPeSurf2dMC(TpsRNG& rng)
	: _rng(rng)
{
	_xstdev = 0.1;
	_moves_accepted = _moves_attempted = 0;
}

void TpsTestPeSurf2dMC::run(int nsteps)
{
	for (int i=0; i<nsteps; i++) {
		double dx = _rng.randNormal(0.0, _xstdev);
		double dy = _rng.randNormal(0.0, _xstdev);
						
		double energy_old = computeHamiltonian();  
		double x_old = _current_timeslice.x[0];
		double y_old = _current_timeslice.x[1];

		_current_timeslice.x[0] += dx;
		_current_timeslice.x[1] += dy;

		double energy_trial = computeHamiltonian();
		double deltaH = -_beta*(energy_trial-energy_old);
		if (exp(deltaH) < _rng.randDouble()) { 
			_current_timeslice.x[0] = x_old;
			_current_timeslice.x[1] = y_old;
		}
		_moves_accepted++;
	}
	_moves_attempted += nsteps;
	callbackOnRunCommand();
}
		
double TpsTestPeSurf2dMC::getAcceptanceProbability()
{
	return _moves_accepted / (double)_moves_attempted;
}

//------------------------
//everything defined below this point is not required...

void TpsTestPeSurf2dMC::setStepSize(double xstdev)
{
	_xstdev = xstdev;
}
